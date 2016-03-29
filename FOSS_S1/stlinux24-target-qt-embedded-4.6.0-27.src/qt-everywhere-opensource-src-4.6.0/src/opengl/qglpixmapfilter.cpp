/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "private/qpixmapfilter_p.h"
#include "private/qpixmapdata_gl_p.h"
#include "private/qpaintengineex_opengl2_p.h"
#include "private/qglengineshadermanager_p.h"
#include "private/qpixmapdata_p.h"
#include "private/qimagepixmapcleanuphooks_p.h"
#include "qglpixmapfilter_p.h"
#include "qgraphicssystem_gl_p.h"
#include "qpaintengine_opengl_p.h"
#include "qcache.h"

#include "qglframebufferobject.h"
#include "qglshaderprogram.h"
#include "qgl_p.h"

#include "private/qapplication_p.h"
#include "private/qmath_p.h"
#include "qmath.h"

QT_BEGIN_NAMESPACE

void QGLPixmapFilterBase::bindTexture(const QPixmap &src) const
{
    const_cast<QGLContext *>(QGLContext::currentContext())->d_func()->bindTexture(src, GL_TEXTURE_2D, GL_RGBA, QGLContext::BindOptions(QGLContext::DefaultBindOption | QGLContext::MemoryManagedBindOption));
}

void QGLPixmapFilterBase::drawImpl(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF& source) const
{
    processGL(painter, pos, src, source);
}

class QGLPixmapColorizeFilter: public QGLCustomShaderStage, public QGLPixmapFilter<QPixmapColorizeFilter>
{
public:
    QGLPixmapColorizeFilter();

    void setUniforms(QGLShaderProgram *program);

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &pixmap, const QRectF &srcRect) const;
};

class QGLPixmapConvolutionFilter: public QGLCustomShaderStage, public QGLPixmapFilter<QPixmapConvolutionFilter>
{
public:
    QGLPixmapConvolutionFilter();
    ~QGLPixmapConvolutionFilter();

    void setUniforms(QGLShaderProgram *program);

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const;

private:
    QByteArray generateConvolutionShader() const;

    mutable QSize m_srcSize;
    mutable int m_prevKernelSize;
};

class QGLPixmapBlurFilter : public QGLCustomShaderStage, public QGLPixmapFilter<QPixmapBlurFilter>
{
public:
    QGLPixmapBlurFilter(QGraphicsBlurEffect::BlurHints hints);

    void setUniforms(QGLShaderProgram *program);

    static QByteArray generateGaussianShader(int radius, bool singlePass = false, bool dropShadow = false);

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const;

private:

    mutable QSize m_textureSize;
    mutable bool m_horizontalBlur;
    mutable bool m_singlePass;
    mutable bool m_animatedBlur;

    mutable qreal m_t;
    mutable QSize m_targetSize;

    mutable bool m_haveCached;
    mutable int m_cachedRadius;
    mutable QGraphicsBlurEffect::BlurHints m_hints;
};

class QGLPixmapDropShadowFilter : public QGLCustomShaderStage, public QGLPixmapFilter<QPixmapDropShadowFilter>
{
public:
    QGLPixmapDropShadowFilter(QGraphicsBlurEffect::BlurHints hints);

    void setUniforms(QGLShaderProgram *program);

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const;

private:
    mutable QSize m_textureSize;
    mutable bool m_horizontalBlur;
    mutable bool m_singlePass;

    mutable bool m_haveCached;
    mutable int m_cachedRadius;
    mutable QGraphicsBlurEffect::BlurHints m_hints;
};

extern QGLWidget *qt_gl_share_widget();

