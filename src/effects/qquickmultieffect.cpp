// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtypes.h"
#include <private/qquickmultieffect_p_p.h>
#include <private/qquickshadereffect_p.h>
#include <private/qquickshadereffectsource_p.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcQuickEffect, "qt.quick.effects")

/*!
    \qmltype MultiEffect
    \instantiates QQuickMultiEffect
    \inqmlmodule QtQuick
    \inherits Item
    \ingroup qtquick-effects
    \brief Applies post-processing effect to an item.

    The MultiEffect type applies a post-processing effect to \c source item.
    Compared to \c {Qt Graphical Effects} module, MultiEffect combines multiple
    effects (blur, shadow, colorize etc.) into a single item and shader. This
    makes it optimal for multiple effects.

    MultiEffect is designed especially for animated effects. There are several
    shader variations and the most optimal one gets selected based on the
    features used.
*/

/*!
    \qmlsignal QtQuick::MultiEffect::shaderChanged()

    This signal is emitted when the used shader changes.
    \sa fragmentShader, vertexShader
*/

QQuickMultiEffect::QQuickMultiEffect(QQuickItem *parent)
    : QQuickItem(*new QQuickMultiEffectPrivate, parent)
{
    setFlag(ItemHasContents);
}

QQuickMultiEffect::~QQuickMultiEffect()
{
}

/*!
    \qmlproperty Item QtQuick::MultiEffect::source

    This property holds the item to be used as a source for the effect.
    If needed, MultiEffect will internally generate a ShaderEffectSource
    as the texture source.
    \sa hasProxySource
*/
QQuickItem *QQuickMultiEffect::source() const
{
    Q_D(const QQuickMultiEffect);
    return d->source();
}

void QQuickMultiEffect::setSource(QQuickItem *item)
{
    Q_D(QQuickMultiEffect);
    d->setSource(item);
}

/*!
    \qmlproperty bool QtQuick::MultiEffect::autoPaddingEnabled

    When blur or shadow effects are enabled and this is set to true (default),
    the item size is padded automatically based on blurMax and blurMultiplier.
    Note that \c paddingRect is always added to the size.

    \sa paddingRect

    \include notes.qdocinc performance item size

    \include notes.qdocinc performance item resize
*/
bool QQuickMultiEffect::autoPaddingEnabled() const
{
    Q_D(const QQuickMultiEffect);
    return d->autoPaddingEnabled();
}

void QQuickMultiEffect::setAutoPaddingEnabled(bool enabled)
{
    Q_D(QQuickMultiEffect);
    d->setAutoPaddingEnabled(enabled);
}

/*!
    \qmlproperty rect QtQuick::MultiEffect::paddingRect

    Set this to increase item size manually so that blur and/or shadows will fit.
    If paddingEnabled is enabled and paddingRect is not set, the item is padded
    to fit maximally blurred item based on blurMax and blurMultiplier. When
    enabling the shadow, you typically needs to take shadowHorizontalOffset and
    shadowVerticalOffset into account and adjust this paddingRect accordingly.

    \include notes.qdocinc performance item size

    \include notes.qdocinc performance item resize
*/
QRectF QQuickMultiEffect::paddingRect() const
{
    Q_D(const QQuickMultiEffect);
    return d->paddingRect();
}

