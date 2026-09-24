#include <cmath>

namespace perlin {
    struct vector2 {
        float x;
        float y;
    };

    vector2 random_unit_vec2(int ix, int iy) {
        // No precomputed gradients mean this works for any number of grid coordinates
        const unsigned w = 8 * sizeof(unsigned);
        const unsigned s = w / 2;
        unsigned a = ix, b = iy;
        a *= 3284157443;

        b ^= a << s | a >> w - s;
        b *= 1911520717;

        a ^= b << s | b >> w - s;
        a *= 2048419325;
        float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

        // Create the vector from the angle
        vector2 vec;
        vec.x = std::sin(random);
        vec.y = std::cos(random);

        return vec;
    }

    float dot_grid_gradient(int ix, int iy, float x, float y) {
        // Get gradient from integer coordinates
        vector2 gradient = random_unit_vec2(ix, iy);
        //vector2 gradient(0, 0);

        // Compute the distance vector
        float dx = x - (float)ix;
        float dy = y - (float)iy;

        // Compute the dot-product
        return (dx * gradient.x + dy * gradient.y);
    }

    float interpolate_cubic(float a0, float a1, float w)
    {
        return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
    }

    float perlin_2D(float x, float y) {
        // Determine grid cell corner coordinates
        int x0 = std::floor(x);
        int y0 = std::floor(y);

        int x1 = x0 + 1;
        int y1 = y0 + 1;

        // Compute Interpolation weights
        float sx = x - (float)x0;
        float sy = y - (float)y0;

        // Compute and interpolate top two corners
        float n0 = dot_grid_gradient(x0, y0, x, y);
        float n1 = dot_grid_gradient(x1, y0, x, y);

        float ix0 = interpolate_cubic(n0, n1, sx);

        // Compute and interpolate bottom two corners
        n0 = dot_grid_gradient(x0, y1, x, y);
        n1 = dot_grid_gradient(x1, y1, x, y);

        float ix1 = interpolate_cubic(n0, n1, sx);

        // Final step: interpolate between the two previously interpolated values, now in y
        float value = interpolate_cubic(ix0, ix1, sy);

        return value;
    }

    class PrecomputedPerlin2D {
    private:
        vector2* vec_buffer;

        int size_x;
        int size_y;

        int min_x = 0;
        int min_y = 0;

    public:
        void precompute(const int minimum_x, const int minimum_y) {
            min_x = minimum_x;
            min_y = minimum_y;

            for (int x = 0; x < size_x; x++) {
                for (int y = 0; y < size_y; y++) {
                    int cord_x = minimum_x + x;
                    int cord_y = minimum_y + y;

                    vec_buffer[x + y * size_x] = random_unit_vec2(cord_x, cord_y);
                }
            }
        }

        float get(float x, float y) const {
            int x0 = std::floor(x);
            int y0 = std::floor(y);

            int x1 = x0 + 1;
            int y1 = y0 + 1;

            int column = x0 - min_x;
            int row = y0 - min_y;

            if (column < 0 || column + 1 >= size_x || row < 0 || row + 1 >= size_y) return 0;

            // Weights
            float sx = x - (float)x0;
            float sy = y - (float)y0;

            int gi = column + row * size_x;

            // Get precomputed vectors
            const vector2& vec_n0 = vec_buffer[gi];
            const vector2& vec_n1 = vec_buffer[gi + 1];
            const vector2& vec_n2 = vec_buffer[gi + size_x];
            const vector2& vec_n3 = vec_buffer[gi + 1 + size_x];

            float n0 = ((x - x0) * vec_n0.x + (y - y0) * vec_n0.y);
            float n1 = ((x - x1) * vec_n1.x + (y - y0) * vec_n1.y);

            float ix0 = interpolate_cubic(n0, n1, sx);

            float n2 = ((x - x0) * vec_n2.x + (y - y1) * vec_n2.y);
            float n3 = ((x - x1) * vec_n3.x + (y - y1) * vec_n3.y);

            float ix1 = interpolate_cubic(n2, n3, sx);

            float value = interpolate_cubic(ix0, ix1, sy);

            return value;
        }

        PrecomputedPerlin2D(const int x, const int y) :
            size_x(x),
            size_y(y)
        {
            vec_buffer = new vector2[x * y];
        };

        ~PrecomputedPerlin2D() {
            delete[] vec_buffer;
        };
    };
};