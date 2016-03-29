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
#include "commands.h"

#define CLEAN_FAIL(a) {delete appName; \
    delete arguments; \
    return (a); }

int qRemoteLaunch(DWORD, BYTE*, DWORD*, BYTE**, IRAPIStream* stream)
{
    if (!stream)
        return -1;

    DWORD bytesRead;
    int appLength;
    wchar_t* appName = 0;
    int argumentsLength;
    wchar_t* arguments = 0;
    int timeout = -1;
    int returnValue = -2;
    DWORD error = 0;

    if (S_OK != stream->Read(&appLength, sizeof(appLength), &bytesRead))
        CLEAN_FAIL(-2);
    appName = (wchar_t*) malloc(sizeof(wchar_t)*(appLength + 1));
    if (S_OK != stream->Read(appName, sizeof(wchar_t)*appLength, &bytesRead))
        CLEAN_FAIL(-2);
    appName[appLength] = '\0';

    if (S_OK != stream->Read(&argumentsLength, sizeof(argumentsLength), &bytesRead))
        CLEAN_FAIL(-2);
    arguments = (wchar_t*) malloc(sizeof(wchar_t)*(argumentsLength + 1));
    if (S_OK != stream->Read(arguments, sizeof(wchar_t)*argumentsLength, &bytesRead))
        CLEAN_FAIL(-2);
    arguments[argumentsLength] = '\0';

    if (S_OK != stream->Read(&timeout, sizeof(timeout), &bytesRead))
        CLEAN_FAIL(-2);

    bool result = qRemoteExecute(appName, arguments, &returnValue, &error, timeout);

    if (timeout != 0) {
        if (S_OK != stream->Write(&returnValue, sizeof(returnValue), &bytesRead))
            CLEAN_FAIL(-4);
        if (S_OK != stream->Write(&error, sizeof(error), &bytesRead))
            CLEAN_FAIL(-5);
    }
    delete appName;
    delete arguments;
    // We need to fail here for the execute, otherwise the calling application will wait
    // forever for the returnValue.
    if (!result)
        return -3;
    return S_OK;
}


bool qRemoteExecute(const wchar_t* program, const wchar_t* arguments, int *returnValue, DWORD* error, int timeout)
{
    *error = 0;

    if (!program)
        return false;

    PROCESS_INFORMATION pid;
    if (!CreateProcess(program, arguments, NULL, NULL, false, 0, NULL, NULL, NULL, &pid)) {
        *error = GetLastError();
        wprintf(L"Could not launch: %s\n", program);
        return false;
    }

    // Timeout is in seconds
    DWORD waitingTime = (timeout == -1) ? INFINITE : timeout * 1000;

    if (waitingTime != 0) {
        DWORD waitStatus = WaitForSingleObject(pid.hProcess, waitingTime);
        if (waitStatus == WAIT_TIMEOUT) {
            TerminateProcess(pid.hProcess, 2);
            return false;
        } else if (waitStatus == WAIT_OBJECT_0 && returnValue) {
            *returnValue = 0;
            DWORD exitCode;
            if (GetExitCodeProcess(pid.hProcess, &exitCode))
                *returnValue = exitCode;
        }
    }
    return true;
}
