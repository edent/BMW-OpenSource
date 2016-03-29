/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "profileevaluator.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

QT_BEGIN_NAMESPACE

static QStringList getSources(const char *var, const char *vvar, const QStringList &baseVPaths,
                              const QString &projectDir, const ProFileEvaluator &visitor)
{
    QStringList vPaths =
        visitor.absolutePathValues(QLatin1String(vvar), projectDir);
    vPaths += baseVPaths;
    vPaths.removeDuplicates();
    return visitor.absoluteFileValues(QLatin1String(var), projectDir, vPaths, 0);
}

void evaluateProFile(const ProFileEvaluator &visitor, QHash<QByteArray, QStringList> *varMap,
                     const QString &projectDir)
{
    QStringList baseVPaths;
    baseVPaths += visitor.absolutePathValues(QLatin1String("VPATH"), projectDir);
    baseVPaths << projectDir; // QMAKE_ABSOLUTE_SOURCE_PATH
    baseVPaths += visitor.absolutePathValues(QLatin1String("DEPENDPATH"), projectDir);
    baseVPaths.removeDuplicates();

    QStringList sourceFiles;
    QString codecForTr;
    QString codecForSource;
    QStringList tsFileNames;

    // app/lib template
    sourceFiles += getSources("SOURCES", "VPATH_SOURCES", baseVPaths, projectDir, visitor);

    sourceFiles += getSources("FORMS", "VPATH_FORMS", baseVPaths, projectDir, visitor);
    sourceFiles += getSources("FORMS3", "VPATH_FORMS3", baseVPaths, projectDir, visitor);

    QStringList vPathsInc = baseVPaths;
    vPathsInc += visitor.absolutePathValues(QLatin1String("INCLUDEPATH"), projectDir);
    vPathsInc.removeDuplicates();
    sourceFiles += visitor.absoluteFileValues(QLatin1String("HEADERS"), projectDir, vPathsInc, 0);

    QDir proDir(projectDir);
    foreach (const QString &tsFile, visitor.values(QLatin1String("TRANSLATIONS")))
        tsFileNames << QFileInfo(proDir, tsFile).filePath();

    QStringList trcodec = visitor.values(QLatin1String("CODEC"))
        + visitor.values(QLatin1String("DEFAULTCODEC"))
        + visitor.values(QLatin1String("CODECFORTR"));
    if (!trcodec.isEmpty())
        codecForTr = trcodec.last();

    QStringList srccodec = visitor.values(QLatin1String("CODECFORSRC"));
    if (!srccodec.isEmpty())
        codecForSource = srccodec.last();

    sourceFiles.sort();
    sourceFiles.removeDuplicates();
    tsFileNames.sort();
    tsFileNames.removeDuplicates();

    varMap->insert("SOURCES", sourceFiles);
    varMap->insert("CODECFORTR", QStringList() << codecForTr);
    varMap->insert("CODECFORSRC", QStringList() << codecForSource);
    varMap->insert("TRANSLATIONS", tsFileNames);
}

bool evaluateProFile(const QString &fileName, bool verbose, QHash<QByteArray, QStringList> *varMap)
{
    QFileInfo fi(fileName);
    if (!fi.exists())
        return false;

    ProFile pro(fi.absoluteFilePath());

    ProFileEvaluator visitor;
    visitor.setVerbose(verbose);

    if (!visitor.queryProFile(&pro))
        return false;

    if (!visitor.accept(&pro))
        return false;

    evaluateProFile(visitor, varMap, fi.absolutePath());

    return true;
}

QT_END_NAMESPACE
