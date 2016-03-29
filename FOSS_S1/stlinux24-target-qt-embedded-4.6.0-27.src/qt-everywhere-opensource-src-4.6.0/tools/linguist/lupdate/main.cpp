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

#include "lupdate.h"

#include <translator.h>
#include <profileevaluator.h>
#include <proreader.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextCodec>

static QString m_defaultExtensions;

static void printOut(const QString & out)
{
    qWarning("%s", qPrintable(out));
}

static void recursiveFileInfoList(const QDir &dir,
    const QSet<QString> &nameFilters, QDir::Filters filter,
    QFileInfoList *fileinfolist)
{
    foreach (const QFileInfo &fi, dir.entryInfoList(filter))
        if (fi.isDir())
            recursiveFileInfoList(QDir(fi.absoluteFilePath()), nameFilters, filter, fileinfolist);
        else if (nameFilters.contains(fi.suffix()))
            fileinfolist->append(fi);
}

static void printUsage()
{
    printOut(QObject::tr(
        "Usage:\n"
        "    lupdate [options] [project-file]...\n"
        "    lupdate [options] [source-file|path]... -ts ts-files\n\n"
        "lupdate is part of Qt's Linguist tool chain. It extracts translatable\n"
        "messages from Qt UI files, C++, Java and JavaScript/QtScript source code.\n"
        "Extracted messages are stored in textual translation source files (typically\n"
        "Qt TS XML). New and modified messages can be merged into existing TS files.\n\n"
        "Options:\n"
        "    -help  Display this information and exit.\n"
        "    -no-obsolete\n"
        "           Drop all obsolete strings.\n"
        "    -extensions <ext>[,<ext>]...\n"
        "           Process files with the given extensions only.\n"
        "           The extension list must be separated with commas, not with whitespace.\n"
        "           Default: '%1'.\n"
        "    -pluralonly\n"
        "           Only include plural form messages.\n"
        "    -silent\n"
        "           Do not explain what is being done.\n"
        "    -no-sort\n"
        "           Do not sort contexts in TS files.\n"
        "    -no-recursive\n"
        "           Do not recursively scan the following directories.\n"
        "    -recursive\n"
        "           Recursively scan the following directories (default).\n"
        "    -I <includepath> or -I<includepath>\n"
        "           Additional location to look for include files.\n"
        "           May be specified multiple times.\n"
        "    -locations {absolute|relative|none}\n"
        "           Specify/override how source code references are saved in TS files.\n"
        "           Default is absolute.\n"
        "    -no-ui-lines\n"
        "           Do not record line numbers in references to UI files.\n"
        "    -disable-heuristic {sametext|similartext|number}\n"
        "           Disable the named merge heuristic. Can be specified multiple times.\n"
        "    -pro <filename>\n"
        "           Name of a .pro file. Useful for files with .pro\n"
        "           file syntax but different file suffix\n"
        "    -source-language <language>[_<region>]\n"
        "           Specify the language of the source strings for new files.\n"
        "           Defaults to POSIX if not specified.\n"
        "    -target-language <language>[_<region>]\n"
        "           Specify the language of the translations for new files.\n"
        "           Guessed from the file name if not specified.\n"
        "    -version\n"
        "           Display the version of lupdate and exit.\n"
    ).arg(m_defaultExtensions));
}

