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

#include "demoitem.h"
#include "menumanager.h"
#include "guide.h"
#include "colors.h"

QHash<QString, SharedImage *> DemoItem::sharedImageHash;
QMatrix DemoItem::matrix;

DemoItem::DemoItem(QGraphicsScene *scene, QGraphicsItem *parent) : QGraphicsItem(parent, scene)
{
    this->opacity = 1.0;
    this->locked = false;
    this->prepared = false;
    this->neverVisible = false;
    this->noSubPixeling = false;
    this->currentAnimation = 0;
    this->currGuide = 0;
    this->guideFrame = 0;
    this->sharedImage = new SharedImage();
    ++this->sharedImage->refCount;
}

DemoItem::~DemoItem()
{
    if(--this->sharedImage->refCount == 0){
        if (!this->hashKey.isEmpty())
            DemoItem::sharedImageHash.remove(this->hashKey);
        delete this->sharedImage;
    }
}

void DemoItem::setNeverVisible(bool never)
{
    Q_UNUSED(never);
/*
    this->neverVisible = never;
    if (never){
        this->setVisible(false);
        QList<QGraphicsItem *> c = children();
        for (int i=0; i<c.size(); i++){
            DemoItem *d = dynamic_cast<DemoItem *>(c[i]); // Don't use dynamic cast because it needs RTTI support.
            if (d)
                d->setNeverVisible(true);
            else{
                c[i]->setVisible(false);
            }
        }
    }
*/
}

void DemoItem::setRecursiveVisible(bool visible){
    if (visible && this->neverVisible){
        this->setVisible(false);
        return;
    }

    this->setVisible(visible);
    QList<QGraphicsItem *> c = children();
    for (int i=0; i<c.size(); i++){
        // DemoItem *d = dynamic_cast<DemoItem *>(c[i]);
        // if (d)
        //     d->setRecursiveVisible(visible);
        // else{
            c[i]->setVisible(visible);
        // }
    }
}

void DemoItem::useGuide(Guide *guide, float startFrame)
{
    this->startFrame = startFrame;
    this->guideFrame = startFrame;
    while (this->guideFrame > guide->startLength + guide->length()){
        if (guide->nextGuide == guide->firstGuide)
            break;

        guide = guide->nextGuide;
    }
    this->currGuide = guide;
}

void DemoItem::guideAdvance(float distance)
{
    this->guideFrame += distance;
    while (this->guideFrame > this->currGuide->startLength + this->currGuide->length()){
        this->currGuide = this->currGuide->nextGuide;
        if (this->currGuide == this->currGuide->firstGuide)
            this->guideFrame -= this->currGuide->lengthAll();
    }
}

void DemoItem::guideMove(float moveSpeed)
{
    this->currGuide->guide(this, moveSpeed);
}

void DemoItem::setPosUsingSheepDog(const QPointF &dest, const QRectF &sceneFence)
{
    this->setPos(dest);
    if (sceneFence.isNull())
        return;

    // I agree. This is not the optimal way of doing it.
    // But don't want for use time on it now....
    float itemWidth = this->boundingRect().width();
    float itemHeight = this->boundingRect().height();
    float fenceRight = sceneFence.x() + sceneFence.width();
    float fenceBottom = sceneFence.y() + sceneFence.height();

    if (this->scenePos().x() < sceneFence.x()) this->moveBy(this->mapFromScene(QPointF(sceneFence.x(), 0)).x(), 0);
    if (this->scenePos().x() > fenceRight - itemWidth) this->moveBy(this->mapFromScene(QPointF(fenceRight - itemWidth, 0)).x(), 0);
    if (this->scenePos().y() < sceneFence.y()) this->moveBy(0, this->mapFromScene(QPointF(0, sceneFence.y())).y());
    if (this->scenePos().y() > fenceBottom - itemHeight) this->moveBy(0, this->mapFromScene(QPointF(0, fenceBottom - itemHeight)).y());
}

void DemoItem::setGuidedPos(const QPointF &pos)
{
    this->guidedPos = pos;
}

QPointF DemoItem::getGuidedPos()
{
    return this->guidedPos;
}

void DemoItem::switchGuide(Guide *guide)
{
    this->currGuide = guide;
    this->guideFrame = 0;
}

bool DemoItem::inTransition()
{
    if (this->currentAnimation)
        return this->currentAnimation->running();
    else
        return false;
}

void DemoItem::setMatrix(const QMatrix &matrix)
{
    DemoItem::matrix = matrix;
}

void DemoItem::useSharedImage(const QString &hashKey)
{
    this->hashKey = hashKey;
    if (!sharedImageHash.contains(hashKey))
        sharedImageHash.insert(hashKey, this->sharedImage);
    else {
        if(--this->sharedImage->refCount == 0)
            delete this->sharedImage;
        this->sharedImage = sharedImageHash.value(hashKey);
        ++this->sharedImage->refCount;
    }
}

bool DemoItem::validateImage()
{
    if ((this->sharedImage->matrix != DemoItem::matrix && !Colors::noRescale) || !(this->sharedImage->image || this->sharedImage->pixmap)){
        // (Re)create image according to new matrix
        delete this->sharedImage->image;
        this->sharedImage->image = 0;
        delete this->sharedImage->pixmap;
        this->sharedImage->pixmap = 0;
        this->sharedImage->matrix = DemoItem::matrix;

        // Let subclass create and draw a new image according to the new matrix
        QImage *image = this->createImage(Colors::noRescale ? QMatrix() : DemoItem::matrix);
        if (image){
            if (Colors::showBoundingRect){
                // draw red transparent rect
                QPainter painter(image);
                painter.fillRect(image->rect(), QColor(255, 0, 0, 50));
                painter.end();
            }

            this->sharedImage->unscaledBoundingRect = this->sharedImage->matrix.inverted().mapRect(image->rect());
            if (Colors::usePixmaps){
                if (image->isNull())
                    this->sharedImage->pixmap = new QPixmap(1, 1);
                else
                    this->sharedImage->pixmap = new QPixmap(image->size());
                this->sharedImage->pixmap->fill(QColor(0, 0, 0, 0));
                QPainter painter(this->sharedImage->pixmap);
                painter.drawImage(0, 0, *image);
                delete image;
            } else {
                this->sharedImage->image = image;
            }
            return true;
        } else
            return false;
    }
    return true;
}

QRectF DemoItem::boundingRect() const
{
    const_cast<DemoItem *>(this)->validateImage();
    return this->sharedImage->unscaledBoundingRect;
}

void DemoItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (this->validateImage()){

        bool wasSmoothPixmapTransform = painter->testRenderHint(QPainter::SmoothPixmapTransform);
        painter->setRenderHint(QPainter::SmoothPixmapTransform);

        if (Colors::noRescale){
            // Let the painter scale the image for us.
            // This may degrade both quality and performance
            if (this->sharedImage->image)
                painter->drawImage(this->pos(), *this->sharedImage->image);
            else
                painter->drawPixmap(this->pos(), *this->sharedImage->pixmap);
        }
        else {
            QMatrix m = painter->worldMatrix();
            painter->setWorldMatrix(QMatrix());
            float x = this->noSubPixeling ? qRound(m.dx()) : m.dx();
            float y = this->noSubPixeling ? qRound(m.dy()) : m.dy();
            if (this->sharedImage->image)
                painter->drawImage(QPointF(x, y), *this->sharedImage->image);
            else
                painter->drawPixmap(QPointF(x, y), *this->sharedImage->pixmap);
        }

        if (!wasSmoothPixmapTransform) {
            painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
        }

    }
}
