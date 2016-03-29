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

#ifndef QGL_P_H
#define QGL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QGLWidget class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtOpenGL/qgl.h"
#include "QtOpenGL/qglcolormap.h"
#include "QtCore/qmap.h"
#include "QtCore/qthread.h"
#include "QtCore/qthreadstorage.h"
#include "QtCore/qhash.h"
#include "QtCore/qatomic.h"
#include "private/qwidget_p.h"
#include "qcache.h"
#include "qglpaintdevice_p.h"

#ifndef QT_OPENGL_ES_1_CL
#define q_vertexType float
#define q_vertexTypeEnum GL_FLOAT
#define f2vt(f)     (f)
#define vt2f(x)     (x)
#define i2vt(i)     (float(i))
#else
#define FLOAT2X(f)      (int( (f) * (65536)))
#define X2FLOAT(x)      (float(x) / 65536.0f)
#define f2vt(f)     FLOAT2X(f)
#define i2vt(i)     ((i)*65536)
#define vt2f(x)     X2FLOAT(x)
#define q_vertexType GLfixed
#define q_vertexTypeEnum GL_FIXED
#endif //QT_OPENGL_ES_1_CL

#ifdef QT_OPENGL_ES
QT_BEGIN_INCLUDE_NAMESPACE
#if defined(QT_OPENGL_ES_2)
#include <EGL/egl.h>
#else
#include <GLES/egl.h>
#endif
QT_END_INCLUDE_NAMESPACE
#endif

QT_BEGIN_NAMESPACE

class QGLContext;
class QGLOverlayWidget;
class QPixmap;
class QPixmapFilter;
#ifdef Q_WS_MAC
# ifdef qDebug
#   define old_qDebug qDebug
#   undef qDebug
# endif
QT_BEGIN_INCLUDE_NAMESPACE
#ifndef QT_MAC_USE_COCOA
# include <AGL/agl.h>
#endif
QT_END_INCLUDE_NAMESPACE
# ifdef old_qDebug
#   undef qDebug
#   define qDebug QT_QDEBUG_MACRO
#   undef old_qDebug
# endif
class QMacWindowChangeEvent;
#endif

#ifdef Q_WS_QWS
class QWSGLWindowSurface;
#endif

#if defined(QT_OPENGL_ES)
class QEglContext;
#endif

QT_BEGIN_INCLUDE_NAMESPACE
#include <QtOpenGL/private/qglextensions_p.h>
QT_END_INCLUDE_NAMESPACE

class QGLFormatPrivate
{
public:
    QGLFormatPrivate()
        : ref(1)
    {
        opts = QGL::DoubleBuffer | QGL::DepthBuffer | QGL::Rgba | QGL::DirectRendering | QGL::StencilBuffer;
#if defined(QT_OPENGL_ES_2)
        opts |= QGL::SampleBuffers;
#endif
        pln = 0;
        depthSize = accumSize = stencilSize = redSize = greenSize = blueSize = alphaSize = -1;
        numSamples = -1;
        swapInterval = -1;
    }
    QGLFormatPrivate(const QGLFormatPrivate *other)
        : ref(1),
          opts(other->opts),
          pln(other->pln),
          depthSize(other->depthSize),
          accumSize(other->accumSize),
          stencilSize(other->stencilSize),
          redSize(other->redSize),
          greenSize(other->greenSize),
          blueSize(other->blueSize),
          alphaSize(other->alphaSize),
          numSamples(other->numSamples),
          swapInterval(other->swapInterval)
    {
    }
    QAtomicInt ref;
    QGL::FormatOptions opts;
    int pln;
    int depthSize;
    int accumSize;
    int stencilSize;
    int redSize;
    int greenSize;
    int blueSize;
    int alphaSize;
    int numSamples;
    int swapInterval;
};

class QGLWidgetPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QGLWidget)
public:
    QGLWidgetPrivate() : QWidgetPrivate()
                       , disable_clear_on_painter_begin(false)
#ifdef Q_WS_QWS
                       , wsurf(0)
#endif
#if defined(Q_WS_X11) && defined(QT_OPENGL_ES)
                       , eglSurfaceWindowId(0)
#endif
        {}

    ~QGLWidgetPrivate() {}

    void init(QGLContext *context, const QGLWidget* shareWidget);
    void initContext(QGLContext *context, const QGLWidget* shareWidget);
    bool renderCxPm(QPixmap *pixmap);
    void cleanupColormaps();

    QGLContext *glcx;
    QGLWidgetGLPaintDevice glDevice;
    bool autoSwap;

    QGLColormap cmap;
