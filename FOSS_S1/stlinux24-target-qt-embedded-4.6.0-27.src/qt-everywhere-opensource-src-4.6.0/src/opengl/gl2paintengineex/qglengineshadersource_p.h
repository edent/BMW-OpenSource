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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//


#ifndef QGL_ENGINE_SHADER_SOURCE_H
#define QGL_ENGINE_SHADER_SOURCE_H

#include "qglengineshadermanager_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(OpenGL)


static const char* const qglslMainVertexShader = "\
    uniform   highp float   depth;\
    void setPosition();\
    void main(void)\
    {\
            setPosition();\
            gl_Position.z = depth * gl_Position.w;\
    }";

static const char* const qglslMainWithTexCoordsVertexShader = "\
    attribute highp  vec2 textureCoordArray; \
    varying   highp  vec2 textureCoords; \
    uniform   highp float   depth;\
    void setPosition();\
    void main(void) \
    {\
            setPosition();\
            gl_Position.z = depth * gl_Position.w;\
            textureCoords = textureCoordArray; \
    }";

static const char* const qglslMainWithTexCoordsAndOpacityVertexShader = "\
    attribute highp vec2  textureCoordArray; \
    attribute lowp  float opacityArray; \
    varying   highp vec2  textureCoords; \
    varying   lowp  float opacity; \
    uniform   highp float depth; \
    void setPosition(); \
    void main(void) \
    { \
            setPosition(); \
            gl_Position.z = depth * gl_Position.w; \
            textureCoords = textureCoordArray; \
            opacity = opacityArray; \
    }";

static const char* const qglslPositionOnlyVertexShader = "\
    attribute highp vec4    vertexCoordsArray;\
    uniform   highp mat4    pmvMatrix;\
    void setPosition(void)\
    {\
            gl_Position = pmvMatrix * vertexCoordsArray;\
    }";

static const char* const qglslUntransformedPositionVertexShader = "\
    attribute highp vec4    vertexCoordsArray;\
    void setPosition(void)\
    {\
            gl_Position = vertexCoordsArray;\
    }";

// Pattern Brush - This assumes the texture size is 8x8 and thus, the inverted size is 0.125
static const char* const qglslPositionWithPatternBrushVertexShader = "\
    attribute highp   vec4  vertexCoordsArray; \
    uniform   highp   mat4  pmvMatrix; \
    uniform   mediump vec2  halfViewportSize; \
    uniform   highp   vec2  invertedTextureSize; \
    uniform   highp   mat3  brushTransform; \
    varying   highp   vec2  patternTexCoords; \
    void setPosition(void) { \
            gl_Position = pmvMatrix * vertexCoordsArray;\
            gl_Position.xy = gl_Position.xy / gl_Position.w; \
            mediump vec2 viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
            mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
            mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
            gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
            gl_Position.w = invertedHTexCoordsZ; \
            patternTexCoords.xy = (hTexCoords.xy * 0.125) * invertedHTexCoordsZ; \
    }";

static const char* const qglslAffinePositionWithPatternBrushVertexShader
                 = qglslPositionWithPatternBrushVertexShader;

static const char* const qglslPatternBrushSrcFragmentShader = "\
    uniform lowp    sampler2D brushTexture;\
    uniform lowp    vec4      patternColor; \
    varying highp   vec2      patternTexCoords;\
    lowp vec4 srcPixel() { \
        return patternColor * (1.0 - texture2D(brushTexture, patternTexCoords).r); \
    }\n";


// Linear Gradient Brush
static const char* const qglslPositionWithLinearGradientBrushVertexShader = "\
    attribute highp   vec4  vertexCoordsArray; \
    uniform   highp   mat4  pmvMatrix; \
    uniform   mediump vec2  halfViewportSize; \
    uniform   highp   vec3  linearData; \
    uniform   highp   mat3  brushTransform; \
    varying   mediump float index; \
    void setPosition() { \
        gl_Position = pmvMatrix * vertexCoordsArray;\
        gl_Position.xy = gl_Position.xy / gl_Position.w; \
        mediump vec2 viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
        mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
        mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
        gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
        gl_Position.w = invertedHTexCoordsZ; \
        index = (dot(linearData.xy, hTexCoords.xy) * linearData.z) * invertedHTexCoordsZ; \
    }";

