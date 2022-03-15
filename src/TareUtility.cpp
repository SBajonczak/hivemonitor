#include "TareUtility.h"
#include <RunningMedian.h>

TareUtility::TareUtility(WeightProcessor *processor)
{
    this->processor = processor;
}

float TareUtility::GetZeroOffset()
{

    this->zeroOffset = this->GetMedianWeight(this->amountWeightSamples, this->delayBetweenSamplesInSeconds);
    return this->zeroOffset;
}

float TareUtility::GetLowerBound()
{
    this->lowerValue = this->GetMedianWeight(this->amountWeightSamples, this->delayBetweenSamplesInSeconds);
    return this->lowerValue;
}

float TareUtility::GetUpperBound()
{
    this->upperValue = this->GetMedianWeight(this->amountWeightSamples, this->delayBetweenSamplesInSeconds);
    return this->upperValue;
}

float TareUtility::GetMedianWeight(int sampleAmount, int delaysbetweenSamples)
{
    RunningMedian weightSamples = RunningMedian(sampleAmount); // create RunningMedian object
    do
    {
        delay(delaysbetweenSamples * 1000);
        weightSamples.add(processor->getWeight());

    } while (weightSamples.getCount() < weightSamples.getSize());
    return weightSamples.getMedian();
}

void TareUtility::DoInitialAdjust(float kgValue)
{
    this->GetZeroOffset();
    this->GetLowerBound();
    this->GetUpperBound();
    this->kgDividerValue = (this->upperValue - this->lowerValue) / (kgValue / 1000.0f);
}
