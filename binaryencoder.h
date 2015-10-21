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

#ifndef BINARYENCODER_H
#define BINARYENCODER_H

#include <QVector>
#include <utility>

namespace chrishenx {

  class BinaryEncoder
  {
  public:
    using Data = QVector<std::pair<double, double>>;

      enum class Method {
          TTL, NRZL, NRZI, BIPOLAR, PSEUDOTERNARY, MANCHESTER, DMANCHESTER
      };

    static const double DEFAULT_TRANS_SPEED; // In seconds
    static const double DEFAULT_AMPLITUDE; // In volts

    BinaryEncoder(QString valueToEncode)
      : BinaryEncoder(valueToEncode, DEFAULT_TRANS_SPEED) {}

    BinaryEncoder(QString valueToEncode, double transSpeed)
      : BinaryEncoder(valueToEncode, transSpeed, DEFAULT_AMPLITUDE) {}

    BinaryEncoder(QString valueToEncode, double transSpeed, double amplitude)
        : mValueToEncode(valueToEncode), mTransSpeed(transSpeed),mAmplitude(amplitude) {
      mN = valueToEncode.length();
    }

    Data generateClock();
    Data generateTTL();
    Data generateNRZL();
    Data generateNRZI();
    Data generateBipolar();
    Data generatePseudoternary();
    Data generateManchester();
    Data generateDManchester();
    Data generateMultilevel(int levels);

    double timeMax() const { return mTimeMax; }

  private:
    QString mValueToEncode;
    double mTransSpeed; // Transmission speed
    double mAmplitude; // Represent volts
    double mTimeMax = 0;
    int mN;
  };

} // chrishenx namespace end


#endif // BINARYENCODER_H