QPixmapFilter *QGL2PaintEngineEx::pixmapFilter(int type, const QPixmapFilter *prototype)
{
    Q_D(QGL2PaintEngineEx);
    switch (type) {
    case QPixmapFilter::ColorizeFilter:
        if (!d->colorizeFilter)
            d->colorizeFilter.reset(new QGLPixmapColorizeFilter);
        return d->colorizeFilter.data();

    case QPixmapFilter::BlurFilter: {
        const QPixmapBlurFilter *proto = static_cast<const QPixmapBlurFilter *>(prototype);
        if (proto->blurHints() & QGraphicsBlurEffect::AnimationHint) {
            if (!d->animationBlurFilter)
                d->animationBlurFilter.reset(new QGLPixmapBlurFilter(proto->blurHints()));
            return d->animationBlurFilter.data();
        }
        if ((proto->blurHints() & QGraphicsBlurEffect::QualityHint) && proto->radius() > 5) {
            if (!d->blurFilter)
                d->blurFilter.reset(new QGLPixmapBlurFilter(QGraphicsBlurEffect::QualityHint));
            return d->blurFilter.data();
        }
        if (!d->fastBlurFilter)
            d->fastBlurFilter.reset(new QGLPixmapBlurFilter(QGraphicsBlurEffect::PerformanceHint));
        return d->fastBlurFilter.data();
        }

    case QPixmapFilter::DropShadowFilter: {
        const QPixmapDropShadowFilter *proto = static_cast<const QPixmapDropShadowFilter *>(prototype);
        if (proto->blurRadius() <= 5) {
            if (!d->fastDropShadowFilter)
                d->fastDropShadowFilter.reset(new QGLPixmapDropShadowFilter(QGraphicsBlurEffect::PerformanceHint));
            return d->fastDropShadowFilter.data();
        }
        if (!d->dropShadowFilter)
            d->dropShadowFilter.reset(new QGLPixmapDropShadowFilter(QGraphicsBlurEffect::QualityHint));
        return d->dropShadowFilter.data();
        }

    case QPixmapFilter::ConvolutionFilter:
        if (!d->convolutionFilter)
            d->convolutionFilter.reset(new QGLPixmapConvolutionFilter);
        return d->convolutionFilter.data();

    default: break;
    }
    return QPaintEngineEx::pixmapFilter(type, prototype);
}

static const char *qt_gl_colorize_filter =
        "uniform lowp vec4 colorizeColor;"
        "uniform lowp float colorizeStrength;"
        "lowp vec4 customShader(lowp sampler2D src, highp vec2 srcCoords)"
        "{"
        "        lowp vec4 srcPixel = texture2D(src, srcCoords);"
        "        lowp float gray = dot(srcPixel.rgb, vec3(0.212671, 0.715160, 0.072169));"
        "        lowp vec3 colorized = 1.0-((1.0-gray)*(1.0-colorizeColor.rgb));"
        "        return vec4(mix(srcPixel.rgb, colorized * srcPixel.a, colorizeStrength), srcPixel.a);"
        "}";

QGLPixmapColorizeFilter::QGLPixmapColorizeFilter()
{
    setSource(qt_gl_colorize_filter);
}

bool QGLPixmapColorizeFilter::processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &) const
{
    QGLPixmapColorizeFilter *filter = const_cast<QGLPixmapColorizeFilter *>(this);

    filter->setOnPainter(painter);
    painter->drawPixmap(pos, src);
    filter->removeFromPainter(painter);

    return true;
}

void QGLPixmapColorizeFilter::setUniforms(QGLShaderProgram *program)
{
    program->setUniformValue("colorizeColor", color());
    program->setUniformValue("colorizeStrength", float(strength()));
}

void QGLPixmapConvolutionFilter::setUniforms(QGLShaderProgram *program)
{
    const qreal *kernel = convolutionKernel();
    int kernelWidth = columns();
    int kernelHeight = rows();
    int kernelSize = kernelWidth * kernelHeight;

    QVarLengthArray<GLfloat> matrix(kernelSize);
    QVarLengthArray<GLfloat> offset(kernelSize * 2);

    for(int i = 0; i < kernelSize; ++i)
        matrix[i] = kernel[i];

    for(int y = 0; y < kernelHeight; ++y) {
        for(int x = 0; x < kernelWidth; ++x) {
            offset[(y * kernelWidth + x) * 2] = x - (kernelWidth / 2);
            offset[(y * kernelWidth + x) * 2 + 1] = (kernelHeight / 2) - y;
        }
    }

    const qreal iw = 1.0 / m_srcSize.width();
    const qreal ih = 1.0 / m_srcSize.height();
    program->setUniformValue("inv_texture_size", iw, ih);
    program->setUniformValueArray("matrix", matrix.constData(), kernelSize, 1);
    program->setUniformValueArray("offset", offset.constData(), kernelSize, 2);
}

// generates convolution filter code for arbitrary sized kernel
QByteArray QGLPixmapConvolutionFilter::generateConvolutionShader() const {
    QByteArray code;
    int kernelWidth = columns();
    int kernelHeight = rows();
    int kernelSize = kernelWidth * kernelHeight;
    code.append("uniform highp vec2 inv_texture_size;\n"
                "uniform mediump float matrix[");
    code.append(QByteArray::number(kernelSize));
    code.append("];\n"
                "uniform highp vec2 offset[");
    code.append(QByteArray::number(kernelSize));
    code.append("];\n");
    code.append("lowp vec4 customShader(lowp sampler2D src, highp vec2 srcCoords) {\n");

    code.append("  int i = 0;\n"
                "  lowp vec4 sum = vec4(0.0);\n"
                "  for (i = 0; i < ");
    code.append(QByteArray::number(kernelSize));
    code.append("; i++) {\n"
                "    sum += matrix[i] * texture2D(src,srcCoords+inv_texture_size*offset[i]);\n"
                "  }\n"
                "  return sum;\n"
                "}");
    return code;
}

