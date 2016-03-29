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

#ifndef DEPLOYMENT_INCL
#define DEPLOYMENT_INCL

#include <qstring.h>
#include <qlist.h>
#include <project.h>

class AbstractRemoteConnection;

struct CopyItem
{
    CopyItem(const QString& f, const QString& t) : from(f) , to(t) { }
    QString from;
    QString to;
};
typedef QList<CopyItem> DeploymentList;

class DeploymentHandler
{
public:
    inline void setConnection(AbstractRemoteConnection*);
    inline AbstractRemoteConnection* connection() const;
    bool deviceCopy(const DeploymentList &deploymentList);
    bool deviceDeploy(const DeploymentList &deploymentList);
    void cleanup(const DeploymentList &deploymentList);
    static void initProjectDeploy(QMakeProject* project, DeploymentList &deploymentList, const QString &testPath = "\\Program Files\\qt_test");
    static void initQtDeploy(QMakeProject* project, DeploymentList &deploymentList, const QString &testPath = "\\Program Files\\qt_test");
private:
    AbstractRemoteConnection* m_connection;
};

inline void DeploymentHandler::setConnection(AbstractRemoteConnection *connection) { m_connection = connection; }
inline AbstractRemoteConnection* DeploymentHandler::connection() const { return m_connection; }
#endif