static const char* const qglslAffinePositionWithLinearGradientBrushVertexShader
                 = qglslPositionWithLinearGradientBrushVertexShader;

static const char* const qglslLinearGradientBrushSrcFragmentShader = "\
    uniform lowp    sampler2D   brushTexture; \
    varying mediump float       index; \
    lowp vec4 srcPixel() { \
        mediump vec2 val = vec2(index, 0.5); \
        return texture2D(brushTexture, val); \
    }\n";


// Conical Gradient Brush
static const char* const qglslPositionWithConicalGradientBrushVertexShader = "\
    attribute highp   vec4  vertexCoordsArray;\
    uniform   highp   mat4  pmvMatrix;\
    uniform   mediump vec2  halfViewportSize; \
    uniform   highp   mat3  brushTransform; \
    varying   highp   vec2  A; \
    void setPosition(void)\
    {\
        gl_Position = pmvMatrix * vertexCoordsArray;\
        gl_Position.xy = gl_Position.xy / gl_Position.w; \
        mediump vec2  viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
        mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
        mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
        gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
        gl_Position.w = invertedHTexCoordsZ; \
        A = hTexCoords.xy * invertedHTexCoordsZ; \
    }";

static const char* const qglslAffinePositionWithConicalGradientBrushVertexShader
                 = qglslPositionWithConicalGradientBrushVertexShader;

static const char* const qglslConicalGradientBrushSrcFragmentShader = "\n\
    #define INVERSE_2PI 0.1591549430918953358 \n\
    uniform lowp    sampler2D   brushTexture; \n\
    uniform mediump float       angle; \
    varying highp   vec2        A; \
    lowp vec4 srcPixel() { \
        highp float t; \
        if (abs(A.y) == abs(A.x)) \
            t = (atan(-A.y + 0.002, A.x) + angle) * INVERSE_2PI; \
        else \
            t = (atan(-A.y, A.x) + angle) * INVERSE_2PI; \
        return texture2D(brushTexture, vec2(t - floor(t), 0.5)); \
    }";


// Radial Gradient Brush
static const char* const qglslPositionWithRadialGradientBrushVertexShader = "\
    attribute highp   vec4 vertexCoordsArray;\
    uniform   highp   mat4 pmvMatrix;\
    uniform   mediump vec2 halfViewportSize; \
    uniform   highp   mat3 brushTransform; \
    uniform   highp   vec2 fmp; \
    varying   highp   float b; \
    varying   highp   vec2  A; \
    void setPosition(void) \
    {\
        gl_Position = pmvMatrix * vertexCoordsArray;\
        gl_Position.xy = gl_Position.xy / gl_Position.w; \
        mediump vec2 viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
        mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
        mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
        gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
        gl_Position.w = invertedHTexCoordsZ; \
        A = hTexCoords.xy * invertedHTexCoordsZ; \
        b = 2.0 * dot(A, fmp); \
    }";

static const char* const qglslAffinePositionWithRadialGradientBrushVertexShader
                 = qglslPositionWithRadialGradientBrushVertexShader;

static const char* const qglslRadialGradientBrushSrcFragmentShader = "\
    uniform lowp    sampler2D   brushTexture; \
    uniform highp   float       fmp2_m_radius2; \
    uniform highp   float       inverse_2_fmp2_m_radius2; \
    varying highp   float       b; \
    varying highp   vec2        A; \
    lowp vec4 srcPixel() { \
        highp float c = -dot(A, A); \
        highp vec2 val = vec2((-b + sqrt(b*b - 4.0*fmp2_m_radius2*c)) * inverse_2_fmp2_m_radius2, 0.5); \
        return texture2D(brushTexture, val); \
    }";


// Texture Brush
static const char* const qglslPositionWithTextureBrushVertexShader = "\
    attribute highp   vec4  vertexCoordsArray; \
    uniform   highp   mat4  pmvMatrix; \
    uniform   mediump vec2  halfViewportSize; \
    uniform   highp   vec2  invertedTextureSize; \
    uniform   highp   mat3  brushTransform; \
    varying   highp   vec2  textureCoords; \
    void setPosition(void) { \
            gl_Position = pmvMatrix * vertexCoordsArray;\
            gl_Position.xy = gl_Position.xy / gl_Position.w; \
            mediump vec2 viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
            mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
            mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
            gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
            gl_Position.w = invertedHTexCoordsZ; \
            textureCoords.xy = (hTexCoords.xy * invertedTextureSize) * gl_Position.w; \
    }";