QGLPixmapConvolutionFilter::QGLPixmapConvolutionFilter()
    : m_prevKernelSize(-1)
{
}

QGLPixmapConvolutionFilter::~QGLPixmapConvolutionFilter()
{
}

bool QGLPixmapConvolutionFilter::processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const
{
    QGLPixmapConvolutionFilter *filter = const_cast<QGLPixmapConvolutionFilter *>(this);

    m_srcSize = src.size();

    int kernelSize = rows() * columns();
    if (m_prevKernelSize == -1 || m_prevKernelSize != kernelSize) {
        filter->setSource(generateConvolutionShader());
        m_prevKernelSize = kernelSize;
    }

    filter->setOnPainter(painter);
    painter->drawPixmap(pos, src, srcRect);
    filter->removeFromPainter(painter);

    return true;
}

static const char *qt_gl_texture_sampling_helper =
    "lowp float texture2DAlpha(lowp sampler2D src, highp vec2 srcCoords) {\n"
    "   return texture2D(src, srcCoords).a;\n"
    "}\n";

QGLPixmapBlurFilter::QGLPixmapBlurFilter(QGraphicsBlurEffect::BlurHints hints)
    : m_animatedBlur(false)
    , m_haveCached(false)
    , m_cachedRadius(0)
    , m_hints(hints)
{
}

// should be even numbers as they will be divided by two
static const int qCachedBlurLevels[] = { 6, 14, 30 };
static const int qNumCachedBlurTextures = sizeof(qCachedBlurLevels) / sizeof(*qCachedBlurLevels);
static const int qMaxCachedBlurLevel = qCachedBlurLevels[qNumCachedBlurTextures - 1];

static qreal qLogBlurLevel(int level)
{
    static bool initialized = false;
    static qreal logBlurLevelCache[qNumCachedBlurTextures];
    if (!initialized) {
        for (int i = 0; i < qNumCachedBlurTextures; ++i)
            logBlurLevelCache[i] = qLn(qCachedBlurLevels[i]);
        initialized = true;
    }
    return logBlurLevelCache[level];
}

class QGLBlurTextureInfo
{
public:
    QGLBlurTextureInfo(QSize size, GLuint textureIds[])
        : m_size(size)
    {
        for (int i = 0; i < qNumCachedBlurTextures; ++i)
            m_textureIds[i] = textureIds[i];
    }

    ~QGLBlurTextureInfo()
    {
        glDeleteTextures(qNumCachedBlurTextures, m_textureIds);
    }

    QSize size() const { return m_size; }
    GLuint textureId(int i) const { return m_textureIds[i]; }

private:
    GLuint m_textureIds[qNumCachedBlurTextures];
    QSize m_size;
};

class QGLBlurTextureCache : public QObject
{
public:
    static QGLBlurTextureCache *cacheForContext(const QGLContext *context);

    QGLBlurTextureCache();
    ~QGLBlurTextureCache();

    QGLBlurTextureInfo *takeBlurTextureInfo(const QPixmap &pixmap);
    bool fitsInCache(const QPixmap &pixmap) const;
    bool hasBlurTextureInfo(const QPixmap &pixmap) const;
    void insertBlurTextureInfo(const QPixmap &pixmap, QGLBlurTextureInfo *info);
    void clearBlurTextureInfo(const QPixmap &pixmap);

    void timerEvent(QTimerEvent *event);

private:
    static void pixmapDestroyed(QPixmap *pixmap);

    QCache<quint64, QGLBlurTextureInfo > cache;

    static QList<QGLBlurTextureCache *> blurTextureCaches;

    int timerId;
};

QList<QGLBlurTextureCache *> QGLBlurTextureCache::blurTextureCaches;

static void QGLBlurTextureCache_free(void *ptr)
{
    delete reinterpret_cast<QGLBlurTextureCache *>(ptr);
}

Q_GLOBAL_STATIC_WITH_ARGS(QGLContextResource, qt_blur_texture_caches, (QGLBlurTextureCache_free))

QGLBlurTextureCache::QGLBlurTextureCache()
    : timerId(0)
{
    cache.setMaxCost(4 * 1024 * 1024);
    blurTextureCaches.append(this);
}

