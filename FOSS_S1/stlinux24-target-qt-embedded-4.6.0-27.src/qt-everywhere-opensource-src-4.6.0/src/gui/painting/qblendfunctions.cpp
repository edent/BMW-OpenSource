/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <qmath.h>
#include "qdrawhelper_p.h"

QT_BEGIN_NAMESPACE

struct SourceOnlyAlpha
{
    inline uchar alpha(uchar src) const { return src; }
    inline quint16 bytemul(quint16 spix) const { return spix; }
};


struct SourceAndConstAlpha
{
    SourceAndConstAlpha(int a) : m_alpha256(a) {
        m_alpha255 = (m_alpha256 * 255) >> 8;
    };
    inline uchar alpha(uchar src) const { return (src * m_alpha256) >> 8; }
    inline quint16 bytemul(quint16 x) const {
        uint t = (((x & 0x07e0)*m_alpha255) >> 8) & 0x07e0;
        t |= (((x & 0xf81f)*(m_alpha255>>2)) >> 6) & 0xf81f;
        return t;
    }
    int m_alpha255;
    int m_alpha256;
};


/************************************************************************
                       RGB16 (565) format target format
 ************************************************************************/

static inline quint16 convert_argb32_to_rgb16(quint32 spix)
{
    quint32 b = spix;
    quint32 g = spix;
    b >>= 8;
    g >>= 5;
    b &= 0x0000f800;
    g &= 0x000007e0;
    spix >>= 3;
    b |= g;
    spix &= 0x0000001f;
    b |= spix;
    return b;
}

struct Blend_RGB16_on_RGB16_NoAlpha {
    inline void write(quint16 *dst, quint16 src) { *dst = src; }
};

struct Blend_RGB16_on_RGB16_ConstAlpha {
    inline Blend_RGB16_on_RGB16_ConstAlpha(quint32 alpha) {
        m_alpha = (alpha * 255) >> 8;
        m_ialpha = 255 - m_alpha;
    }

    inline void write(quint16 *dst, quint16 src) {
        *dst = BYTE_MUL_RGB16(src, m_alpha) + BYTE_MUL_RGB16(*dst, m_ialpha);
    }

    quint32 m_alpha;
    quint32 m_ialpha;
};

struct Blend_ARGB24_on_RGB16_SourceAlpha {
    inline void write(quint16 *dst, const qargb8565 &src) {
        const uint alpha = src.alpha();
        if (alpha) {
            quint16 s = src.rawValue16();
            if (alpha < 255)
                s += BYTE_MUL_RGB16(*dst, 255 - alpha);
            *dst = s;
        }
    }
};

struct Blend_ARGB24_on_RGB16_SourceAndConstAlpha {
    inline Blend_ARGB24_on_RGB16_SourceAndConstAlpha(quint32 alpha) {
        m_alpha = (alpha * 255) >> 8;
    }

    inline void write(quint16 *dst, qargb8565 src) {
        src = src.byte_mul(src.alpha(m_alpha));
        const uint alpha = src.alpha();
        if (alpha) {
            quint16 s = src.rawValue16();
            if (alpha < 255)
                s += BYTE_MUL_RGB16(*dst, 255 - alpha);
            *dst = s;
        }
    }

    quint32 m_alpha;
};

struct Blend_ARGB32_on_RGB16_SourceAlpha {
    inline void write(quint16 *dst, quint32 src) {
        const quint8 alpha = qAlpha(src);
        if(alpha) {
            quint16 s = convert_argb32_to_rgb16(src);
            if(alpha < 255)
                s += BYTE_MUL_RGB16(*dst, 255 - alpha);
            *dst = s;
        }
    }
};

struct Blend_ARGB32_on_RGB16_SourceAndConstAlpha {
    inline Blend_ARGB32_on_RGB16_SourceAndConstAlpha(quint32 alpha) {
        m_alpha = (alpha * 255) >> 8;
    }

    inline void write(quint16 *dst, quint32 src) {
        src = BYTE_MUL(src, m_alpha);
        const quint8 alpha = qAlpha(src);
        if(alpha) {
            quint16 s = convert_argb32_to_rgb16(src);
            if(alpha < 255)
                s += BYTE_MUL_RGB16(*dst, 255 - alpha);
            *dst = s;
        }
    }

    quint32 m_alpha;
};

template <typename SRC, typename T>
void qt_scale_image_16bit(uchar *destPixels, int dbpl,
                          const uchar *srcPixels, int sbpl,
                          const QRectF &targetRect,
                          const QRectF &srcRect,
                          const QRect &clip,
                          T blender)
{
    qreal sx = targetRect.width() / (qreal) srcRect.width();
    qreal sy = targetRect.height() / (qreal) srcRect.height();

    int ix = 0x00010000 / sx;
    int iy = 0x00010000 / sy;

//     qDebug() << "scale:" << endl
//              << " - target" << targetRect << endl
//              << " - source" << srcRect << endl
//              << " - clip" << clip << endl
//              << " - sx=" << sx << " sy=" << sy << " ix=" << ix << " iy=" << iy;

    int cx1 = clip.x();
    int cx2 = clip.x() + clip.width();
    int cy1 = clip.top();
    int cy2 = clip.y() + clip.height();

    int tx1 = qRound(targetRect.left());
    int tx2 = qRound(targetRect.right());
    int ty1 = qRound(targetRect.top());
    int ty2 = qRound(targetRect.bottom());

    if (tx2 < tx1)
        qSwap(tx2, tx1);

    if (ty2 < ty1)
        qSwap(ty2, ty1);

    if (tx1 < cx1)
        tx1 = cx1;

    if (tx2 >= cx2)
        tx2 = cx2;

    if (tx1 >= tx2)
        return;

    if (ty1 < cy1)
        ty1 = cy1;

    if (ty2 >= cy2)
       ty2 = cy2;

    if (ty1 >= ty2)
        return;

    int h = ty2 - ty1;
    int w = tx2 - tx1;

    quint32 basex;
    quint32 srcy;

    if (sx < 0) {
        int dstx = qFloor((tx1 + 0.5 - targetRect.right()) * ix) + 1;
        basex = quint32(srcRect.right() * 65536) + dstx;
    } else {
        int dstx = qCeil((tx1 + 0.5 - targetRect.left()) * ix) - 1;
        basex = quint32(srcRect.left() * 65536) + dstx;
    }
    if (sy < 0) {
        int dsty = qFloor((ty1 + 0.5 - targetRect.bottom()) * iy) + 1;
        srcy = quint32(srcRect.bottom() * 65536) + dsty;
    } else {
        int dsty = qCeil((ty1 + 0.5 - targetRect.top()) * iy) - 1;
        srcy = quint32(srcRect.top() * 65536) + dsty;
    }

    quint16 *dst = ((quint16 *) (destPixels + ty1 * dbpl)) + tx1;

    while (h--) {
        const SRC *src = (const SRC *) (srcPixels + (srcy >> 16) * sbpl);
        int srcx = basex;
        for (int x=0; x<w; ++x) {
            blender.write(&dst[x], src[srcx >> 16]);
            srcx += ix;
        }
        dst = (quint16 *)(((uchar *) dst) + dbpl);
        srcy += iy;
    }
}

