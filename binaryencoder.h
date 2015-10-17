#ifndef BINARYENCODER_H
#define BINARYENCODER_H

#include <QVector>
#include <QPair>

class BinaryEncoder
{
public:
    BinaryEncoder(QString valueToEncode)
        : valueToEncode(valueToEncode) {}

    static QVector<QPair<double, double>> generateClock();
private:
    QString valueToEncode;
};

#endif // BINARYENCODER_H