QGLBlurTextureCache::~QGLBlurTextureCache()
{
    blurTextureCaches.removeAt(blurTextureCaches.indexOf(this));
}

void QGLBlurTextureCache::timerEvent(QTimerEvent *)
{
    killTimer(timerId);
    timerId = 0;

    cache.clear();
}

QGLBlurTextureCache *QGLBlurTextureCache::cacheForContext(const QGLContext *context)
{
    QGLBlurTextureCache *p = reinterpret_cast<QGLBlurTextureCache *>(qt_blur_texture_caches()->value(context));
    if (!p) {
        p = new QGLBlurTextureCache;
        qt_blur_texture_caches()->insert(context, p);
    }
    return p;
}

QGLBlurTextureInfo *QGLBlurTextureCache::takeBlurTextureInfo(const QPixmap &pixmap)
{
    return cache.take(pixmap.cacheKey());
}

void QGLBlurTextureCache::clearBlurTextureInfo(const QPixmap &pixmap)
{
    cache.remove(pixmap.cacheKey());
}

bool QGLBlurTextureCache::hasBlurTextureInfo(const QPixmap &pixmap) const
{
    return cache.contains(pixmap.cacheKey());
}

void QGLBlurTextureCache::insertBlurTextureInfo(const QPixmap &pixmap, QGLBlurTextureInfo *info)
{
    static bool hookAdded = false;
    if (!hookAdded) {
        QImagePixmapCleanupHooks::instance()->addPixmapDestructionHook(pixmapDestroyed);
        hookAdded = true;
    }

    QImagePixmapCleanupHooks::enableCleanupHooks(pixmap);
    cache.insert(pixmap.cacheKey(), info, pixmap.width() * pixmap.height());

    if (timerId)
        killTimer(timerId);

    timerId = startTimer(1000);
}

bool QGLBlurTextureCache::fitsInCache(const QPixmap &pixmap) const
{
    return pixmap.width() * pixmap.height() <= cache.maxCost();
}

void QGLBlurTextureCache::pixmapDestroyed(QPixmap *pixmap)
{
    foreach (QGLBlurTextureCache *cache, blurTextureCaches) {
        if (cache->hasBlurTextureInfo(*pixmap))
            cache->clearBlurTextureInfo(*pixmap);
    }
}

static const char *qt_gl_interpolate_filter =
        "uniform lowp float interpolationValue;"
        "uniform lowp sampler2D interpolateTarget;"
        "uniform highp vec4 interpolateMapping;"
        "lowp vec4 customShader(lowp sampler2D src, highp vec2 srcCoords)"
        "{"
        "    return mix(texture2D(interpolateTarget, interpolateMapping.xy + interpolateMapping.zw * srcCoords),"
        "               texture2D(src, srcCoords), interpolationValue);"
        "}";

