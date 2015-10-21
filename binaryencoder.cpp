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
  static const int POINTS_PER_BIT = 4;
  const int POINT_COUNT = mN * POINTS_PER_BIT;
  Data clock(POINT_COUNT);
  const double f = mTransSpeed * 2; // Clock frecuency
  const double T = 1.0 / f; // Clock period
  double t = 0.0; // current time
  for (int i = 0; i < POINT_COUNT; i += POINTS_PER_BIT)
  {
    clock[i] = make_pair(t, 0.0);
    clock[i + 1] = make_pair(t += T, 0.0);
    clock[i + 2] = make_pair(t, mAmplitude);
    clock[i + 3] = make_pair(t += T, mAmplitude);
  }
  mTimeMax = t;
  return clock;
}

BinaryEncoder::Data BinaryEncoder::generateTTL()
{
  static const int POINTS_PER_BIT = 2;
  const int POINT_COUNT = mN * POINTS_PER_BIT;
  Data ttl(POINT_COUNT);
  const double f = mTransSpeed;
  const double T = 1.0 / f;
  double t = 0.0;
  for (int i = 0; i < POINT_COUNT; i += POINTS_PER_BIT)
  {
    const double amplitude = mValueToEncode[i / POINTS_PER_BIT].digitValue() * mAmplitude;
    ttl[i] = make_pair(t, amplitude);
    ttl[i + 1] = make_pair(t += T, amplitude);
  }
  mTimeMax = t;
  return ttl;
}

BinaryEncoder::Data BinaryEncoder::generateNRZL()
{
  static const int POINTS_PER_BIT = 2;
  const int POINT_COUNT = mN * POINTS_PER_BIT;
  Data nrzl(POINT_COUNT);
  const double f = mTransSpeed;
  const double T = 1.0 / f;
  double t = 0.0;
  for (int i = 0; i < POINT_COUNT; i += POINTS_PER_BIT) {
    const double amplitude = mValueToEncode[i / POINTS_PER_BIT].digitValue() ? 0 : mAmplitude;
    nrzl[i] = make_pair(t, amplitude);
    nrzl[i + 1] = make_pair(t += T, amplitude);
  }
  mTimeMax = t;
  return nrzl;
}

BinaryEncoder::Data BinaryEncoder::generateNRZI()
{
  static const int POINTS_PER_BIT = 2;
  const int POINT_COUNT = mN * POINTS_PER_BIT;
  Data nrzi(POINT_COUNT);
  const double f = mTransSpeed;
  const double T = 1.0 / f;
  double t = 0.0;
  double amplitude = 0;
  for (int i = 0; i < POINT_COUNT; i += POINTS_PER_BIT)
  {
    if (mValueToEncode[i / POINTS_PER_BIT].digitValue()) // Transition?
    {
      amplitude = amplitude == mAmplitude ? 0 : mAmplitude;
    }
    nrzi[i] = make_pair(t, amplitude);
    nrzi[i + 1] = make_pair(t += T, amplitude);
  }
  mTimeMax = t;
  return nrzi;
}

BinaryEncoder::Data BinaryEncoder::generateBipolar()
{
  static const int POINTS_PER_BIT = 2;
  const int POINT_COUNT = mN * POINTS_PER_BIT;
  Data bipolar(POINT_COUNT);
  const double f = mTransSpeed;
  const double T = 1.0 / f;
  double t = 0.0;
  double multiplier = 1;
  for (int i = 0; i < POINT_COUNT; i += POINTS_PER_BIT)
  {
    const int bit = mValueToEncode[i / POINTS_PER_BIT].digitValue();
    const double amplitude = bit ? multiplier * mAmplitude : 0;
    bipolar[i] = make_pair(t, amplitude);
    bipolar[i + 1] = make_pair(t += T, amplitude);
    if (bit)
    {
      multiplier *= -1;
    }
  }
  mTimeMax = t;
  return bipolar;
}

