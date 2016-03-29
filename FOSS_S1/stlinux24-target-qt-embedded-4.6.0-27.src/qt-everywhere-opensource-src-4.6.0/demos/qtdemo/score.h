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

#ifndef SCORE_H
#define SCORE_H

#include <QList>
#include <QHash>
#include "demoitemanimation.h"

typedef QList<DemoItemAnimation *> Movie;
typedef QHash<QString, Movie*> MovieIndex;

class PlayListMember
{
public:
    PlayListMember(Movie *movie, int runMode) : movie(movie), runMode(runMode){};
    Movie *movie;
    int runMode;
};
typedef QList<PlayListMember> PlayList;

class Score
{
public:
    enum LOCK_MODE {LOCK_ITEMS, UNLOCK_ITEMS, SKIP_LOCK};
    enum RUN_MODE {FROM_CURRENT, FROM_START, NEW_ANIMATION_ONLY, ONLY_IF_VISIBLE};

    Score();
    virtual ~Score();

    void playMovie(const QString &indexName, RUN_MODE runMode = FROM_START, LOCK_MODE lockMode = SKIP_LOCK);
    void insertMovie(const QString &indexName, Movie *movie);
    Movie *insertMovie(const QString &indexName);
    void queueMovie(const QString &indexName, RUN_MODE runMode = FROM_START, LOCK_MODE lockMode = SKIP_LOCK);
    void playQue();
    bool hasQueuedMovies(){ return this->playList.size() > 0; };

    MovieIndex index;
    PlayList playList;

private:
    void prepare(Movie *movie, RUN_MODE runMode, LOCK_MODE lockMode);
    void play(Movie *movie, RUN_MODE runMode);
};

#endif // SCORE_H

