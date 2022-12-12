#include "Timer.h"

namespace Vivium {
	const long double Timer::m_NsToS = pow(10, -9);
	const std::chrono::system_clock::time_point Timer::compile_time = std::chrono::system_clock::now();

	double Timer::GetInternalTime() const
	{
		return m_Time;
	}

	void Timer::SetTime(double new_time)
	{
		m_Time = new_time;
	}

	double Timer::GetTime()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - compile_time).count() * m_NsToS;
	}

	void Timer::Start()
	{
		m_Time = GetTime();
	}

	void Timer::Reset()
	{
		m_Time = GetTime();
	}

	std::string Timer::GetTimeString()
	{
		return std::format("{:%H:%M:%OS}", std::chrono::system_clock::now());
	}

	Timer::Timer()
		: m_Time(GetTime())
	{}

	Timer::Timer(const Timer& other)
		: m_Time(other.m_Time) {}

	Timer::Timer(Timer&& other) noexcept
		: m_Time(std::move(other.m_Time)) {}

	Timer& Timer::operator=(const Timer& other)
	{
		m_Time = other.m_Time;

		return *this;
	}

	Timer& Timer::operator=(Timer&& other) noexcept
	{
		m_Time = std::move(other.m_Time);

		return *this;
	}

	double Timer::GetElapsedNoReset()
	{
		return Timer::GetTime() - m_Time;
	}

	double Timer::GetElapsed()
	{
		// Get current time
		double now = Timer::GetTime();
		// Calculate difference
		double elapsed = now - m_Time;
		// Update our time
		m_Time = now;
		// Return elapsed time
		return elapsed;
	}
	
	Timed::Timed(void* data, float lifespan) : m_Data(data), m_Lifespan(lifespan) { m_Timer.Start(); }
}