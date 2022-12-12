#pragma once

#include "Core.h"
#include "Serialiser.h"
#include "Utils.h"

namespace Vivium {
    // Stores version number in format major.minor.patch
    struct VIVIUM_API VersionNumber {
    private:
        // Initialises major, minor, and patch from a version string
        void m_ConstructFromString(const char* string);

    public:
        // major.minor.patch
        uint16_t major;
        uint16_t minor;
        uint16_t patch;

        VersionNumber();
        VersionNumber(uint16_t major, uint16_t minor, uint16_t patch);
        VersionNumber(const char* version_string);
        VersionNumber(const std::string& version_string);
        VersionNumber(const VersionNumber& other);

        void operator=(const char* version_string);
        void operator=(const std::string& version_string);
    };

    bool VIVIUM_API operator==(const VersionNumber& a, const VersionNumber& b);

    std::ostream& operator<<(std::ostream& os, const VersionNumber& n);
}

namespace std {
    template <>
    struct formatter<Vivium::VersionNumber> : formatter<string> {
        auto format(Vivium::VersionNumber version, format_context& ctx) {
            return formatter<string>::format(
                std::format("v{}.{}.{}", version.major, version.minor, version.patch), ctx);
        }
    };
}