void qt_scale_image_rgb16_on_rgb16(uchar *destPixels, int dbpl,
                                   const uchar *srcPixels, int sbpl,
                                   const QRectF &targetRect,
                                   const QRectF &sourceRect,
                                   const QRect &clip,
                                   int const_alpha)
{
#ifdef QT_DEBUG_DRAW
    printf("qt_scale_rgb16_on_rgb16: dst=(%p, %d), src=(%p, %d), target=(%d, %d), [%d x %d], src=(%d, %d) [%d x %d] alpha=%d\n",
           destPixels, dbpl, srcPixels, sbpl,
           targetRect.x(), targetRect.y(), targetRect.width(), targetRect.height(),
           sourceRect.x(), sourceRect.y(), sourceRect.width(), sourceRect.height(),
           const_alpha);
#endif
    if (const_alpha == 256) {
        Blend_RGB16_on_RGB16_NoAlpha noAlpha;
        qt_scale_image_16bit<quint16>(destPixels, dbpl, srcPixels, sbpl,
                                      targetRect, sourceRect, clip, noAlpha);
    } else {
        Blend_RGB16_on_RGB16_ConstAlpha constAlpha(const_alpha);
        qt_scale_image_16bit<quint16>(destPixels, dbpl, srcPixels, sbpl,
                                     targetRect, sourceRect, clip, constAlpha);
    }
}

void qt_scale_image_argb24_on_rgb16(uchar *destPixels, int dbpl,
                                   const uchar *srcPixels, int sbpl,
                                   const QRectF &targetRect,
                                   const QRectF &sourceRect,
                                   const QRect &clip,
                                   int const_alpha)
{
#ifdef QT_DEBUG_DRAW
    printf("qt_scale_argb24_on_rgb16: dst=(%p, %d), src=(%p, %d), target=(%d, %d), [%d x %d], src=(%d, %d) [%d x %d] alpha=%d\n",
           destPixels, dbpl, srcPixels, sbpl,
           targetRect.x(), targetRect.y(), targetRect.width(), targetRect.height(),
           sourceRect.x(), sourceRect.y(), sourceRect.width(), sourceRect.height(),
           const_alpha);
#endif
    if (const_alpha == 256) {
        Blend_ARGB24_on_RGB16_SourceAlpha noAlpha;
        qt_scale_image_16bit<qargb8565>(destPixels, dbpl, srcPixels, sbpl,
                                        targetRect, sourceRect, clip, noAlpha);
    } else {
        Blend_ARGB24_on_RGB16_SourceAndConstAlpha constAlpha(const_alpha);
        qt_scale_image_16bit<qargb8565>(destPixels, dbpl, srcPixels, sbpl,
                                        targetRect, sourceRect, clip, constAlpha);
    }
}


void qt_scale_image_argb32_on_rgb16(uchar *destPixels, int dbpl,
                                    const uchar *srcPixels, int sbpl,
                                    const QRectF &targetRect,
                                    const QRectF &sourceRect,
                                    const QRect &clip,
                                    int const_alpha)
{
#ifdef QT_DEBUG_DRAW
    printf("qt_scale_argb32_on_rgb16: dst=(%p, %d), src=(%p, %d), target=(%d, %d), [%d x %d], src=(%d, %d) [%d x %d] alpha=%d\n",
           destPixels, dbpl, srcPixels, sbpl,
           targetRect.x(), targetRect.y(), targetRect.width(), targetRect.height(),
           sourceRect.x(), sourceRect.y(), sourceRect.width(), sourceRect.height(),
           const_alpha);
#endif
    if (const_alpha == 256) {
        Blend_ARGB32_on_RGB16_SourceAlpha noAlpha;
        qt_scale_image_16bit<quint32>(destPixels, dbpl, srcPixels, sbpl,
                                      targetRect, sourceRect, clip, noAlpha);
    } else {
        Blend_ARGB32_on_RGB16_SourceAndConstAlpha constAlpha(const_alpha);
        qt_scale_image_16bit<quint32>(destPixels, dbpl, srcPixels, sbpl,
                                     targetRect, sourceRect, clip, constAlpha);
    }
}

static void qt_blend_rgb16_on_rgb16(uchar *dst, int dbpl,
                                    const uchar *src, int sbpl,
                                    int w, int h,
                                    int const_alpha)
{
#ifdef QT_DEBUG_DRAW
    printf("qt_blend_rgb16_on_rgb16: dst=(%p, %d), src=(%p, %d), dim=(%d, %d) alpha=%d\n",
           dst, dbpl, src, sbpl, w, h, const_alpha);
#endif

    if (const_alpha == 256) {
        if (w <= 64) {
            while (h--) {
                QT_MEMCPY_USHORT(dst, src, w);
                dst += dbpl;
                src += sbpl;
            }
        } else {
            int length = w << 1;
            while (h--) {
                memcpy(dst, src, length);
                dst += dbpl;
                src += sbpl;
            }
        }
    } else if (const_alpha != 0) {
        SourceAndConstAlpha alpha(const_alpha); // expects the 0-256 range
        quint16 *d = (quint16 *) dst;
        const quint16 *s = (const quint16 *) src;
        quint8 a = (255 * const_alpha) >> 8;
        quint8 ia = 255 - a;
        while (h--) {
            for (int x=0; x<w; ++x) {
                d[x] = BYTE_MUL_RGB16(s[x], a) + BYTE_MUL_RGB16(d[x], ia);
            }
            d = (quint16 *)(((uchar *) d) + dbpl);
            s = (const quint16 *)(((const uchar *) s) + sbpl);
        }
    }
}


