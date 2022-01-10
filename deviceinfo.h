#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QObject>
#include <QBluetoothDeviceInfo>
#include <QString>

class DeviceInfo : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(QString deviceName READ getName NOTIFY deviceChanged)
//    Q_PROPERTY(QString deviceAddress READ getAddress NOTIFY deviceChanged)

public:
    DeviceInfo(const QBluetoothDeviceInfo &info);

    void setDevice(const QBluetoothDeviceInfo &device);
    QString getName()const;
    QString getAddress() const;
    QBluetoothDeviceInfo getDevice() const;


signals:
    void deviceChanged();
private:
    QBluetoothDeviceInfo m_device;

};

#endif // DEVICEINFO_H
