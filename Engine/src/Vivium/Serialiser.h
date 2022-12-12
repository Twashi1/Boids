#pragma once

#include "Core.h"
#include "Stream.h"
#include "Streamable.h"

/*
TODO: add functionality for pointers?
TODO: text serialisation
TODO: cleanup
TODO: possible issues with 0 size vectors?
TODO: add some extra information to error messages for invalid types serialising
*/

namespace Vivium {
	class VIVIUM_API Serialiser {
	private:
		/* WriteBinary */
		template <typename T>
		void m_WriteBinary(Stream& s, const T& data) {
			s.out->write((char*)&data, sizeof(T));
		}

		template <>
		void m_WriteBinary(Stream& s, const std::string& data) {
			s.out->write(data.c_str(), data.length() * sizeof(char));
			s.out->put('\0'); // Write null termination character
		}

		template <typename T>
		void m_WriteBinary(Stream& s, const std::vector<T>& data) {
			std::size_t size = data.size();
			m_WriteBinary<std::size_t>(s, size);

			s.out->write((char*)&data[0], sizeof(T) * size);
		}

		template <>
		void m_WriteBinary(Stream& s, const std::vector<std::string>& data) {
			std::size_t size = data.size();
			m_WriteBinary<std::size_t>(s, size);

			for (std::size_t i = 0; i < size; i++) {
				m_WriteBinary<std::string>(s, data[i]);
			}
		}

		template <typename T>
		void m_WriteBinary(Stream& s, const T* data, std::size_t length) {
			static_assert(!std::is_same_v<T, std::string>, "Cannot write array of strings, use std::vector instead");

			s.out->write((char*)data, sizeof(T) * length);
		}

		template <typename T>
		void m_WriteBinary(Stream& s, const Vector2<T>& data) {
			s.out->write((char*)&data, sizeof(Vector2<T>));
		}

		/* ReadBinary */
		template <typename T>
		void m_ReadBinary(Stream& s, T* memory) {
			char* readbuff = new char[sizeof(T)]; // Allocate buffer for file data
			s.in->read(readbuff, sizeof(T)); // Read file data into buffer

			std::memcpy(memory, readbuff, sizeof(T)); // Copy data from buffer into pointer given

			delete[] readbuff; // Delete buffer after copying the data
		}

		template <>
		void m_ReadBinary(Stream& s, std::string* memory) {
			char lastchar;

			s.in->get(lastchar); // Get first character
			*memory += lastchar; // Add to memory

			// While the last character is not null termination character
			while (lastchar != '\0') {
				s.in->get(lastchar); // Get next character
				*memory += lastchar; // Add next character to memory

				// If character is null termination character, break
				if (lastchar == '\0') break;
			}
		}

		template <typename T>
		void m_ReadBinary(Stream& s, std::vector<T>* memory) {
			// Get size of vector
			std::size_t size;
			m_ReadBinary<std::size_t>(s, &size);

			char* readbuff = new char[sizeof(T) * size];
			s.in->read(readbuff, sizeof(T) * size);

			memory->resize(size);

			// Alternative &(*memory)[0]
			std::memcpy(&memory->at(0), readbuff, sizeof(T) * size);

			delete[] readbuff;
		}

		template <>
		void m_ReadBinary(Stream& s, std::vector<std::string>* memory) {
			std::size_t size;
			m_ReadBinary<std::size_t>(s, &size);

			memory->resize(size);

			for (std::size_t i = 0; i < size; i++) {
				m_ReadBinary<std::string>(s, &memory->at(i));
			}
		}

		template <typename T>
		void m_ReadBinary(Stream& s, T* memory[], std::size_t length) {
			static_assert(!std::is_same_v<T, std::string>, "Cannot read array of strings, use std::vector instead");

			// Assuming memory has already been allocated
			char* readbuff = new char[sizeof(T) * length];
			s.in->read(readbuff, sizeof(T) * length);

			std::memcpy(*memory, readbuff, sizeof(T) * length);

			delete[] readbuff;
		}

		template <typename T>
		void m_ReadBinary(Stream& s, T memory[], std::size_t length) {
			static_assert(!std::is_same_v<T, std::string>, "Cannot read array of strings, use std::vector instead");

			// Assuming memory has already been allocated
			char* readbuff = new char[sizeof(T) * length];
			s.in->read(readbuff, sizeof(T) * length);

			std::memcpy(memory, readbuff, sizeof(T) * length);

			delete[] readbuff;
		}

