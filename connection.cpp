#include "connection.h"
#include <QtBluetooth/qtbluetooth-config.h>
#include <QtCore/qsystemdetection.h>

Connection::Connection(QObject *parent)
    : QObject{parent}
{
    connect(&m_localDevice, &QBluetoothLocalDevice::hostModeStateChanged,
            this, &Connection::hostModeChanged);
}

bool Connection::alive() const
{
#if defined(SIMULATOR) || defined(QT_PLATFORM_UIKIT)
    return true;
#else
    return m_localDevice.isValid() && m_localDevice.hostMode() != QBluetoothLocalDevice::HostPoweredOff;
#endif
}

bool Connection::requiresAddressType() const
{
#if QT_CONFIG(bluez)
    return true;
#else
    return false;
#endif
}

QString Connection::name() const
{
    return m_localDevice.name();
}

QString Connection::address() const
{
    return m_localDevice.address().toString();
}

void Connection::hostModeChanged(QBluetoothLocalDevice::HostMode mode)
{
    emit deviceChanged();
}
