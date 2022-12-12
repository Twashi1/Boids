#pragma once

#include "Core.h"
#include "Logger.h"
#include "Vector2.h"

// TODO: rework to use function pointers and enums to select functions/fractal/etc.

namespace Vivium {
    namespace Noise {
        // Hash integer into integer
        int VIVIUM_API Hash(unsigned int seed, int x);
        // Hash integer into float
        float VIVIUM_API Hashf(unsigned int seed, int x);

        // NOTE: ripped from boost
        int VIVIUM_API HashCombine(int hashed0, int hashed1);

        // Hash 2 integers to integer
        int VIVIUM_API Hash(unsigned int seed, Vector2<int> pos);
        // Hash 2 integers to integer
        int VIVIUM_API Hash(unsigned int seed, int x, int y);
        // Hash 2 integers to float
        float VIVIUM_API Hashf(unsigned int seed, Vector2<int> pos);
        // Hash 2 integers to float
        float VIVIUM_API Hashf(unsigned int seed, int x, int y);

        // Hash Vector2<int> to Vector2<int>
        Vector2<int> VIVIUM_API Hash2(unsigned int seed, Vector2<int> pos);
        // Hash Vector2<int> to Vector2<float>
        Vector2<float> VIVIUM_API Hash2f(unsigned int seed, Vector2<int> pos);

        // Interpolate between two doubles
        float VIVIUM_API Interpolate(float a, float b, float t);

        // Cubic hermite interpolation function (s shaped curve between 0 and 1)
        float Cubic(float t);
        // Quintic hermite interpolation function (smoother s shaped curve between 0 and 1)
        float Quintic(float t);

        class VIVIUM_API __NoiseVirtual {
        protected:
            unsigned int m_Seed = 0;
        public:
            float amplitude = 1.0f; // Multiplier for value of noise
            int wavelength = 1;  // Determines period at which noise repeats

            virtual void SetSeed(unsigned int nseed);

            __NoiseVirtual();
            __NoiseVirtual(unsigned int m_seed, float amplitude, int wavelength);
            virtual ~__NoiseVirtual();
        };

        class VIVIUM_API White : public __NoiseVirtual {
        public:
            using __NoiseVirtual::m_Seed;
            using __NoiseVirtual::amplitude;
            using __NoiseVirtual::wavelength;
            using __NoiseVirtual::SetSeed;

            // Get value noise for 1D coordinate as float
            float Get(int x);
            // Get value noise for 2D coordinate as float
            float Get(int x, int y);
            // Returns array of "length" of random float values
            float* GetList(int x, unsigned int length);
            // Returns 2D array of dimensions (width, height) of random float values (indexed [y][x])
            float** GetList(int x, int y, unsigned int width, unsigned int height);

            // Get value noise for 1D coordinate as byte
            uint8_t GetByte(int x);
            // Get value noise for 2D coordinate as byte
            uint8_t GetByte(int x, int y);
            // Returns array of "length" of random bytes
            uint8_t* GetByteList(int x, unsigned int length);
            // Returns 2D array of dimensions (width, height) of random bytes (indexed [y][x])
            uint8_t** GetByteList(int x, int y, unsigned int width, unsigned int height);

            White();
            White(unsigned int seed, float amplitude, int wavelength);
            ~White();
        };

        class VIVIUM_API Interpolated : public __NoiseVirtual {
        public:
            using __NoiseVirtual::m_Seed;
            using __NoiseVirtual::amplitude;
            using __NoiseVirtual::wavelength;
            using __NoiseVirtual::SetSeed;

            // Get linearly interpolated noise for 1D coordinate as float
            float GetLinear(int x);
            // Get smoothly interpolated noise for 1D coordinate as float
            float GetSmooth(int x);

            // Gets linealy interpolated noise for 2D coordinate as float
            float Get(int x, int y);
            // Gets fractal noise for 2D coordinate as float
            float GetFractal(int x, int y, int octaves);

            // Get linearly interpolated noise for 1D coordinate as byte
            uint8_t GetByteLinear(int x);
            // Get smoothly interpolated noise for 1D coordinate as byte
            uint8_t GetByteSmooth(int x);

            Interpolated();
            Interpolated(unsigned int seed, float amplitude, int wavelength);
            ~Interpolated();
        };

        class VIVIUM_API Cellular : public __NoiseVirtual {
        public:
            using __NoiseVirtual::m_Seed;
            using __NoiseVirtual::amplitude;
            using __NoiseVirtual::wavelength;
            using __NoiseVirtual::SetSeed;

            float Get(int x, int y);

            Cellular();
            Cellular(unsigned int seed, float amplitude, int wavelength);
            ~Cellular();
        };

        class VIVIUM_API Voronoi : public __NoiseVirtual {
        public:
            using __NoiseVirtual::m_Seed;
            using __NoiseVirtual::amplitude;
            using __NoiseVirtual::wavelength;
            using __NoiseVirtual::SetSeed;

            Vector2<float> Get(int x, int y);

            Voronoi();
            Voronoi(unsigned int seed, float amplitude, int wavelength);
            ~Voronoi();
        };
    }
}