#include "devicehandle.h"

#include "deviceinfo.h"
#include <QtEndian>
#include <QRandomGenerator>

DeviceHandle::DeviceHandle(QObject *parent):
Bluetootherror(parent),
m_foundHeartRateService(false),
m_measuring(false),
m_currentValue(0),
m_min(0), m_max(0), m_sum(0), m_avg(0), m_calories(0)
{
#ifdef SIMULATOR
    m_demoTimer.setSingleShot(false);
    m_demoTimer.setInterval(2000);
    connect(&m_demoTimer, &QTimer::timeout, this, &DeviceHandler::updateDemoHR);
    m_demoTimer.start();
    updateDemoHR();
#endif
}

void DeviceHandle::setDevice(DeviceInfo *device)
{
    clearMessages();
    m_currentDevice = device;

#ifdef SIMULATOR
    setInfo(tr("Demo device connected."));
    return;
#endif

    // Disconnect and delete old connection
    if (m_control) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = nullptr;
    }

    // Create new controller and connect it if device available
    if (m_currentDevice) {

        // Make connections
        //! [Connect-Signals-1]
        m_control = QLowEnergyController::createCentral(m_currentDevice->getDevice(), this);
        //! [Connect-Signals-1]
        m_control->setRemoteAddressType(m_addressType);
        //! [Connect-Signals-2]
        connect(m_control, &QLowEnergyController::serviceDiscovered,
                this, &DeviceHandle::serviceDiscovered);
        connect(m_control, &QLowEnergyController::discoveryFinished,
                this, &DeviceHandle::serviceScanDone);

        connect(m_control, &QLowEnergyController::errorOccurred, this,
                [this](QLowEnergyController::Error error) {
                    Q_UNUSED(error);
                    setError("Cannot connect to remote device.");
                });
        connect(m_control, &QLowEnergyController::connected, this, [this]() {
            setInfo("Controller connected. Search services...");
            m_control->discoverServices();
        });
        connect(m_control, &QLowEnergyController::disconnected, this, [this]() {
            setError("LowEnergy controller disconnected");
        });

        // Connect
        m_control->connectToDevice();
        //! [Connect-Signals-2]
    }
}

void DeviceHandle::setAddressType(AddressType type)
{
    switch (type) {
    case DeviceHandle::AddressType::PublicAddress:
        m_addressType = QLowEnergyController::PublicAddress;
        break;
    case DeviceHandle::AddressType::RandomAddress:
        m_addressType = QLowEnergyController::RandomAddress;
        break;
    }
}


bool DeviceHandle::measuring() const
{
return m_measuring;
}

bool DeviceHandle::alive() const
{
#ifdef SIMULATOR
    return true;
#endif

    if (m_service)
        return m_service->state() == QLowEnergyService::RemoteServiceDiscovered;

    return false;
}

int DeviceHandle::hr() const
{
 return m_currentValue;
}

int DeviceHandle::time() const
{
return m_start.secsTo(m_stop);
}

float DeviceHandle::average() const
{

}

int DeviceHandle::maxHR() const
{
 return m_max;
}

int DeviceHandle::minHR() const
{
return m_min;
}

float DeviceHandle::calories() const
{
return m_calories;
}

void DeviceHandle::startMeasurement()
{
    if (alive()) {
        m_start = QDateTime::currentDateTime();
        m_min = 0;
        m_max = 0;
        m_avg = 0;
        m_sum = 0;
        m_calories = 0;
        m_measuring = true;
        m_measurements.clear();
        emit measuringChanged();
    }
}

void DeviceHandle::stopMeasurement()
{
    m_measuring = false;
    emit measuringChanged();
}

void DeviceHandle::disconnectService()
{
    m_foundHeartRateService = false;

    //disable notifications
    if (m_notificationDesc.isValid() && m_service
            && m_notificationDesc.value() == QByteArray::fromHex("0100")) {
        m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0000"));
    } else {
        if (m_control)
            m_control->disconnectFromDevice();

        delete m_service;
        m_service = nullptr;
    }
}

void DeviceHandle::serviceDiscovered(const QBluetoothUuid &gatt)
{
    if (gatt == QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::HeartRate)) {
        setInfo("Heart Rate service discovered. Waiting for service scan to be done...");
        m_foundHeartRateService = true;
    }
}

void DeviceHandle::serviceScanDone()
{
    setInfo("Service scan done.");

    // Delete old service if available
    if (m_service) {
        delete m_service;
        m_service = nullptr;
    }

//! [Filter HeartRate service 2]
    // If heartRateService found, create new service
    if (m_foundHeartRateService)
        m_service = m_control->createServiceObject(QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::HeartRate), this);

    if (m_service) {
        connect(m_service, &QLowEnergyService::stateChanged, this, &DeviceHandle::serviceStateChanged);
        connect(m_service, &QLowEnergyService::characteristicChanged, this, &DeviceHandle::updateHeartRateValue);
        connect(m_service, &QLowEnergyService::descriptorWritten, this, &DeviceHandle::confirmedDescriptorWrite);
        m_service->discoverDetails();
    } else {
        setError("Heart Rate Service not found.");
    }
}

void DeviceHandle::serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::RemoteServiceDiscovering:
        setInfo(tr("Discovering services..."));
        break;
    case QLowEnergyService::RemoteServiceDiscovered:
    {
        setInfo(tr("Service discovered."));

        const QLowEnergyCharacteristic hrChar = m_service->characteristic(QBluetoothUuid(QBluetoothUuid::CharacteristicType::HeartRateMeasurement));
        if (!hrChar.isValid()) {
            setError("HR Data not found.");
            break;
        }

        m_notificationDesc = hrChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (m_notificationDesc.isValid())
            m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));

        break;
    }
    default:
        //nothing for now
        break;
    }

    emit aliveChanged();
}

void DeviceHandle::updateHeartRateValue(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() != QBluetoothUuid(QBluetoothUuid::CharacteristicType::HeartRateMeasurement))
        return;

    auto data = reinterpret_cast<const quint8 *>(value.constData());
    quint8 flags = *data;

    //Heart Rate
    int hrvalue = 0;
    if (flags & 0x1) // HR 16 bit? otherwise 8 bit
        hrvalue = static_cast<int>(qFromLittleEndian<quint16>(data[1]));
    else
        hrvalue = static_cast<int>(data[1]);

    addMeasurement(hrvalue);
}

void DeviceHandle::confirmedDescriptorWrite(const QLowEnergyDescriptor &d, const QByteArray &value)
{
    if (d.isValid() && d == m_notificationDesc && value == QByteArray::fromHex("0000")) {
        //disabled notifications -> assume disconnect intent
        m_control->disconnectFromDevice();
        delete m_service;
        m_service = nullptr;
    }
}

void DeviceHandle::addMeasurement(int value)
{
    m_currentValue = value;

    // If measuring and value is appropriate
    if (m_measuring && value > 30 && value < 250) {

        m_stop = QDateTime::currentDateTime();
        m_measurements << value;

        m_min = m_min == 0 ? value : qMin(value, m_min);
        m_max = qMax(value, m_max);
        m_sum += value;
        m_avg = (double)m_sum / m_measurements.size();
        m_calories = ((-55.0969 + (0.6309 * m_avg) + (0.1988 * 94) + (0.2017 * 24)) / 4.184) * 60 * time()/3600;
    }

    emit statsChanged();
}
