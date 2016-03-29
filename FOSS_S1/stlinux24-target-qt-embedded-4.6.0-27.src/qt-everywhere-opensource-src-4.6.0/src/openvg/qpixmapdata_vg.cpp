/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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

#include "qpixmapdata_vg_p.h"
#include "qpaintengine_vg_p.h"
#include <QtGui/private/qdrawhelper_p.h>
#include "qvg_p.h"

#ifdef QT_SYMBIAN_SUPPORTS_SGIMAGE
#include <graphics/sgimage.h>
typedef EGLImageKHR (*pfnEglCreateImageKHR)(EGLDisplay, EGLContext, EGLenum, EGLClientBuffer, EGLint*);
typedef EGLBoolean (*pfnEglDestroyImageKHR)(EGLDisplay, EGLImageKHR);
typedef VGImage (*pfnVgCreateEGLImageTargetKHR)(VGeglImageKHR);
#endif

QT_BEGIN_NAMESPACE

static int qt_vg_pixmap_serial = 0;

QVGPixmapData::QVGPixmapData(PixelType type)
    : QPixmapData(type, OpenVGClass)
{
    Q_ASSERT(type == QPixmapData::PixmapType);
    vgImage = VG_INVALID_HANDLE;
    vgImageOpacity = VG_INVALID_HANDLE;
    cachedOpacity = 1.0f;
    recreate = true;
#if !defined(QT_NO_EGL)
    context = 0;
#endif
    setSerialNumber(++qt_vg_pixmap_serial);
}

QVGPixmapData::~QVGPixmapData()
{
    if (vgImage != VG_INVALID_HANDLE) {
        // We need to have a context current to destroy the image.
#if !defined(QT_NO_EGL)
        if (context->isCurrent()) {
            vgDestroyImage(vgImage);
            if (vgImageOpacity != VG_INVALID_HANDLE)
                vgDestroyImage(vgImageOpacity);
        } else {
            // We don't currently have a widget surface active, but we
            // need a surface to make the context current.  So use the
            // shared pbuffer surface instead.
            context->makeCurrent(qt_vg_shared_surface());
            vgDestroyImage(vgImage);
            if (vgImageOpacity != VG_INVALID_HANDLE)
                vgDestroyImage(vgImageOpacity);
            context->lazyDoneCurrent();
        }
#else
        vgDestroyImage(vgImage);
        if (vgImageOpacity != VG_INVALID_HANDLE)
            vgDestroyImage(vgImageOpacity);
#endif
    }
#if !defined(QT_NO_EGL)
    if (context)
        qt_vg_destroy_context(context);
#endif
}

QPixmapData *QVGPixmapData::createCompatiblePixmapData() const
{
    return new QVGPixmapData(pixelType());
}

bool QVGPixmapData::isValid() const
{
    return (w > 0 && h > 0);
}

void QVGPixmapData::resize(int wid, int ht)
{
    if (w == wid && h == ht)
        return;

    w = wid;
    h = ht;
    d = 32; // We always use ARGB_Premultiplied for VG pixmaps.
    is_null = (w <= 0 || h <= 0);
    source = QImage();
    recreate = true;

    setSerialNumber(++qt_vg_pixmap_serial);
}

void QVGPixmapData::fromImage
        (const QImage &image, Qt::ImageConversionFlags flags)
{
    if (image.size() == QSize(w, h))
        setSerialNumber(++qt_vg_pixmap_serial);
    else
        resize(image.width(), image.height());
    source = image.convertToFormat(sourceFormat(), flags);
    recreate = true;
}

void QVGPixmapData::fill(const QColor &color)
{
    if (!isValid())
        return;

    if (source.isNull())
        source = QImage(w, h, sourceFormat());

    if (source.depth() == 1) {
        // Pick the best approximate color in the image's colortable.
        int gray = qGray(color.rgba());
        if (qAbs(qGray(source.color(0)) - gray) < qAbs(qGray(source.color(1)) - gray))
            source.fill(0);
        else
            source.fill(1);
    } else {
        source.fill(PREMUL(color.rgba()));
    }

    // Re-upload the image to VG the next time toVGImage() is called.
    recreate = true;
}

bool QVGPixmapData::hasAlphaChannel() const
{
    if (!source.isNull())
        return source.hasAlphaChannel();
    else
        return isValid();
}

