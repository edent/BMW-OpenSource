/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Symbian application wrapper of the Qt Toolkit.
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

// MCRT0.CPP
//
// © Portions copyright (c) 2005-2006 Nokia Corporation.  All rights reserved.
// Copyright (c) Symbian Software Ltd 1997-2004.  All rights reserved.
//

// EPOC32 version of crt0.c for C programs which always want multi-threaded support

#include <e32std.h>
#include <exception> // must be before e32base.h so uncaught_exception gets defined
#include <e32base.h>
#include "estlib.h"

// Needed for QT_TRYCATCH_LEAVING.
#include <qglobal.h>

#ifdef __ARMCC__
__asm int CallMain(int argc, char *argv[], char *envp[])
{
    import main
    code32
    b main
}
#define CALLMAIN(argc, argv, envp) CallMain(argc, argv, envp)
#else
extern "C" int main(int argc, char *argv[], char *envp[]);
#define CALLMAIN(argc, argv, envp) main(argc, argv, envp)
#endif

// Dummy function to handle GCCE toolchain problem
extern "C" GLDEF_C int __GccGlueInit()
{
    return 0;
}

extern "C" IMPORT_C void exit(int ret);

GLDEF_C TInt QtMainWrapper()
{
    int argc = 0;
    char **argv = 0;
    char **envp = 0;
    // get args & environment
    __crt0(argc, argv, envp);
    //Call user(application)'s main
    TRAPD(ret, QT_TRYCATCH_LEAVING(ret = CALLMAIN(argc, argv, envp);));
    delete[] argv;
    delete[] envp;
    return ret;
}


#ifdef __GCC32__

/* stub function inserted into main() by GCC */
extern "C" void __gccmain(void) {}

/* Default GCC entrypoint */
extern "C" TInt _mainCRTStartup(void)
{
    extern TInt _E32Startup();
    return _E32Startup();
}

#endif /* __GCC32__ */

#ifdef __EABI__

// standard entrypoint for C runtime, expected by some linkers
// Symbian OS does not currently use this function
extern "C" void __main() {}
#endif
