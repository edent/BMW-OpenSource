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

#include <QObject>
#include <QCoreApplication>

#include <AudioEqualizerBase.h>
#include <BassBoostBase.h>
#include <DistanceAttenuationBase.h>
#include <DopplerBase.h>
#include <EnvironmentalReverbBase.h>
#include <ListenerOrientationBase.h>
#include <LocationBase.h>
#include <LoudnessBase.h>
#include <SourceOrientationBase.h>
#include <StereoWideningBase.h>

#include "audioequalizer.h"
#include "bassboost.h"

#include "effectfactory.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::EffectFactory
  \internal
*/

QHash<QByteArray, QVariant> EffectFactory::constructEffectDescription(const QString &name,
                                                                      const QString &description)
{
    QHash<QByteArray, QVariant> retval;

    retval.insert("name", name);
    retval.insert("description", description);
    retval.insert("available", true);

    return retval;
}


QHash<QByteArray, QVariant> EffectFactory::audioEffectDescriptions(AbstractAudioEffect::Type type)
{
    switch (type)
    {
        case AbstractAudioEffect::EffectAudioEqualizer:
            return constructEffectDescription(QCoreApplication::translate("Phonon::MMF::EffectFactory", "Audio Equalizer"), "Audio equalizer.");
        case AbstractAudioEffect::EffectBassBoost:
            return constructEffectDescription(QCoreApplication::translate("Phonon::MMF::EffectFactory", "Bass Boost"), "Bass boost.");
        case AbstractAudioEffect::EffectDistanceAttenuation:
            return constructEffectDescription(QCoreApplication::translate("Phonon::MMF::EffectFactory", "Distance Attenuation"), "Distance Attenuation.");
        case AbstractAudioEffect::EffectEnvironmentalReverb:
            return constructEffectDescription(QCoreApplication::translate("Phonon::MMF::EffectFactory", "Environmental Reverb"), "Environmental Reverb.");
        case AbstractAudioEffect::EffectListenerOrientation:
            return constructEffectDescription(QCoreApplication::translate("Phonon::MMF::EffectFactory", "Environmental Reverb"), "Environmental Reverb.");
        case AbstractAudioEffect::EffectLoudness:
            return constructEffectDescription(QCoreApplication::translate("Phonon::MMF::EffectFactory", "Loudness"), "Loudness.");
        case AbstractAudioEffect::EffectSourceOrientation:
            return constructEffectDescription(QCoreApplication::translate("Phonon::MMF::EffectFactory", "Source Orientation"), "Source Orientation.");
        case AbstractAudioEffect::EffectStereoWidening:
            return constructEffectDescription(QCoreApplication::translate("Phonon::MMF::EffectFactory", "Stereo Widening"), "Stereo Widening.");
    }

    Q_ASSERT_X(false, Q_FUNC_INFO, "Unknown effect type.");
    return QHash<QByteArray, QVariant>();
}

AbstractAudioEffect *EffectFactory::createAudioEffect(AbstractAudioEffect::Type type,
                                                      QObject *parent)
{
    Q_ASSERT(parent);

    switch (type)
    {
        case AbstractAudioEffect::EffectBassBoost:
            return new BassBoost(parent);
        case AbstractAudioEffect::EffectAudioEqualizer:
            return new AudioEqualizer(parent);
        case AbstractAudioEffect::EffectDistanceAttenuation:
        case AbstractAudioEffect::EffectEnvironmentalReverb:
        case AbstractAudioEffect::EffectListenerOrientation:
        case AbstractAudioEffect::EffectLoudness:
        case AbstractAudioEffect::EffectSourceOrientation:
        case AbstractAudioEffect::EffectStereoWidening:
            ;
    }

    Q_ASSERT_X(false, Q_FUNC_INFO, "Unknown effect.");
    return 0;
}

template<typename TEffect>
bool isEffectSupported()
{
    AudioPlayer audioPlayer;

    QScopedPointer<TEffect> eff;
    TRAPD(errorCode, eff.reset(TEffect::NewL(*audioPlayer.player())));

    return errorCode != KErrNone;
}

QList<int> EffectFactory::effectIndexes()
{
    QList<int> retval;

    if (isEffectSupported<CAudioEqualizer>())
        retval.append(AbstractAudioEffect::EffectAudioEqualizer);

    if (isEffectSupported<CBassBoost>())
        retval.append(AbstractAudioEffect::EffectBassBoost);

    /* We haven't implemented these yet.
    if (isEffectSupported<CDistanceAttenuation>())
        retval.append(AbstractAudioEffect::EffectDistanceAttenuation);

    if (isEffectSupported<CEnvironmentalReverb>())
        retval.append(AbstractAudioEffect::EffectEnvironmentalReverb);

    if (isEffectSupported<CLoudness>())
        retval.append(AbstractAudioEffect::EffectLoudness);

    if (isEffectSupported<CListenerOrientation>())
        retval.append(AbstractAudioEffect::EffectListenerOrientation);

    if (isEffectSupported<CSourceOrientation>())
        retval.append(AbstractAudioEffect::EffectSourceOrientation);

    if (isEffectSupported<CStereoWidening>())
        retval.append(AbstractAudioEffect::EffectStereoWidening);
    */

    return retval;
}

QT_END_NAMESPACE

