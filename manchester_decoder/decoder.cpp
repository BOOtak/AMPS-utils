#include "decoder.h"
#include <cmath>
#include <iostream>

QVector<int> amplitudesToSquares(QByteArray in) {
    QVector<int> result = QVector<int>();
    result.append(in.at(0));

    for (int i = 1; i < in.size(); i++) {
        if ((in.at(i) >= 0 && in.at(i-1) < 0)
                || (in.at(i) < 0 && in.at(i-1) >= 0)) {
            result.append(in.at(i));
        } else {
            result[result.size() - 1] += in.at(i);
        }
    }

    return result;
}

QVector<int> squaresToRawBits(QVector<int> in) {
    int squares = 0;
    for (int i = 1; i < in.size(); i++) {
        squares += abs(in.at(i));
    }
    float avgSquare = (float)squares / (float)(in.size());
    QVector<int> result = QVector<int>();

    int error_count = 0;
    for (int i = 0; i < in.size(); i++) {
        int currentBit = in.at(i) > 0 ? 1 : 0;

        // append current bit to result vector
        result.append(currentBit);

        // if length of current signal state is too long, add second bit
        if ((abs(in.at(i)) - avgSquare) > 0) {
            result.append(currentBit);
        }
    }
    return result;
}

QVector<int> rawBitsToLogicalBits(QVector<int> in, int* errors) {
    QVector<int> result = QVector<int>();

    int i;
    int error_count = 0;
    for (i = 0; i < in.size() - 1; i += 2) {
        if (in.at(i) == 1 && in.at(i+1) == 0) {
            result.append(0);
        } else if (in.at(i) == 0 && in.at(i+1) == 1) {
            result.append(1);
        } else {
            i += -1;
            error_count ++;
        }
    }
    *errors = error_count;
    return result;
}
