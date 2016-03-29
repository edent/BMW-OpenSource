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

#include "qglengineshadermanager_p.h"
#include "qglengineshadersource_p.h"

#if defined(QT_DEBUG)
#include <QMetaEnum>
#endif


QT_BEGIN_NAMESPACE

static void qt_shared_shaders_free(void *data)
{
    delete reinterpret_cast<QGLEngineSharedShaders *>(data);
}

Q_GLOBAL_STATIC_WITH_ARGS(QGLContextResource, qt_shared_shaders, (qt_shared_shaders_free))

QGLEngineSharedShaders *QGLEngineSharedShaders::shadersForContext(const QGLContext *context)
{
    QGLEngineSharedShaders *p = reinterpret_cast<QGLEngineSharedShaders *>(qt_shared_shaders()->value(context));
    if (!p) {
        QGLShareContextScope scope(context);
        qt_shared_shaders()->insert(context, p = new QGLEngineSharedShaders(context));
    }
    return p;
}

const char* QGLEngineSharedShaders::qShaderSnippets[] = {
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0
};

QGLEngineSharedShaders::QGLEngineSharedShaders(const QGLContext* context)
    : ctxGuard(context)
    , blitShaderProg(0)
    , simpleShaderProg(0)
{

/*
    Rather than having the shader source array statically initialised, it is initialised
    here instead. This is to allow new shader names to be inserted or existing names moved
    around without having to change the order of the glsl strings. It is hoped this will
    make future hard-to-find runtime bugs more obvious and generally give more solid code.
*/
    static bool snippetsPopulated = false;
    if (!snippetsPopulated) {

        const char** code = qShaderSnippets; // shortcut

        code[MainVertexShader] = qglslMainVertexShader;
        code[MainWithTexCoordsVertexShader] = qglslMainWithTexCoordsVertexShader;
        code[MainWithTexCoordsAndOpacityVertexShader] = qglslMainWithTexCoordsAndOpacityVertexShader;

        code[UntransformedPositionVertexShader] = qglslUntransformedPositionVertexShader;
        code[PositionOnlyVertexShader] = qglslPositionOnlyVertexShader;
        code[PositionWithPatternBrushVertexShader] = qglslPositionWithPatternBrushVertexShader;
        code[PositionWithLinearGradientBrushVertexShader] = qglslPositionWithLinearGradientBrushVertexShader;
        code[PositionWithConicalGradientBrushVertexShader] = qglslPositionWithConicalGradientBrushVertexShader;
        code[PositionWithRadialGradientBrushVertexShader] = qglslPositionWithRadialGradientBrushVertexShader;
        code[PositionWithTextureBrushVertexShader] = qglslPositionWithTextureBrushVertexShader;
        code[AffinePositionWithPatternBrushVertexShader] = qglslAffinePositionWithPatternBrushVertexShader;
        code[AffinePositionWithLinearGradientBrushVertexShader] = qglslAffinePositionWithLinearGradientBrushVertexShader;
        code[AffinePositionWithConicalGradientBrushVertexShader] = qglslAffinePositionWithConicalGradientBrushVertexShader;
        code[AffinePositionWithRadialGradientBrushVertexShader] = qglslAffinePositionWithRadialGradientBrushVertexShader;
        code[AffinePositionWithTextureBrushVertexShader] = qglslAffinePositionWithTextureBrushVertexShader;

        code[MainFragmentShader_CMO] = qglslMainFragmentShader_CMO;
        code[MainFragmentShader_CM] = qglslMainFragmentShader_CM;
        code[MainFragmentShader_MO] = qglslMainFragmentShader_MO;
        code[MainFragmentShader_M] = qglslMainFragmentShader_M;
        code[MainFragmentShader_CO] = qglslMainFragmentShader_CO;
        code[MainFragmentShader_C] = qglslMainFragmentShader_C;
        code[MainFragmentShader_O] = qglslMainFragmentShader_O;
        code[MainFragmentShader] = qglslMainFragmentShader;
        code[MainFragmentShader_ImageArrays] = qglslMainFragmentShader_ImageArrays;

        code[ImageSrcFragmentShader] = qglslImageSrcFragmentShader;
        code[ImageSrcWithPatternFragmentShader] = qglslImageSrcWithPatternFragmentShader;
        code[NonPremultipliedImageSrcFragmentShader] = qglslNonPremultipliedImageSrcFragmentShader;
        code[CustomImageSrcFragmentShader] = qglslCustomSrcFragmentShader; // Calls "customShader", which must be appended
        code[SolidBrushSrcFragmentShader] = qglslSolidBrushSrcFragmentShader;
        code[TextureBrushSrcFragmentShader] = qglslTextureBrushSrcFragmentShader;
        code[TextureBrushSrcWithPatternFragmentShader] = qglslTextureBrushSrcWithPatternFragmentShader;
        code[PatternBrushSrcFragmentShader] = qglslPatternBrushSrcFragmentShader;
        code[LinearGradientBrushSrcFragmentShader] = qglslLinearGradientBrushSrcFragmentShader;
        code[RadialGradientBrushSrcFragmentShader] = qglslRadialGradientBrushSrcFragmentShader;
        code[ConicalGradientBrushSrcFragmentShader] = qglslConicalGradientBrushSrcFragmentShader;
        code[ShockingPinkSrcFragmentShader] = qglslShockingPinkSrcFragmentShader;

        code[NoMaskFragmentShader] = "";
        code[MaskFragmentShader] = qglslMaskFragmentShader;
        code[RgbMaskFragmentShaderPass1] = qglslRgbMaskFragmentShaderPass1;
        code[RgbMaskFragmentShaderPass2] = qglslRgbMaskFragmentShaderPass2;
        code[RgbMaskWithGammaFragmentShader] = ""; //###

        code[NoCompositionModeFragmentShader] = "";
        code[MultiplyCompositionModeFragmentShader] = ""; //###
        code[ScreenCompositionModeFragmentShader] = ""; //###
        code[OverlayCompositionModeFragmentShader] = ""; //###
        code[DarkenCompositionModeFragmentShader] = ""; //###
        code[LightenCompositionModeFragmentShader] = ""; //###
        code[ColorDodgeCompositionModeFragmentShader] = ""; //###
        code[ColorBurnCompositionModeFragmentShader] = ""; //###
        code[HardLightCompositionModeFragmentShader] = ""; //###
        code[SoftLightCompositionModeFragmentShader] = ""; //###
        code[DifferenceCompositionModeFragmentShader] = ""; //###
        code[ExclusionCompositionModeFragmentShader] = ""; //###

#if defined(QT_DEBUG)
        // Check that all the elements have been filled:
        for (int i = 0; i < TotalSnippetCount; ++i) {
            if (qShaderSnippets[i] == 0) {
                qFatal("Shader snippet for %s (#%d) is missing!",
                       snippetNameStr(SnippetName(i)).constData(), i);
            }
        }
#endif
        snippetsPopulated = true;
    }

    QGLShader* fragShader;
    QGLShader* vertexShader;
    QByteArray source;

    // Compile up the simple shader:
    source.clear();
    source.append(qShaderSnippets[MainVertexShader]);
    source.append(qShaderSnippets[PositionOnlyVertexShader]);
    vertexShader = new QGLShader(QGLShader::Vertex, context, this);
    vertexShader->compileSourceCode(source);

    source.clear();
    source.append(qShaderSnippets[MainFragmentShader]);
    source.append(qShaderSnippets[ShockingPinkSrcFragmentShader]);
    fragShader = new QGLShader(QGLShader::Fragment, context, this);
    fragShader->compileSourceCode(source);

    simpleShaderProg = new QGLShaderProgram(context, this);
    simpleShaderProg->addShader(vertexShader);
    simpleShaderProg->addShader(fragShader);
    simpleShaderProg->bindAttributeLocation("vertexCoordsArray", QT_VERTEX_COORDS_ATTR);
    simpleShaderProg->link();
    if (!simpleShaderProg->isLinked()) {
        qCritical() << "Errors linking simple shader:"
                    << simpleShaderProg->log();
    }

    // Compile the blit shader:
    source.clear();
    source.append(qShaderSnippets[MainWithTexCoordsVertexShader]);
    source.append(qShaderSnippets[UntransformedPositionVertexShader]);
    vertexShader = new QGLShader(QGLShader::Vertex, context, this);
    vertexShader->compileSourceCode(source);

    source.clear();
    source.append(qShaderSnippets[MainFragmentShader]);
    source.append(qShaderSnippets[ImageSrcFragmentShader]);
    fragShader = new QGLShader(QGLShader::Fragment, context, this);
    fragShader->compileSourceCode(source);

    blitShaderProg = new QGLShaderProgram(context, this);
    blitShaderProg->addShader(vertexShader);
    blitShaderProg->addShader(fragShader);
    blitShaderProg->bindAttributeLocation("textureCoordArray", QT_TEXTURE_COORDS_ATTR);
    blitShaderProg->bindAttributeLocation("vertexCoordsArray", QT_VERTEX_COORDS_ATTR);
    blitShaderProg->link();
    if (!blitShaderProg->isLinked()) {
        qCritical() << "Errors linking blit shader:"
                    << simpleShaderProg->log();
    }

}

