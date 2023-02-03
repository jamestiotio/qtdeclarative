// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qquicktextnode_p.h"

#include "qquicktextnodeengine_p.h"

#include <private/qsgadaptationlayer_p.h>
#include <private/qsgdistancefieldglyphnode_p.h>
#include <private/qquickclipnode_p.h>
#include <private/qquickitem_p.h>
#include <private/qquicktextdocument_p.h>
#include <QtQuick/private/qsgcontext_p.h>

#include <QtCore/qpoint.h>
#include <qtextdocument.h>
#include <qtextlayout.h>
#include <qabstracttextdocumentlayout.h>
#include <qxmlstream.h>
#include <private/qquickstyledtext_p.h>
#include <private/qquicktext_p_p.h>
#include <private/qfont_p.h>
#include <private/qfontengine_p.h>

#include <private/qtextdocumentlayout_p.h>
#include <qhash.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(lcVP)

/*!
  Creates an empty QQuickTextNode
*/
QQuickTextNode::QQuickTextNode(QQuickItem *ownerElement)
    : m_cursorNode(nullptr), m_ownerElement(ownerElement), m_useNativeRenderer(false), m_renderTypeQuality(-1)
{
#ifdef QSG_RUNTIME_DESCRIPTION
    qsgnode_set_description(this, QLatin1String("text"));
#endif
}

QQuickTextNode::~QQuickTextNode()
{
    qDeleteAll(m_textures);
}

QSGGlyphNode *QQuickTextNode::addGlyphs(const QPointF &position, const QGlyphRun &glyphs, const QColor &color,
                                     QQuickText::TextStyle style, const QColor &styleColor,
                                     QSGNode *parentNode)
{
    QSGRenderContext *sg = QQuickItemPrivate::get(m_ownerElement)->sceneGraphRenderContext();
    QRawFont font = glyphs.rawFont();
    bool preferNativeGlyphNode = m_useNativeRenderer;
    if (!preferNativeGlyphNode) {
        QRawFontPrivate *fontPriv = QRawFontPrivate::get(font);
        if (fontPriv->fontEngine->hasUnreliableGlyphOutline()) {
            preferNativeGlyphNode = true;
        } else {
            QFontEngine *fe = QRawFontPrivate::get(font)->fontEngine;
            preferNativeGlyphNode = !fe->isSmoothlyScalable;
        }
    }

    QSGGlyphNode *node = sg->sceneGraphContext()->createGlyphNode(sg, preferNativeGlyphNode, m_renderTypeQuality);

    node->setOwnerElement(m_ownerElement);
    node->setGlyphs(position + QPointF(0, glyphs.rawFont().ascent()), glyphs);
    node->setStyle(style);
    node->setStyleColor(styleColor);
    node->setColor(color);
    node->update();

    /* We flag the geometry as static, but we never call markVertexDataDirty
       or markIndexDataDirty on them. This is because all text nodes are
       discarded when a change occurs. If we start appending/removing from
       existing geometry, then we also need to start marking the geometry as
       dirty.
     */
    node->geometry()->setIndexDataPattern(QSGGeometry::StaticPattern);
    node->geometry()->setVertexDataPattern(QSGGeometry::StaticPattern);

    if (parentNode == nullptr)
        parentNode = this;
    parentNode->appendChildNode(node);

    if (style == QQuickText::Outline && color.alpha() > 0 && styleColor != color) {
        QSGGlyphNode *fillNode = sg->sceneGraphContext()->createGlyphNode(sg, preferNativeGlyphNode, m_renderTypeQuality);
        fillNode->setOwnerElement(m_ownerElement);
        fillNode->setGlyphs(position + QPointF(0, glyphs.rawFont().ascent()), glyphs);
        fillNode->setStyle(QQuickText::Normal);
        fillNode->setPreferredAntialiasingMode(QSGGlyphNode::GrayAntialiasing);
        fillNode->setColor(color);
        fillNode->update();

        fillNode->geometry()->setIndexDataPattern(QSGGeometry::StaticPattern);
        fillNode->geometry()->setVertexDataPattern(QSGGeometry::StaticPattern);

        parentNode->appendChildNode(fillNode);
        fillNode->setRenderOrder(node->renderOrder() + 1);
    }

    return node;
}

void QQuickTextNode::setCursor(const QRectF &rect, const QColor &color)
{
    if (m_cursorNode != nullptr)
        delete m_cursorNode;

    QSGRenderContext *sg = QQuickItemPrivate::get(m_ownerElement)->sceneGraphRenderContext();
    m_cursorNode =  sg->sceneGraphContext()->createInternalRectangleNode(rect, color);
    appendChildNode(m_cursorNode);
}

void QQuickTextNode::clearCursor()
{
    if (m_cursorNode)
        removeChildNode(m_cursorNode);
    delete m_cursorNode;
    m_cursorNode = nullptr;
}

void QQuickTextNode::addRectangleNode(const QRectF &rect, const QColor &color)
{
    QSGRenderContext *sg = QQuickItemPrivate::get(m_ownerElement)->sceneGraphRenderContext();
    appendChildNode(sg->sceneGraphContext()->createInternalRectangleNode(rect, color));
}