template <typename T> void qt_blend_argb24_on_rgb16(uchar *destPixels, int dbpl,
                                                    const uchar *srcPixels, int sbpl,
                                                    int w, int h, const T &alphaFunc)
{
    int srcOffset = w*3;
    int dstJPL = dbpl / 2;
    quint16 *dst = (quint16 *) destPixels;
    int dstExtraStride = dstJPL - w;

    for (int y=0; y<h; ++y) {
        const uchar *src = srcPixels + y * sbpl;
        const uchar *srcEnd = src + srcOffset;
        while (src < srcEnd) {
#if defined(QT_ARCH_ARM) || defined(QT_ARCH_POWERPC) || defined(QT_ARCH_SH) || defined(QT_ARCH_AVR32) || (defined(QT_ARCH_WINDOWSCE) && !defined(_X86_)) || (defined(QT_ARCH_SPARC) && defined(Q_CC_GNU))
            // non-16-bit aligned memory access is not possible on PowerPC,
            // ARM <v6 (QT_ARCH_ARMV6) & SH & AVR32 & SPARC w/GCC
            quint16 spix = (quint16(src[2])<<8) + src[1];
#else
            quint16 spix = *(quint16 *) (src + 1);
#endif
            uchar alpha = alphaFunc.alpha(*src);

            if (alpha == 255) {
                *dst = spix;
            } else if (alpha != 0) {
                quint16 dpix = *dst;
                quint32 sia = 255 - alpha;

                quint16 dr = (dpix & 0x0000f800);
                quint16 dg = (dpix & 0x000007e0);
                quint16 db = (dpix & 0x0000001f);

                quint32 siar = dr * sia;
                quint32 siag = dg * sia;
                quint32 siab = db * sia;

                quint32 rr = ((siar + (siar>>8) + (0x80 <<  8)) >> 8) & 0xf800;
                quint32 rg = ((siag + (siag>>8) + (0x80 <<  3)) >> 8) & 0x07e0;
                quint32 rb = ((siab + (siab>>8) + (0x80 >>  3)) >> 8) & 0x001f;

                *dst = alphaFunc.bytemul(spix) + rr + rg + rb;
            }

            ++dst;
            src += 3;
        }
        dst += dstExtraStride;
    }

}

static void qt_blend_argb24_on_rgb16(uchar *destPixels, int dbpl,
                                     const uchar *srcPixels, int sbpl,
                                     int w, int h,
                                     int const_alpha)
{
#ifdef QT_DEBUG_DRAW
    printf("qt_blend_argb24_on_rgb16: dst=(%p, %d), src=(%p, %d), dim=(%d, %d) alpha=%d\n",
           destPixels, dbpl, srcPixels, sbpl, w, h, const_alpha);
#endif

    if (const_alpha != 256) {
        SourceAndConstAlpha alphaFunc(const_alpha);
        qt_blend_argb24_on_rgb16(destPixels, dbpl, srcPixels, sbpl, w, h, alphaFunc);
    } else {
        SourceOnlyAlpha alphaFunc;
        qt_blend_argb24_on_rgb16(destPixels, dbpl, srcPixels, sbpl, w, h, alphaFunc);
    }
}




static void qt_blend_argb32_on_rgb16_const_alpha(uchar *destPixels, int dbpl,
                                                 const uchar *srcPixels, int sbpl,
                                                 int w, int h,
                                                 int const_alpha)
{
    quint16 *dst = (quint16 *) destPixels;
    const quint32 *src = (const quint32 *) srcPixels;

    const_alpha = (const_alpha * 255) >> 8;
    for (int y=0; y<h; ++y) {
        for (int i = 0; i < w; ++i) {
            uint s = src[i];
            s = BYTE_MUL(s, const_alpha);
            int alpha = qAlpha(s);
            s = convert_argb32_to_rgb16(s);
            s += BYTE_MUL_RGB16(dst[i], 255 - alpha);
            dst[i] = s;
        }
        dst = (quint16 *)(((uchar *) dst) + dbpl);
        src = (const quint32 *)(((const uchar *) src) + sbpl);
    }
}

static void qt_blend_argb32_on_rgb16(uchar *destPixels, int dbpl,
                                     const uchar *srcPixels, int sbpl,
                                     int w, int h,
                                     int const_alpha)
{
    if (const_alpha != 256) {
        qt_blend_argb32_on_rgb16_const_alpha(destPixels, dbpl, srcPixels, sbpl, w, h, const_alpha);
        return;
    }

    quint16 *dst = (quint16 *) destPixels;
    quint32 *src = (quint32 *) srcPixels;

    for (int y=0; y<h; ++y) {
        for (int x=0; x<w; ++x) {

            quint32 spix = src[x];
            quint32 alpha = spix >> 24;

            if (alpha == 255) {
                dst[x] = convert_argb32_to_rgb16(spix);
            } else if (alpha != 0) {
                quint32 dpix = dst[x];

                quint32 sia = 255 - alpha;

                quint32 sr = (spix >> 8) & 0xf800;
                quint32 sg = (spix >> 5) & 0x07e0;
                quint32 sb = (spix >> 3) & 0x001f;

                quint32 dr = (dpix & 0x0000f800);
                quint32 dg = (dpix & 0x000007e0);
                quint32 db = (dpix & 0x0000001f);

                quint32 siar = dr * sia;
                quint32 siag = dg * sia;
                quint32 siab = db * sia;

                quint32 rr = sr + ((siar + (siar>>8) + (0x80 << 8)) >> 8);
                quint32 rg = sg + ((siag + (siag>>8) + (0x80 << 3)) >> 8);
                quint32 rb = sb + ((siab + (siab>>8) + (0x80 >> 3)) >> 8);

                dst[x] = (rr & 0xf800)
                         | (rg & 0x07e0)
                         | (rb);
            }
        }
        dst = (quint16 *) (((uchar *) dst) + dbpl);
        src = (quint32 *) (((uchar *) src) + sbpl);
    }
}