void QVGPixmapData::setAlphaChannel(const QPixmap &alphaChannel)
{
    forceToImage();
    source.setAlphaChannel(alphaChannel.toImage());
}

QImage QVGPixmapData::toImage() const
{
    if (!isValid())
        return QImage();

    if (source.isNull()) {
        source = QImage(w, h, sourceFormat());
        recreate = true;
    }

    return source;
}

QImage *QVGPixmapData::buffer()
{
    forceToImage();
    return &source;
}

QPaintEngine* QVGPixmapData::paintEngine() const
{
    // If the application wants to paint into the QPixmap, we first
    // force it to QImage format and then paint into that.
    // This is simpler than juggling multiple VG contexts.
    const_cast<QVGPixmapData *>(this)->forceToImage();
    return source.paintEngine();
}

// This function works around QImage::bits() making a deep copy if the
// QImage is not const.  We force it to be const and then get the bits.
// XXX: Should add a QImage::constBits() in the future to replace this.
static inline const uchar *qt_vg_imageBits(const QImage& image)
{
    return image.bits();
}

VGImage QVGPixmapData::toVGImage()
{
    if (!isValid())
        return VG_INVALID_HANDLE;

#if !defined(QT_NO_EGL)
    // Increase the reference count on the shared context.
    if (!context)
        context = qt_vg_create_context(0);
#endif

    if (recreate && prevSize != QSize(w, h)) {
        if (vgImage != VG_INVALID_HANDLE) {
            vgDestroyImage(vgImage);
            vgImage = VG_INVALID_HANDLE;
        }
        if (vgImageOpacity != VG_INVALID_HANDLE) {
            vgDestroyImage(vgImageOpacity);
            vgImageOpacity = VG_INVALID_HANDLE;
        }
    } else if (recreate) {
        cachedOpacity = -1.0f;  // Force opacity image to be refreshed later.
    }

    if (vgImage == VG_INVALID_HANDLE) {
        vgImage = vgCreateImage
            (VG_sARGB_8888_PRE, w, h, VG_IMAGE_QUALITY_FASTER);
    }

    if (!source.isNull() && recreate) {
        vgImageSubData
            (vgImage,
             qt_vg_imageBits(source), source.bytesPerLine(),
             VG_sARGB_8888_PRE, 0, 0, w, h);
    }

    recreate = false;
    prevSize = QSize(w, h);

    return vgImage;
}

VGImage QVGPixmapData::toVGImage(qreal opacity)
{
#if !defined(QT_SHIVAVG)
    // Force the primary VG image to be recreated if necessary.
    if (toVGImage() == VG_INVALID_HANDLE)
        return VG_INVALID_HANDLE;

    if (opacity == 1.0f)
        return vgImage;

    // Create an alternative image for the selected opacity.
    if (vgImageOpacity == VG_INVALID_HANDLE || cachedOpacity != opacity) {
        if (vgImageOpacity == VG_INVALID_HANDLE) {
            vgImageOpacity = vgCreateImage
                (VG_sARGB_8888_PRE, w, h, VG_IMAGE_QUALITY_FASTER);
        }
        VGfloat matrix[20] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, opacity,
            0.0f, 0.0f, 0.0f, 0.0f
        };
        vgColorMatrix(vgImageOpacity, vgImage, matrix);
        cachedOpacity = opacity;
    }

    return vgImageOpacity;
#else
    // vgColorMatrix() doesn't work with ShivaVG, so ignore the opacity.
    Q_UNUSED(opacity);
    return toVGImage();
#endif
}

extern int qt_defaultDpiX();
extern int qt_defaultDpiY();

int QVGPixmapData::metric(QPaintDevice::PaintDeviceMetric metric) const
{
    switch (metric) {
    case QPaintDevice::PdmWidth:
        return w;
    case QPaintDevice::PdmHeight:
        return h;
    case QPaintDevice::PdmNumColors:
        return 0;
    case QPaintDevice::PdmDepth:
        return d;
    case QPaintDevice::PdmWidthMM:
        return qRound(w * 25.4 / qt_defaultDpiX());
    case QPaintDevice::PdmHeightMM:
        return qRound(h * 25.4 / qt_defaultDpiY());
    case QPaintDevice::PdmDpiX:
    case QPaintDevice::PdmPhysicalDpiX:
        return qt_defaultDpiX();
    case QPaintDevice::PdmDpiY:
    case QPaintDevice::PdmPhysicalDpiY:
        return qt_defaultDpiY();
    default:
        qWarning("QVGPixmapData::metric(): Invalid metric");
        return 0;
    }
}

