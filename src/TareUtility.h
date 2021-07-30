/*
  Temperatures.h - Lib for gtting weights 
*/
#ifndef TareUtility_H
#define TareUtility_H

#include "WeightProcessor.h"

class TareUtility
{

  const int delayBetweenSamplesInSeconds = 3;
  const int amountWeightSamples = 10;

public:
  TareUtility(WeightProcessor *processor);
  // Get the Offset value, when no weight is places
  float GetZeroOffset();
  // Get lower bound;
  float GetLowerBound();
  // Get the upper bound
  float GetUpperBound();
  // Get the Values to Adjust.
  void DoInitialAdjust(float kgValue);
  // Get weight
  float GetMedianWeight(int sampleAmount, int delaysbetweenSamples);

private:
  WeightProcessor *processor;
  float zeroOffset;
  float upperValue;
  float lowerValue;
  float kgDividerValue;
  float GetWeight();
};

#endif