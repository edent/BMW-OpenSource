/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "animation.h"

#include <QPointF>
#include <QIODevice>
#include <QDataStream>

class Frame 
{
public:
    Frame() {
    }

    int nodeCount() const 
    {
        return m_nodePositions.size();
    }

    void setNodeCount(int nodeCount)
    {
        while (nodeCount > m_nodePositions.size())
            m_nodePositions.append(QPointF());            
        
        while (nodeCount < m_nodePositions.size())
            m_nodePositions.removeLast();
    }

    QPointF nodePos(int idx) const
    {
        return m_nodePositions.at(idx);
    }

    void setNodePos(int idx, const QPointF &pos)
    {
        m_nodePositions[idx] = pos;
    }
    
private:
    QList<QPointF> m_nodePositions;
};

Animation::Animation()
{
    m_currentFrame = 0;
    m_frames.append(new Frame);
}

Animation::~Animation() 
{
    qDeleteAll(m_frames);
}

void Animation::setTotalFrames(int totalFrames)
{
    while (m_frames.size() < totalFrames)
        m_frames.append(new Frame);    

    while (totalFrames < m_frames.size())
        delete m_frames.takeLast();    
}

int Animation::totalFrames() const
{
    return m_frames.size();
}

void Animation::setCurrentFrame(int currentFrame)
{
    m_currentFrame = qMax(qMin(currentFrame, totalFrames()-1), 0);
}

int Animation::currentFrame() const
{
    return m_currentFrame;
}

void Animation::setNodeCount(int nodeCount)
{
    Frame *frame = m_frames.at(m_currentFrame);
    frame->setNodeCount(nodeCount);
}

int Animation::nodeCount() const
{
    Frame *frame = m_frames.at(m_currentFrame);
    return frame->nodeCount();
}

void Animation::setNodePos(int idx, const QPointF &pos)
{
    Frame *frame = m_frames.at(m_currentFrame);
    frame->setNodePos(idx, pos);
}

QPointF Animation::nodePos(int idx) const
{
    Frame *frame = m_frames.at(m_currentFrame);
    return frame->nodePos(idx);
}

QString Animation::name() const
{
    return m_name;
}

void Animation::setName(const QString &name)
{
    m_name = name;
}

void Animation::save(QIODevice *device) const
{
    QDataStream stream(device);
    stream << m_name;
    stream << m_frames.size();
    foreach (Frame *frame, m_frames) {
        stream << frame->nodeCount();
        for (int i=0; i<frame->nodeCount(); ++i)
            stream << frame->nodePos(i);
    }
}

void Animation::load(QIODevice *device)
{
    if (!m_frames.isEmpty())
        qDeleteAll(m_frames);

    m_frames.clear();

    QDataStream stream(device);
    stream >> m_name;
    
    int frameCount;
    stream >> frameCount;

    for (int i=0; i<frameCount; ++i) {
        
        int nodeCount;
        stream >> nodeCount;
        
        Frame *frame = new Frame;
        frame->setNodeCount(nodeCount);

        for (int j=0; j<nodeCount; ++j) {
            QPointF pos;
            stream >> pos;

            frame->setNodePos(j, pos);
        }

        m_frames.append(frame);
    }
}
