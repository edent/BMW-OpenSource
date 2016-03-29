/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "bassboost.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::BassBoost
  \internal
*/

BassBoost::BassBoost(QObject *parent) : AbstractAudioEffect::AbstractAudioEffect(parent,
                                                                                QList<EffectParameter>())
{
}

void BassBoost::parameterChanged(const int,
                                 const QVariant &)
{
    // We should never be called, because we have no parameters.
}

bool BassBoost::activateOn(CPlayerType *player)
{
    m_effect.reset(CBassBoost::NewL(*player, true));
    return true;
}

QT_END_NAMESPACE

