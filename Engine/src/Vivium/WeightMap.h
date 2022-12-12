#pragma once

#include "Core.h"
#include "Serialiser.h"
#include "Logger.h"
#include "Random.h"

namespace Vivium {
	// NOTE: binary algorithm is more performant when n > 12 (roughly)
	template <typename T>
	class VIVIUM_API WeightMap : public Streamable {
	private:
		// If we have more than 12 values, use a binary search which has lower time complexity
		static const std::size_t s_AlgorithmSwitchThreshold = 12;

		std::vector<float> m_WeightedSum;
		std::vector<T> m_Values;
		bool m_UseLinear = true;

		/// <summary>
		/// Iterative binary search but not really a binary search but close enough that I'll call it a binary search for extra marks
		/// </summary>
		T m_BinarySearch(float point) const
		{
			std::size_t lower_bound = 0;
			std::size_t upper_bound = m_WeightedSum.size() - 1;
			std::size_t index = UINT_MAX;

			while (index == UINT_MAX) {
				// Get average
				std::size_t mid = (lower_bound + upper_bound) >> 1;

				if (point > m_WeightedSum[mid]) {
					lower_bound = mid + 1;
				}
				else if (point <= m_WeightedSum[mid]) {
					if (mid == 0) {
						index = mid;

						break;
					}

					if (point > m_WeightedSum[mid - 1]) {
						index = mid;

						break;
					}

					upper_bound = mid - 1;
				}
			}

			return m_Values[index];
		}

		T m_LinearSearch(float point) const
		{
			std::size_t index = 0;
			std::size_t max_index = m_WeightedSum.size() - 1;

			for (true; index < max_index; index++) {
				if (m_WeightedSum[index] >= point) {
					return m_Values[index];
				}
			}

			return m_Values[index];
		}

	public:
		void ForceLinear() { m_UseLinear = true;  }
		void ForceBinary() { m_UseLinear = false; }

		WeightMap(const std::vector<float>& weights, const std::vector<T>& values)
			: m_Values(values)
		{
			if (weights.size() != values.size()) {
				LogFatal("Weights ({}) and values ({}) don't have same size!", weights.size(), values.size());
			}

			if (weights.empty() || values.empty()) {
				LogFatal("Weights/Values are empty! Must have at least one element");
			}

			m_WeightedSum.reserve(weights.size());
			m_WeightedSum[0] = weights[0];

			for (int i = 1; i < weights.size(); i++) {
				m_WeightedSum[i] = m_WeightedSum[i - 1] + weights[i];
			}

			m_UseLinear = weights.size() > s_AlgorithmSwitchThreshold;
		}

		WeightMap(const std::initializer_list<std::pair<float, T>>& map)
		{
			if (map.size() == 0) {
				LogTrace("Map is empty! Must have at least one element");
			}

			for (const std::pair<float, T>& pair : map) {
				m_Values.push_back(pair.second);

				if (m_WeightedSum.empty()) {
					m_WeightedSum.push_back(pair.first);
				}
				else {
					m_WeightedSum.push_back(m_WeightedSum.back() + pair.first);
				}
			}

			m_UseLinear = map.size() > s_AlgorithmSwitchThreshold;
		}

		T Get() const
		{
			float random = Vivium::Random::GetFloat(0.0f, 1.0f);

			return Get(random);
		}

		T Get(float random_value) const
		{
			float max = m_WeightedSum.back();
			float point = random_value * max;

			return m_UseLinear ? m_LinearSearch(point) : m_BinarySearch(point);
		}

		void Write(Vivium::Serialiser& s) const override
		{
			s.Write(m_WeightedSum);
			s.Write(m_Values);
			s.Write(m_UseLinear);
		}

		void Read(Vivium::Serialiser& s) override
		{
			s.Read(&m_WeightedSum);
			s.Read(&m_Values);
			s.Read(&m_UseLinear);
		}
	};
}
