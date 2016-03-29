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

#include <QStringList>
#include <QtPlugin>

#include <apgcli.h> // for RApaLsSession
#include <apmrec.h> // for CDataTypeArray
#include <apmstd.h> // for TDataType

#include "abstractaudioeffect.h"
#include "ancestormovemonitor.h"
#include "audiooutput.h"
#include "audioplayer.h"
#include "backend.h"
#include "effectfactory.h"
#include "mediaobject.h"
#include "utils.h"
#include "videowidget.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::VolumeObserver
  \internal
*/

/*! \class MMF::Backend
  \internal
*/

Backend::Backend(QObject *parent)
    : QObject(parent)
    , m_ancestorMoveMonitor(new AncestorMoveMonitor(this))
{
    TRACE_CONTEXT(Backend::Backend, EBackend);
    TRACE_ENTRY_0();

    setProperty("identifier",     QLatin1String("phonon_mmf"));
    setProperty("backendName",    QLatin1String("MMF"));
    setProperty("backendComment", QLatin1String("Backend using Symbian Multimedia Framework (MMF)"));
    setProperty("backendVersion", QLatin1String("0.1"));
    setProperty("backendWebsite", QLatin1String("http://qt.nokia.com/"));

    TRACE_EXIT_0();
}

QObject *Backend::createObject(BackendInterface::Class c, QObject *parent, const QList<QVariant> &args)
{
    TRACE_CONTEXT(Backend::createObject, EBackend);
    TRACE_ENTRY("class %d", c);

    QObject* result = 0;

    switch (c) {
    case AudioOutputClass:
        result = new AudioOutput(this, parent);
        break;

    case MediaObjectClass:
        result = new MediaObject(parent);
        break;

    case VolumeFaderEffectClass:
    case VisualizationClass:
    case VideoDataOutputClass:
    case EffectClass:
    {
        Q_ASSERT(args.count() == 1);
        Q_ASSERT(args.first().type() == QVariant::Int);
        const AbstractAudioEffect::Type effect = AbstractAudioEffect::Type(args.first().toInt());

        return EffectFactory::createAudioEffect(effect, parent);
    }
    case VideoWidgetClass:
        result = new VideoWidget(m_ancestorMoveMonitor.data(), qobject_cast<QWidget *>(parent));
        break;

    default:
        TRACE_PANIC(InvalidBackendInterfaceClass);
    }

    TRACE_RETURN("0x%08x", result);
}

QList<int> Backend::objectDescriptionIndexes(ObjectDescriptionType type) const
{
    TRACE_CONTEXT(Backend::objectDescriptionIndexes, EAudioApi);
    TRACE_ENTRY_0();
    QList<int> retval;

    switch(type)
    {
        case EffectType:
            retval.append(EffectFactory::effectIndexes());
            break;
        case AudioOutputDeviceType:
            // We only have one possible output device, but we need at least
            // one.
            retval.append(AudioOutput::AudioOutputDeviceID);
            break;
        default:
            ;
    }

    TRACE_EXIT_0();
    return retval;
}

QHash<QByteArray, QVariant> Backend::objectDescriptionProperties(ObjectDescriptionType type, int index) const
{
    TRACE_CONTEXT(Backend::connectNodes, EBackend);

    switch (type) {
        case EffectType:
            return EffectFactory::audioEffectDescriptions(AbstractAudioEffect::Type(index));
        case AudioOutputDeviceType:
            return AudioOutput::audioOutputDescription(index);
        default:
            return QHash<QByteArray, QVariant>();
    }
}

bool Backend::startConnectionChange(QSet<QObject *>)
{
    return true;
}

bool Backend::connectNodes(QObject *source, QObject *target)
{
    TRACE_CONTEXT(Backend::connectNodes, EBackend);
    TRACE_ENTRY("source 0x%08x target 0x%08x", source, target);
    Q_ASSERT(qobject_cast<MediaNode *>(source));
    Q_ASSERT(qobject_cast<MediaNode *>(target));

    MediaNode *const mediaSource = static_cast<MediaNode *>(source);
    MediaNode *const mediaTarget = static_cast<MediaNode *>(target);

    return mediaSource->connectMediaNode(mediaTarget);
}

bool Backend::disconnectNodes(QObject *source, QObject *target)
{
    TRACE_CONTEXT(Backend::disconnectNodes, EBackend);
    TRACE_ENTRY("source 0x%08x target 0x%08x", source, target);
    Q_ASSERT(qobject_cast<MediaNode *>(source));
    Q_ASSERT(qobject_cast<MediaNode *>(target));

    const bool result = static_cast<MediaNode *>(source)->disconnectMediaNode(static_cast<MediaNode *>(target));

    TRACE_RETURN("%d", result);
}

bool Backend::endConnectionChange(QSet<QObject *>)
{
    return true;
}

void getAvailableMimeTypesL(QStringList& result)
{
    RApaLsSession apaSession;
    User::LeaveIfError(apaSession.Connect());
    CleanupClosePushL(apaSession);

    static const TInt DataTypeArrayGranularity = 8;
    CDataTypeArray* array = new(ELeave) CDataTypeArray(DataTypeArrayGranularity);
    CleanupStack::PushL(array);

    apaSession.GetSupportedDataTypesL(*array);

    for (TInt i = 0; i < array->Count(); ++i) {
        const TPtrC mimeType = array->At(i).Des();
        const MediaType mediaType = Utils::mimeTypeToMediaType(mimeType);
        if (MediaTypeAudio == mediaType or MediaTypeVideo == mediaType) {
            result.append(qt_TDesC2QString(mimeType));
        }
    }

    CleanupStack::PopAndDestroy(2); // apaSession, array
}

QStringList Backend::availableMimeTypes() const
{
    QStringList result;

    // There is no way to return an error from this function, so we just
    // have to trap and ignore exceptions...
    TRAP_IGNORE(getAvailableMimeTypesL(result));

    result.sort();

    return result;
}

Q_EXPORT_PLUGIN2(phonon_mmf, Phonon::MMF::Backend);

QT_END_NAMESPACE