static void initializeTexture(GLuint id, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool QGLPixmapBlurFilter::processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &) const
{
    QGLPixmapBlurFilter *filter = const_cast<QGLPixmapBlurFilter *>(this);

    QGLContext *ctx = const_cast<QGLContext *>(QGLContext::currentContext());
    QGLBlurTextureCache *blurTextureCache = QGLBlurTextureCache::cacheForContext(ctx);

    if ((m_hints & QGraphicsBlurEffect::AnimationHint) && blurTextureCache->fitsInCache(src)) {
        QRect targetRect = src.rect().adjusted(-qMaxCachedBlurLevel, -qMaxCachedBlurLevel, qMaxCachedBlurLevel, qMaxCachedBlurLevel);
        // ensure even dimensions (going to divide by two)
        targetRect.setWidth((targetRect.width() + 1) & ~1);
        targetRect.setHeight((targetRect.height() + 1) & ~1);

        QGLBlurTextureInfo *info = 0;
        if (blurTextureCache->hasBlurTextureInfo(src)) {
            info = blurTextureCache->takeBlurTextureInfo(src);
        } else {
            m_animatedBlur = false;
            m_hints = QGraphicsBlurEffect::QualityHint;
            m_singlePass = false;

            QGLFramebufferObjectFormat format;
            format.setInternalTextureFormat(GLenum(GL_RGBA));
            QGLFramebufferObject *fbo = qgl_fbo_pool()->acquire(targetRect.size() / 2, format, true);

            if (!fbo)
                return false;

            QPainter fboPainter(fbo);
            QGL2PaintEngineEx *engine = static_cast<QGL2PaintEngineEx *>(fboPainter.paintEngine());

            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            // ensure GL_LINEAR filtering is used for scaling down to half the size
            fboPainter.setRenderHint(QPainter::SmoothPixmapTransform);
            fboPainter.setCompositionMode(QPainter::CompositionMode_Source);
            fboPainter.drawPixmap(qMaxCachedBlurLevel / 2, qMaxCachedBlurLevel / 2,
                    targetRect.width() / 2 - qMaxCachedBlurLevel, targetRect.height() / 2 - qMaxCachedBlurLevel, src);

            GLuint textures[qNumCachedBlurTextures]; // blur textures
            glGenTextures(qNumCachedBlurTextures, textures);
            GLuint temp; // temp texture
            glGenTextures(1, &temp);

            initializeTexture(temp, fbo->width(), fbo->height());
            m_textureSize = fbo->size();

            int currentBlur = 0;

            QRect fboRect(0, 0, fbo->width(), fbo->height());
            GLuint sourceTexture = fbo->texture();
            for (int i = 0; i < qNumCachedBlurTextures; ++i) {
                int targetBlur = qCachedBlurLevels[i] / 2;

                int blurDelta = qRound(qSqrt(targetBlur * targetBlur - currentBlur * currentBlur));
                QByteArray source = generateGaussianShader(blurDelta);
                filter->setSource(source);

                currentBlur = targetBlur;

                // now we're going to be nasty and keep using the same FBO with different textures
                glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                        GL_TEXTURE_2D, temp, 0);

                m_horizontalBlur = true;
                filter->setOnPainter(&fboPainter);
                engine->drawTexture(fboRect, sourceTexture, fbo->size(), fboRect);
                filter->removeFromPainter(&fboPainter);

                sourceTexture = textures[i];
                initializeTexture(sourceTexture, fbo->width(), fbo->height());

                glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                        GL_TEXTURE_2D, textures[i], 0);

                m_horizontalBlur = false;
                filter->setOnPainter(&fboPainter);
                engine->drawTexture(fboRect, temp, fbo->size(), fboRect);
                filter->removeFromPainter(&fboPainter);
            }

            glDeleteTextures(1, &temp);

            // reattach the original FBO texture
            glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                    GL_TEXTURE_2D, fbo->texture(), 0);

            fboPainter.end();

            qgl_fbo_pool()->release(fbo);

            info = new QGLBlurTextureInfo(fboRect.size(), textures);
        }

        if (!m_haveCached || !m_animatedBlur) {
            m_haveCached = true;
            m_animatedBlur = true;
            m_hints = QGraphicsBlurEffect::AnimationHint;
            filter->setSource(qt_gl_interpolate_filter);
        }

        QGL2PaintEngineEx *engine = static_cast<QGL2PaintEngineEx *>(painter->paintEngine());
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        filter->setOnPainter(painter);

        qreal logRadius = qLn(radius());

        int t;
        for (t = -1; t < qNumCachedBlurTextures - 2; ++t) {
            if (logRadius < qLogBlurLevel(t+1))
                break;
        }

        qreal logBase = t >= 0 ? qLogBlurLevel(t) : 0;
        m_t = qBound(qreal(0), (logRadius - logBase) / (qLogBlurLevel(t+1) - logBase), qreal(1));

        m_textureSize = info->size();

        glActiveTexture(GL_TEXTURE0 + 3);
        if (t >= 0) {
            glBindTexture(GL_TEXTURE_2D, info->textureId(t));
            m_targetSize = info->size();
        } else {
            QGLTexture *texture =
                ctx->d_func()->bindTexture(src, GL_TEXTURE_2D, GL_RGBA,
                                           QGLContext::InternalBindOption
                                           | QGLContext::CanFlipNativePixmapBindOption);
            m_targetSize = src.size();
            if (!(texture->options & QGLContext::InvertedYBindOption))
                m_targetSize.setHeight(-m_targetSize.height());
        }

        // restrict the target rect to the max of the radii we are interpolating between
        int radiusDelta = qMaxCachedBlurLevel - qCachedBlurLevels[t+1];
        targetRect = targetRect.translated(pos.toPoint()).adjusted(radiusDelta, radiusDelta, -radiusDelta, -radiusDelta);

        radiusDelta /= 2;
        QRect sourceRect = QRect(QPoint(), m_textureSize).adjusted(radiusDelta, radiusDelta, -radiusDelta, -radiusDelta);

        engine->drawTexture(targetRect, info->textureId(t+1), m_textureSize, sourceRect);

        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, 0);

        filter->removeFromPainter(painter);
        blurTextureCache->insertBlurTextureInfo(src, info);

        return true;
    }

    if (blurTextureCache->hasBlurTextureInfo(src))
        blurTextureCache->clearBlurTextureInfo(src);

    int actualRadius = qRound(radius());
    int filterRadius = actualRadius;
    int fastRadii[] = { 1, 2, 3, 5, 8, 15, 25 };
    if (!(m_hints & QGraphicsBlurEffect::QualityHint)) {
        uint i = 0;
        for (; i < (sizeof(fastRadii)/sizeof(*fastRadii))-1; ++i) {
            if (fastRadii[i+1] > filterRadius)
                break;
        }
        filterRadius = fastRadii[i];
    }

    m_singlePass = filterRadius <= 3;

    if (!m_haveCached || m_animatedBlur || filterRadius != m_cachedRadius) {
        // Only regenerate the shader from source if parameters have changed.
        m_haveCached = true;
        m_animatedBlur = false;
        m_cachedRadius = filterRadius;
        QByteArray source = generateGaussianShader(filterRadius, m_singlePass);
        filter->setSource(source);
    }

    QRect targetRect = QRectF(src.rect()).translated(pos).adjusted(-actualRadius, -actualRadius, actualRadius, actualRadius).toAlignedRect();

    if (m_singlePass) {
        // prepare for updateUniforms
        m_textureSize = src.size();

        // ensure GL_LINEAR filtering is used
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        filter->setOnPainter(painter);
        QBrush pixmapBrush = src;
        pixmapBrush.setTransform(QTransform::fromTranslate(pos.x(), pos.y()));
        painter->fillRect(targetRect, pixmapBrush);
        filter->removeFromPainter(painter);
    } else {
        QGLFramebufferObjectFormat format;
        format.setInternalTextureFormat(GLenum(src.hasAlphaChannel() ? GL_RGBA : GL_RGB));
        QGLFramebufferObject *fbo = qgl_fbo_pool()->acquire(targetRect.size(), format);

        if (!fbo)
            return false;

        // prepare for updateUniforms
        m_textureSize = src.size();

        // horizontal pass, to pixmap
        m_horizontalBlur = true;

        QPainter fboPainter(fbo);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        // ensure GL_LINEAR filtering is used
        fboPainter.setRenderHint(QPainter::SmoothPixmapTransform);
        fboPainter.setCompositionMode(QPainter::CompositionMode_Source);
        filter->setOnPainter(&fboPainter);
        QBrush pixmapBrush = src;
        pixmapBrush.setTransform(QTransform::fromTranslate(actualRadius, actualRadius));
        fboPainter.fillRect(QRect(0, 0, targetRect.width(), targetRect.height()), pixmapBrush);
        filter->removeFromPainter(&fboPainter);
        fboPainter.end();

        QGL2PaintEngineEx *engine = static_cast<QGL2PaintEngineEx *>(painter->paintEngine());

        // vertical pass, to painter
        m_horizontalBlur = false;
        m_textureSize = fbo->size();

        painter->save();
        // ensure GL_LINEAR filtering is used
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        filter->setOnPainter(painter);
        engine->drawTexture(targetRect, fbo->texture(), fbo->size(), QRect(QPoint(), targetRect.size()).translated(0, fbo->height() - targetRect.height()));
        filter->removeFromPainter(painter);
        painter->restore();

        qgl_fbo_pool()->release(fbo);
    }

    return true;
}