		template <typename T>
		void m_ReadBinary(Stream& s, Vector2<T>* memory) {
			char* readbuff = new char[sizeof(Vector2<T>)];
			s.in->read(readbuff, sizeof(Vector2<T>));

			std::memcpy(memory, readbuff, sizeof(Vector2<T>));

			delete[] readbuff;
		}

		Stream m_Stream;
		int m_Flags;

	public:
		Serialiser(int flags);

		void BeginRead(const char* path);
		void BeginWrite(const char* path);

		void EndRead();
		void EndWrite();

		~Serialiser() = default;

		template <typename T>
		void Write(const T& object) {
			if constexpr (!IsStreamable<T>) {
				LogError("Object type {} is not Streamable", typeid(T).name());
			}
			// If the object inherits Streamable, call Streamable's write
			else if constexpr (std::is_base_of_v<Streamable, T>) {
				object.Write(*this);
			}
			// Its a trivial type
			else {
				m_WriteBinary(m_Stream, object);
			}
		}

		template <typename T>
		void Write(const std::vector<T>& object) {
			if constexpr (!IsStreamable<T>) {
				LogError("Object type {} is not Streamable", typeid(T).name());
			}
			// If the object inherits Streamable, call Streamable's write
			else if constexpr (std::is_base_of_v<Streamable, T>) {
				std::size_t size = object.size();
				Write<std::size_t>(size);

				for (std::size_t i = 0; i < size; i++) {
					Write<T>(object[i]);
				}
			}
			// Its a trivial type
			else {
				m_WriteBinary(m_Stream, object);
			}
		}

		template <typename T, std::size_t Size>
		void Write(const std::array<T, Size>& object) {
			if constexpr (!IsStreamable<T>) {
				LogError("Object type {} is not Streamable", typeid(T).name());
			}
			// If the object inherits Streamable, call Streamable's write
			else if constexpr (std::is_base_of_v<Streamable, T>) {
				for (std::size_t i = 0; i < Size; i++) {
					Write<T>(object[i]);
				}
			}
			// Its a trivial type
			else {
				m_WriteBinary(m_Stream, object);
			}
		}

		template <typename Key_t, typename Value_t>
		void Write(const std::map<Key_t, Value_t>& object) {
			if constexpr (!IsStreamable<Key_t> || !IsStreamable<Value_t>) {
				LogError("Object contains type that is not Streamable");
			}
			else {
				// Write amount of pairs in map
				std::size_t size = object.size();
				Write<std::size_t>(size);
				// Iterate map
				for (auto& [key, value] : object) {
					// Write key and value
					Write<Key_t>(key);
					Write<Value_t>(value);
				}
			}
		}

		template <typename Key_t, typename Value_t>
		void Write(const std::unordered_map<Key_t, Value_t>& object) {
			if constexpr (!IsStreamable<Key_t> || !IsStreamable<Value_t>) {
				LogError("Object contains type that is not Streamable");
			}
			else {
				// Write amount of pairs in map
				std::size_t size = object.size();
				Write<std::size_t>(size);
				// Iterate map
				for (auto& [key, value] : object) {
					// Write key and value
					Write<Key_t>(key);
					Write<Value_t>(value);
				}
			}
		}

		template <typename First_t, typename Second_t>
		void Write(const std::pair<First_t, Second_t>& object) {
			if constexpr (!IsStreamable<First_t> || !IsStreamable<Second_t>) {
				LogError("Object {} contains type that is not streamable", typeid(std::pair<First_t, Second_t>).name());
			}
			else {
				Write<First_t>(object.first);
				Write<Second_t>(object.second);
			}
		}

		template <typename T>
		void Write(const T* object_array, std::size_t length) {
			if constexpr (!IsStreamable<T>) {
				LogError("Object type {} is not Streamable", typeid(T).name());
			}
			// If the object inherits Streamable, call Streamable's write
			else if constexpr (std::is_base_of_v<Streamable, T>) {
				for (std::size_t i = 0; i < length; i++) {
					Write<T>(object_array[i]);
				}
			}
			// Its a trivial type
			else {
				m_WriteBinary(m_Stream, object_array, length);
			}
		}

		// NOTE: assuming the memory has been allocated for us
		template <typename T>
		void Read(T* memory) {
			// Ensure object is streamable
			if constexpr (!IsStreamable<T>) {
				LogError("Object type {} is not Streamable", typeid(T).name());
			}
			// If the object inherits Streamable, call Streamable's read
			else if constexpr (std::is_base_of_v<Streamable, T>) {
				memory->Read(*this);
			}
			// Its a trivial type
			else {
				m_ReadBinary(m_Stream, memory);
			}
		}