#if defined (QT_DEBUG)
QByteArray QGLEngineSharedShaders::snippetNameStr(SnippetName name)
{
    QMetaEnum m = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("SnippetName"));
    return QByteArray(m.valueToKey(name));
}
#endif

// The address returned here will only be valid until next time this function is called.
QGLEngineShaderProg *QGLEngineSharedShaders::findProgramInCache(const QGLEngineShaderProg &prog)
{
    for (int i = 0; i < cachedPrograms.size(); ++i) {
        QGLEngineShaderProg *cachedProg = cachedPrograms[i];
        if (*cachedProg == prog) {
            // Move the program to the top of the list as a poor-man's cache algo
            cachedPrograms.move(i, 0);
            return cachedProg;
        }
    }

    QByteArray source;
    source.append(qShaderSnippets[prog.mainFragShader]);
    source.append(qShaderSnippets[prog.srcPixelFragShader]);
    if (prog.srcPixelFragShader == CustomImageSrcFragmentShader)
        source.append(prog.customStageSource);
    if (prog.compositionFragShader)
        source.append(qShaderSnippets[prog.compositionFragShader]);
    if (prog.maskFragShader)
        source.append(qShaderSnippets[prog.maskFragShader]);
    QGLShader* fragShader = new QGLShader(QGLShader::Fragment, ctxGuard.context(), this);
    fragShader->compileSourceCode(source);

    source.clear();
    source.append(qShaderSnippets[prog.mainVertexShader]);
    source.append(qShaderSnippets[prog.positionVertexShader]);
    QGLShader* vertexShader = new QGLShader(QGLShader::Vertex, ctxGuard.context(), this);
    vertexShader->compileSourceCode(source);

#if defined(QT_DEBUG)
    // Name the shaders for easier debugging
    QByteArray description;
    description.append("Fragment shader: main=");
    description.append(snippetNameStr(prog.mainFragShader));
    description.append(", srcPixel=");
    description.append(snippetNameStr(prog.srcPixelFragShader));
    if (prog.compositionFragShader) {
        description.append(", composition=");
        description.append(snippetNameStr(prog.compositionFragShader));
    }
    if (prog.maskFragShader) {
        description.append(", mask=");
        description.append(snippetNameStr(prog.maskFragShader));
    }
    fragShader->setObjectName(QString::fromLatin1(description));

    description.clear();
    description.append("Vertex shader: main=");
    description.append(snippetNameStr(prog.mainVertexShader));
    description.append(", position=");
    description.append(snippetNameStr(prog.positionVertexShader));
    vertexShader->setObjectName(QString::fromLatin1(description));
#endif

    QGLEngineShaderProg* newProg = new QGLEngineShaderProg(prog);

    // If the shader program's not found in the cache, create it now.
    newProg->program = new QGLShaderProgram(ctxGuard.context(), this);
    newProg->program->addShader(vertexShader);
    newProg->program->addShader(fragShader);

    // We have to bind the vertex attribute names before the program is linked:
    newProg->program->bindAttributeLocation("vertexCoordsArray", QT_VERTEX_COORDS_ATTR);
    if (newProg->useTextureCoords)
        newProg->program->bindAttributeLocation("textureCoordArray", QT_TEXTURE_COORDS_ATTR);
    if (newProg->useOpacityAttribute)
        newProg->program->bindAttributeLocation("opacityArray", QT_OPACITY_ATTR);

    newProg->program->link();
    if (!newProg->program->isLinked()) {
        QLatin1String none("none");
        QLatin1String br("\n");
        QString error;
        error = QLatin1String("Shader program failed to link,")
#if defined(QT_DEBUG)
            + br
            + QLatin1String("  Shaders Used:") + br
            + QLatin1String("    ") + vertexShader->objectName() + QLatin1String(": ") + br
            + QLatin1String(vertexShader->sourceCode()) + br
            + QLatin1String("    ") + fragShader->objectName() + QLatin1String(": ") + br
            + QLatin1String(fragShader->sourceCode()) + br
#endif
            + QLatin1String("  Error Log:\n")
            + QLatin1String("    ") + newProg->program->log();
        qWarning() << error;
        delete newProg; // Deletes the QGLShaderProgram in it's destructor
        newProg = 0;
    }
    else {
        if (cachedPrograms.count() > 30) {
            // The cache is full, so delete the last 5 programs in the list.
            // These programs will be least used, as a program us bumped to
            // the top of the list when it's used.
            for (int i = 0; i < 5; ++i) {
                delete cachedPrograms.last();
                cachedPrograms.removeLast();
            }
        }

        cachedPrograms.insert(0, newProg);
    }

    return newProg;
}