void QGLPixmapBlurFilter::setUniforms(QGLShaderProgram *program)
{
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (m_animatedBlur) {
        program->setUniformValue("interpolateTarget", 3);
        program->setUniformValue("interpolationValue", GLfloat(m_t));

        if (m_textureSize == m_targetSize) {
            program->setUniformValue("interpolateMapping", 0.0f, 0.0f, 1.0f, 1.0f);
        } else {
            float offsetX = (-qMaxCachedBlurLevel - 0.5) / qreal(m_targetSize.width());
            float offsetY = (-qMaxCachedBlurLevel - 0.5) / qreal(m_targetSize.height());

            if (m_targetSize.height() < 0)
                offsetY = 1 + offsetY;

            float scaleX = 2.0f * qreal(m_textureSize.width()) / qreal(m_targetSize.width());
            float scaleY = 2.0f * qreal(m_textureSize.height()) / qreal(m_targetSize.height());

            program->setUniformValue("interpolateMapping", offsetX, offsetY, scaleX, scaleY);
        }

        return;
    }

    if (m_hints & QGraphicsBlurEffect::QualityHint) {
        if (m_singlePass)
            program->setUniformValue("delta", 1.0 / m_textureSize.width(), 1.0 / m_textureSize.height());
        else if (m_horizontalBlur)
            program->setUniformValue("delta", 1.0 / m_textureSize.width(), 0.0);
        else
            program->setUniformValue("delta", 0.0, 1.0 / m_textureSize.height());
    } else {
        qreal blur = radius() / qreal(m_cachedRadius);

        if (m_singlePass)
            program->setUniformValue("delta", blur / m_textureSize.width(), blur / m_textureSize.height());
        else if (m_horizontalBlur)
            program->setUniformValue("delta", blur / m_textureSize.width(), 0.0);
        else
            program->setUniformValue("delta", 0.0, blur / m_textureSize.height());
    }
}

