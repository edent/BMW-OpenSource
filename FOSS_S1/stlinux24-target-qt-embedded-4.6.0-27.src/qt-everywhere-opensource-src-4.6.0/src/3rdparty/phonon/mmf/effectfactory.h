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

#ifndef PHONON_MMF_EFFECTFACTORY_H
#define PHONON_MMF_EFFECTFACTORY_H

#include "abstractaudioeffect.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace MMF
{

/**
 * @short Contains utility functions related to effects.
 */
class EffectFactory
{
public:
    /**
     * @short Creates an audio effect of type @p type.
     */
    static AbstractAudioEffect *createAudioEffect(AbstractAudioEffect::Type type,
                                                  QObject *parent);

    /**
     * @short Return the properties for effect @p type.
     *
     * This handles the effects for
     * BackendInterface::objectDescriptionProperties().
     */
    static QHash<QByteArray, QVariant> audioEffectDescriptions(AbstractAudioEffect::Type type);

    /**
     * @short Returns the indexes for the supported effects.
     *
     * This handles the effects for
     * BackendInterface::objectDescriptionIndexes().
     */
    static QList<int> effectIndexes();

private:
    static inline QHash<QByteArray, QVariant> constructEffectDescription(const QString &name,
                                                                         const QString &description);

    /**
     * This class is not supposed to be instantiated, so disable
     * the default constructor.
     */
    inline EffectFactory();
    Q_DISABLE_COPY(EffectFactory)
};
}
}

QT_END_NAMESPACE

#endif

