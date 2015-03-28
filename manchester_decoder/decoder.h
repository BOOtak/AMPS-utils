#ifndef DECODER_H
#define DECODER_H

#include <QByteArray>
#include <QChar>
#include <QVector>

QVector<int> amplitudesToSquares(QByteArray in, int *offset);
QVector<int> squaresToRawBits(QVector<int> in);
QVector<char> rawBitsToLogicalBits(QVector<int> in, bool inversed, int *errors, int *oddBit);

#endif // DECODER_H
