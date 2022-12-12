#include "Noise.h"

namespace Vivium {
    namespace Noise {
        // Chris Wellons 3 round function
        int Hash(unsigned int seed, int x) {
            long u = (long)x + INT_MAX;
            unsigned int v = u;

            v ^= seed;
            v ^= v >> 17;
            v *= 0xed5ad4bbU;
            v ^= v >> 11;
            v *= 0xac4c1b51U;
            v ^= v >> 15;
            v *= 0x31848babU;
            v ^= v >> 14;

            u = v;
            u -= INT_MAX;

            return u;
        }

        // NOTE: cantor pairing
        int HashCombine(int hashed0, int hashed1) {
            return ((hashed0 + hashed1) * (hashed0 + hashed1 + 1) / 2) + hashed1;
        }

        float Hashf(unsigned int seed, int x)
        {
            // Hash an integer
            unsigned int v = Hash(seed, x);

            // Do some bit magic to put the integer's bits into the mantissa of a float (forgot where I ripped this from as well)
            const unsigned int mantissa_bitmask = 0x007FFFFFu;
            const unsigned int one_binary_value = 0x3F800000u;

            v &= mantissa_bitmask;
            v |= one_binary_value;

            float f = (*(float*)&v) - 1.0f;

            return f;
        }

        int Hash(unsigned int seed, Vector2<int> pos)
        {
            return HashCombine(pos.x, pos.y);
        }

        int Hash(unsigned int seed, int x, int y)
        {
            return HashCombine(x, y);
        }

        float Hashf(unsigned int seed, Vector2<int> pos)
        {
            return Hashf(seed, Hash(seed, pos));
        }

        float Hashf(unsigned int seed, int x, int y)
        {
            return Hashf(seed, Hash(seed, x, y));
        }

        Vector2<int> Hash2(unsigned int seed, Vector2<int> pos)
        {
            return Vector2<int>(
                Hash(seed, HashCombine(pos.x, pos.y)),
                Hash(seed, HashCombine(pos.y, pos.x))
            );
        }

        Vector2<float> Hash2f(unsigned int seed, Vector2<int> pos)
        {
            return Vector2<float>(
                Hashf(seed, HashCombine(pos.x, Hash(pos.y, pos.y))),
                Hashf(seed, HashCombine(pos.y, Hash(pos.x, pos.x)))
            );
        }

        float Interpolate(float a, float b, float t)
        {
            return a + (t * (b - a));
        }

        float Cubic(float t)
        {
            return t * t * (3 - (2 * t));
        }

        // Gives the speed of interpolation
        float Quintic(float t)
        {
            return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
        }

        float White::Get(int x)
        {
            return Hashf(m_Seed, x / wavelength) * amplitude;
        }

        float White::Get(int x, int y)
        {
            return Hashf(m_Seed, Vector2<int>(x, y) / wavelength) * amplitude;
        }

        float* White::GetList(int x, unsigned int length)
        {
            float* values = new float[length];

            for (int i = 0; i < length; i++) {
                values[i] = Get(m_Seed, x);
            }

            return values;
        }

        float** White::GetList(int x, int y, unsigned int width, unsigned int height)
        {
            float** values = new float* [height];

            for (int j = 0; j < height; j++) {
                float* row = new float[width];

                for (int i = 0; i < width; x++) {
                    row[i] = Get(m_Seed, i + x);
                }

                values[j] = row;
            }

            return values;
        }

        uint8_t White::GetByte(int x)
        {
            return (Hash(m_Seed, x / wavelength) & 0xff) * amplitude;
        }

        uint8_t White::GetByte(int x, int y)
        {
            return (Hash(m_Seed, Vector2<int>(x, y) / wavelength) & 0xff) * amplitude;
        }

        uint8_t* White::GetByteList(int x, unsigned int length)
        {
            uint8_t* values = new uint8_t[length];

            for (int i = 0; i < length; i++) {
                values[i] = GetByte(m_Seed, i + x);
            }

            return values;
        }

        uint8_t** White::GetByteList(int x, int y, unsigned int width, unsigned int height)
        {
            uint8_t** values = new uint8_t * [height]; // 2D list of unsigned bytes

            for (int j = 0; j < height; j++) {
                uint8_t* row = new uint8_t[width];

                for (int i = 0; i < width; x++) {
                    row[i] = GetByte(m_Seed, i + x);
                }

                values[j] = row;
            }

            return values;
        }

        White::White(unsigned int seed, float amplitude, int wavelength)
            : __NoiseVirtual(seed, amplitude, wavelength)
        {}

        White::~White() {}

        float Interpolated::GetLinear(int x)
        {
            float current = Hashf(m_Seed, x / wavelength);
            float next = Hashf(m_Seed, (x + 1) / wavelength);

            // Set speed based on distance from previous random point to next random point
            float speed = float((x / wavelength) & wavelength) / float(wavelength);

            return Interpolate(current, next, speed);
        }

        float Interpolated::GetSmooth(int x)
        {
            float current = Hashf(m_Seed, x / wavelength);
            float next = Hashf(m_Seed, (x + 1) / wavelength);

            // Set speed based on distance from previous random point to next random point
            float speed = float((x / wavelength) & wavelength) / float(wavelength);

            return Interpolate(current, next, Cubic(speed));
        }

        uint8_t Interpolated::GetByteLinear(int x)
        {
            return GetLinear(x) * 255;
        }

