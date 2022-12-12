#pragma once

#include "Core.h"
#include "Timer.h"
#include "Vector2.h"

#define VIVIUM_TRACE 0
#define VIVIUM_INFO  1
#define VIVIUM_WARN  2
#define VIVIUM_ERROR 3
#define VIVIUM_FATAL 4

// TODO: remove all this VIVIUM_SCOPE stuff, unused

// Only define the following if we're within the Vivium engine
#ifdef VIVIUM_EXPOSE_CORE
	#define LogFatal(msg, ...)	Vivium::__LogBase(std::format(msg, __VA_ARGS__), __LINE__, __FUNCSIG__, "Fatal", VIVIUM_FATAL)
	#define LogError(msg, ...)	Vivium::__LogBase(std::format(msg, __VA_ARGS__), __LINE__, __FUNCSIG__, "Error", VIVIUM_ERROR)
	#define LogWarn(msg, ...)	Vivium::__LogBase(std::format(msg, __VA_ARGS__), __LINE__, __FUNCSIG__, "Warn",  VIVIUM_WARN)
	#define LogInfo(msg, ...)	Vivium::__LogBase(std::format(msg, __VA_ARGS__), __LINE__, __FUNCSIG__, "Info",  VIVIUM_INFO)
	#define LogTrace(msg, ...)	Vivium::__LogBase(std::format(msg, __VA_ARGS__), __LINE__, __FUNCSIG__, "Trace", VIVIUM_TRACE)

	#ifdef __VIVIUM_SCOPE_ALL
		#define VIVIUM_SCOPE Vivium::__AddScope(__FILE__, __FUNCSIG__); Vivium::__ScopeTracker()
	#else
		#define VIVIUM_SCOPE
	#endif

	#define DISPLAY_CALL_STACK_ON_ERROR
#endif

namespace Vivium {
#ifdef VIVIUM_EXPOSE_CORE
	struct VIVIUM_API __Scope {
		const char* file;
		const char* func_sig;

		__Scope(const char* file, const char* func_sig);
	};

	static std::stack<__Scope> call_stack;

	struct VIVIUM_API __ScopeTracker {
		__ScopeTracker() = default;
		~__ScopeTracker();
	};

	void VIVIUM_API __AddScope(const char* file, const char* func_sig);

	void DisplayCallStack();

	std::string VIVIUM_API __SignatureToString(const std::string& msg, int line, const char* func_sig);
	void VIVIUM_API __LogBase(const std::string& msg, int line, const char* func_sig, const char* error_severity_message, int severity);
	void GLAPIENTRY __GLLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	const char* __GLDebugSeverityToString(GLenum severity);
	const char* __GLDebugTypeToString(GLenum type);
	const char* __GLDebugSourceToString(GLenum source);

	class VIVIUM_API Logger {
	public:
		template <typename T>
		static std::string PrettyPrint(const T& value)
		{
			std::stringstream ss;
			ss << value;
			return ss.str();
		}

		template <>
		static std::string PrettyPrint(const std::string& value)
		{
			std::stringstream ss;
			ss << "\"" << value << "\"";
			return ss.str();
		}

		template <typename T, std::size_t Size>
		static std::string PrettyPrint(const std::array<T, Size>& values)
		{
			std::stringstream ss;
			ss << "[";

			for (std::size_t i = 0; i < Size; i++) {
				ss << PrettyPrint(values[i]);

				if (i != Size - 1) {
					ss << ", ";
				}
			}

			ss << "]";

			return ss.str();
		}

		template <typename T>
		static std::string PrettyPrint(const std::vector<T>& values)
		{
			std::stringstream ss;
			ss << "[";

			for (std::size_t i = 0; i < values.size(); i++) {
				ss << PrettyPrint(values[i]);

				if (i != values.size() - 1) {
					ss << ", ";
				}
			}

			ss << "]";

			return ss.str();
		}

		template <typename T>
		static std::string PrettyPrint(const T* values, const std::size_t& length)
		{
			std::stringstream ss;
			ss << "[";

			for (std::size_t i = 0; i < length; i++) {
				ss << PrettyPrint(values[i]);

				if (i != length - 1) {
					ss << ", ";
				}
			}

			ss << "]";

			return ss.str();
		}
		
		// TODO: value_t might be list
		template <typename Key_t, typename Value_t>
		static std::string PrettyPrint(const std::map<Key_t, Value_t>& object)
		{
			std::stringstream ss;
			ss << "{\n";

			std::size_t i = 0;
			std::size_t obj_size = object.size();

			for (auto& [key, value] : object) {
				ss << "    " << PrettyPrint(key) << ": " << PrettyPrint(value);

				if (i != obj_size - 1) {
					ss << ",";
				}

				ss << "\n";

				i++;
			}

			ss << "}";

			return ss.str();
		}

		template <typename Key_t, typename Value_t>
		static std::string PrettyPrint(const std::unordered_map<Key_t, Value_t>& object)
		{
			std::stringstream ss;
			ss << "{\n";

			std::size_t i = 0;
			std::size_t obj_size = object.size();

			for (auto& [key, value] : object) {
				ss << "    " << PrettyPrint(key) << ": " << PrettyPrint(value);

				if (i != obj_size - 1) {
					ss << ",";
				}

				ss << "\n";

				i++;
			}

			ss << "}";

			return ss.str();
		}

		static void SetLogLevel(int level);
		static int GetLogLevel();
		static int GetGLSeverity();
		static int s_LogLevel;
	};
#endif
}