static void qt_blend_rgb32_on_rgb16(uchar *destPixels, int dbpl,
                                    const uchar *srcPixels, int sbpl,
                                    int w, int h,
                                    int const_alpha)
{
#ifdef QT_DEBUG_DRAW
    printf("qt_blend_rgb32_on_rgb16: dst=(%p, %d), src=(%p, %d), dim=(%d, %d) alpha=%d\n",
           destPixels, dbpl, srcPixels, sbpl, w, h, const_alpha);
#endif

    if (const_alpha != 256) {
        qt_blend_argb32_on_rgb16(destPixels, dbpl, srcPixels, sbpl, w, h, const_alpha);
        return;
    }

    const quint32 *src = (const quint32 *) srcPixels;
    int srcExtraStride = (sbpl >> 2) - w;

    int dstJPL = dbpl / 2;

    quint16 *dst = (quint16 *) destPixels;
    quint16 *dstEnd = dst + dstJPL * h;

    int dstExtraStride = dstJPL - w;

    while (dst < dstEnd) {
        const quint32 *srcEnd = src + w;
        while (src < srcEnd) {
            *dst = convert_argb32_to_rgb16(*src);
            ++dst;
            ++src;
        }
        dst += dstExtraStride;
        src += srcExtraStride;
    }
}



/************************************************************************
                       RGB32 (-888) format target format
 ************************************************************************/

static void qt_blend_argb32_on_argb32(uchar *destPixels, int dbpl,
                                      const uchar *srcPixels, int sbpl,
                                      int w, int h,
                                      int const_alpha)
{
#ifdef QT_DEBUG_DRAW
    fprintf(stdout, "qt_blend_argb32_on_argb32: dst=(%p, %d), src=(%p, %d), dim=(%d, %d) alpha=%d\n",
            destPixels, dbpl, srcPixels, sbpl, w, h, const_alpha);
    fflush(stdout);
#endif

    const uint *src = (const uint *) srcPixels;
    uint *dst = (uint *) destPixels;
    if (const_alpha == 256) {
        for (int y=0; y<h; ++y) {
            for (int x=0; x<w; ++x) {
                uint s = src[x];
                if (s >= 0xff000000)
                    dst[x] = s;
                else if (s != 0)
                    dst[x] = s + BYTE_MUL(dst[x], qAlpha(~s));
            }
            dst = (quint32 *)(((uchar *) dst) + dbpl);
            src = (const quint32 *)(((const uchar *) src) + sbpl);
        }
    } else if (const_alpha != 0) {
        const_alpha = (const_alpha * 255) >> 8;
        for (int y=0; y<h; ++y) {
            for (int x=0; x<w; ++x) {
                uint s = BYTE_MUL(src[x], const_alpha);
                dst[x] = s + BYTE_MUL(dst[x], qAlpha(~s));
            }
            dst = (quint32 *)(((uchar *) dst) + dbpl);
            src = (const quint32 *)(((const uchar *) src) + sbpl);
        }
    }
}


static void qt_blend_rgb32_on_rgb32(uchar *destPixels, int dbpl,
                             const uchar *srcPixels, int sbpl,
                             int w, int h,
                             int const_alpha)
{
#ifdef QT_DEBUG_DRAW
    fprintf(stdout, "qt_blend_rgb32_on_rgb32: dst=(%p, %d), src=(%p, %d), dim=(%d, %d) alpha=%d\n",
            destPixels, dbpl, srcPixels, sbpl, w, h, const_alpha);
    fflush(stdout);
#endif

    if (const_alpha != 256) {
        qt_blend_argb32_on_argb32(destPixels, dbpl, srcPixels, sbpl, w, h, const_alpha);
        return;
    }

    const uint *src = (const uint *) srcPixels;
    uint *dst = (uint *) destPixels;
    if (w <= 64) {
        for (int y=0; y<h; ++y) {
            qt_memconvert(dst, src, w);
            dst = (quint32 *)(((uchar *) dst) + dbpl);
            src = (const quint32 *)(((const uchar *) src) + sbpl);
        }
    } else {
        int len = w * 4;
        for (int y=0; y<h; ++y) {
            memcpy(dst, src, len);
            dst = (quint32 *)(((uchar *) dst) + dbpl);
            src = (const quint32 *)(((const uchar *) src) + sbpl);
        }
    }
}



struct Blend_RGB32_on_RGB32_NoAlpha {
    inline void write(quint32 *dst, quint32 src) { *dst = src; }
};

struct Blend_RGB32_on_RGB32_ConstAlpha {
    inline Blend_RGB32_on_RGB32_ConstAlpha(quint32 alpha) {
        m_alpha = (alpha * 255) >> 8;
        m_ialpha = 255 - m_alpha;
    }

    inline void write(quint32 *dst, quint32 src) {
        *dst = BYTE_MUL(src, m_alpha) + BYTE_MUL(*dst, m_ialpha);
    }

    quint32 m_alpha;
    quint32 m_ialpha;
};

struct Blend_ARGB32_on_ARGB32_SourceAlpha {
    inline void write(quint32 *dst, quint32 src) {
        *dst = src + BYTE_MUL(*dst, qAlpha(~src));
    }
};

struct Blend_ARGB32_on_ARGB32_SourceAndConstAlpha {
    inline Blend_ARGB32_on_ARGB32_SourceAndConstAlpha(quint32 alpha) {
        m_alpha = (alpha * 255) >> 8;
        m_ialpha = 255 - m_alpha;
    }

    inline void write(quint32 *dst, quint32 src) {
        src = BYTE_MUL(src, m_alpha);
        *dst = src + BYTE_MUL(*dst, qAlpha(~src));
    }

    quint32 m_alpha;
    quint32 m_ialpha;
};

