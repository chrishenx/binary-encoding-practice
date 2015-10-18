/*
    The MIT License (MIT)

    Copyright (c) 2015 Christian González León

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
  */

#include "binaryencoder.h"

#include <QDebug>

using namespace std;
using namespace chrishenx;

const double BinaryEncoder::DEFAULT_TRANS_SPEED = 1;
const double BinaryEncoder::DEFAULT_AMPLITUDE = 5;

BinaryEncoder::Data BinaryEncoder::generateClock()
{
    const int POINT_COUNT = mN * 4; // Four points for each bit
    Data clock(POINT_COUNT);
    const double f = mTransSpeed * 2; // Clock frecuency
    const double T2 = 1.0 / f; // Clock period divided
    double t = 0; // current time
    for (int i = 0; i < POINT_COUNT; i += 4) {
        clock[i] = make_pair(t, 0.0);
        clock[i + 1] = make_pair(t += T2, 0.0);
        clock[i + 2] = make_pair(t, mAmplitud);
        clock[i + 3] = make_pair(t += T2, mAmplitud);
    }
    mTimeMax = t;
    return clock;
}
