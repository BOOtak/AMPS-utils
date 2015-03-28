#include "decoder.h"
#include <cmath>
#include <iostream>

QVector<int> amplitudesToSquares(QByteArray in, int* extraDataOffset) {
    QVector<int> result = QVector<int>();
    int lastSquareIndex = 0;
    result.append(in.at(0));

    for (int i = 1; i < in.size(); i++) {
        if ((in.at(i) >= 0 && in.at(i-1) < 0)
                || (in.at(i) < 0 && in.at(i-1) >= 0)) {
            result.append(in.at(i));
            lastSquareIndex = i;
        } else {
            result[result.size() - 1] += in.at(i);
        }
    }

    // There is some chance that we broke signal apart between 2 blocks of
    // data. So let's remove last piece of data and read it on next
    // iteration as part of next block.
    *extraDataOffset = in.size() - lastSquareIndex;
    result.removeLast();

    return result;
}

QVector<int> squaresToRawBits(QVector<int> in) {
    int squares = 0;
    for (int i = 1; i < in.size(); i++) {
        squares += abs(in.at(i));
    }
    float avgSquare = (float)squares / (float)(in.size());
    QVector<int> result = QVector<int>();

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

QVector<char> rawBitsToLogicalBits(QVector<int> in, bool inversed, int* errors, int *oddBit) {
    QVector<char> result = QVector<char>();

    int i;
    int error_count = 0;
    for (i = 0; i < in.size() - 1; i += 2) {
        if (in.at(i) == 1 && in.at(i+1) == 0) {
            result.append(inversed ? '1' : '0');
        } else if (in.at(i) == 0 && in.at(i+1) == 1) {
            result.append(inversed ? '0' : '1');
        } else {
            // Error correction.
            //
            // We found invalid raw bits combination which cannot be
            // interpreted as vaid manchester code. We assume that this error
            // caused by dublication of raw bit so we remove dublicated bit
            // and continue decoding rest of manchester code vector.
            // E.g we found that combination:
            //
            // 01, 10, 10, 10, 11, 00, 10, 10
            //                 ^^
            //                 ||
            //
            // OK, it's weird and can't be decoded from manchester code.
            // So we make one step backward, and two steps forward to ignore
            // this error and parse rest of code correctly:
            //
            // 01, 10, 10, 10, 1, 10, 01, 01, 0
            //                    ^^
            //                    ||
            //
            // Look at last 0 we have after error correction. I'll tell you
            // about that in the comment below.
            i += -1;
            error_count ++;
        }
    }

    // If we have uneven numbers of errors, it means that we remove uneven
    // number of dublicated bits during error correction routine. So final
    // length of raw bits vector is odd too and we didn't parsed last bit. So,
    // we should return last bit to process it on next iteration.

    if (error_count % 2 != in.size() % 2) {
        *oddBit = in.last();
    }

    *errors = error_count;
    return result;
}