template <typename T> void qt_scale_image_32bit(uchar *destPixels, int dbpl,
                                                const uchar *srcPixels, int sbpl,
                                                const QRectF &targetRect,
                                                const QRectF &srcRect,
                                                const QRect &clip,
                                                T blender)
{
    qreal sx = targetRect.width() / (qreal) srcRect.width();
    qreal sy = targetRect.height() / (qreal) srcRect.height();

    int ix = 0x00010000 / sx;
    int iy = 0x00010000 / sy;

//     qDebug() << "scale:" << endl
//              << " - target" << targetRect << endl
//              << " - source" << srcRect << endl
//              << " - clip" << clip << endl
//              << " - sx=" << sx << " sy=" << sy << " ix=" << ix << " iy=" << iy;

    int cx1 = clip.x();
    int cx2 = clip.x() + clip.width();
    int cy1 = clip.top();
    int cy2 = clip.y() + clip.height();

    int tx1 = qRound(targetRect.left());
    int tx2 = qRound(targetRect.right());
    int ty1 = qRound(targetRect.top());
    int ty2 = qRound(targetRect.bottom());

    if (tx2 < tx1)
        qSwap(tx2, tx1);

    if (ty2 < ty1)
        qSwap(ty2, ty1);

    if (tx1 < cx1)
        tx1 = cx1;

    if (tx2 >= cx2)
        tx2 = cx2;

    if (tx1 >= tx2)
        return;

    if (ty1 < cy1)
        ty1 = cy1;

    if (ty2 >= cy2)
       ty2 = cy2;

    if (ty1 >= ty2)
        return;

    int h = ty2 - ty1;
    int w = tx2 - tx1;

    quint32 basex;
    quint32 srcy;

    if (sx < 0) {
        int dstx = qFloor((tx1 + 0.5 - targetRect.right()) * ix) + 1;
        basex = quint32(srcRect.right() * 65536) + dstx;
    } else {
        int dstx = qCeil((tx1 + 0.5 - targetRect.left()) * ix) - 1;
        basex = quint32(srcRect.left() * 65536) + dstx;
    }
    if (sy < 0) {
        int dsty = qFloor((ty1 + 0.5 - targetRect.bottom()) * iy) + 1;
        srcy = quint32(srcRect.bottom() * 65536) + dsty;
    } else {
        int dsty = qCeil((ty1 + 0.5 - targetRect.top()) * iy) - 1;
        srcy = quint32(srcRect.top() * 65536) + dsty;
    }

    quint32 *dst = ((quint32 *) (destPixels + ty1 * dbpl)) + tx1;

    while (h--) {
        const uint *src = (const quint32 *) (srcPixels + (srcy >> 16) * sbpl);
        int srcx = basex;
        for (int x=0; x<w; ++x) {
            blender.write(&dst[x], src[srcx >> 16]);
            srcx += ix;
        }
        dst = (quint32 *)(((uchar *) dst) + dbpl);
        srcy += iy;
    }
}

void qt_scale_image_rgb32_on_rgb32(uchar *destPixels, int dbpl,
                                   const uchar *srcPixels, int sbpl,
                                   const QRectF &targetRect,
                                   const QRectF &sourceRect,
                                   const QRect &clip,
                                   int const_alpha)
{
#ifdef QT_DEBUG_DRAW
    printf("qt_scale_rgb32_on_rgb32: dst=(%p, %d), src=(%p, %d), target=(%d, %d), [%d x %d], src=(%d, %d) [%d x %d] alpha=%d\n",
           destPixels, dbpl, srcPixels, sbpl,
           targetRect.x(), targetRect.y(), targetRect.width(), targetRect.height(),
           sourceRect.x(), sourceRect.y(), sourceRect.width(), sourceRect.height(),
           const_alpha);
#endif
    if (const_alpha == 256) {
        Blend_RGB32_on_RGB32_NoAlpha noAlpha;
        qt_scale_image_32bit(destPixels, dbpl, srcPixels, sbpl,
                             targetRect, sourceRect, clip, noAlpha);
    } else {
        Blend_RGB32_on_RGB32_ConstAlpha constAlpha(const_alpha);
        qt_scale_image_32bit(destPixels, dbpl, srcPixels, sbpl,
                             targetRect, sourceRect, clip, constAlpha);
    }
}

void qt_scale_image_argb32_on_argb32(uchar *destPixels, int dbpl,
                                     const uchar *srcPixels, int sbpl,
                                     const QRectF &targetRect,
                                     const QRectF &sourceRect,
                                     const QRect &clip,
                                     int const_alpha)
{
#ifdef QT_DEBUG_DRAW
    printf("qt_scale_argb32_on_argb32: dst=(%p, %d), src=(%p, %d), target=(%d, %d), [%d x %d], src=(%d, %d) [%d x %d] alpha=%d\n",
           destPixels, dbpl, srcPixels, sbpl,
           targetRect.x(), targetRect.y(), targetRect.width(), targetRect.height(),
           sourceRect.x(), sourceRect.y(), sourceRect.width(), sourceRect.height(),
           const_alpha);
#endif
    if (const_alpha == 256) {
        Blend_ARGB32_on_ARGB32_SourceAlpha sourceAlpha;
        qt_scale_image_32bit(destPixels, dbpl, srcPixels, sbpl,
                             targetRect, sourceRect, clip, sourceAlpha);
    } else {
        Blend_ARGB32_on_ARGB32_SourceAndConstAlpha constAlpha(const_alpha);
        qt_scale_image_32bit(destPixels, dbpl, srcPixels, sbpl,
                             targetRect, sourceRect, clip, constAlpha);
    }
}

struct QTransformImageVertex
{
    qreal x, y, u, v; // destination coordinates (x, y) and source coordinates (u, v)
};