// Force the pixmap data to be in QImage format.
void QVGPixmapData::forceToImage()
{
    if (!isValid())
        return;

    if (source.isNull())
        source = QImage(w, h, sourceFormat());

    recreate = true;
}

QImage::Format QVGPixmapData::sourceFormat() const
{
    return QImage::Format_ARGB32_Premultiplied;
}

/*
    \internal

    Returns the VGImage that is storing the contents of \a pixmap.
    Returns VG_INVALID_HANDLE if \a pixmap is not owned by the OpenVG
    graphics system or \a pixmap is invalid.

    This function is typically used to access the backing store
    for a pixmap when executing raw OpenVG calls.  It must only
    be used when a QPainter is active and the OpenVG paint engine
    is in use by the QPainter.

    \sa {QtOpenVG Module}
*/
Q_OPENVG_EXPORT VGImage qPixmapToVGImage(const QPixmap& pixmap)
{
    QPixmapData *pd = pixmap.pixmapData();
    if (!pd)
        return VG_INVALID_HANDLE; // null QPixmap
    if (pd->classId() == QPixmapData::OpenVGClass) {
        QVGPixmapData *vgpd = static_cast<QVGPixmapData *>(pd);
        if (vgpd->isValid())
            return vgpd->toVGImage();
    }
    return VG_INVALID_HANDLE;
}

#if defined(Q_OS_SYMBIAN)
void QVGPixmapData::cleanup()
{
    is_null = w = h = 0;
    recreate = false;
    source = QImage();
}

void QVGPixmapData::fromNativeType(void* pixmap, NativeType type)
{
#if defined(QT_SYMBIAN_SUPPORTS_SGIMAGE) && !defined(QT_NO_EGL)
    if (type == QPixmapData::SgImage && pixmap) {
        RSgImage *sgImage = reinterpret_cast<RSgImage*>(pixmap);
        // when "0" used as argument then
        // default display, context are used
        if (!context)
            context = qt_vg_create_context(0);

        if (vgImage != VG_INVALID_HANDLE) {
            vgDestroyImage(vgImage);
            vgImage = VG_INVALID_HANDLE;
        }
        if (vgImageOpacity != VG_INVALID_HANDLE) {
            vgDestroyImage(vgImageOpacity);
            vgImageOpacity = VG_INVALID_HANDLE;
        }
        prevSize = QSize();

        TInt err = 0;

        err = SgDriver::Open();
        if(err != KErrNone) {
            cleanup();
            return;
        }

        if(sgImage->IsNull()) {
            cleanup();
            SgDriver::Close();
            return;
        }

        TSgImageInfo sgImageInfo;
        err = sgImage->GetInfo(sgImageInfo);
        if(err != KErrNone) {
            cleanup();
            SgDriver::Close();
            return;
        }

        pfnEglCreateImageKHR eglCreateImageKHR = (pfnEglCreateImageKHR) eglGetProcAddress("eglCreateImageKHR");
        pfnEglDestroyImageKHR eglDestroyImageKHR = (pfnEglDestroyImageKHR) eglGetProcAddress("eglDestroyImageKHR");
        pfnVgCreateEGLImageTargetKHR vgCreateEGLImageTargetKHR = (pfnVgCreateEGLImageTargetKHR) eglGetProcAddress("vgCreateEGLImageTargetKHR");

        if(eglGetError() != EGL_SUCCESS || !eglCreateImageKHR || !eglDestroyImageKHR || !vgCreateEGLImageTargetKHR) {
            cleanup();
            SgDriver::Close();
            return;
        }

        const EGLint KEglImageAttribs[] = {EGL_IMAGE_PRESERVED_SYMBIAN, EGL_TRUE, EGL_NONE};
        EGLImageKHR eglImage = eglCreateImageKHR(context->display(),
                EGL_NO_CONTEXT,
                EGL_NATIVE_PIXMAP_KHR,
                (EGLClientBuffer)sgImage,
                (EGLint*)KEglImageAttribs);

        if(eglGetError() != EGL_SUCCESS) {
            cleanup();
            SgDriver::Close();
            return;
        }

        vgImage = vgCreateEGLImageTargetKHR(eglImage);
        if(vgGetError() != VG_NO_ERROR) {
            cleanup();
            eglDestroyImageKHR(context->display(), eglImage);
            SgDriver::Close();
            return;
        }

        w = sgImageInfo.iSizeInPixels.iWidth;
        h = sgImageInfo.iSizeInPixels.iHeight;
        d = 32; // We always use ARGB_Premultiplied for VG pixmaps.
        is_null = (w <= 0 || h <= 0);
        source = QImage();
        recreate = false;
        prevSize = QSize(w, h);
        setSerialNumber(++qt_vg_pixmap_serial);
        // release stuff
        eglDestroyImageKHR(context->display(), eglImage);
        SgDriver::Close();
    } else if (type == QPixmapData::FbsBitmap) {

    }
#else
    Q_UNUSED(pixmap);
    Q_UNUSED(type);
#endif
}

