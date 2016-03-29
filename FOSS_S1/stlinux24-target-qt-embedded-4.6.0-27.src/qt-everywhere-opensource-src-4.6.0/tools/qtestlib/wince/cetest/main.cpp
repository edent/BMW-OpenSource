/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifdef QT_CETEST_NO_ACTIVESYNC
#   include "cetcpsyncconnection.h"
#else
#   include "activesyncconnection.h"
#endif

#include "deployment.h"
#include <option.h>
#include <project.h>
#include <property.h>
#include <qstringlist.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <iostream>
using namespace std;

const int debugLevel = 0;
void debugOutput(const QString& text, int level)
{
    if (level <= debugLevel)
        cout << qPrintable(text) << endl;
}

// needed for QMake sources to compile
QString project_builtin_regx() { return QString();}
static QString pwd;
QString qmake_getpwd()
{
    if(pwd.isNull())
        pwd = QDir::currentPath();
    return pwd;
}
bool qmake_setpwd(const QString &p)
{
    if(QDir::setCurrent(p)) {
        pwd = QDir::currentPath();
        return true;
    }
    return false;
}

namespace TestConfiguration {
    QString localExecutable;
    QString localQtConf;
    QString remoteTestPath;
    QString remoteLibraryPath;
    QString remoteExecutable;
    QString remoteResultFile;

    bool testDebug;
    void init()
    {
        testDebug = true;
        localQtConf = QLatin1String("no");
        remoteTestPath = QLatin1String("\\Program Files\\qt_test");
        remoteLibraryPath = remoteTestPath;
        remoteResultFile = QLatin1String("\\qt_test_results.txt");
    }
}

void usage()
{
    cout <<
        "QTestLib options\n"
        " -functions : Returns a list of current testfunctions\n"
        " -xml       : Outputs results as XML document\n"
        " -lightxml  : Outputs results as stream of XML tags\n"
        " -o filename: Writes all output into a file\n"
        " -silent    : Only outputs warnings and failures\n"
        " -v1        : Print enter messages for each testfunction\n"
        " -v2        : Also print out each QVERIFY/QCOMPARE/QTEST\n"
        " -vs        : Print every signal emitted\n"
        " -eventdelay ms    : Set default delay for mouse and keyboard simulation to ms milliseconds\n"
        " -keydelay ms      : Set default delay for keyboard simulation to ms milliseconds\n"
        " -mousedelay ms    : Set default delay for mouse simulation to ms milliseconds\n"
        " -keyevent-verbose : Turn on verbose messages for keyboard simulation\n"
        " -maxwarnings n    : Sets the maximum amount of messages to output.\n"
        "                     0 means unlimited, default: 2000\n"
        " -help             : This help\n";
    cout <<
        "cetest specific options\n"
        " -debug            : Test debug version[default]\n"
        " -release          : Test release version\n"
        " -libpath <path>   : Remote path to deploy Qt libraries to\n"
        " -qt-delete        : Delete the Qt libraries after execution\n"
        " -project-delete   : Delete the project file(s) after execution\n"
        " -delete           : Delete everything deployed after execution\n"
        " -conf             : Specify location of qt.conf file\n"
        " -f <file>         : Specify project file\n"
        " -cache <file>     : Specify .qmake.cache file to use\n"
        " -d                : Increase qmake debugging \n"
        " -timeout <value>  : Specify a timeout value after which the test will be terminated\n"
        "                     -1 specifies waiting forever (default)\n"
        "                      0 specifies starting the process detached\n"
        "                     >0 wait <value> seconds\n"
        "\n";
}

