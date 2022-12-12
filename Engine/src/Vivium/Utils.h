#pragma once

#include "Core.h"
#include "Logger.h"
#include "Random.h"

#include <stb_image_write.h>

namespace Vivium {
	namespace Utils {
		template <typename T>
		void Remove(std::vector<T>& data, const T& object)
		{
			if (data.size() == 0) { LogWarn("Erasing object from empty list"); return; }
			// Remove all occurences of object from data, and return new end of list
			auto new_end = std::remove(data.begin(), data.end(), object);

			// Erase all elements from the new end of the list to the old end of the list
			data.erase(new_end, data.end());
		}

		// Removes substr from str
		VIVIUM_API void EraseSubstring(std::string& str, const ::std::string& substr);
		// Removes substr from str
		VIVIUM_API void EraseSubstring(std::string& str, const char* substr);

		// Returns substr removed from str
		VIVIUM_API std::string EraseSubstring(const char* str, const ::std::string& substr);
		// Returns substr removed from str
		VIVIUM_API std::string EraseSubstring(const char* str, const char* substr);

		// Reads an entire file into one string on heap (NOTE: remember to delete the string once done with it)
		VIVIUM_API std::string* ReadFile(const ::std::string& path);

		// Splits string by delimiter
		VIVIUM_API std::vector<std::string> SplitString(const ::std::string& s, const ::std::string& delim);

		VIVIUM_API void SaveAsBitmap(unsigned char* data, int width, int height, int bpp, const char* path);

		template <typename T>
		std::size_t RandomWeightedChoice(const std::vector<T>& weights, T random_value)
		{
			T sum = std::accumulate(weights.begin(), weights.end(), 0);

			random_value *= sum;

			for (std::size_t i = 0; i < weights.size(); i++) {
				if (random_value < weights[i]) {
					return i;
				}

				random_value -= weights[i];
			}

			LogWarn("Weird stuff in RandomWeightedChoice");

			return NULL;
		}

		// TODO: some sort of indexing method would be nice
		class VIVIUM_API VoidArray {
		private:
			template <typename T>
			static void m_Make(void* arr, std::size_t offset, T arg)
			{
				char* char_arr = (char*)arr;
				std::memcpy(char_arr + offset, &arg, sizeof(arg));
			}

			template <typename T, typename... Ts>
			static void m_Make(void* arr, std::size_t offset, T arg, Ts... args)
			{
				char* char_arr = (char*)arr;
				std::memcpy(char_arr + offset, &arg, sizeof(arg));

				m_Make(arr, offset + sizeof(arg), args...);
			}

		public:
			template <typename... Ts>
			static void* Make(std::size_t& size, Ts... args)
			{
				size = (sizeof(Ts) + ... + 0);
				void* data = malloc(size);

				m_Make(data, 0, args...);

				return data;
			}
		};
	}
}