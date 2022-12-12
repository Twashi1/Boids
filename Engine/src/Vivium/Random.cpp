#include "Random.h"
#include "Application.h"

namespace Vivium {
	std::mt19937 Random::m_Generator;

	void Random::m_Init()
	{
		m_Generator = std::mt19937(
			std::chrono::system_clock::now().time_since_epoch().count()
		);
	}

	bool Random::GetBool()
	{
		std::uniform_int_distribution<int> dist(0, 1);

		return dist(m_Generator);
	}

	int Random::GetInt(int min, int max)
	{
		std::uniform_int_distribution<int> dist(min, max);

		return dist(m_Generator);
	}

	float Random::GetFloat(float min, float max)
	{
		std::uniform_real_distribution<float> dist(min, max);

		return dist(m_Generator);
	}

	Vector2<float> Random::GetVector2f(float min, float max)
	{
		std::uniform_real_distribution<float> dist(min, max);

		return Vector2<float>(dist(m_Generator), dist(m_Generator));
	}

	Vector2<float> Random::GetVectorAbs2f(float min, float max)
	{
		std::uniform_real_distribution<float> dist(min, max);

		int flipX = GetInt(0, 1) * 2 - 1;
		int flipY = GetInt(0, 1) * 2 - 1;

		return Vector2<float>(dist(m_Generator) * flipX, dist(m_Generator) * flipY);
	}

	Vector2<float> Random::GetVector2f(float magnitude)
	{
		std::uniform_real_distribution<float> dist(0.0f, 2.0f * Math::PI); // Generate random angle (radians)

		float angle = dist(m_Generator);

		return Vector2<float>(std::cos(angle), std::sin(angle)) * magnitude;
	}
}