#ifndef QT_OPENGL_ES
    QMap<QString, int> displayListCache;
#endif

    bool disable_clear_on_painter_begin;

#if defined(Q_WS_WIN)
    void updateColormap();
    QGLContext *olcx;
#elif defined(Q_WS_X11)
    QGLOverlayWidget *olw;
#if defined(QT_OPENGL_ES)
    void recreateEglSurface(bool force);
    WId eglSurfaceWindowId;
#endif
#elif defined(Q_WS_MAC)
    QGLContext *olcx;
    void updatePaintDevice();
#elif defined(Q_WS_QWS)
    QWSGLWindowSurface *wsurf;
#endif
};

class QGLContextResource;
class QGLSharedResourceGuard;

// QGLContextPrivate has the responsibility of creating context groups.
// QGLContextPrivate and QGLShareRegister will both maintain the reference counter and destroy
// context groups when needed.
// QGLShareRegister has the responsibility of keeping the context pointer up to date.
class QGLContextGroup
{
public:
    ~QGLContextGroup();

    QGLExtensionFuncs &extensionFuncs() {return m_extensionFuncs;}
    const QGLContext *context() const {return m_context;}
    bool isSharing() const { return m_shares.size() >= 2; }

    void addGuard(QGLSharedResourceGuard *guard);
    void removeGuard(QGLSharedResourceGuard *guard);
private:
    QGLContextGroup(const QGLContext *context) : m_context(context), m_guards(0), m_refs(1) { }

    QGLExtensionFuncs m_extensionFuncs;
    const QGLContext *m_context; // context group's representative
    QList<const QGLContext *> m_shares;
    QHash<QGLContextResource *, void *> m_resources;
    QGLSharedResourceGuard *m_guards; // double-linked list of active guards.
    QAtomicInt m_refs;

    void cleanupResources(const QGLContext *ctx);

    friend class QGLShareRegister;
    friend class QGLContext;
    friend class QGLContextPrivate;
    friend class QGLContextResource;
};

class QGLTexture;

class QGLContextPrivate
{
    Q_DECLARE_PUBLIC(QGLContext)
public:
    explicit QGLContextPrivate(QGLContext *context) : internal_context(false), q_ptr(context) {group = new QGLContextGroup(context);}
    ~QGLContextPrivate();
    QGLTexture *bindTexture(const QImage &image, GLenum target, GLint format,
                            QGLContext::BindOptions options);
    QGLTexture *bindTexture(const QImage &image, GLenum target, GLint format, const qint64 key,
                            QGLContext::BindOptions options);
    QGLTexture *bindTexture(const QPixmap &pixmap, GLenum target, GLint format,
                            QGLContext::BindOptions options);
    QGLTexture *textureCacheLookup(const qint64 key, GLenum target);
    void init(QPaintDevice *dev, const QGLFormat &format);
    QImage convertToGLFormat(const QImage &image, bool force_premul, GLenum texture_format);
    int maxTextureSize();

    void cleanup();

#if defined(Q_WS_WIN)
    HGLRC rc;
    HDC dc;
    WId        win;
    int pixelFormatId;
    QGLCmap* cmap;
    HBITMAP hbitmap;
    HDC hbitmap_hdc;
#endif
#if defined(QT_OPENGL_ES)
    QEglContext *eglContext;
    EGLSurface eglSurface;
#elif defined(Q_WS_X11) || defined(Q_WS_MAC)
    void* cx;
#endif
#if defined(Q_WS_X11) || defined(Q_WS_MAC)
    void* vi;
#endif
#if defined(Q_WS_X11)
    void* pbuf;
    quint32 gpm;
    int screen;
    QHash<QPixmapData*, QPixmap> boundPixmaps;
    QGLTexture *bindTextureFromNativePixmap(QPixmapData*, const qint64 key,
                                            QGLContext::BindOptions options);
    static void destroyGlSurfaceForPixmap(QPixmapData*);
    static void unbindPixmapFromTexture(QPixmapData*);
#endif
#if defined(Q_WS_MAC)
    bool update;
    void *tryFormat(const QGLFormat &format);
    void clearDrawable();
#endif
    QGLFormat glFormat;
    QGLFormat reqFormat;
    GLuint fbo;