static const char* const qglslAffinePositionWithTextureBrushVertexShader
                 = qglslPositionWithTextureBrushVertexShader;

#if defined(QT_OPENGL_ES_2)
// OpenGL ES does not support GL_REPEAT wrap modes for NPOT textures. So instead,
// we emulate GL_REPEAT by only taking the fractional part of the texture coords.
// TODO: Special case POT textures which don't need this emulation
static const char* const qglslTextureBrushSrcFragmentShader = "\
    varying highp   vec2      textureCoords; \
    uniform lowp    sampler2D brushTexture; \
    lowp vec4 srcPixel() { \
        return texture2D(brushTexture, fract(textureCoords)); \
    }";
#else
static const char* const qglslTextureBrushSrcFragmentShader = "\
    varying highp   vec2      textureCoords; \
    uniform lowp    sampler2D brushTexture; \
    lowp vec4 srcPixel() { \
        return texture2D(brushTexture, textureCoords); \
    }";
#endif

static const char* const qglslTextureBrushSrcWithPatternFragmentShader = "\
    varying highp   vec2      textureCoords; \
    uniform lowp    vec4      patternColor; \
    uniform lowp    sampler2D brushTexture; \
    lowp vec4 srcPixel() { \
        return patternColor * (1.0 - texture2D(brushTexture, textureCoords).r); \
    }";

// Solid Fill Brush
static const char* const qglslSolidBrushSrcFragmentShader = "\
    uniform lowp vec4 fragmentColor; \
    lowp vec4 srcPixel() { \
        return fragmentColor; \
    }";

static const char* const qglslImageSrcFragmentShader = "\
    varying highp   vec2    textureCoords; \
    uniform lowp  sampler2D imageTexture; \
    lowp vec4 srcPixel() { \
        return texture2D(imageTexture, textureCoords); \
    }";

static const char* const qglslCustomSrcFragmentShader = "\
    varying highp vec2      textureCoords; \
    uniform lowp  sampler2D imageTexture; \
    lowp vec4 customShader(lowp sampler2D texture, highp vec2 coords); \
    lowp vec4 srcPixel() { \
        return customShader(imageTexture, textureCoords); \
    }";

static const char* const qglslImageSrcWithPatternFragmentShader = "\
    varying highp vec2      textureCoords; \
    uniform lowp  vec4      patternColor; \
    uniform lowp  sampler2D imageTexture; \
    lowp vec4 srcPixel() { \
        return patternColor * (1.0 - texture2D(imageTexture, textureCoords).r); \
    }\n";

static const char* const qglslNonPremultipliedImageSrcFragmentShader = "\
    varying highp vec2      textureCoords; \
    uniform lowp  sampler2D imageTexture; \
    lowp vec4 srcPixel() { \
        lowp vec4 sample = texture2D(imageTexture, textureCoords); \
        sample.rgb = sample.rgb * sample.a; \
        return sample; \
    }";

static const char* const qglslShockingPinkSrcFragmentShader = "\
    lowp vec4 srcPixel() { \
        return vec4(0.98, 0.06, 0.75, 1.0); \
    }";

static const char* const qglslMainFragmentShader_ImageArrays = "\
    varying lowp float opacity; \
    lowp vec4 srcPixel(); \
    void main() { \
        gl_FragColor = srcPixel() * opacity; \
    }";

static const char* const qglslMainFragmentShader_CMO = "\
    uniform lowp float globalOpacity; \
    lowp vec4 srcPixel(); \
    lowp vec4 applyMask(lowp vec4); \
    lowp vec4 compose(lowp vec4); \
    void main() { \
        gl_FragColor = applyMask(compose(srcPixel()*globalOpacity))); \
    }";

