#ifndef BLUETOOTHERROR_H
#define BLUETOOTHERROR_H

#include <QObject>

class Bluetootherror : public QObject
{
    Q_OBJECT
public:
    explicit Bluetootherror(QObject *parent = nullptr);


    QString error() const;
    void setError(const QString& error);

    QString info() const;
    void setInfo(const QString& info);

    void clearMessages();

signals:
    void errorChanged();
    void infoChanged();

private:
    QString m_error;
    QString m_info;

};

#endif // BLUETOOTHERROR_H
