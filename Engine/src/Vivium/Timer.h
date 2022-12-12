#pragma once

#include "Core.h"

namespace Vivium {
	class VIVIUM_API Timer {
	private:
		static const long double m_NsToS; // Nanoseconds to seconds ratio
		double m_Time;

	public:
		static const std::chrono::system_clock::time_point compile_time;

		static double GetTime(); // Get time since program compiled
		static std::string GetTimeString();

		Timer();
		Timer(const Timer& other);
		Timer(Timer&& other) noexcept;

		Timer& operator=(const Timer& other);
		Timer& operator=(Timer&& other) noexcept;

		double GetInternalTime() const;
		void SetTime(double new_time);

		double GetElapsedNoReset();
		// Gets time since last GetElapsed or Start call
		double GetElapsed();
		// Start calculating elapsed time
		void Start();
		// Stop calculating elapsed time/reset timer
		void Reset();
	};

	// NOTE: made this for things that should display temporarily, but i probably won't use it after all
	class VIVIUM_API Timed {
	private:
		void* m_Data;
		Timer m_Timer;
		float m_Lifespan;
		bool isDead = false;

	public:
		Timed(void* data, float lifespan);

		// https://www.nextptr.com/tutorial/ta1227747841/the-stdshared_ptrvoid-as-arbitrary-userdata-pointer
		template <typename T>
		T GetData() {
			if (isDead) return nullptr;

			return (T*)m_Data;
		}

		bool IsExpired() const {
			return isDead;
		}

		// Returns if object is still alive
		template <typename T>
		bool Update() {
			if (isDead) return false;

			float time_alive = m_Timer.GetElapsedNoReset();

			if (time_alive > m_Lifespan) {
				isDead = true;

				((T*)m_Data)->~T();
				m_Data = nullptr;

				return false;
			}

			return true;
		}
	};
}