BinaryEncoder::Data BinaryEncoder::generatePseudoternary()
{
  static const int POINTS_PER_BIT = 2;
  const int POINT_COUNT = mN * POINTS_PER_BIT;
  Data pseudoternary(POINT_COUNT);
  const double f = mTransSpeed;
  const double T = 1.0 / f;
  double t = 0.0;
  double multiplier = 1;
  for (int i = 0; i < POINT_COUNT; i += POINTS_PER_BIT)
  {
    const int bit = mValueToEncode[i / POINTS_PER_BIT].digitValue();
    const double amplitude = !bit ? multiplier * mAmplitude : 0;
    pseudoternary[i] = make_pair(t, amplitude);
    pseudoternary[i + 1] = make_pair(t += T, amplitude);
    if (!bit)
    {
      multiplier *= -1;
    }
  }
  mTimeMax = t;
  return pseudoternary;
}

BinaryEncoder::Data BinaryEncoder::generateManchester()
{
  static const int POINTS_PER_BIT = 4;
  const int POINT_COUNT = mN * POINTS_PER_BIT;
  Data manchester(POINT_COUNT);
  const double f = mTransSpeed * 2; // Clock frecuency
  const double T = 1.0 / f; // Clock period
  double t = 0.0; // current time
  for (int i = 0; i < POINT_COUNT; i += POINTS_PER_BIT)
  {
    const int bit = mValueToEncode[i / POINTS_PER_BIT].digitValue();
    const double amp1 = bit ? -mAmplitude : mAmplitude;
    const double amp2 = -amp1;
    manchester[i] = make_pair(t, amp1);
    manchester[i + 1] = make_pair(t += T, amp1);
    manchester[i + 2] = make_pair(t, amp2);
    manchester[i + 3] = make_pair(t += T, amp2);
  }
  mTimeMax = t;
  return manchester;
}

BinaryEncoder::Data BinaryEncoder::generateDManchester()
{
  static const int POINTS_PER_BIT = 4;
  const int POINT_COUNT = mN * POINTS_PER_BIT;
  Data manchester(POINT_COUNT);
  const double f = mTransSpeed * 2; // Clock frecuency
  const double T = 1.0 / f; // Clock period
  double t = 0.0; // current time
  double amplitude = mAmplitude;
  for (int i = 0; i < POINT_COUNT; i += POINTS_PER_BIT)
  {
    manchester[i] = make_pair(t, amplitude);
    if (mValueToEncode[i / POINTS_PER_BIT].digitValue())
    { // Make no transition
      manchester[i + 1] = make_pair(t += T, amplitude);
      manchester[i + 2] = make_pair(t, amplitude *= -1);
      manchester[i + 3] = make_pair(t += T, amplitude);
    }
    else
    { // Make transition
      manchester[i + 1] = make_pair(t, amplitude *= -1);
      manchester[i + 2] = make_pair(t += T, amplitude);
      manchester[i + 3] = make_pair(t, amplitude *= -1);
      t += T;
      if (i + 4 == POINT_COUNT) {
        // When the input ends with zero, its necesary add one point
        manchester << make_pair(t, amplitude);
      }
    }
  }
  mTimeMax = t;
  return manchester;
}

BinaryEncoder::Data BinaryEncoder::generateMultilevel(int levels)
{
  static const int POINTS_PER_BIT = 2;
  const int POINT_COUNT = mN * POINTS_PER_BIT;
  Data multilevel(POINT_COUNT);
  const double f = mTransSpeed * 2; // Clock frecuency
  const double T = 1.0 / f; // Clock period
  double t = 0.0;
  double amplitude = -mAmplitude;
  const double levelIncrement = mAmplitude / (levels - 1);
  for (int i = 0; i < POINT_COUNT; i += POINTS_PER_BIT)
  {
    if (mValueToEncode[i / POINTS_PER_BIT].digitValue())
    {
      if (amplitude < mAmplitude)
      {
        amplitude += levelIncrement;
      }
      else if (amplitude > -mAmplitude)
      {
        amplitude -= levelIncrement;
      }
    }
    multilevel[i] = make_pair(t, amplitude);
    multilevel[i + 1] = make_pair(t += T, amplitude);
  }
  return multilevel;
}
