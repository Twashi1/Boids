#pragma once

#include "Vector2.h"

namespace Vivium {
	class Serialiser;

	class VIVIUM_API Streamable {
	public:
		// TODO streamable constructor?
		virtual void Write(Serialiser& s) const = 0;
		virtual void Read(Serialiser& s) = 0;

		virtual ~Streamable() = default;
	
	protected:
		Streamable() = default;
	};

	// For streaming types which only need their raw memory to be serialised/deserialised to be valid
	class VIVIUM_API StreamablePOD {
	public:
		StreamablePOD() = default;
		virtual ~StreamablePOD() = default;
	};

	template <typename> struct __vector2_checker : std::false_type {};
	// NOTE: only serialisable types can be passed to a vector anyway
	template <typename value_t> struct __vector2_checker<Vector2<value_t>> : std::true_type {};

	template <typename T> inline constexpr bool __BaseStreamableTypes =
	   std::is_arithmetic_v<T>
	|| std::is_same_v<T, std::string>
	|| std::is_same_v<T, bool>
	|| std::is_base_of_v<StreamablePOD, T>
	|| __vector2_checker<T>::value;

	template <typename T> concept IsBaseStreamable = __BaseStreamableTypes<T>;

	template <typename first_t, typename second_t=void> struct __pair_checker { static constexpr bool value = false; };

	template <typename first_t, typename second_t> struct __pair_checker<std::pair<first_t, second_t>> {
		static constexpr bool value = IsStreamable<first_t> && IsStreamable<second_t>; 
	};

	template <typename T> concept IsStreamable =
	   std::is_base_of_v<Streamable, T>
	|| __BaseStreamableTypes<T>
	|| IsBaseStreamable<typename std::underlying_type<T>::type>
	|| __pair_checker<T>::value;
}