void QGLEngineSharedShaders::cleanupCustomStage(QGLCustomShaderStage* stage)
{
    // Remove any shader programs which has this as the custom shader src:
    for (int i = 0; i < cachedPrograms.size(); ++i) {
        QGLEngineShaderProg *cachedProg = cachedPrograms[i];
        if (cachedProg->customStageSource == stage->source()) {
            delete cachedProg;
            cachedPrograms.removeAt(i);
            i--;
        }
    }
}


QGLEngineShaderManager::QGLEngineShaderManager(QGLContext* context)
    : ctx(context),
      shaderProgNeedsChanging(true),
      srcPixelType(Qt::NoBrush),
      opacityMode(NoOpacity),
      maskType(NoMask),
      compositionMode(QPainter::CompositionMode_SourceOver),
      customSrcStage(0),
      currentShaderProg(0)
{
    sharedShaders = QGLEngineSharedShaders::shadersForContext(context);
    connect(sharedShaders, SIGNAL(shaderProgNeedsChanging()), this, SLOT(shaderProgNeedsChangingSlot()));
}

QGLEngineShaderManager::~QGLEngineShaderManager()
{
    //###
    removeCustomStage();
}

uint QGLEngineShaderManager::getUniformLocation(Uniform id)
{
    QVector<uint> &uniformLocations = currentShaderProg->uniformLocations;
    if (uniformLocations.isEmpty())
        uniformLocations.fill(GLuint(-1), NumUniforms);

    static const char *uniformNames[] = {
        "imageTexture",
        "patternColor",
        "globalOpacity",
        "depth",
        "pmvMatrix",
        "maskTexture",
        "fragmentColor",
        "linearData",
        "angle",
        "halfViewportSize",
        "fmp",
        "fmp2_m_radius2",
        "inverse_2_fmp2_m_radius2",
        "invertedTextureSize",
        "brushTransform",
        "brushTexture"
    };

    if (uniformLocations.at(id) == GLuint(-1))
        uniformLocations[id] = currentShaderProg->program->uniformLocation(uniformNames[id]);

    return uniformLocations.at(id);
}


