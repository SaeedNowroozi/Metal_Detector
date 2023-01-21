#ifndef JSON_H
#define JSON_H

#include <QObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>

class Json : public QObject
{
    Q_OBJECT
public:
    explicit Json(QObject *parent = nullptr);
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

signals:

};

#endif // JSON_H
