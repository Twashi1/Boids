#pragma once

#include "Core.h"
#include "Timer.h"
#include "Vector2.h"

namespace Vivium {
	class Application;

	class VIVIUM_API Random {
	private:
		static std::mt19937 m_Generator;

		static void m_Init();

	public:
		static bool GetBool();
		static int GetInt(int min, int max);
		static float GetFloat(float min, float max);
		static Vector2<float> GetVector2f(float min, float max);
		static Vector2<float> GetVectorAbs2f(float min, float max);
		static Vector2<float> GetVector2f(float magnitude);

		friend Application;
	};
}