void QQuickTextNode::addImage(const QRectF &rect, const QImage &image)
{
    QSGRenderContext *sg = QQuickItemPrivate::get(m_ownerElement)->sceneGraphRenderContext();
    QSGInternalImageNode *node = sg->sceneGraphContext()->createInternalImageNode(sg);
    QSGTexture *texture = sg->createTexture(image);
    if (m_ownerElement->smooth())
        texture->setFiltering(QSGTexture::Linear);
    m_textures.append(texture);
    node->setTargetRect(rect);
    node->setInnerTargetRect(rect);
    node->setTexture(texture);
    if (m_ownerElement->smooth())
        node->setFiltering(QSGTexture::Linear);
    appendChildNode(node);
    node->update();
}

void QQuickTextNode::addTextDocument(const QPointF &position, QTextDocument *textDocument,
                                  const QColor &textColor,
                                  QQuickText::TextStyle style, const QColor &styleColor,
                                  const QColor &anchorColor,
                                  const QColor &selectionColor, const QColor &selectedTextColor,
                                  int selectionStart, int selectionEnd)
{
    QQuickTextNodeEngine engine;
    engine.setTextColor(textColor);
    engine.setSelectedTextColor(selectedTextColor);
    engine.setSelectionColor(selectionColor);
    engine.setAnchorColor(anchorColor);
    engine.setPosition(position);

    QList<QTextFrame *> frames;
    frames.append(textDocument->rootFrame());
    while (!frames.isEmpty()) {
        QTextFrame *textFrame = frames.takeFirst();
        frames.append(textFrame->childFrames());

        engine.addFrameDecorations(textDocument, textFrame);

        if (textFrame->firstPosition() > textFrame->lastPosition()
         && textFrame->frameFormat().position() != QTextFrameFormat::InFlow) {
            const int pos = textFrame->firstPosition() - 1;
            auto *a = static_cast<QtPrivate::ProtectedLayoutAccessor *>(textDocument->documentLayout());
            QTextCharFormat format = a->formatAccessor(pos);
            QRectF rect = a->frameBoundingRect(textFrame);

            QTextBlock block = textFrame->firstCursorPosition().block();
            engine.setCurrentLine(block.layout()->lineForTextPosition(pos - block.position()));
            engine.addTextObject(block, rect.topLeft(), format, QQuickTextNodeEngine::Unselected, textDocument,
                                 pos, textFrame->frameFormat().position());
        } else {
            QTextFrame::iterator it = textFrame->begin();

            while (!it.atEnd()) {
                Q_ASSERT(!engine.currentLine().isValid());

                QTextBlock block = it.currentBlock();
                engine.addTextBlock(textDocument, block, position, textColor, anchorColor, selectionStart, selectionEnd,
                                    (textDocument->characterCount() > QQuickTextPrivate::largeTextSizeThreshold ?
                                         m_ownerElement->clipRect() : QRectF()));
                ++it;
            }
        }
    }

    engine.addToSceneGraph(this, style, styleColor);
}

void QQuickTextNode::addTextLayout(const QPointF &position, QTextLayout *textLayout, const QColor &color,
                                QQuickText::TextStyle style, const QColor &styleColor,
                                const QColor &anchorColor,
                                const QColor &selectionColor, const QColor &selectedTextColor,
                                int selectionStart, int selectionEnd,
                                int lineStart, int lineCount)
{
    QQuickTextNodeEngine engine;
    engine.setTextColor(color);
    engine.setSelectedTextColor(selectedTextColor);
    engine.setSelectionColor(selectionColor);
    engine.setAnchorColor(anchorColor);
    engine.setPosition(position);

#if QT_CONFIG(im)
    int preeditLength = textLayout->preeditAreaText().size();
    int preeditPosition = textLayout->preeditAreaPosition();
#endif

    QVarLengthArray<QTextLayout::FormatRange> colorChanges;
    engine.mergeFormats(textLayout, &colorChanges);

    // If there's a lot of text, insert only the range of lines that can possibly be visible within the viewport.
    QRectF viewport;
    if (m_ownerElement->flags().testFlag(QQuickItem::ItemObservesViewport)) {
        viewport = m_ownerElement->clipRect();
        qCDebug(lcVP) << "text viewport" << viewport;
    }
    lineCount = lineCount >= 0
            ? qMin(lineStart + lineCount, textLayout->lineCount())
            : textLayout->lineCount();

    bool inViewport = false;
    for (int i=lineStart; i<lineCount; ++i) {
        QTextLine line = textLayout->lineAt(i);

        int start = line.textStart();
        int length = line.textLength();
        int end = start + length;

#if QT_CONFIG(im)
        if (preeditPosition >= 0
         && preeditPosition >= start
         && preeditPosition < end) {
            end += preeditLength;
        }
#endif
        if (viewport.isNull() || (line.y() + line.height() > viewport.top() && line.y() < viewport.bottom())) {
            if (!inViewport && !viewport.isNull()) {
                m_firstLineInViewport = i;
                qCDebug(lcVP) << "first line in viewport" << i << "@" << line.y();
            }
            inViewport = true;
            engine.setCurrentLine(line);
            engine.addGlyphsForRanges(colorChanges, start, end, selectionStart, selectionEnd);
        } else if (inViewport) {
            Q_ASSERT(!viewport.isNull());
            m_firstLinePastViewport = i;
            qCDebug(lcVP) << "first omitted line past bottom of viewport" << i << "@" << line.y();
            break; // went past the bottom of the viewport, so we're done
        }
    }

    engine.addToSceneGraph(this, style, styleColor);
}

void QQuickTextNode::deleteContent()
{
    while (firstChild() != nullptr)
        delete firstChild();
    m_cursorNode = nullptr;
    qDeleteAll(m_textures);
    m_textures.clear();
}

QT_END_NAMESPACE
