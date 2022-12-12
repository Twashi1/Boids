#include "VersionNumber.h"

namespace Vivium {
    std::string to_string(const VersionNumber& version_number)
    {
        return std::format("v{}.{}.{}", version_number.major, version_number.minor, version_number.patch);
    }

    bool operator==(const VersionNumber& a, const VersionNumber& b)
    {
        return a.major == b.major && a.minor == b.minor && a.patch == b.patch;
    }

    std::ostream& operator<<(std::ostream& os, const VersionNumber& n)
    {
        return os << "v" << n.major << "." << n.minor << "." << n.patch;
    }

    void VersionNumber::m_ConstructFromString(const char* string)
    {
        std::vector<std::string> parts = Utils::SplitString(string, ".");

        // Remove v from first element in parts
        Utils::EraseSubstring(parts[0], "v");

        // Convert strings to integers and store in major, minor, and patch
        major = stoi(parts[0]);
        minor = stoi(parts[1]);
        patch = stoi(parts[2]);
    }

    VersionNumber::VersionNumber()
        : major(0), minor(0), patch(0)
    {
    }

    VersionNumber::VersionNumber(uint16_t major, uint16_t minor, uint16_t patch)
        : major(major), minor(minor), patch(patch)
    {}

    VersionNumber::VersionNumber(const char* version_string)
    {
        m_ConstructFromString(version_string);
    }

    VersionNumber::VersionNumber(const std::string& version_string)
    {
        m_ConstructFromString(version_string.c_str());
    }

    VersionNumber::VersionNumber(const VersionNumber& other)
        : major(other.major), minor(other.minor), patch(other.patch)
    {}

    void VersionNumber::operator=(const char* version_string)
    {
        m_ConstructFromString(version_string);
    }

    void VersionNumber::operator=(const std::string& version_string)
    {
        m_ConstructFromString(version_string.c_str());
    }
}