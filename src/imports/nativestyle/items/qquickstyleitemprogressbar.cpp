/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Controls 2 module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qquickstyleitemprogressbar.h"

QFont QQuickStyleItemProgressBar::styleFont(QQuickItem *control)
{
    return style()->font(QStyle::CE_ProgressBarLabel, controlSize(control));
}

void QQuickStyleItemProgressBar::connectToControl()
{
    QQuickStyleItem::connectToControl();
    auto progressBar = control<QQuickProgressBar>();
    connect(progressBar, &QQuickProgressBar::fromChanged, this, &QQuickStyleItem::markImageDirty);
    connect(progressBar, &QQuickProgressBar::toChanged, this, &QQuickStyleItem::markImageDirty);
    connect(progressBar, &QQuickProgressBar::positionChanged, this, &QQuickStyleItem::markImageDirty);
}

StyleItemGeometry QQuickStyleItemProgressBar::calculateGeometry()
{
    QStyleOptionProgressBar styleOption;
    initStyleOption(styleOption);

    StyleItemGeometry geometry;
    geometry.minimumSize = style()->sizeFromContents(QStyle::CT_ProgressBar, &styleOption, QSize(0, 0));
    geometry.implicitSize = geometry.minimumSize;
    styleOption.rect = QRect(QPoint(0, 0), geometry.implicitSize);
    geometry.contentRect = style()->subElementRect(QStyle::SE_ProgressBarContents, &styleOption);
    geometry.layoutRect = style()->subElementRect(QStyle::SE_ProgressBarLayoutItem, &styleOption);
    geometry.ninePatchMargins = style()->ninePatchMargins(QStyle::CE_ProgressBar, &styleOption, geometry.minimumSize);

    return geometry;
}

void QQuickStyleItemProgressBar::paintEvent(QPainter *painter)
{
    QStyleOptionProgressBar styleOption;
    initStyleOption(styleOption);
    if (m_subControl == Groove) {
        styleOption.rect = style()->subElementRect(QStyle::SE_ProgressBarGroove, &styleOption);
        style()->drawControl(QStyle::CE_ProgressBarGroove, &styleOption, painter);
    } else {
        styleOption.rect = style()->subElementRect(QStyle::SE_ProgressBarContents, &styleOption);
        style()->drawControl(QStyle::CE_ProgressBarContents, &styleOption, painter);
    }
}

void QQuickStyleItemProgressBar::initStyleOption(QStyleOptionProgressBar &styleOption)
{
    initStyleOptionBase(styleOption);
    auto progressBar = control<QQuickProgressBar>();

    styleOption.state = QStyle::State_Horizontal;

    if (progressBar->isIndeterminate()) {
        styleOption.minimum = 0;
        styleOption.maximum = 0;
    } else {
        styleOption.minimum = progressBar->from();
        styleOption.maximum = progressBar->to();
        styleOption.progress = progressBar->value();
    }
}
