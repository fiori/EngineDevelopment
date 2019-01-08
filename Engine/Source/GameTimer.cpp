#include <Windows.h>//QueryPerformanceFrequency
#include "../Headers/GameTimer.h"

GameTimer::GameTimer() :mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0), mPauseTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
	__int64 countsPerSec;
	//Retrieves the frequency of the performance counter.
	//The frequency of the performance counter is fixed at system boot and is consistent across all processors.
	//Therefore, the frequency need only be queried upon application initialization, and the result can be cached.
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

const float GameTimer::GameTime()
{
	//If we are stopped, do not count the time that has passed since we stopped.
	//To correct this, we can subtract the paused time from mStopTime
	//
	// previous paused time
	// |<----------->|
	// ---*------------*-------------*-------*-----------*------> time
	// mBaseTime mStopTime mCurrTime

	if (mStopped)
	{
		return (float)(((mStopTime - mPauseTime) - mBaseTime) * mSecondsPerCount);
	}

	// The distance mCurrTime - mBaseTime includes paused time,
	// which we do not want to count. To correct this, we can subtract
	// the paused time from mCurrTime:
	//
	// (mCurrTime - mPausedTime) - mBaseTime
	//
	// |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	// mBaseTime mStopTime startTime mCurrTime

	else
	{
		return (float)(((mCurrTime - mPauseTime) - mBaseTime) * mSecondsPerCount);
	}
}

const float GameTimer::DeltaTime()
{
	return mDeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	//Accumulate the time elapsed between stop and start
	//
	// |<-------d------->|
	// ---------------*-----------------*------------> time
	// mStopTime startTime
	//If we are resuming the timer from a stopped state
	if (mStopped)
	{
		//Accumulate paused time
		mPauseTime += (startTime - mStopTime);

		//since we are starting the timer back up, the current previous time is not valid,
		//as it occured while paused.
		//So reset it to the current time.
		mPrevTime = startTime;

		//no longer stopped
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	//IF already stopped, don't do anything
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		//Save the time we stopped at and update the stopped boolean
		mStopTime = currTime;
		mStopped = true;
	}
}

void GameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}
	//Get the time this frame.
	__int64 currTime;
	//Retrieves the current value of the performance counter, which is a high resolution (<1us)
	//time stamp that can be used for time-interval measurements.
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	//Time difference between this frame and the previous.
	mDeltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;

	//Prepare for next frame.
	mPrevTime = mCurrTime;

	//Force non negative. The DXSDK's CDXUTTimer mentions that if the processor goes into a power save mode
	//or we get shuffled to another processor, then mDeltaTime can be negative
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}

GameTimer::~GameTimer()
= default;