void* QVGPixmapData::toNativeType(NativeType type)
{
#if defined(QT_SYMBIAN_SUPPORTS_SGIMAGE) && !defined(QT_NO_EGL)
    if (type == QPixmapData::SgImage) {
        toVGImage();

        if(!isValid() || vgImage == VG_INVALID_HANDLE)
            return 0;

        TInt err = 0;

        err = SgDriver::Open();
        if(err != KErrNone)
            return 0;

        TSgImageInfo sgInfo;
        sgInfo.iPixelFormat = EUidPixelFormatARGB_8888_PRE;
        sgInfo.iSizeInPixels.SetSize(w, h);
        sgInfo.iUsage = ESgUsageOpenVgImage | ESgUsageOpenVgTarget;
        sgInfo.iShareable = ETrue;
        sgInfo.iCpuAccess = ESgCpuAccessNone;
        sgInfo.iScreenId = KSgScreenIdMain; //KSgScreenIdAny;
        sgInfo.iUserAttributes = NULL;
        sgInfo.iUserAttributeCount = 0;

        RSgImage *sgImage = q_check_ptr(new RSgImage());
        err = sgImage->Create(sgInfo, NULL, NULL);
        if(err != KErrNone) {
            SgDriver::Close();
            return 0;
        }

        pfnEglCreateImageKHR eglCreateImageKHR = (pfnEglCreateImageKHR) eglGetProcAddress("eglCreateImageKHR");
        pfnEglDestroyImageKHR eglDestroyImageKHR = (pfnEglDestroyImageKHR) eglGetProcAddress("eglDestroyImageKHR");
        pfnVgCreateEGLImageTargetKHR vgCreateEGLImageTargetKHR = (pfnVgCreateEGLImageTargetKHR) eglGetProcAddress("vgCreateEGLImageTargetKHR");

        if(eglGetError() != EGL_SUCCESS || !eglCreateImageKHR || !eglDestroyImageKHR || !vgCreateEGLImageTargetKHR) {
            SgDriver::Close();
            return 0;
        }

        const EGLint KEglImageAttribs[] = {EGL_IMAGE_PRESERVED_SYMBIAN, EGL_TRUE, EGL_NONE};
        EGLImageKHR eglImage = eglCreateImageKHR(context->display(),
                EGL_NO_CONTEXT,
                EGL_NATIVE_PIXMAP_KHR,
                (EGLClientBuffer)sgImage,
                (EGLint*)KEglImageAttribs);
        if(eglGetError() != EGL_SUCCESS) {
            sgImage->Close();
            SgDriver::Close();
            return 0;
        }

        VGImage dstVgImage = vgCreateEGLImageTargetKHR(eglImage);
        if(vgGetError() != VG_NO_ERROR) {
            eglDestroyImageKHR(context->display(), eglImage);
            sgImage->Close();
            SgDriver::Close();
            return 0;
        }

        vgCopyImage(dstVgImage, 0, 0,
                vgImage, 0, 0,
                w, h, VG_FALSE);

        if(vgGetError() != VG_NO_ERROR) {
            sgImage->Close();
            sgImage = 0;
        }
        // release stuff
        vgDestroyImage(dstVgImage);
        eglDestroyImageKHR(context->display(), eglImage);
        SgDriver::Close();
        return reinterpret_cast<void*>(sgImage);
    } else if (type == QPixmapData::FbsBitmap) {
        return 0;
    }
#else
    Q_UNUSED(type);
    return 0;
#endif
}
#endif //Q_OS_SYMBIAN

QT_END_NAMESPACE