		template <typename T>
		void Read(std::vector<T>* memory) {
			// Ensure object is streamable
			if constexpr (!IsStreamable<T>) {
				LogError("Object type {} is not Streamable", typeid(T).name());
			}
			// If the object inherits Streamable, call Streamable's read
			else if constexpr (std::is_base_of_v<Streamable, T>) {
				std::size_t size;
				Read<std::size_t>(&size);

				memory->resize(size);

				for (std::size_t i = 0; i < size; i++) {
					Read<T>(&memory->at(i));
				}
			}
			// Its a trivial type
			else {
				m_ReadBinary(m_Stream, memory);
			}
		}

		template <typename T, std::size_t Size>
		void Read(std::array<T, Size>* memory) {
			// Ensure object is streamable
			if constexpr (!IsStreamable<T>) {
				LogError("Object type {} is not Streamable", typeid(T).name());
			}
			// If the object inherits Streamable, call Streamable's read
			else if constexpr (std::is_base_of_v<Streamable, T>) {
				for (std::size_t i = 0; i < Size; i++) {
					Read<T>(&memory->at(i));
				}
			}
			// Its a trivial type
			else {
				m_ReadBinary(m_Stream, memory);
			}
		}

		template <typename Key_t, typename Value_t>
		void Read(std::map<Key_t, Value_t>* memory) {
			assert(std::is_default_constructible_v<Key_t> && std::is_default_constructible_v<Value_t>, "Key or value are not default constructible");

			if constexpr (!IsStreamable<Key_t> || !IsStreamable<Value_t>) {
				LogError("Object contains type that is not Streamable");
			}
			else {
				// Read size
				std::size_t size = 0;
				Read<std::size_t>(&size);

				for (std::size_t i = 0; i < size; i++) {
					// Requires default constructor
					Key_t key;
					Value_t value;

					Read<Key_t>(&key);
					Read<Value_t>(&value);

					// Requires copy constructor?
					memory->insert({ key, value });
				}
			}
		}

		template <typename Key_t, typename Value_t>
		void Read(std::unordered_map<Key_t, Value_t>* memory) {
			assert(std::is_default_constructible_v<Key_t> && std::is_default_constructible_v<Value_t>, "Key or value are not default constructible");

			if constexpr (!IsStreamable<Key_t> || !IsStreamable<Value_t>) {
				LogError("Object contains type that is not Streamable");
			}
			else {
				// Read size
				std::size_t size = 0;
				Read<std::size_t>(&size);

				memory->reserve(size);

				for (std::size_t i = 0; i < size; i++) {
					// Requires default constructor
					Key_t key;
					Value_t value;

					Read<Key_t>(&key);
					Read<Value_t>(&value);

					// Requires copy constructor?
					memory->insert({ key, value });
				}
			}
		}

		template <typename First_t, typename Second_t>
		void Read(std::pair<First_t, Second_t>* memory) {
			if constexpr (!IsStreamable<First_t> || !IsStreamable<Second_t>) {
				LogError("Object contains type that is not Streamable");
			}
			else {
				Read<First_t>(&(memory->first));
				Read<Second_t>(&(memory->second));
			}
		}

		template <typename T>
		void Read(T* memory[], std::size_t length) {
			// Ensure object is streamable
			if constexpr (!IsStreamable<T>) {
				LogError("Object type {} is not Streamable", typeid(T).name());
			}
			// If the object inherits Streamable, call Streamable's read
			else if constexpr (std::is_base_of_v<Streamable, T>) {
				for (std::size_t i = 0; i < length; i++) {
					Read<T>(&(*memory)[i]);
				}
			}
			// Its a trivial type
			else {
				m_ReadBinary(m_Stream, memory, length);
			}
		}

		template <typename T>
		void Read(T memory[], std::size_t length) {
			// Ensure object is streamable
			if constexpr (!IsStreamable<T>) {
				LogError("Object type {} is not Streamable", typeid(T).name());
			}
			// If the object inherits Streamable, call Streamable's read
			else if constexpr (std::is_base_of_v<Streamable, T>) {
				for (std::size_t i = 0; i < length; i++) {
					Read<T>(&memory[i]);
				}
			}
			// Its a trivial type
			else {
				m_ReadBinary(m_Stream, memory, length);
			}
		}
	};
}