static inline qreal gaussian(qreal dx, qreal sigma)
{
    return exp(-dx * dx / (2 * sigma * sigma)) / (Q_2PI * sigma * sigma);
}

QByteArray QGLPixmapBlurFilter::generateGaussianShader(int radius, bool singlePass, bool dropShadow)
{
    Q_ASSERT(radius >= 1);

    radius = qMin(127, radius);

    static QCache<uint, QByteArray> shaderSourceCache;
    uint key = radius | (int(singlePass) << 7) | (int(dropShadow) << 8);
    QByteArray *cached = shaderSourceCache.object(key);
    if (cached)
        return *cached;

    QByteArray source;
    source.reserve(1000);
    source.append(qt_gl_texture_sampling_helper);

    source.append("uniform highp vec2      delta;\n");
    if (dropShadow)
        source.append("uniform mediump vec4    shadowColor;\n");
    source.append("lowp vec4 customShader(lowp sampler2D src, highp vec2 srcCoords) {\n");

    QVector<qreal> sampleOffsets;
    QVector<qreal> weights;

    QVector<qreal> gaussianComponents;

    qreal sigma = radius / 1.65;

    qreal sum = 0;
    for (int i = -radius; i < radius; ++i) {
        float value = gaussian(i, sigma);
        gaussianComponents << value;
        sum += value;
    }

    // normalize
    for (int i = 0; i < gaussianComponents.size(); ++i)
        gaussianComponents[i] /= sum;

    for (int i = 0; i < gaussianComponents.size() - 1; i += 2) {
        qreal weight = gaussianComponents.at(i) + gaussianComponents.at(i + 1);
        qreal offset = i - radius + gaussianComponents.at(i + 1) / weight;

        sampleOffsets << offset;
        weights << weight;
    }

    int limit = sampleOffsets.size();
    if (singlePass)
        limit *= limit;

    QByteArray baseCoordinate = "srcCoords";

    for (int i = 0; i < limit; ++i) {
        QByteArray coordinate = baseCoordinate;

        qreal weight;
        if (singlePass) {
            const int xIndex = i % sampleOffsets.size();
            const int yIndex = i / sampleOffsets.size();

            const qreal deltaX = sampleOffsets.at(xIndex);
            const qreal deltaY = sampleOffsets.at(yIndex);
            weight = weights.at(xIndex) * weights.at(yIndex);

            if (!qFuzzyCompare(deltaX, deltaY)) {
                coordinate.append(" + vec2(delta.x * float(");
                coordinate.append(QByteArray::number(deltaX));
                coordinate.append("), delta.y * float(");
                coordinate.append(QByteArray::number(deltaY));
                coordinate.append("))");
            } else if (!qFuzzyIsNull(deltaX)) {
                coordinate.append(" + delta * float(");
                coordinate.append(QByteArray::number(deltaX));
                coordinate.append(")");
            }
        } else {
            const qreal delta = sampleOffsets.at(i);
            weight = weights.at(i);
            if (!qFuzzyIsNull(delta)) {
                coordinate.append(" + delta * float(");
                coordinate.append(QByteArray::number(delta));
                coordinate.append(")");
            }
        }

        if (i == 0) {
            if (dropShadow)
                source.append("    mediump float sample = ");
            else
                source.append("    mediump vec4 sample = ");
        } else {
            if (dropShadow)
                source.append("    sample += ");
            else
                source.append("    sample += ");
        }

        source.append("texture2D(src, ");
        source.append(coordinate);
        source.append(")");

        if (dropShadow)
            source.append(".a");

        if (!qFuzzyCompare(weight, qreal(1))) {
            source.append(" * float(");
            source.append(QByteArray::number(weight));
            source.append(");\n");
        } else {
            source.append(";\n");
        }
    }

    source.append("    return ");
    if (dropShadow)
        source.append("shadowColor * ");
    source.append("sample;\n");
    source.append("}\n");

    cached = new QByteArray(source);
    shaderSourceCache.insert(key, cached);

    return source;
}

