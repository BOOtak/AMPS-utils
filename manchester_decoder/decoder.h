#ifndef DECODER_H
#define DECODER_H

#include <QByteArray>
#include <QVector>

QVector<int> amplitudesToSquares(QByteArray in);
QVector<int> squaresToRawBits(QVector<int> in);
QVector<int> rawBitsToLogicalBits(QVector<int> in, int *errors);

#endif // DECODER_H
