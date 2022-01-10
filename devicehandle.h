#ifndef DEVICEHANDLE_H
#define DEVICEHANDLE_H

#include "bluetootherror.h"

#include <QObject>

#include <QDateTime>
#include <QList>
#include <QTimer>

#include <QLowEnergyController>
#include <QLowEnergyService>

class DeviceInfo;

class DeviceHandle : public Bluetootherror
{
    Q_OBJECT
public:
    enum class AddressType {
        PublicAddress,
        RandomAddress
    };
    Q_ENUM(AddressType)

    DeviceHandle(QObject *parent = nullptr);

    void setDevice(DeviceInfo *device);
    void setAddressType(AddressType type);
    AddressType addressType() const;

    bool measuring() const;
    bool alive() const;

    // Statistics
    int hr() const;
    int time() const;
    float average() const;
    int maxHR() const;
    int minHR() const;
    float calories() const;

signals:
    void measuringChanged();
    void aliveChanged();
    void statsChanged();

public slots:
    void startMeasurement();
    void stopMeasurement();
    void disconnectService();

private:
    //QLowEnergyController
    void serviceDiscovered(const QBluetoothUuid &gatt);
    void serviceScanDone();

    //QLowEnergyService
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void updateHeartRateValue(const QLowEnergyCharacteristic &c,
                              const QByteArray &value);
    void confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                              const QByteArray &value);

#ifdef SIMULATOR
    void updateDemoHR();
#endif
private:
    void addMeasurement(int value);

    QLowEnergyController *m_control = nullptr;
    QLowEnergyService *m_service = nullptr;
    QLowEnergyDescriptor m_notificationDesc;
    DeviceInfo *m_currentDevice = nullptr;

    bool m_foundHeartRateService;
    bool m_measuring;
    int m_currentValue, m_min, m_max, m_sum;
    float m_avg, m_calories;

    // Statistics
    QDateTime m_start;
    QDateTime m_stop;

    QList<int> m_measurements;
    QLowEnergyController::RemoteAddressType m_addressType = QLowEnergyController::PublicAddress;

#ifdef SIMULATOR
    QTimer m_demoTimer;
#endif
};

#endif // DEVICEHANDLE_H
