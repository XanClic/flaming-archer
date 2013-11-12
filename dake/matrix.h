#ifndef MATRIX_H
#define MATRIX_H

#include <cstring>

#include "vector.h"


namespace dake
{

class mat4
{
    private:
        float d[16];

    public:
        mat4(const vec4 &c1, const vec4 &c2, const vec4 &c3, const vec4 &c4)
        { memcpy(&d[ 0], c1, 4 * sizeof(float)); memcpy(&d[ 4], c2, 4 * sizeof(float)); memcpy(&d[ 8], c3, 4 * sizeof(float)); memcpy(&d[12], c4, 4 * sizeof(float)); }

        mat4(const float *v)
        { memcpy(d, v, sizeof(d)); }

        mat4(const mat4 &m)
        { memcpy(d, m.d, sizeof(d)); }

        mat4(void)
        { memset(d, 0, sizeof(d)); d[0] = d[5] = d[10] = d[15] = 1.f; }


        const vec4 &operator[](int i) const
        { return *reinterpret_cast<const vec4 *>(&d[i * 4]); }

        vec4 &operator[](int i)
        { return *reinterpret_cast<vec4 *>(&d[i * 4]); }


        mat4 &operator=(const mat4 &m)
        { memcpy(d, m.d, sizeof(d)); return *this; }

        mat4 &operator*=(const mat4 &m);

        mat4 operator*(const mat4 &m) const;
        vec4 operator*(const vec4 &v) const;


        mat4 &translate(const vec3 &vec);
        mat4 &rotate(float angle, const vec3 &axis);
        mat4 &scale(const vec3 &fac);

        mat4 translated(const vec3 &vec) const;
        mat4 rotated(float angle, const vec3 &axis) const;
        mat4 scaled(const vec3 &fac) const;

        float det(void);
        void transposed_invert(void);


        static mat4 translation(const vec3 &vec)
        { return mat4(vec4(1.f, 0.f, 0.f, 0.f), vec4(0.f, 1.f, 0.f, 0.f), vec4(0.f, 0.f, 1.f, 0.f), vec4(vec.x(), vec.y(), vec.z(), 1.f)); }


        operator const float *(void) const { return d; }
        operator float *(void) { return d; }
};

}

#endif