    uint valid : 1;
    uint sharing : 1;
    uint initDone : 1;
    uint crWin : 1;
    uint internal_context : 1;
    uint version_flags_cached : 1;
    QPaintDevice *paintDevice;
    QColor transpColor;
    QGLContext *q_ptr;
    QGLFormat::OpenGLVersionFlags version_flags;

    QGLContextGroup *group;
    GLint max_texture_size;

    GLuint current_fbo;
    QPaintEngine *active_engine;

    static inline QGLContextGroup *contextGroup(const QGLContext *ctx) { return ctx->d_ptr->group; }

#ifdef Q_WS_WIN
    static inline QGLExtensionFuncs& extensionFuncs(const QGLContext *ctx) { return ctx->d_ptr->group->extensionFuncs(); }
#endif

#if defined(Q_WS_X11) || defined(Q_WS_MAC) || defined(Q_WS_QWS)
    static QGLExtensionFuncs qt_extensionFuncs;
    static inline QGLExtensionFuncs& extensionFuncs(const QGLContext *) { return qt_extensionFuncs; }
#endif

    static void setCurrentContext(QGLContext *context);
};

// ### make QGLContext a QObject in 5.0 and remove the proxy stuff
class Q_OPENGL_EXPORT QGLSignalProxy : public QObject
{
    Q_OBJECT
public:
    QGLSignalProxy() : QObject() {}
    void emitAboutToDestroyContext(const QGLContext *context) {
        emit aboutToDestroyContext(context);
    }
    static QGLSignalProxy *instance();
Q_SIGNALS:
    void aboutToDestroyContext(const QGLContext *context);
};

// GL extension definitions
class QGLExtensions {
public:
    enum Extension {
        TextureRectangle        = 0x00000001,
        SampleBuffers           = 0x00000002,
        GenerateMipmap          = 0x00000004,
        TextureCompression      = 0x00000008,
        FragmentProgram         = 0x00000010,
        MirroredRepeat          = 0x00000020,
        FramebufferObject       = 0x00000040,
        StencilTwoSide          = 0x00000080,
        StencilWrap             = 0x00000100,
        PackedDepthStencil      = 0x00000200,
        NVFloatBuffer           = 0x00000400,
        PixelBufferObject       = 0x00000800,
        FramebufferBlit         = 0x00001000,
        NPOTTextures            = 0x00002000,
        BGRATextureFormat       = 0x00004000
    };
    Q_DECLARE_FLAGS(Extensions, Extension)

    static Extensions glExtensions;
    static bool nvidiaFboNeedsFinish;
    static void init(); // sys dependent
    static void init_extensions(); // general: called by init()
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLExtensions::Extensions)


class Q_AUTOTEST_EXPORT QGLShareRegister
{
public:
    QGLShareRegister() {}
    ~QGLShareRegister() {}

    void addShare(const QGLContext *context, const QGLContext *share);
    QList<const QGLContext *> shares(const QGLContext *context);
    void removeShare(const QGLContext *context);
};

extern Q_OPENGL_EXPORT QGLShareRegister* qgl_share_reg();

// Temporarily make a context current if not already current or
// shared with the current contex.  The previous context is made
// current when the object goes out of scope.
class Q_OPENGL_EXPORT QGLShareContextScope
{
public:
    QGLShareContextScope(const QGLContext *ctx)
        : m_oldContext(0)
    {
        QGLContext *currentContext = const_cast<QGLContext *>(QGLContext::currentContext());
        if (currentContext != ctx && !QGLContext::areSharing(ctx, currentContext)) {
            m_oldContext = currentContext;
            m_ctx = const_cast<QGLContext *>(ctx);
            m_ctx->makeCurrent();
        } else {
            m_ctx = currentContext;
        }
    }

    operator QGLContext *()
    {
        return m_ctx;
    }

    QGLContext *operator->()
    {
        return m_ctx;
    }

    ~QGLShareContextScope()
    {
        if (m_oldContext)
            m_oldContext->makeCurrent();
    }

private:
    QGLContext *m_oldContext;
    QGLContext *m_ctx;
};

class QGLTexture {
public:
    QGLTexture(QGLContext *ctx = 0, GLuint tx_id = 0, GLenum tx_target = GL_TEXTURE_2D,
               QGLContext::BindOptions opt = QGLContext::DefaultBindOption)
        : context(ctx),
          id(tx_id),
          target(tx_target),
          options(opt)
#if defined(Q_WS_X11)
        , boundPixmap(0)
#endif
    {}

