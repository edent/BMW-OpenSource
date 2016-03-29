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

#include "score.h"
#include "colors.h"
#include "demoitem.h"

Score::Score()
{
}

Score::~Score()
{
    // NB! Deleting all movies.
    qDeleteAll(this->index);
}

void Score::prepare(Movie *movie, RUN_MODE runMode, LOCK_MODE lockMode)
{
    if (lockMode == LOCK_ITEMS){
        for (int i=0; i<movie->size(); ++i){
            if (runMode == ONLY_IF_VISIBLE && !movie->at(i)->demoItem()->isVisible())
                continue;
            movie->at(i)->lockItem(true);
            movie->at(i)->prepare();
        }
    }
    else if (lockMode == UNLOCK_ITEMS){
        for (int i=0; i<movie->size(); ++i){
            if (runMode == ONLY_IF_VISIBLE && !movie->at(i)->demoItem()->isVisible())
                continue;
            movie->at(i)->lockItem(false);
            movie->at(i)->prepare();
        }
    }
    else {
        for (int i=0; i<movie->size(); ++i){
            if (runMode == ONLY_IF_VISIBLE && !movie->at(i)->demoItem()->isVisible())
                continue;
            movie->at(i)->prepare();
        }
    }
}

void Score::play(Movie *movie, RUN_MODE runMode)
{
    if (runMode == NEW_ANIMATION_ONLY){
        for (int i=0; i<movie->size(); ++i)
            if (movie->at(i)->notOwnerOfItem())
                movie->at(i)->play(true);
    }
    else if (runMode == ONLY_IF_VISIBLE){
        for (int i=0; i<movie->size(); ++i)
            if (movie->at(i)->demoItem()->isVisible())
                movie->at(i)->play(runMode == FROM_START);
    }
    else {
        for (int i=0; i<movie->size(); ++i)
            movie->at(i)->play(runMode == FROM_START);
    }
}

void Score::playMovie(const QString &indexName, RUN_MODE runMode, LOCK_MODE lockMode)
{
    MovieIndex::iterator movieIterator = this->index.find(indexName);
    if (movieIterator == this->index.end())
        return;

    Movie *movie = *movieIterator;
    this->prepare(movie, runMode, lockMode);
    this->play(movie, runMode);
}

void Score::queueMovie(const QString &indexName, RUN_MODE runMode, LOCK_MODE lockMode)
{
    MovieIndex::iterator movieIterator = this->index.find(indexName);
    if (movieIterator == this->index.end()){
        if (Colors::verbose)
            qDebug() << "Queuing movie:" << indexName << "(does not exist)";
        return;
    }

    Movie *movie = *movieIterator;
    this->prepare(movie, runMode, lockMode);
    this->playList.append(PlayListMember(movie, int(runMode)));
    if (Colors::verbose)
        qDebug() << "Queuing movie:" << indexName;
}

void Score::playQue()
{
    int movieCount = this->playList.size();
    for (int i=0; i<movieCount; i++)
        this->play(this->playList.at(i).movie, RUN_MODE(this->playList.at(i).runMode));
    this->playList.clear();
    if (Colors::verbose)
        qDebug() << "********* Playing que *********";
}

void Score::insertMovie(const QString &indexName, Movie *movie)
{
    this->index.insert(indexName, movie);
}

Movie *Score::insertMovie(const QString &indexName)
{
    Movie *movie = new Movie();
    insertMovie(indexName, movie);
    return movie;
}