void QGLEngineShaderManager::optimiseForBrushTransform(const QTransform &transform)
{
    Q_UNUSED(transform); // Currently ignored
}

void QGLEngineShaderManager::setDirty()
{
    shaderProgNeedsChanging = true;
}

void QGLEngineShaderManager::setSrcPixelType(Qt::BrushStyle style)
{
    if (srcPixelType == PixelSrcType(style))
        return;

    srcPixelType = style;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setSrcPixelType(PixelSrcType type)
{
    if (srcPixelType == type)
        return;

    srcPixelType = type;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setOpacityMode(OpacityMode mode)
{
    if (opacityMode == mode)
        return;

    opacityMode = mode;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setMaskType(MaskType type)
{
    if (maskType == type)
        return;

    maskType = type;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setCompositionMode(QPainter::CompositionMode mode)
{
    if (compositionMode == mode)
        return;

    compositionMode = mode;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setCustomStage(QGLCustomShaderStage* stage)
{
    if (customSrcStage)
        removeCustomStage();
    customSrcStage = stage;
    shaderProgNeedsChanging = true;
}

void QGLEngineShaderManager::removeCustomStage()
{
    if (customSrcStage)
        customSrcStage->setInactive();
    customSrcStage = 0;
    shaderProgNeedsChanging = true;
}

QGLShaderProgram* QGLEngineShaderManager::currentProgram()
{
    return currentShaderProg->program;
}

QGLShaderProgram* QGLEngineShaderManager::simpleProgram()
{
    return sharedShaders->simpleProgram();
}

QGLShaderProgram* QGLEngineShaderManager::blitProgram()
{
    return sharedShaders->blitProgram();
}



// Select & use the correct shader program using the current state.
// Returns true if program needed changing.
bool QGLEngineShaderManager::useCorrectShaderProg()
{
    if (!shaderProgNeedsChanging)
        return false;

    bool useCustomSrc = customSrcStage != 0;
    if (useCustomSrc && srcPixelType != QGLEngineShaderManager::ImageSrc && srcPixelType != Qt::TexturePattern) {
        useCustomSrc = false;
        qWarning("QGLEngineShaderManager - Ignoring custom shader stage for non image src");
    }

    QGLEngineShaderProg requiredProgram;

    bool texCoords = false;

    // Choose vertex shader shader position function (which typically also sets
    // varyings) and the source pixel (srcPixel) fragment shader function:
    requiredProgram.positionVertexShader = QGLEngineSharedShaders::InvalidSnippetName;
    requiredProgram.srcPixelFragShader = QGLEngineSharedShaders::InvalidSnippetName;
    bool isAffine = brushTransform.isAffine();
    if ( (srcPixelType >= Qt::Dense1Pattern) && (srcPixelType <= Qt::DiagCrossPattern) ) {
        if (isAffine)
            requiredProgram.positionVertexShader = QGLEngineSharedShaders::AffinePositionWithPatternBrushVertexShader;
        else
            requiredProgram.positionVertexShader = QGLEngineSharedShaders::PositionWithPatternBrushVertexShader;

        requiredProgram.srcPixelFragShader = QGLEngineSharedShaders::PatternBrushSrcFragmentShader;
    }
    else switch (srcPixelType) {
        default:
        case Qt::NoBrush:
            qFatal("QGLEngineShaderManager::useCorrectShaderProg() - Qt::NoBrush style is set");
            break;
        case QGLEngineShaderManager::ImageSrc:
            requiredProgram.srcPixelFragShader = QGLEngineSharedShaders::ImageSrcFragmentShader;
            requiredProgram.positionVertexShader = QGLEngineSharedShaders::PositionOnlyVertexShader;
            texCoords = true;
            break;
        case QGLEngineShaderManager::NonPremultipliedImageSrc:
            requiredProgram.srcPixelFragShader = QGLEngineSharedShaders::NonPremultipliedImageSrcFragmentShader;
            requiredProgram.positionVertexShader = QGLEngineSharedShaders::PositionOnlyVertexShader;
            texCoords = true;
            break;
        case QGLEngineShaderManager::PatternSrc:
            requiredProgram.srcPixelFragShader = QGLEngineSharedShaders::ImageSrcWithPatternFragmentShader;
            requiredProgram.positionVertexShader = QGLEngineSharedShaders::PositionOnlyVertexShader;
            texCoords = true;
            break;
        case QGLEngineShaderManager::TextureSrcWithPattern:
            requiredProgram.srcPixelFragShader = QGLEngineSharedShaders::TextureBrushSrcWithPatternFragmentShader;
            requiredProgram.positionVertexShader = isAffine ? QGLEngineSharedShaders::AffinePositionWithTextureBrushVertexShader
                                                : QGLEngineSharedShaders::PositionWithTextureBrushVertexShader;
            break;
        case Qt::SolidPattern:
            requiredProgram.srcPixelFragShader = QGLEngineSharedShaders::SolidBrushSrcFragmentShader;
            requiredProgram.positionVertexShader = QGLEngineSharedShaders::PositionOnlyVertexShader;
            break;
        case Qt::LinearGradientPattern:
            requiredProgram.srcPixelFragShader = QGLEngineSharedShaders::LinearGradientBrushSrcFragmentShader;
            requiredProgram.positionVertexShader = isAffine ? QGLEngineSharedShaders::AffinePositionWithLinearGradientBrushVertexShader
                                                : QGLEngineSharedShaders::PositionWithLinearGradientBrushVertexShader;
            break;
        case Qt::ConicalGradientPattern:
            requiredProgram.srcPixelFragShader = QGLEngineSharedShaders::ConicalGradientBrushSrcFragmentShader;
            requiredProgram.positionVertexShader = isAffine ? QGLEngineSharedShaders::AffinePositionWithConicalGradientBrushVertexShader
                                                : QGLEngineSharedShaders::PositionWithConicalGradientBrushVertexShader;
            break;
        case Qt::RadialGradientPattern:
            requiredProgram.srcPixelFragShader = QGLEngineSharedShaders::RadialGradientBrushSrcFragmentShader;
            requiredProgram.positionVertexShader = isAffine ? QGLEngineSharedShaders::AffinePositionWithRadialGradientBrushVertexShader
                                                : QGLEngineSharedShaders::PositionWithRadialGradientBrushVertexShader;
            break;
        case Qt::TexturePattern:
            requiredProgram.srcPixelFragShader = QGLEngineSharedShaders::TextureBrushSrcFragmentShader;
            requiredProgram.positionVertexShader = isAffine ? QGLEngineSharedShaders::AffinePositionWithTextureBrushVertexShader
                                                : QGLEngineSharedShaders::PositionWithTextureBrushVertexShader;
            break;
    };

    if (useCustomSrc) {
        requiredProgram.srcPixelFragShader = QGLEngineSharedShaders::CustomImageSrcFragmentShader;
        requiredProgram.customStageSource = customSrcStage->source();
    }

    const bool hasCompose = compositionMode > QPainter::CompositionMode_Plus;
    const bool hasMask = maskType != QGLEngineShaderManager::NoMask;

    // Choose fragment shader main function:
    if (opacityMode == AttributeOpacity) {
        Q_ASSERT(!hasCompose && !hasMask);
        requiredProgram.mainFragShader = QGLEngineSharedShaders::MainFragmentShader_ImageArrays;
    } else {
        bool useGlobalOpacity = (opacityMode == UniformOpacity);
        if (hasCompose && hasMask && useGlobalOpacity)
            requiredProgram.mainFragShader = QGLEngineSharedShaders::MainFragmentShader_CMO;
        if (hasCompose && hasMask && !useGlobalOpacity)
            requiredProgram.mainFragShader = QGLEngineSharedShaders::MainFragmentShader_CM;
        if (!hasCompose && hasMask && useGlobalOpacity)
            requiredProgram.mainFragShader = QGLEngineSharedShaders::MainFragmentShader_MO;
        if (!hasCompose && hasMask && !useGlobalOpacity)
            requiredProgram.mainFragShader = QGLEngineSharedShaders::MainFragmentShader_M;
        if (hasCompose && !hasMask && useGlobalOpacity)
            requiredProgram.mainFragShader = QGLEngineSharedShaders::MainFragmentShader_CO;
        if (hasCompose && !hasMask && !useGlobalOpacity)
            requiredProgram.mainFragShader = QGLEngineSharedShaders::MainFragmentShader_C;
        if (!hasCompose && !hasMask && useGlobalOpacity)
            requiredProgram.mainFragShader = QGLEngineSharedShaders::MainFragmentShader_O;
        if (!hasCompose && !hasMask && !useGlobalOpacity)
            requiredProgram.mainFragShader = QGLEngineSharedShaders::MainFragmentShader;
    }

    if (hasMask) {
        if (maskType == PixelMask) {
            requiredProgram.maskFragShader = QGLEngineSharedShaders::MaskFragmentShader;
            texCoords = true;
        } else if (maskType == SubPixelMaskPass1) {
            requiredProgram.maskFragShader = QGLEngineSharedShaders::RgbMaskFragmentShaderPass1;
            texCoords = true;
        } else if (maskType == SubPixelMaskPass2) {
            requiredProgram.maskFragShader = QGLEngineSharedShaders::RgbMaskFragmentShaderPass2;
            texCoords = true;
        } else if (maskType == SubPixelWithGammaMask) {
            requiredProgram.maskFragShader = QGLEngineSharedShaders::RgbMaskWithGammaFragmentShader;
            texCoords = true;
        } else {
            qCritical("QGLEngineShaderManager::useCorrectShaderProg() - Unknown mask type");
        }
    } else {
        requiredProgram.maskFragShader = QGLEngineSharedShaders::NoMaskFragmentShader;
    }

    if (hasCompose) {
        switch (compositionMode) {
            case QPainter::CompositionMode_Multiply:
                requiredProgram.compositionFragShader = QGLEngineSharedShaders::MultiplyCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Screen:
                requiredProgram.compositionFragShader = QGLEngineSharedShaders::ScreenCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Overlay:
                requiredProgram.compositionFragShader = QGLEngineSharedShaders::OverlayCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Darken:
                requiredProgram.compositionFragShader = QGLEngineSharedShaders::DarkenCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Lighten:
                requiredProgram.compositionFragShader = QGLEngineSharedShaders::LightenCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_ColorDodge:
                requiredProgram.compositionFragShader = QGLEngineSharedShaders::ColorDodgeCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_ColorBurn:
                requiredProgram.compositionFragShader = QGLEngineSharedShaders::ColorBurnCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_HardLight:
                requiredProgram.compositionFragShader = QGLEngineSharedShaders::HardLightCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_SoftLight:
                requiredProgram.compositionFragShader = QGLEngineSharedShaders::SoftLightCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Difference:
                requiredProgram.compositionFragShader = QGLEngineSharedShaders::DifferenceCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Exclusion:
                requiredProgram.compositionFragShader = QGLEngineSharedShaders::ExclusionCompositionModeFragmentShader;
                break;
            default:
                qWarning("QGLEngineShaderManager::useCorrectShaderProg() - Unsupported composition mode");
        }
    } else {
        requiredProgram.compositionFragShader = QGLEngineSharedShaders::NoCompositionModeFragmentShader;
    }

    // Choose vertex shader main function
    if (opacityMode == AttributeOpacity) {
        Q_ASSERT(texCoords);
        requiredProgram.mainVertexShader = QGLEngineSharedShaders::MainWithTexCoordsAndOpacityVertexShader;
    } else if (texCoords) {
        requiredProgram.mainVertexShader = QGLEngineSharedShaders::MainWithTexCoordsVertexShader;
    } else {
        requiredProgram.mainVertexShader = QGLEngineSharedShaders::MainVertexShader;
    }
    requiredProgram.useTextureCoords = texCoords;
    requiredProgram.useOpacityAttribute = (opacityMode == AttributeOpacity);

    // At this point, requiredProgram is fully populated so try to find the program in the cache
    currentShaderProg = sharedShaders->findProgramInCache(requiredProgram);

    if (currentShaderProg) {
        currentShaderProg->program->bind();
        if (useCustomSrc)
            customSrcStage->setUniforms(currentShaderProg->program);
    }

    shaderProgNeedsChanging = false;
    return true;
}

QT_END_NAMESPACE
