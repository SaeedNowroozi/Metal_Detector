#include "bluetootherror.h"

Bluetootherror::Bluetootherror(QObject *parent)
    : QObject{parent}
{

}

QString Bluetootherror::error() const
{
return m_error;
}

void Bluetootherror::setError(const QString &error)
{
    if (m_error != error) {
        m_error = error;
        emit errorChanged();
    }
}

QString Bluetootherror::info() const
{
return m_info;
}

void Bluetootherror::setInfo(const QString &info)
{
    if (m_info != info) {
        m_info = info;
        emit infoChanged();
    }
}

void Bluetootherror::clearMessages()
{
    setInfo("");
    setError("");
}
