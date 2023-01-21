#ifndef DETECT_H
#define DETECT_H

#include <QObject>

class Detect : public QObject
{
    Q_OBJECT
public:
    explicit Detect(QObject *parent = nullptr);

signals:

};

#endif // DETECT_H