template <class SrcT, class DestT, class Blender>
void qt_transform_image_rasterize(DestT *destPixels, int dbpl,
                                  const SrcT *srcPixels, int sbpl,
                                  const QTransformImageVertex &topLeft, const QTransformImageVertex &bottomLeft,
                                  const QTransformImageVertex &topRight, const QTransformImageVertex &bottomRight,
                                  const QRect &sourceRect,
                                  const QRect &clip,
                                  qreal topY, qreal bottomY,
                                  int dudx, int dvdx, int dudy, int dvdy, int u0, int v0,
                                  Blender blender)
{
    int fromY = qMax(qRound(topY), clip.top());
    int toY = qMin(qRound(bottomY), clip.top() + clip.height());
    if (fromY >= toY)
        return;

    qreal leftSlope = (bottomLeft.x - topLeft.x) / (bottomLeft.y - topLeft.y);
    qreal rightSlope = (bottomRight.x - topRight.x) / (bottomRight.y - topRight.y);
    int dx_l = int(leftSlope * 0x10000);
    int dx_r = int(rightSlope * 0x10000);
    int x_l = int((topLeft.x + (0.5 + fromY - topLeft.y) * leftSlope + 0.5) * 0x10000);
    int x_r = int((topRight.x + (0.5 + fromY - topRight.y) * rightSlope + 0.5) * 0x10000);

    int fromX, toX, x1, x2, u, v, i, ii;
    DestT *line;
    for (int y = fromY; y < toY; ++y) {
        line = reinterpret_cast<DestT *>(reinterpret_cast<uchar *>(destPixels) + y * dbpl);

        fromX = qMax(x_l >> 16, clip.left());
        toX = qMin(x_r >> 16, clip.left() + clip.width());
        if (fromX < toX) {
            // Because of rounding, we can get source coordinates outside the source image.
            // Clamp these coordinates to the source rect to avoid segmentation fault and
            // garbage on the screen.

            // Find the first pixel on the current scan line where the source coordinates are within the source rect.
            x1 = fromX;
            u = x1 * dudx + y * dudy + u0;
            v = x1 * dvdx + y * dvdy + v0;
            for (; x1 < toX; ++x1) {
                int uu = u >> 16;
                int vv = v >> 16;
                if (uu >= sourceRect.left() && uu < sourceRect.left() + sourceRect.width()
                    && vv >= sourceRect.top() && vv < sourceRect.top() + sourceRect.height()) {
                    break;
                }
                u += dudx;
                v += dvdx;
            }

            // Find the last pixel on the current scan line where the source coordinates are within the source rect.
            x2 = toX;
            u = (x2 - 1) * dudx + y * dudy + u0;
            v = (x2 - 1) * dvdx + y * dvdy + v0;
            for (; x2 > x1; --x2) {
                int uu = u >> 16;
                int vv = v >> 16;
                if (uu >= sourceRect.left() && uu < sourceRect.left() + sourceRect.width()
                    && vv >= sourceRect.top() && vv < sourceRect.top() + sourceRect.height()) {
                    break;
                }
                u -= dudx;
                v -= dvdx;
            }

            // Set up values at the beginning of the scan line.
            u = fromX * dudx + y * dudy + u0;
            v = fromX * dvdx + y * dvdy + v0;
            line += fromX;

            // Beginning of the scan line, with per-pixel checks.
            i = x1 - fromX;
            while (i) {
                int uu = qBound(sourceRect.left(), u >> 16, sourceRect.left() + sourceRect.width() - 1);
                int vv = qBound(sourceRect.top(), v >> 16, sourceRect.top() + sourceRect.height() - 1);
                blender.write(line, reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + vv * sbpl)[uu]);
                u += dudx;
                v += dvdx;
                ++line;
                --i;
            }

            // Middle of the scan line, without checks.
            // Manual loop unrolling.
            i = x2 - x1;
            ii = i >> 3;
            while (ii) {
                blender.write(&line[0], reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx;
                blender.write(&line[1], reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx;
                blender.write(&line[2], reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx;
                blender.write(&line[3], reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx;
                blender.write(&line[4], reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx;
                blender.write(&line[5], reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx;
                blender.write(&line[6], reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx;
                blender.write(&line[7], reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx;
                line += 8;
                --ii;
            }
            switch (i & 7) {
                case 7: blender.write(line, reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx; ++line;
                case 6: blender.write(line, reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx; ++line;
                case 5: blender.write(line, reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx; ++line;
                case 4: blender.write(line, reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx; ++line;
                case 3: blender.write(line, reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx; ++line;
                case 2: blender.write(line, reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx; ++line;
                case 1: blender.write(line, reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + (v >> 16) * sbpl)[u >> 16]); u += dudx; v += dvdx; ++line;
            }

            // End of the scan line, with per-pixel checks.
            i = toX - x2;
            while (i) {
                int uu = qBound(sourceRect.left(), u >> 16, sourceRect.left() + sourceRect.width() - 1);
                int vv = qBound(sourceRect.top(), v >> 16, sourceRect.top() + sourceRect.height() - 1);
                blender.write(line, reinterpret_cast<const SrcT *>(reinterpret_cast<const uchar *>(srcPixels) + vv * sbpl)[uu]);
                u += dudx;
                v += dvdx;
                ++line;
                --i;
            }
        }
        x_l += dx_l;
        x_r += dx_r;
    }
}

template <class SrcT, class DestT, class Blender>
void qt_transform_image(DestT *destPixels, int dbpl,
                        const SrcT *srcPixels, int sbpl,
                        const QRectF &targetRect,
                        const QRectF &sourceRect,
                        const QRect &clip,
                        const QTransform &targetRectTransform,
                        Blender blender)
{
    enum Corner
    {
        TopLeft,
        TopRight,
        BottomRight,
        BottomLeft
    };

    // map source rectangle to destination.
    QTransformImageVertex v[4];
    v[TopLeft].u = v[BottomLeft].u = sourceRect.left();
    v[TopLeft].v = v[TopRight].v = sourceRect.top();
    v[TopRight].u = v[BottomRight].u = sourceRect.right();
    v[BottomLeft].v = v[BottomRight].v = sourceRect.bottom();
    targetRectTransform.map(targetRect.left(), targetRect.top(), &v[TopLeft].x, &v[TopLeft].y);
    targetRectTransform.map(targetRect.right(), targetRect.top(), &v[TopRight].x, &v[TopRight].y);
    targetRectTransform.map(targetRect.left(), targetRect.bottom(), &v[BottomLeft].x, &v[BottomLeft].y);
    targetRectTransform.map(targetRect.right(), targetRect.bottom(), &v[BottomRight].x, &v[BottomRight].y);

    // find topmost vertex.
    int topmost = 0;
    for (int i = 1; i < 4; ++i) {
        if (v[i].y < v[topmost].y)
            topmost = i;
    }
    // rearrange array such that topmost vertex is at index 0.
    switch (topmost) {
    case 1:
        {
            QTransformImageVertex t = v[0];
            for (int i = 0; i < 3; ++i)
                v[i] = v[i+1];
            v[3] = t;
        }
        break;
    case 2:
        qSwap(v[0], v[2]);
        qSwap(v[1], v[3]);
        break;
    case 3:
        {
            QTransformImageVertex t = v[3];
            for (int i = 3; i > 0; --i)
                v[i] = v[i-1];
            v[0] = t;
        }
        break;
    }

    // if necessary, swap vertex 1 and 3 such that 1 is to the left of 3.
    qreal dx1 = v[1].x - v[0].x;
    qreal dy1 = v[1].y - v[0].y;
    qreal dx2 = v[3].x - v[0].x;
    qreal dy2 = v[3].y - v[0].y;
    if (dx1 * dy2 - dx2 * dy1 > 0)
        qSwap(v[1], v[3]);

    QTransformImageVertex u = {v[1].x - v[0].x, v[1].y - v[0].y, v[1].u - v[0].u, v[1].v - v[0].v};
    QTransformImageVertex w = {v[2].x - v[0].x, v[2].y - v[0].y, v[2].u - v[0].u, v[2].v - v[0].v};

    qreal det = u.x * w.y - u.y * w.x;
    if (det == 0)
        return;

    qreal invDet = 1.0 / det;
    qreal m11, m12, m21, m22, mdx, mdy;

    m11 = (u.u * w.y - u.y * w.u) * invDet;
    m12 = (u.x * w.u - u.u * w.x) * invDet;
    m21 = (u.v * w.y - u.y * w.v) * invDet;
    m22 = (u.x * w.v - u.v * w.x) * invDet;
    mdx = v[0].u - m11 * v[0].x - m12 * v[0].y;
    mdy = v[0].v - m21 * v[0].x - m22 * v[0].y;

    int dudx = int(m11 * 0x10000);
    int dvdx = int(m21 * 0x10000);
    int dudy = int(m12 * 0x10000);
    int dvdy = int(m22 * 0x10000);
    int u0 = qCeil((0.5 * m11 + 0.5 * m12 + mdx) * 0x10000) - 1;
    int v0 = qCeil((0.5 * m21 + 0.5 * m22 + mdy) * 0x10000) - 1;

    int x1 = qFloor(sourceRect.left());
    int y1 = qFloor(sourceRect.top());
    int x2 = qCeil(sourceRect.right());
    int y2 = qCeil(sourceRect.bottom());
    QRect sourceRectI(x1, y1, x2 - x1, y2 - y1);

    // rasterize trapezoids.
    if (v[1].y < v[3].y) {
        qt_transform_image_rasterize(destPixels, dbpl, srcPixels, sbpl, v[0], v[1], v[0], v[3], sourceRectI, clip, v[0].y, v[1].y, dudx, dvdx, dudy, dvdy, u0, v0, blender);
        qt_transform_image_rasterize(destPixels, dbpl, srcPixels, sbpl, v[1], v[2], v[0], v[3], sourceRectI, clip, v[1].y, v[3].y, dudx, dvdx, dudy, dvdy, u0, v0, blender);
        qt_transform_image_rasterize(destPixels, dbpl, srcPixels, sbpl, v[1], v[2], v[3], v[2], sourceRectI, clip, v[3].y, v[2].y, dudx, dvdx, dudy, dvdy, u0, v0, blender);
    } else {
        qt_transform_image_rasterize(destPixels, dbpl, srcPixels, sbpl, v[0], v[1], v[0], v[3], sourceRectI, clip, v[0].y, v[3].y, dudx, dvdx, dudy, dvdy, u0, v0, blender);
        qt_transform_image_rasterize(destPixels, dbpl, srcPixels, sbpl, v[0], v[1], v[3], v[2], sourceRectI, clip, v[3].y, v[1].y, dudx, dvdx, dudy, dvdy, u0, v0, blender);
        qt_transform_image_rasterize(destPixels, dbpl, srcPixels, sbpl, v[1], v[2], v[3], v[2], sourceRectI, clip, v[1].y, v[2].y, dudx, dvdx, dudy, dvdy, u0, v0, blender);
    }
}

void qt_transform_image_rgb16_on_rgb16(uchar *destPixels, int dbpl,
                                       const uchar *srcPixels, int sbpl,
                                       const QRectF &targetRect,
                                       const QRectF &sourceRect,
                                       const QRect &clip,
                                       const QTransform &targetRectTransform,
                                       int const_alpha)
{
    if (const_alpha == 256) {
        Blend_RGB16_on_RGB16_NoAlpha noAlpha;
        qt_transform_image(reinterpret_cast<quint16 *>(destPixels), dbpl,
                           reinterpret_cast<const quint16 *>(srcPixels), sbpl,
                           targetRect, sourceRect, clip, targetRectTransform, noAlpha);
    } else {
        Blend_RGB16_on_RGB16_ConstAlpha constAlpha(const_alpha);
        qt_transform_image(reinterpret_cast<quint16 *>(destPixels), dbpl,
                           reinterpret_cast<const quint16 *>(srcPixels), sbpl,
                           targetRect, sourceRect, clip, targetRectTransform, constAlpha);
    }
}

void qt_transform_image_argb24_on_rgb16(uchar *destPixels, int dbpl,
                                        const uchar *srcPixels, int sbpl,
                                        const QRectF &targetRect,
                                        const QRectF &sourceRect,
                                        const QRect &clip,
                                        const QTransform &targetRectTransform,
                                        int const_alpha)
{
    if (const_alpha == 256) {
        Blend_ARGB24_on_RGB16_SourceAlpha noAlpha;
        qt_transform_image(reinterpret_cast<quint16 *>(destPixels), dbpl,
                           reinterpret_cast<const qargb8565 *>(srcPixels), sbpl,
                           targetRect, sourceRect, clip, targetRectTransform, noAlpha);
    } else {
        Blend_ARGB24_on_RGB16_SourceAndConstAlpha constAlpha(const_alpha);
        qt_transform_image(reinterpret_cast<quint16 *>(destPixels), dbpl,
                           reinterpret_cast<const qargb8565 *>(srcPixels), sbpl,
                           targetRect, sourceRect, clip, targetRectTransform, constAlpha);
    }
}


void qt_transform_image_argb32_on_rgb16(uchar *destPixels, int dbpl,
                                        const uchar *srcPixels, int sbpl,
                                        const QRectF &targetRect,
                                        const QRectF &sourceRect,
                                        const QRect &clip,
                                        const QTransform &targetRectTransform,
                                        int const_alpha)
{
    if (const_alpha == 256) {
        Blend_ARGB32_on_RGB16_SourceAlpha noAlpha;
        qt_transform_image(reinterpret_cast<quint16 *>(destPixels), dbpl,
                           reinterpret_cast<const quint32 *>(srcPixels), sbpl,
                           targetRect, sourceRect, clip, targetRectTransform, noAlpha);
    } else {
        Blend_ARGB32_on_RGB16_SourceAndConstAlpha constAlpha(const_alpha);
        qt_transform_image(reinterpret_cast<quint16 *>(destPixels), dbpl,
                           reinterpret_cast<const quint32 *>(srcPixels), sbpl,
                           targetRect, sourceRect, clip, targetRectTransform, constAlpha);
    }
}


void qt_transform_image_rgb32_on_rgb32(uchar *destPixels, int dbpl,
                                       const uchar *srcPixels, int sbpl,
                                       const QRectF &targetRect,
                                       const QRectF &sourceRect,
                                       const QRect &clip,
                                       const QTransform &targetRectTransform,
                                       int const_alpha)
{
    if (const_alpha == 256) {
        Blend_RGB32_on_RGB32_NoAlpha noAlpha;
        qt_transform_image(reinterpret_cast<quint32 *>(destPixels), dbpl,
                           reinterpret_cast<const quint32 *>(srcPixels), sbpl,
                           targetRect, sourceRect, clip, targetRectTransform, noAlpha);
    } else {
        Blend_RGB32_on_RGB32_ConstAlpha constAlpha(const_alpha);
        qt_transform_image(reinterpret_cast<quint32 *>(destPixels), dbpl,
                           reinterpret_cast<const quint32 *>(srcPixels), sbpl,
                           targetRect, sourceRect, clip, targetRectTransform, constAlpha);
    }
}

void qt_transform_image_argb32_on_argb32(uchar *destPixels, int dbpl,
                                         const uchar *srcPixels, int sbpl,
                                         const QRectF &targetRect,
                                         const QRectF &sourceRect,
                                         const QRect &clip,
                                         const QTransform &targetRectTransform,
                                         int const_alpha)
{
    if (const_alpha == 256) {
        Blend_ARGB32_on_ARGB32_SourceAlpha sourceAlpha;
        qt_transform_image(reinterpret_cast<quint32 *>(destPixels), dbpl,
                           reinterpret_cast<const quint32 *>(srcPixels), sbpl,
                           targetRect, sourceRect, clip, targetRectTransform, sourceAlpha);
    } else {
        Blend_ARGB32_on_ARGB32_SourceAndConstAlpha constAlpha(const_alpha);
        qt_transform_image(reinterpret_cast<quint32 *>(destPixels), dbpl,
                           reinterpret_cast<const quint32 *>(srcPixels), sbpl,
                           targetRect, sourceRect, clip, targetRectTransform, constAlpha);
    }
}

SrcOverScaleFunc qScaleFunctions[QImage::NImageFormats][QImage::NImageFormats] = {
    {   // Format_Invalid
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_Mono
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_MonoLSB
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_Indexed8
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB32
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        qt_scale_image_rgb32_on_rgb32,      // Format_RGB32,
        0,      // Format_ARGB32,
        qt_scale_image_argb32_on_argb32,    // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB32
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB32_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        qt_scale_image_rgb32_on_rgb32,          // Format_RGB32,
        0,      // Format_ARGB32,
        qt_scale_image_argb32_on_argb32,        // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB16
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        qt_scale_image_argb32_on_rgb16,       // Format_ARGB32_Premultiplied,
        qt_scale_image_rgb16_on_rgb16,        // Format_RGB16,
        qt_scale_image_argb24_on_rgb16,       // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB8565_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB666
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB6666_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB555
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB8555_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB888
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB444
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB4444_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    }
};


SrcOverBlendFunc qBlendFunctions[QImage::NImageFormats][QImage::NImageFormats] = {
    {   // Format_Invalid
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_Mono
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_MonoLSB
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_Indexed8
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB32
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        qt_blend_rgb32_on_rgb32,        // Format_RGB32,
        0,      // Format_ARGB32,
        qt_blend_argb32_on_argb32,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB32
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB32_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        qt_blend_rgb32_on_rgb32,        // Format_RGB32,
        0,      // Format_ARGB32,
        qt_blend_argb32_on_argb32,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB16
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        qt_blend_rgb32_on_rgb16,  // Format_RGB32,
        0,      // Format_ARGB32,
        qt_blend_argb32_on_rgb16, // Format_ARGB32_Premultiplied,
        qt_blend_rgb16_on_rgb16,  // Format_RGB16,
        qt_blend_argb24_on_rgb16, // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB8565_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB666
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB6666_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB555
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB8555_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB888
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB444
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB4444_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    }
};

SrcOverTransformFunc qTransformFunctions[QImage::NImageFormats][QImage::NImageFormats] = {
    {   // Format_Invalid
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_Mono
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_MonoLSB
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_Indexed8
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB32
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        qt_transform_image_rgb32_on_rgb32,      // Format_RGB32,
        0,      // Format_ARGB32,
        qt_transform_image_argb32_on_argb32,    // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB32
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB32_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        qt_transform_image_rgb32_on_rgb32,          // Format_RGB32,
        0,      // Format_ARGB32,
        qt_transform_image_argb32_on_argb32,        // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB16
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        qt_transform_image_argb32_on_rgb16,       // Format_ARGB32_Premultiplied,
        qt_transform_image_rgb16_on_rgb16,        // Format_RGB16,
        qt_transform_image_argb24_on_rgb16,       // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB8565_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB666
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB6666_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB555
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB8555_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB888
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_RGB444
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    },
    {   // Format_ARGB4444_Premultiplied
        0,      // Format_Invalid,
        0,      // Format_Mono,
        0,      // Format_MonoLSB,
        0,      // Format_Indexed8,
        0,      // Format_RGB32,
        0,      // Format_ARGB32,
        0,      // Format_ARGB32_Premultiplied,
        0,      // Format_RGB16,
        0,      // Format_ARGB8565_Premultiplied,
        0,      // Format_RGB666,
        0,      // Format_ARGB6666_Premultiplied,
        0,      // Format_RGB555,
        0,      // Format_ARGB8555_Premultiplied,
        0,      // Format_RGB888,
        0,      // Format_RGB444,
        0       // Format_ARGB4444_Premultiplied,
    }
};

QT_END_NAMESPACE
