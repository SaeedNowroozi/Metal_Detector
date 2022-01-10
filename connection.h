#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QBluetoothLocalDevice>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);

    bool alive() const;
    bool requiresAddressType() const;
    QString name() const;
    QString address() const;

signals:
    void deviceChanged();

private slots:
    void hostModeChanged(QBluetoothLocalDevice::HostMode mode);

private:
    QBluetoothLocalDevice m_localDevice;
};

#endif // CONNECTION_H
