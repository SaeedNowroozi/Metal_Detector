#ifndef DEVICEFIND_H
#define DEVICEFIND_H

#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>

#include "bluetootherror.h"

class Deviceinfo;
class DeviceHandle;

class Devicefind : public Bluetootherror
{
    Q_OBJECT
public:
    Devicefind(DeviceHandle *handler, QObject *parent = nullptr);
    ~Devicefind();

    bool scanning() const;

    QVariant devices();

public slots:
    void startSearch();
    void connectToService(const QString &address);

private slots:
    void addDevice(const QBluetoothDeviceInfo&device);
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void scanFinished();

signals:
    void scanningChanged();
    void devicesChanged();

private:
    DeviceHandle *m_deviceHandler;
    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;
    QList<QObject*> m_devices;

#ifdef SIMULATOR
    QTimer m_demoTimer;
#endif

};

#endif // DEVICEFIND_H