    ~QGLTexture() {
        if (options & QGLContext::MemoryManagedBindOption) {
            Q_ASSERT(context);
            QGLShareContextScope scope(context);
#if defined(Q_WS_X11)
            // Although glXReleaseTexImage is a glX call, it must be called while there
            // is a current context - the context the pixmap was bound to a texture in.
            // Otherwise the release doesn't do anything and you get BadDrawable errors
            // when you come to delete the context.
            if (boundPixmap)
                QGLContextPrivate::unbindPixmapFromTexture(boundPixmap);
#endif
            glDeleteTextures(1, &id);
        }
     }

    QGLContext *context;
    GLuint id;
    GLenum target;

    QGLContext::BindOptions options;

#if defined(Q_WS_X11)
    QPixmapData* boundPixmap;
#endif

};

class QGLTextureCache {
public:
    QGLTextureCache();
    ~QGLTextureCache();

    void insert(QGLContext *ctx, qint64 key, QGLTexture *texture, int cost);
    void remove(quint64 key) { m_cache.remove(key); }
    bool remove(QGLContext *ctx, GLuint textureId);
    void removeContextTextures(QGLContext *ctx);
    int size() { return m_cache.size(); }
    void setMaxCost(int newMax) { m_cache.setMaxCost(newMax); }
    int maxCost() {return m_cache.maxCost(); }
    QGLTexture* getTexture(quint64 key) { return m_cache.object(key); }

    static QGLTextureCache *instance();
    static void deleteIfEmpty();
    static void imageCleanupHook(qint64 cacheKey);
    static void cleanupTextures(QPixmap* pixmap);
#ifdef Q_WS_X11
    // X11 needs to catch pixmap data destruction to delete EGL/GLX pixmap surfaces
    static void cleanupPixmapSurfaces(QPixmap* pixmap);
#endif

private:
    QCache<qint64, QGLTexture> m_cache;
};


extern Q_OPENGL_EXPORT QPaintEngine* qt_qgl_paint_engine();

bool qt_gl_preferGL2Engine();

inline GLenum qt_gl_preferredTextureFormat()
{
    return QSysInfo::ByteOrder == QSysInfo::BigEndian ? GL_RGBA : GL_BGRA;
}

inline GLenum qt_gl_preferredTextureTarget()
{
#if defined(QT_OPENGL_ES_2)
    return GL_TEXTURE_2D;
#else
    return (QGLExtensions::glExtensions & QGLExtensions::TextureRectangle)
           && !qt_gl_preferGL2Engine()
           ? GL_TEXTURE_RECTANGLE_NV
           : GL_TEXTURE_2D;
#endif
}

// One resource per group of shared contexts.
class Q_AUTOTEST_EXPORT QGLContextResource
{
public:
    typedef void (*FreeFunc)(void *);
    QGLContextResource(FreeFunc f);
    ~QGLContextResource();
    // Set resource 'value' for 'key' and all its shared contexts.
    void insert(const QGLContext *key, void *value);
    // Return resource for 'key' or a shared context.
    void *value(const QGLContext *key);
    // Cleanup 'value' in response to a context group being destroyed.
    void cleanup(const QGLContext *ctx, void *value);
private:
    FreeFunc free;
    QAtomicInt active;
};

// Put a guard around a GL object identifier and its context.
// When the context goes away, a shared context will be used
// in its place.  If there are no more shared contexts, then
// the identifier is returned as zero - it is assumed that the
// context destruction cleaned up the identifier in this case.
class Q_OPENGL_EXPORT QGLSharedResourceGuard
{
public:
    QGLSharedResourceGuard(const QGLContext *context)
        : m_group(0), m_id(0), m_next(0), m_prev(0)
    {
        setContext(context);
    }
    QGLSharedResourceGuard(const QGLContext *context, GLuint id)
        : m_group(0), m_id(id), m_next(0), m_prev(0)
    {
        setContext(context);
    }
    ~QGLSharedResourceGuard();

    const QGLContext *context() const
    {
        return m_group ? m_group->context() : 0;
    }

    void setContext(const QGLContext *context);

    GLuint id() const
    {
        return m_id;
    }

    void setId(GLuint id)
    {
        m_id = id;
    }

private:
    QGLContextGroup *m_group;
    GLuint m_id;
    QGLSharedResourceGuard *m_next;
    QGLSharedResourceGuard *m_prev;

    friend class QGLContextGroup;
};

QT_END_NAMESPACE

#endif // QGL_P_H