        uint8_t Interpolated::GetByteSmooth(int x)
        {
            return GetSmooth(x) * 255;
        }

        Interpolated::Interpolated()
            : __NoiseVirtual()
        {}

        Interpolated::Interpolated(unsigned int seed, float amplitude, int wavelength)
            : __NoiseVirtual(seed, amplitude, wavelength)
        {}

        Interpolated::~Interpolated() {}

        void __NoiseVirtual::SetSeed(unsigned int nseed)
        {
            m_Seed = nseed;
        }

        __NoiseVirtual::__NoiseVirtual()
            : m_Seed(0), amplitude(1.0f), wavelength(1)
        {}

        __NoiseVirtual::__NoiseVirtual(unsigned int m_seed, float amplitude, int wavelength)
            : m_Seed(m_seed), amplitude(amplitude), wavelength(wavelength)
        {}

        __NoiseVirtual::~__NoiseVirtual() {}

        float Interpolated::Get(int x, int y)
        {
            // Get value relative to wavelength 
            Vector2<float> p = Vector2<float>(x, y) / (float)wavelength;

            // Calculate surrounding points/lattice
            Vector2<float> l0 = p.floor();
            Vector2<float> l1 = p.floor() + Vector2<float>(1.0f, 0.0f);
            Vector2<float> l2 = p.floor() + Vector2<float>(0.0f, 1.0f);
            Vector2<float> l3 = p.floor() + Vector2<float>(1.0f, 1.0f);

            // Calculate gradients for each lattice point
            float r0 = Hashf(m_Seed, (Vector2<int>)l0);
            float r1 = Hashf(m_Seed, (Vector2<int>)l1);
            float r2 = Hashf(m_Seed, (Vector2<int>)l2);
            float r3 = Hashf(m_Seed, (Vector2<int>)l3);

            // Calculate interpolation speeds for x and y
            float tx = Quintic(p.x - l0.x);
            float ty = Quintic(p.y - l0.y);

            // Interpolate for each x slice
            float p0p1 = Interpolate(r0, r1, tx);
            float p2p3 = Interpolate(r2, r3, tx);

            // Interpolate between each x slice
            return Interpolate(p0p1, p2p3, ty) * amplitude;
        }

        float Interpolated::GetFractal(int x, int y, int octaves)
        {
            constexpr float SCALE = 0.5f;

            float original_amp = amplitude;
            int original_wl = wavelength;

            float v = 0.0;
            float total_amplitude = 0.0;

            for (int i = 0; i < octaves; i++) {
                v += Get(x, y);

                total_amplitude += amplitude;
                amplitude *= SCALE;
                wavelength *= SCALE;
            }

            amplitude = original_amp;
            wavelength = original_wl;

            return v / total_amplitude;
        }

        White::White()
            : __NoiseVirtual()
        {}

        float Cellular::Get(int x, int y)
        {
            // https://godotshaders.com/snippet/voronoi/
            // https://www.ronja-tutorials.com/post/028-voronoi-noise/
            // https://thebookofshaders.com/12/

            Vector2<float> p = Vector2<float>(x, y) / (float)wavelength;
            Vector2<float> floor_p = p.floor();

            float min_sqr_dist = FLT_MAX;

            for (float yoff = -1.0f; yoff <= 1.0f; yoff++) {
                for (float xoff = -1.0f; xoff <= 1.0f; xoff++) {
                    Vector2<float> offset(xoff, yoff);
                    Vector2<float> lattice_point = floor_p + offset;
                    Vector2<float> random_point = lattice_point + Hash2f(m_Seed, lattice_point);

                    min_sqr_dist = std::min(min_sqr_dist, p.sqr_distance(random_point));
                }
            }

            float min_dist = std::sqrt(min_sqr_dist);

            return std::min(min_dist, 1.0f);
        }

        Cellular::Cellular()
            : __NoiseVirtual()
        {}

        Cellular::Cellular(unsigned int seed, float amplitude, int wavelength)
            : __NoiseVirtual(seed, amplitude, wavelength)
        {}

        Cellular::~Cellular() {}

        Vector2<float> Voronoi::Get(int x, int y)
        {
            // https://godotshaders.com/snippet/voronoi/
            // https://www.ronja-tutorials.com/post/028-voronoi-noise/
            // https://thebookofshaders.com/12/

            Vector2<float> p = Vector2<float>(x, y) / (float)wavelength;
            Vector2<float> floor_p = p.floor();

            float min_sqr_dist = FLT_MAX;
            Vector2<float> min_point = FLT_MAX;

            for (float yoff = -1.0f; yoff <= 1.0f; yoff++) {
                for (float xoff = -1.0f; xoff <= 1.0f; xoff++) {
                    Vector2<float> offset(xoff, yoff);
                    Vector2<float> lattice_point = floor_p + offset;
                    Vector2<float> random_point = lattice_point + Hash2f(m_Seed, lattice_point);
                    float sqr_distance = p.sqr_distance(random_point);

                    if (sqr_distance < min_sqr_dist) {
                        min_sqr_dist = sqr_distance;
                        min_point = random_point - lattice_point;
                    }
                }
            }

            return min_point;
        }

        Voronoi::Voronoi()
            : __NoiseVirtual()
        {}

        Voronoi::Voronoi(unsigned int seed, float amplitude, int wavelength)
            : __NoiseVirtual(seed, amplitude, wavelength)
        {}

        Voronoi::~Voronoi() {}
    }
}