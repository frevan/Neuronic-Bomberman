#include "nel_fps.h"



namespace nel {

TFpsCalculator::TFpsCalculator()
:	Clock(),
	FrameTimeCache(),
	CacheIdx(0),
	CacheCount(0)
{ 
	Value = 0.0;
}

void TFpsCalculator::NewFrame() 
{ 
	int64_t elapsed = Clock.restart().asMicroseconds();
	AddValueToCache(elapsed);
	CalculateValue();
};

void TFpsCalculator::AddValueToCache(int64_t value)
{
	FrameTimeCache[CacheIdx] = value;
	
	CacheIdx++;
	if (CacheIdx >= CACHESIZECONST)
		CacheIdx = 0;

	if (CacheCount < CACHESIZECONST)
		CacheCount++;
}

void TFpsCalculator::CalculateValue()
{
	int64_t totalTime = 0;
	for (int i = 0; i < CacheCount; i++)
		totalTime += FrameTimeCache[i];

	Value = (CacheCount*1.0f) / (totalTime*1.0f/1000000.f);
}

};	// namespace nel