static void updateTsFiles(const Translator &fetchedTor, const QStringList &tsFileNames,
    const QByteArray &codecForTr, const QString &sourceLanguage, const QString &targetLanguage,
    UpdateOptions options, bool *fail)
{
    QDir dir;
    QString err;
    foreach (const QString &fileName, tsFileNames) {
        QString fn = dir.relativeFilePath(fileName);
        ConversionData cd;
        Translator tor;
        cd.m_sortContexts = !(options & NoSort);
        if (QFile(fileName).exists()) {
            if (!tor.load(fileName, cd, QLatin1String("auto"))) {
                printOut(cd.error());
                *fail = true;
                continue;
            }
            tor.resolveDuplicates();
            cd.clearErrors();
            if (!codecForTr.isEmpty() && codecForTr != tor.codecName())
                qWarning("lupdate warning: Codec for tr() '%s' disagrees with "
                         "existing file's codec '%s'. Expect trouble.",
                         codecForTr.constData(), tor.codecName().constData());
            if (!targetLanguage.isEmpty() && targetLanguage != tor.languageCode())
                qWarning("lupdate warning: Specified target language '%s' disagrees with "
                         "existing file's language '%s'. Ignoring.",
                         qPrintable(targetLanguage), qPrintable(tor.languageCode()));
            if (!sourceLanguage.isEmpty() && sourceLanguage != tor.sourceLanguageCode())
                qWarning("lupdate warning: Specified source language '%s' disagrees with "
                         "existing file's language '%s'. Ignoring.",
                         qPrintable(sourceLanguage), qPrintable(tor.sourceLanguageCode()));
        } else {
            if (!codecForTr.isEmpty())
                tor.setCodecName(codecForTr);
            if (!targetLanguage.isEmpty())
                tor.setLanguageCode(targetLanguage);
            else
                tor.setLanguageCode(Translator::guessLanguageCodeFromFileName(fileName));
            if (!sourceLanguage.isEmpty())
                tor.setSourceLanguageCode(sourceLanguage);
        }
        tor.makeFileNamesAbsolute(QFileInfo(fileName).absoluteDir());
        if (options & NoLocations)
            tor.setLocationsType(Translator::NoLocations);
        else if (options & RelativeLocations)
            tor.setLocationsType(Translator::RelativeLocations);
        else if (options & AbsoluteLocations)
            tor.setLocationsType(Translator::AbsoluteLocations);
        if (options & Verbose)
            printOut(QObject::tr("Updating '%1'...\n").arg(fn));

        UpdateOptions theseOptions = options;
        if (tor.locationsType() == Translator::NoLocations) // Could be set from file
            theseOptions |= NoLocations;
        Translator out = merge(tor, fetchedTor, theseOptions, err);
        if (!codecForTr.isEmpty())
            out.setCodecName(codecForTr);

        if ((options & Verbose) && !err.isEmpty()) {
            printOut(err);
            err.clear();
        }
        if (options & PluralOnly) {
            if (options & Verbose)
                printOut(QObject::tr("Stripping non plural forms in '%1'...\n").arg(fn));
            out.stripNonPluralForms();
        }
        if (options & NoObsolete)
            out.stripObsoleteMessages();
        out.stripEmptyContexts();

        out.normalizeTranslations(cd);
        if (!cd.errors().isEmpty()) {
            printOut(cd.error());
            cd.clearErrors();
        }
        if (!out.save(fileName, cd, QLatin1String("auto"))) {
            printOut(cd.error());
            *fail = true;
        }
    }
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    m_defaultExtensions = QLatin1String("ui,c,c++,cc,cpp,cxx,ch,h,h++,hh,hpp,hxx");

    QStringList args = app.arguments();
    QString defaultContext; // This was QLatin1String("@default") before.
    Translator fetchedTor;
    QByteArray codecForTr;
    QByteArray codecForSource;
    QStringList tsFileNames;
    QStringList proFiles;
    QMultiHash<QString, QString> allCSources;
    QSet<QString> projectRoots;
    QStringList sourceFiles;
    QStringList includePath;
    QString targetLanguage;
    QString sourceLanguage;

    UpdateOptions options =
        Verbose | // verbose is on by default starting with Qt 4.2
        HeuristicSameText | HeuristicSimilarText | HeuristicNumber;
    int numFiles = 0;
    bool standardSyntax = true;
    bool metTsFlag = false;
    bool recursiveScan = true;

    QString extensions = m_defaultExtensions;
    QSet<QString> extensionsNameFilters;

    for (int  i = 1; i < argc; ++i) {
        if (args.at(i) == QLatin1String("-ts"))
            standardSyntax = false;
    }

    for (int i = 1; i < argc; ++i) {
        QString arg = args.at(i);
        if (arg == QLatin1String("-help")
                || arg == QLatin1String("--help")
                || arg == QLatin1String("-h")) {
            printUsage();
            return 0;
        } else if (arg == QLatin1String("-pluralonly")) {
            options |= PluralOnly;
            continue;
        } else if (arg == QLatin1String("-noobsolete")
                || arg == QLatin1String("-no-obsolete")) {
            options |= NoObsolete;
            continue;
        } else if (arg == QLatin1String("-silent")) {
            options &= ~Verbose;
            continue;
        } else if (arg == QLatin1String("-target-language")) {
            ++i;
            if (i == argc) {
                qWarning("The option -target-language requires a parameter.");
                return 1;
            }
            targetLanguage = args[i];
            continue;
        } else if (arg == QLatin1String("-source-language")) {
            ++i;
            if (i == argc) {
                qWarning("The option -source-language requires a parameter.");
                return 1;
            }
            sourceLanguage = args[i];
            continue;
        } else if (arg == QLatin1String("-disable-heuristic")) {
            ++i;
            if (i == argc) {
                qWarning("The option -disable-heuristic requires a parameter.");
                return 1;
            }
            arg = args[i];
            if (arg == QLatin1String("sametext")) {
                options &= ~HeuristicSameText;
            } else if (arg == QLatin1String("similartext")) {
                options &= ~HeuristicSimilarText;
            } else if (arg == QLatin1String("number")) {
                options &= ~HeuristicNumber;
            } else {
                qWarning("Invalid heuristic name passed to -disable-heuristic.");
                return 1;
            }
            continue;
        } else if (arg == QLatin1String("-locations")) {
            ++i;
            if (i == argc) {
                qWarning("The option -locations requires a parameter.");
                return 1;
            }
            if (args[i] == QLatin1String("none")) {
                options |= NoLocations;
            } else if (args[i] == QLatin1String("relative")) {
                options |= RelativeLocations;
            } else if (args[i] == QLatin1String("absolute")) {
                options |= AbsoluteLocations;
            } else {
                qWarning("Invalid parameter passed to -locations.");
                return 1;
            }
            continue;
        } else if (arg == QLatin1String("-no-ui-lines")) {
            options |= NoUiLines;
            continue;
        } else if (arg == QLatin1String("-verbose")) {
            options |= Verbose;
            continue;
        } else if (arg == QLatin1String("-no-recursive")) {
            recursiveScan = false;
            continue;
        } else if (arg == QLatin1String("-recursive")) {
            recursiveScan = true;
            continue;
        } else if (arg == QLatin1String("-no-sort")
                   || arg == QLatin1String("-nosort")) {
            options |= NoSort;
            continue;
        } else if (arg == QLatin1String("-version")) {
            printOut(QObject::tr("lupdate version %1\n").arg(QLatin1String(QT_VERSION_STR)));
            return 0;
        } else if (arg == QLatin1String("-ts")) {
            metTsFlag = true;
            continue;
        } else if (arg == QLatin1String("-extensions")) {
            ++i;
            if (i == argc) {
                qWarning("The -extensions option should be followed by an extension list.");
                return 1;
            }
            extensions = args[i];
            continue;
        } else if (arg == QLatin1String("-pro")) {
            ++i;
            if (i == argc) {
                qWarning("The -pro option should be followed by a filename of .pro file.");
                return 1;
            }
            proFiles += args[i];
            numFiles++;
            continue;
        } else if (arg.startsWith(QLatin1String("-I"))) {
            if (arg.length() == 2) {
                ++i;
                if (i == argc) {
                    qWarning("The -I option should be followed by a path.");
                    return 1;
                }
                includePath += args[i];
            } else {
                includePath += args[i].mid(2);
            }
            continue;
        } else if (arg.startsWith(QLatin1String("-")) && arg != QLatin1String("-")) {
            qWarning("Unrecognized option '%s'", qPrintable(arg));
            return 1;
        }

        numFiles++;

        QString fullText;

        codecForTr.clear();
        codecForSource.clear();

        if (metTsFlag) {
            bool found = false;
            foreach (const Translator::FileFormat &fmt, Translator::registeredFileFormats()) {
                if (arg.endsWith(QLatin1Char('.') + fmt.extension, Qt::CaseInsensitive)) {
                    QFileInfo fi(arg);
                    if (!fi.exists() || fi.isWritable()) {
                        tsFileNames.append(QFileInfo(arg).absoluteFilePath());
                    } else {
                        qWarning("lupdate warning: For some reason, '%s' is not writable.\n",
                                qPrintable(arg));
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                qWarning("lupdate error: File '%s' has no recognized extension\n",
                         qPrintable(arg));
                return 1;
            }
        } else if (arg.endsWith(QLatin1String(".pro"), Qt::CaseInsensitive)
                || arg.endsWith(QLatin1String(".pri"), Qt::CaseInsensitive)) {
            proFiles << arg;
        } else {
            QFileInfo fi(arg);
            if (!fi.exists()) {
                qWarning("lupdate error: File '%s' does not exists\n", qPrintable(arg));
                return 1;
            } else if (fi.isDir()) {
                if (options & Verbose)
                    printOut(QObject::tr("Scanning directory '%1'...").arg(arg));
                QDir dir = QDir(fi.filePath());
                projectRoots.insert(dir.absolutePath() + QLatin1Char('/'));
                if (extensionsNameFilters.isEmpty()) {
                    foreach (QString ext, extensions.split(QLatin1Char(','))) {
                        ext = ext.trimmed();
                        if (ext.startsWith(QLatin1Char('.')))
                            ext.remove(0, 1);
                        extensionsNameFilters.insert(ext);
                    }
                }
                QDir::Filters filters = QDir::Files | QDir::NoSymLinks;
                if (recursiveScan)
                    filters |= QDir::AllDirs | QDir::NoDotAndDotDot;
                QFileInfoList fileinfolist;
                recursiveFileInfoList(dir, extensionsNameFilters, filters, &fileinfolist);
                int scanRootLen = dir.absolutePath().length();
                foreach (const QFileInfo &fi, fileinfolist) {
                    QString fn = QDir::cleanPath(fi.absoluteFilePath());
                    sourceFiles << fn;

                    if (!fn.endsWith(QLatin1String(".java"))
                        && !fn.endsWith(QLatin1String(".ui"))
                        && !fn.endsWith(QLatin1String(".js"))
                        && !fn.endsWith(QLatin1String(".qs"))) {
                        int offset = 0;
                        int depth = 0;
                        do {
                            offset = fn.lastIndexOf(QLatin1Char('/'), offset - 1);
                            QString ffn = fn.mid(offset + 1);
                            allCSources.insert(ffn, fn);
                        } while (++depth < 3 && offset > scanRootLen);
                    }
                }
            } else {
                sourceFiles << QDir::cleanPath(fi.absoluteFilePath());;
            }
        }
    } // for args

    foreach (const QString &proFile, proFiles)
        projectRoots.insert(QDir::cleanPath(QFileInfo(proFile).absolutePath()) + QLatin1Char('/'));

    bool firstPass = true;
    bool fail = false;
    while (firstPass || !proFiles.isEmpty()) {
        ConversionData cd;
        cd.m_defaultContext = defaultContext;
        cd.m_noUiLines = options & NoUiLines;
        cd.m_projectRoots = projectRoots;
        cd.m_includePath = includePath;
        cd.m_allCSources = allCSources;

        QStringList tsFiles = tsFileNames;
        if (proFiles.count() > 0) {
            QFileInfo pfi(proFiles.takeFirst());
            QHash<QByteArray, QStringList> variables;

            ProFileEvaluator visitor;
            visitor.setVerbose(options & Verbose);

            ProFile pro(pfi.absoluteFilePath());
            if (!visitor.queryProFile(&pro))
                return 2;
            if (!visitor.accept(&pro))
                return 2;

            if (visitor.templateType() == ProFileEvaluator::TT_Subdirs) {
                QDir proDir(pfi.absoluteDir());
                foreach (const QString &subdir, visitor.values(QLatin1String("SUBDIRS"))) {
                    QString subPro = QDir::cleanPath(proDir.absoluteFilePath(subdir));
                    QFileInfo subInfo(subPro);
                    if (subInfo.isDir())
                        proFiles << (subPro + QLatin1Char('/')
                                     + subInfo.fileName() + QLatin1String(".pro"));
                    else
                        proFiles << subPro;
                }
                continue;
            }

            cd.m_includePath += visitor.values(QLatin1String("INCLUDEPATH"));

            evaluateProFile(visitor, &variables, pfi.absolutePath());

            sourceFiles = variables.value("SOURCES");

            QStringList tmp = variables.value("CODECFORTR");
            if (!tmp.isEmpty() && !tmp.first().isEmpty()) {
                codecForTr = tmp.first().toLatin1();
                fetchedTor.setCodecName(codecForTr);
            }
            tmp = variables.value("CODECFORSRC");
            if (!tmp.isEmpty() && !tmp.first().isEmpty()) {
                codecForSource = tmp.first().toLatin1();
                if (!QTextCodec::codecForName(codecForSource))
                    qWarning("lupdate warning: Codec for source '%s' is invalid. Falling back to codec for tr().",
                             codecForSource.constData());
                else
                    cd.m_codecForSource = codecForSource;
            }

            tsFiles += variables.value("TRANSLATIONS");
        }

        QStringList sourceFilesCpp;
        for (QStringList::iterator it = sourceFiles.begin(); it != sourceFiles.end(); ++it) {
            if (it->endsWith(QLatin1String(".java"), Qt::CaseInsensitive))
                loadJava(fetchedTor, *it, cd);
            else if (it->endsWith(QLatin1String(".ui"), Qt::CaseInsensitive)
                     || it->endsWith(QLatin1String(".jui"), Qt::CaseInsensitive))
                loadUI(fetchedTor, *it, cd);
            else if (it->endsWith(QLatin1String(".js"), Qt::CaseInsensitive)
                     || it->endsWith(QLatin1String(".qs"), Qt::CaseInsensitive))
                loadQScript(fetchedTor, *it, cd);
            else
                sourceFilesCpp << *it;
        }
        loadCPP(fetchedTor, sourceFilesCpp, cd);
        if (!cd.error().isEmpty())
            printOut(cd.error());

        tsFiles.sort();
        tsFiles.removeDuplicates();

        if (!tsFiles.isEmpty())
            updateTsFiles(fetchedTor, tsFiles, codecForTr, sourceLanguage, targetLanguage, options, &fail);

        firstPass = false;
    }

    if (numFiles == 0) {
        printUsage();
        return 1;
    }

    return fail ? 1 : 0;
}