int main(int argc, char **argv)
{
    QStringList arguments;
    for (int i=0; i<argc; ++i)
        arguments.append(QString::fromLatin1(argv[i]));

    TestConfiguration::init();

    QStringList launchArguments;
    QString resultFile;
    QString proFile;
    QString cacheFile;
    int timeout = -1;
    bool cleanupQt = false;
    bool cleanupProject = false;

    for (int i=1; i<arguments.size(); ++i) {
        if (arguments.at(i).toLower() == QLatin1String("-help")
                    || arguments.at(i).toLower() == QLatin1String("--help")
                    || arguments.at(i).toLower() == QLatin1String("/?")) {
            usage();
            return 0;
        } else if (arguments.at(i).toLower() == QLatin1String("-o")) {
            if (++i == arguments.size()) {
                cout << "Error: No output file specified!" << endl;
                return -1;
            }
            resultFile = arguments.at(i);
        } else if (arguments.at(i).toLower() == QLatin1String("-eventdelay")
                    || arguments.at(i).toLower() == QLatin1String("-keydelay")
                    || arguments.at(i).toLower() == QLatin1String("-mousedelay")
                    || arguments.at(i).toLower() == QLatin1String("-maxwarnings")) {
            launchArguments.append(arguments.at(i++));
            if (i == arguments.size()) {
                cout << "Please specify value for:" << qPrintable(arguments.at(i-1).mid(1)) << endl;
                return -1;
            }
            launchArguments.append(arguments.at(i));
        } else if (arguments.at(i).toLower() == QLatin1String("-debug")) {
            TestConfiguration::testDebug = true;
            Option::before_user_vars.append("CONFIG-=release");
            Option::before_user_vars.append("CONFIG+=debug");
        } else if (arguments.at(i).toLower() == QLatin1String("-release")) {
            TestConfiguration::testDebug = false;
            Option::before_user_vars.append("CONFIG-=debug");
            Option::before_user_vars.append("CONFIG+=release");
        } else if (arguments.at(i).toLower() == QLatin1String("-libpath")) {
            if (++i == arguments.size()) {
                cout << "Error: No library path specified!" << endl;
                return -1;
            }
            TestConfiguration::remoteLibraryPath = arguments.at(i);
        } else if (arguments.at(i).toLower() == QLatin1String("-qt-delete")) {
            cleanupQt = true;
        } else if (arguments.at(i).toLower() == QLatin1String("-project-delete")) {
            cleanupProject = true;
        } else if (arguments.at(i).toLower() == QLatin1String("-delete")) {
            cleanupQt = true;
            cleanupProject = true;
        } else if (arguments.at(i).toLower() == QLatin1String("-conf")) {
            if (++i == arguments.size()) {
                cout << "Error: No qt.conf file specified!" << endl;
                return -1;
            }
            if (!QFileInfo(arguments.at(i)).exists())
                cout << "Warning: could not find qt.conf file at:" << qPrintable(arguments.at(i)) << endl;
            else
                TestConfiguration::localQtConf = arguments.at(i);
        } else if (arguments.at(i).toLower() == QLatin1String("-f")) {
            if (++i == arguments.size()) {
                cout << "Error: No output file specified!" << endl;
                return -1;
            }
            proFile = arguments.at(i);
        } else if (arguments.at(i).toLower() == QLatin1String("-cache")) {
            if (++i == arguments.size()) {
                cout << "Error: No cache file specified!" << endl;
                return -1;
            }
            cacheFile = arguments.at(i);
        } else if (arguments.at(i).toLower() == QLatin1String("-d")) {
            Option::debug_level++;
        } else if (arguments.at(i).toLower() == QLatin1String("-timeout")) {
            if (++i == arguments.size()) {
                cout << "Error: No timeout value specified!" << endl;
                return -1;
            }
            timeout = QString(arguments.at(i)).toInt();
        } else {
            launchArguments.append(arguments.at(i));
        }
    }

    // check for .pro file
    if (proFile.isEmpty()) {
        proFile = QDir::current().dirName() + QLatin1String(".pro");
        if (!QFileInfo(proFile).exists()) {
            cout << "Error: Could not find project file in current directory." << endl;
            return -1;
        }
        debugOutput(QString::fromLatin1("Using Project File:").append(proFile),1);
    }else {
        if (!QFileInfo(proFile).exists()) {
            cout << "Error: Project file does not exist " << qPrintable(proFile) << endl;
            return -1;
        }
    }

    Option::before_user_vars.append("CONFIG+=build_pass");

    // read target and deployment rules passing the .pro to use instead of
    //      relying on qmake guessing the .pro to use
    int qmakeArgc = 2;
    QByteArray ba(QFile::encodeName(proFile));
    char* proFileEncodedName =  ba.data();
    char* qmakeArgv[2] = { "qmake.exe", proFileEncodedName };

    Option::qmake_mode = Option::QMAKE_GENERATE_NOTHING;
    Option::output_dir = qmake_getpwd();
    if (!cacheFile.isEmpty())
        Option::mkfile::cachefile = cacheFile;
    int ret = Option::init(qmakeArgc, qmakeArgv);
    if(ret != Option::QMAKE_CMDLINE_SUCCESS) {
        cout << "Error: could not parse " << qPrintable(proFile) << endl;
        return -1;
    }

    QMakeProperty prop;
    QMakeProject project(&prop);

    project.read(proFile);
    if (project.values("TEMPLATE").join(" ").toLower() != QString("app")) {
        cout << "Error: Can only test executables!" << endl;
        return -1;
    }
    // Check wether the project is still in debug/release mode after reading
    // If .pro specifies to be one mode only, we need to accept this
    if (project.isActiveConfig("debug"))
        TestConfiguration::testDebug = true;
    else
        TestConfiguration::testDebug = false;

    // determine what is the real mkspec to use if the default mkspec is being used
    if (Option::mkfile::qmakespec.endsWith("/default"))
        project.values("QMAKESPEC") = project.values("QMAKESPEC_ORIGINAL");
    else
        project.values("QMAKESPEC") = QStringList() << Option::mkfile::qmakespec;

   // ensure that QMAKESPEC is non-empty .. to meet requirements of QList::at()
   if (project.values("QMAKESPEC").isEmpty()){
       cout << "Error: QMAKESPEC not set after parsing " << qPrintable(proFile) << endl;
       return -1;
   }

   // ensure that QT_CE_C_RUNTIME is non-empty .. to meet requirements of QList::at()
   if (project.values("QT_CE_C_RUNTIME").isEmpty()){
       cout << "Error: QT_CE_C_RUNTIME not defined in mkspec/qconfig.pri " << qPrintable(project.values("QMAKESPEC").join(" "));
       return -1;
   }

    QString destDir = project.values("DESTDIR").join(" ");
    if (!destDir.isEmpty()) {
        if (QDir::isRelativePath(destDir)) {
            QFileInfo fi(proFile);
            if (destDir == QLatin1String("."))
                destDir = fi.absolutePath() + "/" + destDir + "/" + (TestConfiguration::testDebug ? "debug" : "release");
            else
                destDir = fi.absolutePath() + QDir::separator() + destDir;
        }
    } else {
        QFileInfo fi(proFile);
        destDir = fi.absolutePath();
        destDir += QDir::separator() + QLatin1String(TestConfiguration::testDebug ? "debug" : "release");
    }

    DeploymentList qtDeploymentList;
    DeploymentList projectDeploymentList;

    TestConfiguration::localExecutable = Option::fixPathToLocalOS(destDir + QDir::separator() + project.values("TARGET").join(" ") + QLatin1String(".exe"));
    TestConfiguration::remoteTestPath = QLatin1String("\\Program Files\\") + Option::fixPathToLocalOS(project.values("TARGET").join(QLatin1String(" ")));
    if (!arguments.contains(QLatin1String("-libpath"), Qt::CaseInsensitive))
        TestConfiguration::remoteLibraryPath = TestConfiguration::remoteTestPath;

    QString targetExecutable = Option::fixPathToLocalOS(project.values("TARGET").join(QLatin1String(" ")));
    int last = targetExecutable.lastIndexOf(QLatin1Char('\\'));
    targetExecutable = targetExecutable.mid( last == -1 ? 0 : last+1 );
    TestConfiguration::remoteExecutable = TestConfiguration::remoteTestPath + QDir::separator() + targetExecutable + QLatin1String(".exe");
    projectDeploymentList.append(CopyItem(TestConfiguration::localExecutable , TestConfiguration::remoteExecutable));

    // deploy
#ifdef QT_CETEST_NO_ACTIVESYNC
    CeTcpSyncConnection connection;
#else
    ActiveSyncConnection connection;
#endif
    if (!connection.connect()) {
        cout << "Error: Could not connect to device!" << endl;
        return -1;
    }
    DeploymentHandler deployment;
    deployment.setConnection(&connection);

    deployment.initQtDeploy(&project, qtDeploymentList, TestConfiguration::remoteLibraryPath);
    deployment.initProjectDeploy(&project , projectDeploymentList, TestConfiguration::remoteTestPath);

    // add qt.conf
    if (TestConfiguration::localQtConf != QLatin1String("no")) {
        QString qtConfOrigin = QFileInfo(TestConfiguration::localQtConf).absoluteFilePath();
        QString qtConfTarget = Option::fixPathToLocalOS(TestConfiguration::remoteTestPath + QDir::separator() + QLatin1String("qt.conf"));
        projectDeploymentList.append(CopyItem(qtConfOrigin, qtConfTarget));
    }

    if (!deployment.deviceDeploy(qtDeploymentList) || !deployment.deviceDeploy(projectDeploymentList)) {
        cout << "Error: Could not copy file(s) to device" << endl;
        return -1;
    }

    // launch
    launchArguments.append("-o");
    launchArguments.append(TestConfiguration::remoteResultFile);

    cout << endl << "Remote Launch:" << qPrintable(TestConfiguration::remoteExecutable) << " " << qPrintable(launchArguments.join(" ")) << endl;
    if (!connection.execute(TestConfiguration::remoteExecutable, launchArguments.join(" "), timeout)) {
        cout << "Error: Could not execute target file" << endl;
        return -1;
    }


    // copy result file
    // show results
    if (resultFile.isEmpty()) {
        QString tempResultFile = Option::fixPathToLocalOS(QDir::tempPath() + "/qt_ce_temp_result_file.txt");
        if (connection.copyFileFromDevice(TestConfiguration::remoteResultFile, tempResultFile)) {
            QFile file(tempResultFile);
            QByteArray arr;
            if (file.open(QIODevice::ReadOnly)) {
                arr = file.readAll();
                cout << arr.constData() << endl;
            }
            file.close();
            file.remove();
        }
    } else {
        connection.copyFileFromDevice(TestConfiguration::remoteResultFile, resultFile);
    }

    // delete
    connection.deleteFile(TestConfiguration::remoteResultFile);
    if (cleanupQt)
        deployment.cleanup(qtDeploymentList);
    if (cleanupProject)
        deployment.cleanup(projectDeploymentList);
    return 0;
}
