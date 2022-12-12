#pragma once

#define _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING

#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <random>
#include <ranges>
#include <regex>
#include <set>
#include <stack>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <Windows.h>

// Extern libraries
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <irrKlang.h>

/* Flags */
#define VIVIUM_FLAG_DRAW_PHYSICS_CIRCLES			0x0000000000000001ULL
#define VIVIUM_FLAG_FUNCTION_SIGNATURE_LOGGING		0x0000000000000002ULL
#define VIVIUM_FLAG_PRINT_SHADER_COMPILATION		0x0000000000000004ULL

#ifdef __VIVIUM_BUILD_DLL
	#define VIVIUM_EXPOSE_CORE
	#define VIVIUM_API __declspec(dllexport)
#else
	#define VIVIUM_API __declspec(dllimport)
#endif

namespace Vivium {
	class VIVIUM_API Flag {
	private:
		typedef unsigned long long flag_t; // Total of 64 flags
		static flag_t m_Flags;

	public:
		static void Set(flag_t flag, bool val = true);
		static void Toggle(flag_t flag);
		static bool Test(flag_t flag);
	};

	template <typename T> concept Arithmetic = std::is_arithmetic_v<T>;
	template <typename T> concept Signed = std::is_signed_v<T>;
	template <typename T> concept Integral = std::is_integral_v<T>;
	template <typename T> concept FloatingPoint = std::is_floating_point_v<T>;
}