static const char* const qglslMainFragmentShader_CM = "\
    lowp vec4 srcPixel(); \
    lowp vec4 applyMask(lowp vec4); \
    lowp vec4 compose(lowp vec4); \
    void main() { \
        gl_FragColor = applyMask(compose(srcPixel())); \
    }";

static const char* const qglslMainFragmentShader_MO = "\
    uniform lowp float globalOpacity; \
    lowp vec4 srcPixel(); \
    lowp vec4 applyMask(lowp vec4); \
    void main() { \
        gl_FragColor = applyMask(srcPixel()*globalOpacity); \
    }";

static const char* const qglslMainFragmentShader_M = "\
    lowp vec4 srcPixel(); \
    lowp vec4 applyMask(lowp vec4); \
    void main() { \
        gl_FragColor = applyMask(srcPixel()); \
    }";

static const char* const qglslMainFragmentShader_CO = "\
    uniform lowp float globalOpacity; \
    lowp vec4 srcPixel(); \
    lowp vec4 compose(lowp vec4); \
    void main() { \
        gl_FragColor = compose(srcPixel()*globalOpacity); \
    }";

static const char* const qglslMainFragmentShader_C = "\
    lowp vec4 srcPixel(); \
    lowp vec4 compose(lowp vec4); \
    void main() { \
        gl_FragColor = compose(srcPixel()); \
    }";

static const char* const qglslMainFragmentShader_O = "\
    uniform lowp float globalOpacity; \
    lowp vec4 srcPixel(); \
    void main() { \
        gl_FragColor = srcPixel()*globalOpacity; \
    }";

static const char* const qglslMainFragmentShader = "\
    lowp vec4 srcPixel(); \
    void main() { \
        gl_FragColor = srcPixel(); \
    }";

static const char* const qglslMaskFragmentShader = "\
    varying highp   vec2      textureCoords;\
    uniform lowp    sampler2D maskTexture;\
    lowp vec4 applyMask(lowp vec4 src) \
    {\
        lowp vec4 mask = texture2D(maskTexture, textureCoords); \
        return src * mask.a; \
    }";

// For source over with subpixel antialiasing, the final color is calculated per component as follows
// (.a is alpha component, .c is red, green or blue component):
// alpha = src.a * mask.c * opacity
// dest.c = dest.c * (1 - alpha) + src.c * alpha
//
// In the first pass, calculate: dest.c = dest.c * (1 - alpha) with blend funcs: zero, 1 - source color
// In the second pass, calculate: dest.c = dest.c + src.c * alpha with blend funcs: one, one
//
// If source is a solid color (src is constant), only the first pass is needed, with blend funcs: constant, 1 - source color

// For source composition with subpixel antialiasing, the final color is calculated per component as follows:
// alpha = src.a * mask.c * opacity
// dest.c = dest.c * (1 - mask.c) + src.c * alpha
//

static const char* const qglslRgbMaskFragmentShaderPass1 = "\
    varying highp   vec2      textureCoords;\
    uniform lowp    sampler2D maskTexture;\
    lowp vec4 applyMask(lowp vec4 src) \
    {\
        lowp vec4 mask = texture2D(maskTexture, textureCoords); \
        return src.a * mask; \
    }";

static const char* const qglslRgbMaskFragmentShaderPass2 = "\
    varying highp   vec2      textureCoords;\
    uniform lowp    sampler2D maskTexture;\
    lowp vec4 applyMask(lowp vec4 src) \
    {\
        lowp vec4 mask = texture2D(maskTexture, textureCoords); \
        return src * mask; \
    }";

/*
    Left to implement:
        RgbMaskFragmentShader,
        RgbMaskWithGammaFragmentShader,

        MultiplyCompositionModeFragmentShader,
        ScreenCompositionModeFragmentShader,
        OverlayCompositionModeFragmentShader,
        DarkenCompositionModeFragmentShader,
        LightenCompositionModeFragmentShader,
        ColorDodgeCompositionModeFragmentShader,
        ColorBurnCompositionModeFragmentShader,
        HardLightCompositionModeFragmentShader,
        SoftLightCompositionModeFragmentShader,
        DifferenceCompositionModeFragmentShader,
        ExclusionCompositionModeFragmentShader,
*/

QT_END_NAMESPACE

QT_END_HEADER

#endif // GLGC_SHADER_SOURCE_H