void QQuickMultiEffect::setPaddingRect(const QRectF &rect)
{
    Q_D(QQuickMultiEffect);
    d->setPaddingRect(rect);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::brightness

    This property defines how much the source brightness is increased or
    decreased.

    The value ranges from -1.0 to 1.0. By default, the property is set to \c
    0.0 (no change).
*/
qreal QQuickMultiEffect::brightness() const
{
    Q_D(const QQuickMultiEffect);
    return d->brightness();
}

void QQuickMultiEffect::setBrightness(qreal brightness)
{
    Q_D(QQuickMultiEffect);
    d->setBrightness(brightness);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::contrast

    This property defines how much the source contrast is increased or
    decreased.

    The value ranges from -1.0 to 1.0. By default, the property is set to \c
    0.0 (no change).
*/
qreal QQuickMultiEffect::contrast() const
{
    Q_D(const QQuickMultiEffect);
    return d->contrast();
}

void QQuickMultiEffect::setContrast(qreal contrast)
{
    Q_D(QQuickMultiEffect);
    d->setContrast(contrast);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::saturation

    This property defines how much the source saturation is increased or
    decreased.

    The value ranges from -1.0 (totally desaturated) to inf. By default,
    the property is set to \c 0.0 (no change).
*/
qreal QQuickMultiEffect::saturation() const
{
    Q_D(const QQuickMultiEffect);
    return d->saturation();
}

void QQuickMultiEffect::setSaturation(qreal saturation)
{
    Q_D(QQuickMultiEffect);
    d->setSaturation(saturation);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::colorize

    This property defines how much the source is colorized with the
    colorizeColor.

    The value ranges from 0.0 (no colorized) to 1.0 (fully colorized).
    By default, the property is set to \c 0.0 (no change).
*/
qreal QQuickMultiEffect::colorize() const
{
    Q_D(const QQuickMultiEffect);
    return d->colorize();
}

void QQuickMultiEffect::setColorize(qreal colorize)
{
    Q_D(QQuickMultiEffect);
    d->setColorize(colorize);
}

/*!
    \qmlproperty color QtQuick::MultiEffect::colorizeColor

    This property defines the RGBA color value which is used to
    colorize the source.

    By default, the property is set to \c  Qt.rgba(1.0, 0.0, 0.0, 1.0) (red).
*/
QColor QQuickMultiEffect::colorizeColor() const
{
    Q_D(const QQuickMultiEffect);
    return d->colorizeColor();
}

void QQuickMultiEffect::setColorizeColor(const QColor &color)
{
    Q_D(QQuickMultiEffect);
    d->setColorizeColor(color);
}

/*!
    \qmlproperty bool QtQuick::MultiEffect::blurEnabled

    Enables the blur effect.

    \include notes.qdocinc performance shader regen
*/
bool QQuickMultiEffect::blurEnabled() const
{
    Q_D(const QQuickMultiEffect);
    return d->blurEnabled();
}

void QQuickMultiEffect::setBlurEnabled(bool enabled)
{
    Q_D(QQuickMultiEffect);
    d->setBlurEnabled(enabled);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::blur

    This property defines how much blur (radius) is applied to the source.

    The value ranges from 0.0 (no blur) to 1.0 (full blur). By default,
    the property is set to \c 0.0 (no change). The amount of full blur
    is affected by blurMax and blurMultiplier.

    \b {Performance note:} If you don't need to go close to 1.0 at any point
    of blur animations, consider reducing blurMax or blurMultiplier for
    optimal performance.
*/
qreal QQuickMultiEffect::blur() const
{
    Q_D(const QQuickMultiEffect);
    return d->blur();
}

void QQuickMultiEffect::setBlur(qreal blur)
{
    Q_D(QQuickMultiEffect);
    d->setBlur(blur);
}

/*!
    \qmlproperty int QtQuick::MultiEffect::blurMax

    This property defines the maximum pixel radius that blur with value
    1.0 will reach.

    Meaningful range of this value is from 2 (subtle blur) to 64 (high
    blur). By default, the property is set to \c 32. For the most optimal
    performance, select as small value as you need.

    \note This affects to both blur and shadow effects.

    \include notes.qdocinc performance shader regen

    \include notes.qdocinc performance item resize
*/
int QQuickMultiEffect::blurMax() const
{
    Q_D(const QQuickMultiEffect);
    return d->blurMax();
}

void QQuickMultiEffect::setBlurMax(int blurMax)
{
    Q_D(QQuickMultiEffect);
    d->setBlurMax(blurMax);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::blurMultiplier

    This property defines a multiplier for extending the blur radius.

    The value ranges from 0.0 (not multiplied) to inf. By default,
    the property is set to \c 0.0. Incresing the multiplier extends the
    blur radius, but decreases the blur quality. This is more performant
    option for a bigger blur radius than blurMax as it doesn't increase
    the amount of texture lookups.

    \note This affects to both blur and shadow effects.

    \include notes.qdocinc performance item resize
*/
qreal QQuickMultiEffect::blurMultiplier() const
{
    Q_D(const QQuickMultiEffect);
    return d->blurMultiplier();
}

void QQuickMultiEffect::setBlurMultiplier(qreal blurMultiplier)
{
    Q_D(QQuickMultiEffect);
    d->setBlurMultiplier(blurMultiplier);
}

/*!
    \qmlproperty bool QtQuick::MultiEffect::shadowEnabled

    Enables the shadow effect.

    \include notes.qdocinc performance shader regen
*/
bool QQuickMultiEffect::shadowEnabled() const
{
    Q_D(const QQuickMultiEffect);
    return d->shadowEnabled();
}

void QQuickMultiEffect::setShadowEnabled(bool enabled)
{
    Q_D(QQuickMultiEffect);
    d->setShadowEnabled(enabled);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::shadowOpacity

    This property defines the opacity of the drop shadow. This value
    is multiplied with the \c shadowColor alpha value.

    The value ranges from 0.0 (fully transparent) to 1.0 (fully opaque).
    By default, the property is set to \c 1.0.
*/
qreal QQuickMultiEffect::shadowOpacity() const
{
    Q_D(const QQuickMultiEffect);
    return d->shadowOpacity();
}

void QQuickMultiEffect::setShadowOpacity(qreal shadowOpacity)
{
    Q_D(QQuickMultiEffect);
    d->setShadowOpacity(shadowOpacity);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::shadowBlur

    This property defines how much blur (radius) is applied to the shadow.

    The value ranges from 0.0 (no blur) to 1.0 (full blur). By default,
    the property is set to \c 1.0. The amount of full blur
    is affected by blurMax and blurMultiplier.

    \b {Performance note:} The most optimal way to reduce shadow blurring is
    to make blurMax smaller (if it isn't needed for item blur). Just remember
    to not adjust blurMax during animations.
*/
qreal QQuickMultiEffect::shadowBlur() const
{
    Q_D(const QQuickMultiEffect);
    return d->shadowBlur();
}

void QQuickMultiEffect::setShadowBlur(qreal shadowBlur)
{
    Q_D(QQuickMultiEffect);
    d->setShadowBlur(shadowBlur);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::shadowHorizontalOffset

    This property defines the horizontal offset of the shadow from the
    item center.

    The value ranges from -inf to inf. By default, the property is set
    to \c 10.0.

    \note When moving shadow position away from center and adding
    shadowBlur, you possibly also need to increase the paddingRect
    accordingly if you want the shadow to not be clipped.
*/
qreal QQuickMultiEffect::shadowHorizontalOffset() const
{
    Q_D(const QQuickMultiEffect);
    return d->shadowHorizontalOffset();
}

void QQuickMultiEffect::setShadowHorizontalOffset(qreal offset)
{
    Q_D(QQuickMultiEffect);
    d->setShadowHorizontalOffset(offset);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::shadowVerticalOffset

    This property defines the vertical offset of the shadow from the
    item center.

    The value ranges from -inf to inf. By default,
    the property is set to \c 10.0.

    \note When moving shadow position away from center and adding
    shadowBlur, you possibly also need to increase the paddingRect
    accordingly if you want the shadow to not be clipped.
*/
qreal QQuickMultiEffect::shadowVerticalOffset() const
{
    Q_D(const QQuickMultiEffect);
    return d->shadowVerticalOffset();
}

void QQuickMultiEffect::setShadowVerticalOffset(qreal offset)
{
    Q_D(QQuickMultiEffect);
    d->setShadowVerticalOffset(offset);
}

/*!
    \qmlproperty color QtQuick::MultiEffect::shadowColor

    This property defines the RGBA color value which is used to colorize
    the shadow. It is useful for example when a shadow is used for
    simulating a glow effect.

    By default, the property is set to \c {Qt.rgba(0.0, 0.0, 0.0, 1.0)}
    (black).
*/
QColor QQuickMultiEffect::shadowColor() const
{
    Q_D(const QQuickMultiEffect);
    return d->shadowColor();
}

void QQuickMultiEffect::setShadowColor(const QColor &color)
{
    Q_D(QQuickMultiEffect);
    d->setShadowColor(color);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::shadowScale

    This property defines the scale of the shadow. Scaling is applied from
    the center of the item.

    The value ranges from 0 to inf. By default, the property is set to
    \c 1.0.

    \note When increasing the shadowScale, you possibly also need to
    increase the paddingRect accordingly to avoid the shadow from being
    clipped.
*/
qreal QQuickMultiEffect::shadowScale() const
{
    Q_D(const QQuickMultiEffect);
    return d->shadowScale();
}

void QQuickMultiEffect::setShadowScale(qreal shadowScale)
{
    Q_D(QQuickMultiEffect);
    d->setShadowScale(shadowScale);
}

/*!
    \qmlproperty bool QtQuick::MultiEffect::maskEnabled

    Enables the mask effect.

    \include notes.qdocinc performance shader regen
*/
bool QQuickMultiEffect::maskEnabled() const
{
    Q_D(const QQuickMultiEffect);
    return d->maskEnabled();
}

void QQuickMultiEffect::setMaskEnabled(bool enabled)
{
    Q_D(QQuickMultiEffect);
    d->setMaskEnabled(enabled);
}

/*!
    \qmlproperty Item QtQuick::MultiEffect::maskSource

    Source item for the mask effect. Should point to ShaderEffectSource,
    item with \c {layer.enabled} set to \c true, or to an item that can be
    directly used as a texture source (e.g. \l [QML] Image). The alpha
    channel of the source item is used for masking.
*/
QQuickItem *QQuickMultiEffect::maskSource() const
{
    Q_D(const QQuickMultiEffect);
    return d->maskSource();
}

void QQuickMultiEffect::setMaskSource(QQuickItem *item)
{
    Q_D(QQuickMultiEffect);
    d->setMaskSource(item);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::maskThresholdLow

    This property defines a lower threshold value for the mask pixels.
    The mask pixels that have an alpha value below this property are used
    to completely mask away the corresponding pixels from the source item.
    The mask pixels that have a higher alpha value are used to alphablend
    the source item to the display.

    The value ranges from 0.0 (alpha value 0) to 1.0 (alpha value 255). By
    default, the property is set to \c 0.0.
*/
qreal QQuickMultiEffect::maskThresholdLow() const
{
    Q_D(const QQuickMultiEffect);
    return d->maskThresholdLow();
}

void QQuickMultiEffect::setMaskThresholdLow(qreal threshold)
{
    Q_D(QQuickMultiEffect);
    d->setMaskThresholdLow(threshold);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::maskSpreadLow

    This property defines the smoothness of the mask edges near the
    maskThresholdLow. Setting higher spread values softens the transition
    from the transparent mask pixels towards opaque mask pixels by adding
    interpolated values between them.

    The value ranges from 0.0 (sharp mask edge) to 1.0 (smooth mask edge).
    By default, the property is set to \c 0.0.
*/
qreal QQuickMultiEffect::maskSpreadLow() const
{
    Q_D(const QQuickMultiEffect);
    return d->maskSpreadLow();
}

void QQuickMultiEffect::setMaskSpreadLow(qreal spread)
{
    Q_D(QQuickMultiEffect);
    d->setMaskSpreadLow(spread);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::maskThresholdUp

    This property defines an upper threshold value for the mask pixels.
    The mask pixels that have an alpha value below this property are used
    to completely mask away the corresponding pixels from the source item.
    The mask pixels that have a higher alpha value are used to alphablend
    the source item to the display.

    The value ranges from 0.0 (alpha value 0) to 1.0 (alpha value 255). By
    default, the property is set to \c 1.0.
*/
qreal QQuickMultiEffect::maskThresholdUp() const
{
    Q_D(const QQuickMultiEffect);
    return d->maskThresholdUp();
}

void QQuickMultiEffect::setMaskThresholdUp(qreal threshold)
{
    Q_D(QQuickMultiEffect);
    d->setMaskThresholdUp(threshold);
}

/*!
    \qmlproperty real QtQuick::MultiEffect::maskSpreadUp

    This property defines the smoothness of the mask edges near the
    maskThresholdUp. Using higher spread values softens the transition
    from the transparent mask pixels towards opaque mask pixels by adding
    interpolated values between them.

    The value ranges from 0.0 (sharp mask edge) to 1.0 (smooth mask edge).
    By default, the property is set to \c 0.0.
*/
qreal QQuickMultiEffect::maskSpreadUp() const
{
    Q_D(const QQuickMultiEffect);
    return d->maskSpreadUp();
}

void QQuickMultiEffect::setMaskSpreadUp(qreal spread)
{
    Q_D(QQuickMultiEffect);
    d->setMaskSpreadUp(spread);
}

/*!
    \qmlproperty bool QtQuick::MultiEffect::maskInverted

    This property switches the mask to the opposite side; instead of
    masking away the content outside maskThresholdLow and maskThresholdUp,
    content between them will get masked away.

    By default, the property is set to \c false.
*/
bool QQuickMultiEffect::maskInverted() const
{
    Q_D(const QQuickMultiEffect);
    return d->maskInverted();
}

void QQuickMultiEffect::setMaskInverted(bool inverted)
{
    Q_D(QQuickMultiEffect);
    d->setMaskInverted(inverted);
}

/*!
    \qmlproperty rect QtQuick::MultiEffect::itemRect

    Read-only access to effect item rectangle. This can be used e.g. to see
    the area item covers.
*/
QRectF QQuickMultiEffect::itemRect() const
{
    Q_D(const QQuickMultiEffect);
    return d->itemRect();
}

/*!
    \qmlproperty string QtQuick::MultiEffect::fragmentShader
    \readonly

    Read-only access to filename of the currently used fragment shader.
*/
QString QQuickMultiEffect::fragmentShader() const
{
    Q_D(const QQuickMultiEffect);
    return d->fragmentShader();
}

/*!
    \qmlproperty string QtQuick::MultiEffect::vertexShader
    \readonly

    Read-only access to filename of the currently used vertex shader.
*/
QString QQuickMultiEffect::vertexShader() const
{
    Q_D(const QQuickMultiEffect);
    return d->vertexShader();
}

/*!
    \qmlproperty bool QtQuick::MultiEffect::hasProxySource
    \readonly

    Returns true when the MultiEffect internally creates \l ShaderEffectSource
    for the \l source item and false when \l source item is used as-is.
    For example when source is \l Image element or \l Item with \c layer.enabled
    set to \c true, this additional proxy source is not needed.
*/
bool QQuickMultiEffect::hasProxySource() const
{
    Q_D(const QQuickMultiEffect);
    return d->hasProxySource();
}

// *** protected ***

void QQuickMultiEffect::componentComplete()
{
    Q_D(QQuickMultiEffect);
    QQuickItem::componentComplete();
    d->initialize();
}

void QQuickMultiEffect::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_D(QQuickMultiEffect);
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    if (width() > 0 && height() > 0)
        d->handleGeometryChange(newGeometry, oldGeometry);
}

void QQuickMultiEffect::itemChange(ItemChange change, const ItemChangeData &value)
{
    Q_D(QQuickMultiEffect);
    d->handleItemChange(change, value);
    QQuickItem::itemChange(change, value);
}

// *** private ***

QQuickMultiEffectPrivate::QQuickMultiEffectPrivate()
{
}

QQuickMultiEffectPrivate::~QQuickMultiEffectPrivate()
{
}

void QQuickMultiEffectPrivate::handleGeometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_UNUSED(oldGeometry);
    Q_UNUSED(newGeometry);
    initialize();
    if (!m_shaderEffect)
        return;
    updateBlurItemSizes();
    updateSourcePadding();
}

void QQuickMultiEffectPrivate::handleItemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    Q_UNUSED(value);
    if (change == QQuickItem::ItemSceneChange)
        initialize();
}


QQuickItem *QQuickMultiEffectPrivate::source() const
{
    return m_sourceItem;
}

void QQuickMultiEffectPrivate::setSource(QQuickItem *item)
{
    Q_Q(QQuickMultiEffect);
    if (item == m_sourceItem)
        return;

    m_sourceItem = item;
    if (m_shaderSource)
        m_shaderSource->setInput(m_sourceItem);

    updateSourcePadding();
    q->update();
    Q_EMIT q->sourceChanged();
}

bool QQuickMultiEffectPrivate::autoPaddingEnabled() const
{
    return m_autoPaddingEnabled;
}

void QQuickMultiEffectPrivate::setAutoPaddingEnabled(bool enabled)
{
    Q_Q(QQuickMultiEffect);
    if (enabled == m_autoPaddingEnabled)
        return;

    m_autoPaddingEnabled = enabled;
    updateSourcePadding();
    q->update();
    Q_EMIT q->autoPaddingEnabledChanged();
}

QRectF QQuickMultiEffectPrivate::paddingRect() const
{
    return m_paddingRect;
}

void QQuickMultiEffectPrivate::setPaddingRect(const QRectF &rect)
{
    Q_Q(QQuickMultiEffect);
    if (rect == m_paddingRect)
        return;
    m_paddingRect = rect;
    updateCenterOffset();
    updateSourcePadding();
    q->update();
    emit q->paddingRectChanged();
}

qreal QQuickMultiEffectPrivate::brightness() const
{
    return m_brightness;
}

void QQuickMultiEffectPrivate::setBrightness(qreal brightness)
{
    Q_Q(QQuickMultiEffect);
    if (brightness == m_brightness)
        return;

    m_brightness = brightness;
    if (m_shaderEffect)
        m_shaderEffect->setProperty("brightness", m_brightness);

    q->update();
    Q_EMIT q->brightnessChanged();
}

qreal QQuickMultiEffectPrivate::contrast() const
{
    return m_contrast;
}

void QQuickMultiEffectPrivate::setContrast(qreal contrast)
{
    Q_Q(QQuickMultiEffect);
    if (contrast == m_contrast)
        return;

    m_contrast = contrast;
    if (m_shaderEffect)
        m_shaderEffect->setProperty("contrast", m_contrast);

    q->update();
    Q_EMIT q->contrastChanged();
}

qreal QQuickMultiEffectPrivate::saturation() const
{
    return m_saturation;
}

void QQuickMultiEffectPrivate::setSaturation(qreal saturation)
{
    Q_Q(QQuickMultiEffect);
    if (saturation == m_saturation)
        return;

    m_saturation = saturation;
    if (m_shaderEffect)
        m_shaderEffect->setProperty("saturation", m_saturation);

    q->update();
    Q_EMIT q->saturationChanged();
}

qreal QQuickMultiEffectPrivate::colorize() const
{
    return m_colorize;
}

void QQuickMultiEffectPrivate::setColorize(qreal colorize)
{
    Q_Q(QQuickMultiEffect);
    if (colorize == m_colorize)
        return;

    m_colorize = colorize;
    updateColorizeColor();

    q->update();
    Q_EMIT q->colorizeChanged();
}

QColor QQuickMultiEffectPrivate::colorizeColor() const
{
    return m_colorizeColor;
}

void QQuickMultiEffectPrivate::setColorizeColor(const QColor &color)
{
    Q_Q(QQuickMultiEffect);
    if (color == m_colorizeColor)
        return;

    m_colorizeColor = color;
    updateColorizeColor();

    q->update();
    Q_EMIT q->colorizeColorChanged();
}

bool QQuickMultiEffectPrivate::blurEnabled() const
{
    return m_blurEnabled;
}

void QQuickMultiEffectPrivate::setBlurEnabled(bool enabled)
{
    Q_Q(QQuickMultiEffect);
    if (enabled == m_blurEnabled)
        return;

    m_blurEnabled = enabled;
    updateSourcePadding();
    updateBlurLevel();
    updateEffectShaders();
    if (m_shaderEffect)
        m_shaderEffect->setProperty("blurEnabled", m_blurEnabled);

    q->update();
    Q_EMIT q->blurEnabledChanged();
}

qreal QQuickMultiEffectPrivate::blur() const
{
    return m_blur;
}

void QQuickMultiEffectPrivate::setBlur(qreal blur)
{
    Q_Q(QQuickMultiEffect);
    if (blur == m_blur)
        return;

    m_blur = blur;
    updateBlurWeights();
    if (m_shaderEffect)
        m_shaderEffect->setProperty("blur", m_blur);

    q->update();
    Q_EMIT q->blurChanged();
}

int QQuickMultiEffectPrivate::blurMax() const
{
    return m_blurMax;
}

void QQuickMultiEffectPrivate::setBlurMax(int blurMax)
{
    Q_Q(QQuickMultiEffect);
    if (blurMax == m_blurMax)
        return;

    m_blurMax = blurMax;
    updateSourcePadding();
    updateBlurLevel();
    updateBlurItemSizes();
    updateBlurWeights();
    updateShadowBlurWeights();
    updateEffectShaders();
    if (m_shaderEffect)
        m_shaderEffect->setProperty("blurMax", m_blurMax);

    q->update();
    Q_EMIT q->blurMaxChanged();
}

qreal QQuickMultiEffectPrivate::blurMultiplier() const
{
    return m_blurMultiplier;
}

void QQuickMultiEffectPrivate::setBlurMultiplier(qreal blurMultiplier)
{
    Q_Q(QQuickMultiEffect);
    if (blurMultiplier == m_blurMultiplier)
        return;

    m_blurMultiplier = blurMultiplier;
    updateSourcePadding();
    updateBlurItemSizes();
    updateBlurWeights();
    updateShadowBlurWeights();
    if (m_shaderEffect)
        m_shaderEffect->setProperty("blurMultiplier", m_blurMultiplier);

    q->update();
    Q_EMIT q->blurMultiplierChanged();
}

bool QQuickMultiEffectPrivate::shadowEnabled() const
{
    return m_shadowEnabled;
}

void QQuickMultiEffectPrivate::setShadowEnabled(bool enabled)
{
    Q_Q(QQuickMultiEffect);
    if (enabled == m_shadowEnabled)
        return;

    m_shadowEnabled = enabled;
    updateSourcePadding();
    updateBlurLevel();
    updateEffectShaders();
    if (m_shaderEffect)
        m_shaderEffect->setProperty("shadowEnabled", m_shadowEnabled);

    q->update();
    Q_EMIT q->shadowEnabledChanged();
}

qreal QQuickMultiEffectPrivate::shadowOpacity() const
{
    return m_shadowOpacity;
}

void QQuickMultiEffectPrivate::setShadowOpacity(qreal shadowOpacity)
{
    Q_Q(QQuickMultiEffect);
    if (shadowOpacity == m_shadowOpacity)
        return;

    m_shadowOpacity = shadowOpacity;
    updateShadowColor();

    q->update();
    Q_EMIT q->shadowOpacityChanged();
}

qreal QQuickMultiEffectPrivate::shadowBlur() const
{
    return m_shadowBlur;
}

void QQuickMultiEffectPrivate::setShadowBlur(qreal shadowBlur)
{
    Q_Q(QQuickMultiEffect);
    if (shadowBlur == m_shadowBlur)
        return;

    m_shadowBlur = shadowBlur;
    updateShadowBlurWeights();
    if (m_shaderEffect)
        m_shaderEffect->setProperty("shadowBlur", m_shadowBlur);

    q->update();
    Q_EMIT q->shadowBlurChanged();
}

qreal QQuickMultiEffectPrivate::shadowHorizontalOffset() const
{
    return m_shadowHorizontalOffset;
}

void QQuickMultiEffectPrivate::setShadowHorizontalOffset(qreal offset)
{
    Q_Q(QQuickMultiEffect);
    if (offset == m_shadowHorizontalOffset)
        return;

    m_shadowHorizontalOffset = offset;
    updateShadowOffset();

    q->update();
    Q_EMIT q->shadowHorizontalOffsetChanged();
}

qreal QQuickMultiEffectPrivate::shadowVerticalOffset() const
{
    return m_shadowVerticalOffset;
}

void QQuickMultiEffectPrivate::setShadowVerticalOffset(qreal offset)
{
    Q_Q(QQuickMultiEffect);
    if (offset == m_shadowVerticalOffset)
        return;

    m_shadowVerticalOffset = offset;
    updateShadowOffset();

    q->update();
    Q_EMIT q->shadowVerticalOffsetChanged();
}

QColor QQuickMultiEffectPrivate::shadowColor() const
{
    return m_shadowColor;
}

void QQuickMultiEffectPrivate::setShadowColor(const QColor &color)
{
    Q_Q(QQuickMultiEffect);
    if (color == m_shadowColor)
        return;

    m_shadowColor = color;
    updateShadowColor();

    q->update();
    Q_EMIT q->shadowColorChanged();
}

qreal QQuickMultiEffectPrivate::shadowScale() const
{
    return m_shadowScale;
}

void QQuickMultiEffectPrivate::setShadowScale(qreal shadowScale)
{
    Q_Q(QQuickMultiEffect);
    if (shadowScale == m_shadowScale)
        return;

    m_shadowScale = shadowScale;
    updateCenterOffset();
    if (m_shaderEffect)
        m_shaderEffect->setProperty("shadowScale", 1.0 / m_shadowScale);

    q->update();
    Q_EMIT q->shadowScaleChanged();
}

bool QQuickMultiEffectPrivate::maskEnabled() const
{
    return m_maskEnabled;
}

void QQuickMultiEffectPrivate::setMaskEnabled(bool enabled)
{
    Q_Q(QQuickMultiEffect);
    if (enabled == m_maskEnabled)
        return;

    m_maskEnabled = enabled;
    updateEffectShaders();
    if (m_shaderEffect)
        m_shaderEffect->setProperty("maskEnabled", m_maskEnabled);

    q->update();
    Q_EMIT q->maskEnabledChanged();
}

QQuickItem *QQuickMultiEffectPrivate::maskSource() const
{
    return m_maskSourceItem;
}

void QQuickMultiEffectPrivate::setMaskSource(QQuickItem *item)
{
    Q_Q(QQuickMultiEffect);
    if (item == m_maskSourceItem)
        return;

    m_maskSourceItem = item;
    if (m_shaderEffect) {
        auto maskSourceVariant = QVariant::fromValue<QQuickItem*>(m_maskSourceItem);
        m_shaderEffect->setProperty("maskSrc", maskSourceVariant);
    }

    q->update();
    Q_EMIT q->maskSourceChanged();
}

qreal QQuickMultiEffectPrivate::maskThresholdLow() const
{
    return m_maskThresholdLow;
}

void QQuickMultiEffectPrivate::setMaskThresholdLow(qreal threshold)
{
    Q_Q(QQuickMultiEffect);
    if (threshold == m_maskThresholdLow)
        return;

    m_maskThresholdLow = threshold;
    updateMaskThresholdSpread();

    q->update();
    Q_EMIT q->maskThresholdLowChanged();
}

qreal QQuickMultiEffectPrivate::maskSpreadLow() const
{
    return m_maskSpreadLow;
}

void QQuickMultiEffectPrivate::setMaskSpreadLow(qreal spread)
{
    Q_Q(QQuickMultiEffect);
    if (spread == m_maskSpreadLow)
        return;

    m_maskSpreadLow = spread;
    updateMaskThresholdSpread();

    q->update();
    Q_EMIT q->maskSpreadLowChanged();
}

qreal QQuickMultiEffectPrivate::maskThresholdUp() const
{
    return m_maskThresholdUp;
}

void QQuickMultiEffectPrivate::setMaskThresholdUp(qreal threshold)
{
    Q_Q(QQuickMultiEffect);
    if (threshold == m_maskThresholdUp)
        return;

    m_maskThresholdUp = threshold;
    updateMaskThresholdSpread();

    q->update();
    Q_EMIT q->maskThresholdUpChanged();
}

qreal QQuickMultiEffectPrivate::maskSpreadUp() const
{
    return m_maskSpreadUp;
}

void QQuickMultiEffectPrivate::setMaskSpreadUp(qreal spread)
{
    Q_Q(QQuickMultiEffect);
    if (spread == m_maskSpreadUp)
        return;

    m_maskSpreadUp = spread;
    updateMaskThresholdSpread();

    q->update();
    Q_EMIT q->maskSpreadUpChanged();
}

bool QQuickMultiEffectPrivate::maskInverted() const
{
    return m_maskInverted;
}

void QQuickMultiEffectPrivate::setMaskInverted(bool inverted)
{
    Q_Q(QQuickMultiEffect);
    if (inverted == m_maskInverted)
        return;

    m_maskInverted = inverted;
    if (m_shaderEffect)
        m_shaderEffect->setProperty("maskInverted", float(m_maskInverted));

    q->update();
    Q_EMIT q->maskInvertedChanged();
}

QRectF QQuickMultiEffectPrivate::itemRect() const
{
    if (!m_shaderEffect  || !m_shaderSource)
        return QRectF();

    QRectF sourceRect = m_shaderSource->sourceRect();
    if (sourceRect.width() > 0 && sourceRect.height() > 0)
        return sourceRect;
    else
        return m_shaderEffect->boundingRect();
}

QString QQuickMultiEffectPrivate::fragmentShader() const
{
    return m_fragShader;
}

QString QQuickMultiEffectPrivate::vertexShader() const
{
    return m_vertShader;
}

bool QQuickMultiEffectPrivate::hasProxySource() const
{
    return m_shaderSource && m_shaderSource->isActive();
}

// This initializes the component. It will be ran once, when
// the component is ready and it has a valid size.
void QQuickMultiEffectPrivate::initialize()
{
    Q_Q(QQuickMultiEffect);
    if (m_initialized)
        return;
    if (!q->isComponentComplete())
        return;
    if (!q->window())
        return;
    if (q->width() <= 0 || q->height() <= 0)
        return;

    m_shaderEffect = new QQuickShaderEffect(q);
    m_shaderSource = new QGfxSourceProxy(q);
    QObject::connect(m_shaderSource, &QGfxSourceProxy::outputChanged, q, [this] { proxyOutputChanged(); });
    QObject::connect(m_shaderSource, &QGfxSourceProxy::activeChanged, q, &QQuickMultiEffect::hasProxySourceChanged);

    m_shaderEffect->setParentItem(q);
    m_shaderEffect->setSize(q->size());

    m_shaderSource->setParentItem(q);
    m_shaderSource->setSize(q->size());
    m_shaderSource->setInput(m_sourceItem);

    updateCenterOffset();
    updateMaskThresholdSpread();
    updateBlurWeights();
    updateShadowBlurWeights();
    updateColorizeColor();
    updateShadowOffset();

    // Create properties
    auto sourceVariant = QVariant::fromValue<QQuickItem*>(m_shaderSource->output());
    m_shaderEffect->setProperty("src", sourceVariant);
    m_shaderEffect->setProperty("brightness", m_brightness);
    m_shaderEffect->setProperty("contrast", m_contrast);
    m_shaderEffect->setProperty("saturation", m_saturation);
    m_shaderEffect->setProperty("blurEnabled", m_blurEnabled);
    m_shaderEffect->setProperty("blur", m_blur);
    m_shaderEffect->setProperty("blurMax", m_blurMax);
    m_shaderEffect->setProperty("blurMultiplier", m_blurMultiplier);
    m_shaderEffect->setProperty("shadowEnabled", m_shadowEnabled);
    m_shaderEffect->setProperty("shadowOpacity", m_shadowOpacity);
    m_shaderEffect->setProperty("shadowBlur", m_shadowBlur);
    m_shaderEffect->setProperty("shadowColor", m_shadowColor);
    m_shaderEffect->setProperty("shadowScale", 1.0 / m_shadowScale);
    m_shaderEffect->setProperty("maskEnabled", m_maskEnabled);
    auto maskSourceVariant = QVariant::fromValue<QQuickItem*>(m_maskSourceItem);
    m_shaderEffect->setProperty("maskSrc", maskSourceVariant);
    m_shaderEffect->setProperty("maskInverted", float(m_maskInverted));
    m_shaderEffect->setProperty("centerOffset", m_centerOffset);

    updateBlurLevel();
    updateBlurItemSizes();
    updateSourcePadding();

    updateEffectShaders();

    m_initialized = true;
}

void QQuickMultiEffectPrivate::updateMaskThresholdSpread()
{
    m_maskThresholdSpread = QVector4D(
                m_maskThresholdLow,
                m_maskSpreadLow + 1.0,
                m_maskThresholdUp,
                m_maskSpreadUp + 1.0
                );
    if (m_shaderEffect)
        m_shaderEffect->setProperty("mask", m_maskThresholdSpread);
}

void QQuickMultiEffectPrivate::updateCenterOffset()
{
    if (!m_shaderEffect)
        return;

    const qreal scale = 1.0 / m_shadowScale;
    m_centerOffset = QVector2D((1.0 - scale) * (0.5 + 0.5 * (m_paddingRect.x() - m_paddingRect.width()) / m_shaderEffect->width()),
                               (1.0 - scale) * (0.5 + 0.5 * (m_paddingRect.y() - m_paddingRect.height()) / m_shaderEffect->height()));
    m_shaderEffect->setProperty("centerOffset", m_centerOffset);
}

void QQuickMultiEffectPrivate::updateShadowOffset()
{
    if (!m_shaderEffect)
        return;

    QVector2D shadowOffset = QVector2D(m_shadowHorizontalOffset / m_shaderEffect->width(), m_shadowVerticalOffset / m_shaderEffect->height());
    m_shaderEffect->setProperty("shadowOffset", shadowOffset);
}

void QQuickMultiEffectPrivate::updateColorizeColor()
{
    if (!m_shaderEffect)
        return;

    int alpha = std::clamp(int(m_colorizeColor.alpha() * m_colorize), 0, 255);
    QColor colorizeColor = m_colorizeColor;
    colorizeColor.setAlpha(alpha);
    m_shaderEffect->setProperty("colorizeColor", colorizeColor);
}

void QQuickMultiEffectPrivate::updateShadowColor()
{
    if (!m_shaderEffect)
        return;

    int alpha = std::clamp(int(m_shadowColor.alpha() * m_shadowOpacity), 0, 255);
    QColor shadowColor = m_shadowColor;
    shadowColor.setAlpha(alpha);
    m_shaderEffect->setProperty("shadowColor", shadowColor);
}

float QQuickMultiEffectPrivate::calculateLod(float blurAmount)
{
    return qSqrt(blurAmount * float(m_blurMax) / 64.0f) * 1.2f - 0.2f;
}

float QQuickMultiEffectPrivate::blurWeight(float v)
{
    return std::max(0.0f, std::min(1.0f, 1.0f - v * 2.0f));
}

void QQuickMultiEffectPrivate::getBlurWeights(float blurLod, QVector4D &blurWeight1, QVector2D &blurWeight2)
{
    float bw1 = blurWeight(std::fabs(blurLod - 0.1f));
    float bw2 = blurWeight(std::fabs(blurLod - 0.3f));
    float bw3 = blurWeight(std::fabs(blurLod - 0.5f));
    float bw4 = blurWeight(std::fabs(blurLod - 0.7f));
    float bw5 = blurWeight(std::fabs(blurLod - 0.9f));
    float bw6 = blurWeight(std::fabs(blurLod - 1.1f));
    float bsum = bw1 + bw2 + bw3 + bw4 + bw5 + bw6;
    blurWeight1 = QVector4D(bw1 / bsum, bw2 / bsum, bw3 / bsum, bw4 / bsum);
    blurWeight2 = QVector2D(bw5 / bsum, bw6 / bsum);
}

void QQuickMultiEffectPrivate::updateBlurWeights()
{
    if (!m_shaderEffect)
        return;
    float blurLod = calculateLod(m_blur);
    getBlurWeights(blurLod, m_blurWeight1, m_blurWeight2);
    m_shaderEffect->setProperty("blurWeight1", m_blurWeight1);
    m_shaderEffect->setProperty("blurWeight2", m_blurWeight2);
}

void QQuickMultiEffectPrivate::updateShadowBlurWeights()
{
    if (!m_shaderEffect)
        return;
    float blurLod = calculateLod(m_shadowBlur);
    getBlurWeights(blurLod, m_shadowBlurWeight1, m_shadowBlurWeight2);
    m_shaderEffect->setProperty("shadowBlurWeight1", m_shadowBlurWeight1);
    m_shaderEffect->setProperty("shadowBlurWeight2", m_shadowBlurWeight2);
}

void QQuickMultiEffectPrivate::updateBlurItemSizes()
{
    Q_Q(QQuickMultiEffect);
    if (m_blurEffects.isEmpty())
        return;
    for (int i = 0; i < m_blurEffects.size(); i++) {
        auto *blurEffect = m_blurEffects[i];
        const QSizeF firstItemSize = QSizeF(std::ceil(q->width() / 64) * 64,
                                            std::ceil(q->height() / 64) * 64);
        QSizeF itemSize = (i == 0) ? firstItemSize : m_blurEffects[i - 1]->size() * 0.5;
        blurEffect->setSize(itemSize);

        const QVector2D offset((1.0 + m_blurMultiplier) / itemSize.width(),
                               (1.0 + m_blurMultiplier) / itemSize.height());
        blurEffect->setProperty("offset", offset);
    }
}

void QQuickMultiEffectPrivate::updateEffectShaders()
{
    Q_Q(QQuickMultiEffect);
    if (!q->isComponentComplete())
        return;

    QString vShader = QStringLiteral("multieffect_c");
    if (m_shadowEnabled)
        vShader += QStringLiteral("s");

    QString fShader = QStringLiteral("multieffect_c");
    if (m_maskEnabled)
        fShader += QStringLiteral("m");
    if (m_blurEnabled && m_blurMax > 0)
        fShader += QStringLiteral("b");
    if (m_shadowEnabled)
        fShader += QStringLiteral("s");

    fShader += QString::number(m_blurLevel);

    bool shaderChanged = false;
    if (fShader != m_fragShader) {
        shaderChanged = true;
        m_fragShader = fShader;
        QUrl fs = QUrl(QStringLiteral("qrc:/data/shaders/%1.frag.qsb").arg(m_fragShader));
        m_shaderEffect->setFragmentShader(fs);
        Q_EMIT q->fragmentShaderChanged();
    }
    if (vShader != m_vertShader) {
        shaderChanged = true;
        m_vertShader = vShader;
        QUrl vs = QUrl(QStringLiteral("qrc:/data/shaders/%1.vert.qsb").arg(m_vertShader));
        m_shaderEffect->setVertexShader(vs);
        Q_EMIT q->vertexShaderChanged();
    }
    if (shaderChanged) {
        qCDebug(lcQuickEffect) << this << "Shaders: " << m_fragShader << m_vertShader;
        Q_EMIT q->shaderChanged();
    }
}

void QQuickMultiEffectPrivate::updateBlurLevel(bool forceUpdate)
{
    int blurLevel = 0;
    if ((m_blurEnabled || m_shadowEnabled) && m_blurMax > 0) {
        if (m_blurMax > 32)
            blurLevel = 3;
        else if (m_blurMax > 16)
            blurLevel = 2;
        else
            blurLevel = 1;
    }

    if (blurLevel != m_blurLevel || (blurLevel > 0 && m_blurEffects.isEmpty()) || forceUpdate) {
        // Blur level has changed or blur items need to be
        // initially created.
        updateBlurItemsAmount(blurLevel);
    }
    m_blurLevel = blurLevel;
}

void QQuickMultiEffectPrivate::updateBlurItemsAmount(int blurLevel)
{
    Q_Q(QQuickMultiEffect);
    if (!m_shaderEffect)
        return;

    // Lowest blur level uses 3 items, highest 5 items.
    int itemsAmount = blurLevel == 0 ? 0 : blurLevel + 2;

    if (m_blurEffects.size() < itemsAmount) {
        // Add more blur items.
        // Note that by design blur items are only added and never reduced
        // during the lifetime of the effect component.
        const auto engine = qmlEngine(q);
        QUrl blurVs = QUrl(QStringLiteral("qrc:/data/shaders/bluritems.vert.qsb"));
        QUrl blurFs = QUrl(QStringLiteral("qrc:/data/shaders/bluritems.frag.qsb"));
        QQmlComponent blurComponent(engine, QUrl(QStringLiteral("qrc:/data/BlurItem.qml")));
        for (int i = m_blurEffects.size(); i < itemsAmount; i++) {
            auto blurEffect = qobject_cast<QQuickShaderEffect*>(blurComponent.create());
            blurEffect->setParent(q);
            blurEffect->setParentItem(q);
            blurEffect->setVisible(false);
            auto sourceVariant = QVariant::fromValue<QQuickItem*>(blurEffect);
            QString sourceProperty = QStringLiteral("blurSrc%1").arg(i + 1);
            m_shaderEffect->setProperty(sourceProperty.toUtf8(), sourceVariant);
            // Initial value to avoid "'source' does not have a matching property" warning.
            // Will be updated with the correct one few lines forward.
            blurEffect->setProperty("source", sourceVariant);
            QQuickItemPrivate *priv = QQuickItemPrivate::get(blurEffect);
            priv->layer()->setEnabled(true);
            priv->layer()->setSmooth(true);
            blurEffect->setVertexShader(blurVs);
            blurEffect->setFragmentShader(blurFs);
            m_blurEffects << blurEffect;
        }
    }

    // Set the blur items source components
    static const auto dummyShaderSource = new QQuickShaderEffectSource(q);
    for (int i = 0; i < m_blurEffects.size(); i++) {
        auto *blurEffect = m_blurEffects[i];
        auto sourceItem = (i >= itemsAmount) ?
                    static_cast<QQuickItem *>(dummyShaderSource) : (i == 0) ?
                        static_cast<QQuickItem *>(m_shaderSource->output()) :
                        static_cast<QQuickItem *>(m_blurEffects[i - 1]);
        auto sourceVariant = QVariant::fromValue<QQuickItem*>(sourceItem);
        blurEffect->setProperty("source", sourceVariant);
    }
}

void QQuickMultiEffectPrivate::updateSourcePadding()
{
    Q_Q(QQuickMultiEffect);
    if (!m_shaderEffect || !m_shaderSource)
        return;

    const bool blurItemsNeeded = (m_blurEnabled || m_shadowEnabled) && (m_blurMax > 0);
    const int itemPadding = m_autoPaddingEnabled && blurItemsNeeded ? 0.8 * m_blurMax * (1.0 + m_blurMultiplier) : 0;

    // Set the shader effect size
    if (m_paddingRect != QRectF() || itemPadding > 0) {
        QRectF effectRect(-m_paddingRect.x() - itemPadding,
                          -m_paddingRect.y() - itemPadding,
                          q->width() + m_paddingRect.x() + m_paddingRect.width() + (itemPadding * 2),
                          q->height() + m_paddingRect.y() + m_paddingRect.height() + (itemPadding * 2));
        m_shaderEffect->setX(effectRect.x());
        m_shaderEffect->setY(effectRect.y());
        m_shaderEffect->setWidth(effectRect.width());
        m_shaderEffect->setHeight(effectRect.height());

        // Set the source size
        m_shaderSource->setSize(m_shaderEffect->size());

        // Set the source rect
        const qreal baseWidth = m_sourceItem ? m_sourceItem->width() : 1;
        const qreal baseHeight = m_sourceItem ? m_sourceItem->height() : 1;
        const qreal widthMultiplier = baseWidth / q->width();
        const qreal heightMultiplier = baseHeight / q->height();
        const qreal xPadding = itemPadding * widthMultiplier;
        const qreal yPadding = itemPadding * heightMultiplier;
        QRectF rect = QRectF(m_paddingRect.x() * widthMultiplier,
                             m_paddingRect.y() * heightMultiplier,
                             m_paddingRect.width() * widthMultiplier,
                             m_paddingRect.height() * heightMultiplier);
        QRectF sourceRect = QRectF(-rect.x() - xPadding,
                                   -rect.y() - yPadding,
                                   baseWidth + rect.x() + rect.width() + xPadding * 2,
                                   baseHeight + rect.y() + rect.height() + yPadding * 2);
        m_shaderSource->setSourceRect(sourceRect);
    } else {
        m_shaderEffect->setX(0);
        m_shaderEffect->setY(0);
        m_shaderEffect->setSize(q->size());
        m_shaderSource->setSize(q->size());
        m_shaderSource->setSourceRect(QRectF());
    }

    updateShadowOffset();
    updateProxyActiveCheck();
    Q_EMIT q->paddingRectChanged();
    Q_EMIT q->itemRectChanged();
    Q_EMIT q->itemSizeChanged();
}

void QQuickMultiEffectPrivate::proxyOutputChanged()
{
    if (!m_shaderSource)
        return;

    auto sourceVariant = QVariant::fromValue<QQuickItem*>(m_shaderSource->output());
    m_shaderEffect->setProperty("src", sourceVariant);

    // Force updating the blur items since the source output has changed
    updateBlurLevel(true);
    updateBlurItemSizes();
    updateSourcePadding();
}

void QQuickMultiEffectPrivate::updateProxyActiveCheck()
{
    if (!m_shaderSource)
        return;

    m_shaderSource->polish();
}

QT_END_NAMESPACE

#include "moc_qquickmultieffect_p.cpp"