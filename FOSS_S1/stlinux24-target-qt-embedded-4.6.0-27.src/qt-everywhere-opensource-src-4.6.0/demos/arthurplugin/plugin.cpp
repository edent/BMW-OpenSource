/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerCustomWidgetInterface>

#include <QtCore/qplugin.h>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>

#include "xform.h"
#include "pathdeform.h"
#include "gradients.h"
#include "pathstroke.h"
#include "hoverpoints.h"
#include "composition.h"

QT_FORWARD_DECLARE_CLASS(QDesignerFormEditorInterface)

// Specify "text" to be a singleline property (no richtext)
static inline QString textSingleLinePropertyDeclaration(const QString &className)
{
    QString rc = QLatin1String(
            "<customwidgets>\n"
            "  <customwidget>\n"
            "    <class>");
    rc += className;
    rc += QLatin1String("</class>\n"
            "    <propertyspecifications>\n"
            "      <stringpropertyspecification name=\"text\" type=\"singleline\"/>\n"
            "    </propertyspecifications>\n"
            "  </customwidget>\n"
            "</customwidgets>\n");
    return rc;
}

// Plain XML for a custom widget
static inline QString customWidgetDomXml(const QString &className,
                                         const QString &customSection = QString())
{
    QString rc = QLatin1String("<ui language=\"c++\"><widget class=\"");
    rc += className;
    rc += QLatin1String("\" name=\"");
    QString objectName = className;
    objectName[0] = objectName.at(0).toLower();
    rc += objectName;
    rc += QLatin1String("\"/>");
    rc += customSection;
    rc += QLatin1String("</ui>");
    return rc;
}

class PathDeformRendererEx : public PathDeformRenderer
{
    Q_OBJECT
public:
    PathDeformRendererEx(QWidget *parent) : PathDeformRenderer(parent) { }
    QSize sizeHint() const { return QSize(300, 200); }
};

class DemoPlugin : public QDesignerCustomWidgetInterface
{
    Q_INTERFACES(QDesignerCustomWidgetInterface)

protected:
    explicit DemoPlugin(const QString &className, const QString &customSection = QString());

public:
    QString name() const { return m_className; }
    bool isContainer() const { return false; }
    bool isInitialized() const { return m_initialized; }
    QIcon icon() const { return QIcon(); }
    QString codeTemplate() const { return QString(); }
    QString whatsThis() const { return QString(); }
    QString toolTip() const { return QString(); }
    QString group() const { return "Arthur Widgets [Demo]"; }
    void initialize(QDesignerFormEditorInterface *)
    {
        if (m_initialized)
            return;
        m_initialized = true;
    }
    QString domXml() const { return m_domXml; }

private:
    const QString m_className;
    const QString m_domXml;
    bool m_initialized;
};

DemoPlugin::DemoPlugin(const QString &className, const QString &customSection) :
    m_className(className),
    m_domXml(customWidgetDomXml(className, customSection)),
    m_initialized(false)
{
}

class DeformPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT

public:
    explicit DeformPlugin(QObject *parent = 0);
    QString includeFile() const { return QLatin1String("deform.h"); }

    QWidget *createWidget(QWidget *parent)
    {
        PathDeformRenderer *deform = new PathDeformRendererEx(parent);
        deform->setRadius(70);
        deform->setAnimated(false);
        deform->setFontSize(20);
        deform->setText(QLatin1String("Arthur Widgets Demo"));

        return deform;
    }
};

DeformPlugin::DeformPlugin(QObject *parent) :
    QObject(parent),
    DemoPlugin(QLatin1String("PathDeformRendererEx"),
               textSingleLinePropertyDeclaration(QLatin1String("PathDeformRendererEx")))
{
}

class XFormRendererEx : public XFormView
{
    Q_OBJECT
public:
    XFormRendererEx(QWidget *parent) : XFormView(parent) {}
    QSize sizeHint() const { return QSize(300, 200); }
};

class XFormPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    explicit XFormPlugin(QObject *parent = 0);
    QString includeFile() const { return QLatin1String("xform.h"); }

    QWidget *createWidget(QWidget *parent)
    {
        XFormRendererEx *xform = new XFormRendererEx(parent);
        xform->setText(QLatin1String("Qt - Hello World!!"));
        xform->setPixmap(QPixmap(QLatin1String(":/trolltech/arthurplugin/bg1.jpg")));
        return xform;
    }
};

XFormPlugin::XFormPlugin(QObject *parent) :
    QObject(parent),
    DemoPlugin(QLatin1String("XFormRendererEx"),
               textSingleLinePropertyDeclaration(QLatin1String("XFormRendererEx")))
{
}

class GradientEditorPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    explicit GradientEditorPlugin(QObject *parent = 0) : QObject(parent), DemoPlugin(QLatin1String("GradientEditor")) { }
    QString includeFile() const { return "gradients.h"; }

    QWidget *createWidget(QWidget *parent)
    {
        GradientEditor *editor = new GradientEditor(parent);
        return editor;
    }
};

class GradientRendererEx : public GradientRenderer
{
    Q_OBJECT
public:
    GradientRendererEx(QWidget *p) : GradientRenderer(p) { }
    QSize sizeHint() const { return QSize(300, 200); }
};

class GradientRendererPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    GradientRendererPlugin(QObject *parent = 0) : QObject(parent), DemoPlugin(QLatin1String("GradientRendererEx")) { }
    QString includeFile() const { return QLatin1String("gradients.h"); }

    QWidget *createWidget(QWidget *parent)
    {
        GradientRenderer *renderer = new GradientRendererEx(parent);
        renderer->setConicalGradient();
        return renderer;
    }
};

class PathStrokeRendererEx : public PathStrokeRenderer
{
    Q_OBJECT
public:
    explicit PathStrokeRendererEx(QWidget *p) : PathStrokeRenderer(p) { }
    QSize sizeHint() const { return QSize(300, 200); }
};

class StrokeRenderPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    explicit StrokeRenderPlugin(QObject *parent = 0) : QObject(parent), DemoPlugin(QLatin1String("PathStrokeRendererEx")) { }
    QString includeFile() const { return QLatin1String("pathstroke.h"); }

    QWidget *createWidget(QWidget *parent)
    {
        PathStrokeRenderer *stroke = new PathStrokeRendererEx(parent);
        return stroke;
    }
};


class CompositionModePlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    explicit CompositionModePlugin(QObject *parent = 0) : QObject(parent), DemoPlugin(QLatin1String("CompositionRenderer")) { }
    QString includeFile() const { return QLatin1String("composition.h"); }

    QWidget *createWidget(QWidget *parent)
    {
        CompositionRenderer *renderer = new CompositionRenderer(parent);
        renderer->setAnimationEnabled(false);
        return renderer;
    }
};


class ArthurPlugins : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    explicit ArthurPlugins(QObject *parent = 0);
    QList<QDesignerCustomWidgetInterface*> customWidgets() const { return m_plugins; }

private:
    QList<QDesignerCustomWidgetInterface *> m_plugins;
};

ArthurPlugins::ArthurPlugins(QObject *parent) :
    QObject(parent)
{
    m_plugins << new DeformPlugin(this)
              << new XFormPlugin(this)
              << new GradientEditorPlugin(this)
              << new GradientRendererPlugin(this)
              << new StrokeRenderPlugin(this)
              << new CompositionModePlugin(this);
}

#include "plugin.moc"

Q_EXPORT_PLUGIN2(ArthurPlugins, ArthurPlugins)