QGLPixmapDropShadowFilter::QGLPixmapDropShadowFilter(QGraphicsBlurEffect::BlurHints hints)
    : m_haveCached(false)
    , m_cachedRadius(0)
    , m_hints(hints)
{
}

bool QGLPixmapDropShadowFilter::processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const
{
    QGLPixmapDropShadowFilter *filter = const_cast<QGLPixmapDropShadowFilter *>(this);

    int actualRadius = qRound(blurRadius());
    int filterRadius = actualRadius;
    m_singlePass = filterRadius <= 3;

    if (!m_haveCached || filterRadius != m_cachedRadius) {
        // Only regenerate the shader from source if parameters have changed.
        m_haveCached = true;
        m_cachedRadius = filterRadius;
        QByteArray source = QGLPixmapBlurFilter::generateGaussianShader(filterRadius, m_singlePass, true);
        filter->setSource(source);
    }

    QRect targetRect = QRectF(src.rect()).translated(pos + offset()).adjusted(-actualRadius, -actualRadius, actualRadius, actualRadius).toAlignedRect();

    if (m_singlePass) {
        // prepare for updateUniforms
        m_textureSize = src.size();

        painter->save();
        // ensure GL_LINEAR filtering is used
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        filter->setOnPainter(painter);
        QBrush pixmapBrush = src;
        pixmapBrush.setTransform(QTransform::fromTranslate(pos.x() + offset().x(), pos.y() + offset().y()));
        painter->fillRect(targetRect, pixmapBrush);
        filter->removeFromPainter(painter);
        painter->restore();
    } else {
        QGLFramebufferObjectFormat format;
        format.setInternalTextureFormat(GLenum(src.hasAlphaChannel() ? GL_RGBA : GL_RGB));
        QGLFramebufferObject *fbo = qgl_fbo_pool()->acquire(targetRect.size(), format);

        if (!fbo)
            return false;

        // prepare for updateUniforms
        m_textureSize = src.size();

        // horizontal pass, to pixmap
        m_horizontalBlur = true;

        QPainter fboPainter(fbo);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        // ensure GL_LINEAR filtering is used
        fboPainter.setRenderHint(QPainter::SmoothPixmapTransform);
        fboPainter.setCompositionMode(QPainter::CompositionMode_Source);
        filter->setOnPainter(&fboPainter);
        QBrush pixmapBrush = src;
        pixmapBrush.setTransform(QTransform::fromTranslate(actualRadius, actualRadius));
        fboPainter.fillRect(QRect(0, 0, targetRect.width(), targetRect.height()), pixmapBrush);
        filter->removeFromPainter(&fboPainter);
        fboPainter.end();

        QGL2PaintEngineEx *engine = static_cast<QGL2PaintEngineEx *>(painter->paintEngine());

        // vertical pass, to painter
        m_horizontalBlur = false;
        m_textureSize = fbo->size();

        painter->save();
        // ensure GL_LINEAR filtering is used
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        filter->setOnPainter(painter);
        engine->drawTexture(targetRect, fbo->texture(), fbo->size(), QRectF(0, fbo->height() - targetRect.height(), targetRect.width(), targetRect.height()));
        filter->removeFromPainter(painter);
        painter->restore();

        qgl_fbo_pool()->release(fbo);
    }

    // Now draw the actual pixmap over the top.
    painter->drawPixmap(pos, src, srcRect);

    return true;
}

void QGLPixmapDropShadowFilter::setUniforms(QGLShaderProgram *program)
{
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    QColor col = color();
    if (m_horizontalBlur && !m_singlePass) {
        program->setUniformValue("shadowColor", 1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        qreal alpha = col.alphaF();
        program->setUniformValue("shadowColor", col.redF() * alpha,
                                                col.greenF() * alpha,
                                                col.blueF() * alpha,
                                                alpha);
    }

    if (m_hints & QGraphicsBlurEffect::QualityHint) {
        if (m_singlePass)
            program->setUniformValue("delta", 1.0 / m_textureSize.width(), 1.0 / m_textureSize.height());
        else if (m_horizontalBlur)
            program->setUniformValue("delta", 1.0 / m_textureSize.width(), 0.0);
        else
            program->setUniformValue("delta", 0.0, 1.0 / m_textureSize.height());
    } else {
        qreal blur = blurRadius() / qreal(m_cachedRadius);

        if (m_singlePass)
            program->setUniformValue("delta", blur / m_textureSize.width(), blur / m_textureSize.height());
        else if (m_horizontalBlur)
            program->setUniformValue("delta", blur / m_textureSize.width(), 0.0);
        else
            program->setUniformValue("delta", 0.0, blur / m_textureSize.height());
    }
}

QT_END_NAMESPACE
