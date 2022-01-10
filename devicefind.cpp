#include "devicefind.h"
#include "devicefind.h"
#include "devicehandle.h"


Devicefind::Devicefind(DeviceHandle *handler, QObject *parent):
    Bluetootherror(parent),
    m_deviceHandler(handler)
{
    //! [devicediscovery-1]
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &Devicefind::addDevice);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred, this,
            &Devicefind::scanError);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &Devicefind::scanFinished);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &Devicefind::scanFinished);
    //! [devicediscovery-1]


#ifdef SIMULATOR
    m_demoTimer.setSingleShot(true);
    m_demoTimer.setInterval(2000);
    connect(&m_demoTimer, &QTimer::timeout, this, &DeviceFinder::scanFinished);
#endif
}

bool Devicefind::scanning() const
{
#ifdef SIMULATOR
    return m_demoTimer.isActive();
#else
    return m_deviceDiscoveryAgent->isActive();
#endif
}

QVariant Devicefind::devices()
{
    return QVariant::fromValue(m_devices);
}

void Devicefind::startSearch()
{
    clearMessages();
    m_deviceHandler->setDevice(nullptr);
    qDeleteAll(m_devices);
    m_devices.clear();

    emit devicesChanged();

#ifdef SIMULATOR
    m_demoTimer.start();
#else
    //! [devicediscovery-2]
    m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    //! [devicediscovery-2]
#endif
    emit scanningChanged();
    setInfo(tr("Scanning for devices..."));
}

void Devicefind::connectToService(const QString &address)
{
    m_deviceDiscoveryAgent->stop();

    DeviceInfo *currentDevice = nullptr;
    for (QObject *entry : qAsConst(m_devices)) {
        auto device = qobject_cast<DeviceInfo *>(entry);
        if (device && device->getAddress() == address ) {
            currentDevice = device;
            break;
        }
    }

    if (currentDevice)
        m_deviceHandler->setDevice(currentDevice);

    clearMessages();
}

void Devicefind::addDevice(const QBluetoothDeviceInfo &device)
{
    // If device is LowEnergy-device, add it to the list
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        m_devices.append(new DeviceInfo(device));
        setInfo(tr("Low Energy device found. Scanning more..."));
//! [devicediscovery-3]
        emit devicesChanged();
//! [devicediscovery-4]
    }
    //...
}

void Devicefind::scanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        setError(tr("The Bluetooth adaptor is powered off."));
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        setError(tr("Writing or reading from the device resulted in an error."));
    else
        setError(tr("An unknown error has occurred."));
}

void Devicefind::scanFinished()
{
#ifdef SIMULATOR
    // Only for testing
    for (int i = 0; i < 4; i++)
        m_devices.append(new DeviceInfo(QBluetoothDeviceInfo()));
#endif

    if (m_devices.isEmpty())
        setError(tr("No Low Energy devices found."));
    else
        setInfo(tr("Scanning done."));

    emit scanningChanged();
    emit devicesChanged();
}

Devicefind::~Devicefind()
{
    qDeleteAll(m_devices);
    m_devices.clear();
}
