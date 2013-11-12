#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>


namespace dake
{

#ifdef __GNUC__
typedef float raw_v4f __attribute__((vector_size(16)));
#endif

class vec4
{
    private:
#ifdef __GNUC__
        union
        {
            raw_v4f vec;
            float d[4];
        };
#else
        float vec[4];
#endif


    public:
        vec4(void)
#ifdef __GNUC__
        { vec = (raw_v4f){ 0.f, 0.f, 0.f, 0.f }; }
#else
        { vec[0] = vec[1] = vec[2] = vec[3] = 0.f; }
#endif

        vec4(float x, float y, float z, float w)
#ifdef __GNUC__
        { vec = (raw_v4f){ x, y, z, w }; }
#else
        { vec[0] = x; vec[1] = y; vec[2] = z; vec[3] = w; }
#endif

#ifdef __GNUC__
        vec4(raw_v4f dat)
        { vec = dat; }
#endif

        vec4(const vec4 &ov)
#ifdef __GNUC__
        { vec = ov.vec; }
#else
        { vec[0] = ov[0]; vec[1] = ov[1]; vec[2] = ov[2]; vec[3] = ov[3]; }
#endif

        vec4 &operator=(float v)
#ifdef __GNUC__
        { vec = (raw_v4f){ v, v, v, v }; return *this; }
#else
        { vec[0] = vec[1] = vec[2] = vec[3] = v; return *this; }
#endif

        vec4 &operator=(const vec4 &ov)
#ifdef __GNUC__
        { vec = ov.vec; return *this; }
#else
        { vec[0] = ov[0]; vec[1] = ov[1]; vec[2] = ov[2]; vec[3] = ov[3]; return *this; }
#endif

        vec4 operator-(void) const
#ifdef __GNUC__
        { return vec4(-vec); }
#else
        { return vec4(-vec[0], -vec[1], -vec[2], -vec[3]); }
#endif

        vec4 &operator+=(const vec4 &ov)
#ifdef __GNUC__
        { vec += ov.vec; return *this; }
#else
        { vec[0] += ov[0]; vec[1] += ov[1]; vec[2] += ov[2]; vec[3] += ov[3]; return *this; }
#endif

        vec4 operator+(const vec4 &ov) const
#ifdef __GNUC__
        { return vec4(vec + ov.vec); }
#else
        { return vec4(vec[0] + ov[0], vec[1] + ov[1], vec[2] + ov[2], vec[3] + ov[3]); }
#endif

        vec4 &operator-=(const vec4 &ov)
#ifdef __GNUC__
        { vec -= ov.vec; return *this; }
#else
        { vec[0] -= ov[0]; vec[1] -= ov[1]; vec[2] -= ov[2]; vec[3] -= ov[3]; return *this; }
#endif

        vec4 operator-(const vec4 &ov) const
#ifdef __GNUC__
        { return vec4(vec - ov.vec); }
#else
        { return vec4(vec[0] - ov[0], vec[1] - ov[1], vec[2] - ov[2], vec[3] - ov[3]); }
#endif

        vec4 &operator*=(float v)
#ifdef __GNUC__
        { vec *= v; return *this; }
#else
        { vec[0] *= v; vec[1] *= v; vec[2] *= v; vec[3] *= v; return *this; }
#endif

        vec4 operator*(float v) const
#ifdef __GNUC__
        { return vec4(vec * v); }
#else
        { return vec4(vec[0] * v, vec[1] * v, vec[2] * v, vec[3] * v); }
#endif

        vec4 &operator/=(float v)
#ifdef __GNUC__
        { vec /= v; return *this; }
#else
        { vec[0] /= v; vec[1] /= v; vec[2] /= v; vec[3] /= v; return *this; }
#endif

        vec4 operator/(float v) const
#ifdef __GNUC__
        { return vec4(vec / v); }
#else
        { return vec4(vec[0] / v, vec[1] / v, vec[2] / v, vec[3] / v); }
#endif

        float length(void) const
#ifdef __GNUC__
        { raw_v4f tmp = vec * vec; return sqrtf(tmp[0] + tmp[1] + tmp[2] + tmp[3]); }
#else
        { return sqrtf(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]); }
#endif

        vec4 &normalize(void)
        { return *this /= length(); }

        vec4 normalized(void) const
        { return *this / length(); }

        operator const float *(void) const
#ifdef __GNUC__
        { return d; }
#else
        { return vec; }
#endif

        operator float *(void)
#ifdef __GNUC__
        { return d; }
#else
        { return vec; }
#endif

              float &operator[](int i)       { return vec[i]; }
        const float &operator[](int i) const { return vec[i]; }
              float &x(void)                 { return vec[0]; }
        const float &x(void)           const { return vec[0]; }
              float &y(void)                 { return vec[1]; }
        const float &y(void)           const { return vec[1]; }
              float &z(void)                 { return vec[2]; }
        const float &z(void)           const { return vec[2]; }
              float &w(void)                 { return vec[3]; }
        const float &w(void)           const { return vec[3]; }
};


static vec4 operator*(float f, const vec4 &v)
{ return v * f; }


class vec3
{
    private:
        float vec[3];


    public:
        vec3(void)
        { vec[0] = vec[1] = vec[2] = 0.f; }

        vec3(float x, float y, float z)
        { vec[0] = x; vec[1] = y; vec[2] = z; }

        vec3(const vec3 &ov)
        { vec[0] = ov[0]; vec[1] = ov[1]; vec[2] = ov[2]; }

        vec3(const vec4 &ov)
        { vec[0] = ov[0]; vec[1] = ov[1]; vec[2] = ov[2]; }

        vec3 &operator=(float v)
        { vec[0] = vec[1] = vec[2] = v; return *this; }

        vec3 &operator=(const vec3 &ov)
        { vec[0] = ov[0]; vec[1] = ov[1]; vec[2] = ov[2]; return *this; }

        vec3 operator-(void) const
        { return vec3(-vec[0], -vec[1], -vec[2]); }

        vec3 &operator+=(const vec3 &ov)
        { vec[0] += ov[0]; vec[1] += ov[1]; vec[2] += ov[2]; return *this; }

        vec3 operator+(const vec3 &ov) const
        { return vec3(vec[0] + ov[0], vec[1] + ov[1], vec[2] + ov[2]); }

        vec3 &operator-=(const vec3 &ov)
        { vec[0] -= ov[0]; vec[1] -= ov[1]; vec[2] -= ov[2]; return *this; }

        vec3 operator-(const vec3 &ov) const
        { return vec3(vec[0] - ov[0], vec[1] - ov[1], vec[2] - ov[2]); }

        vec3 &operator*=(float v)
        { vec[0] *= v; vec[1] *= v; vec[2] *= v; return *this; }

        vec3 operator*(float v) const
        { return vec3(vec[0] * v, vec[1] * v, vec[2] * v); }

        vec3 &operator/=(float v)
        { vec[0] /= v; vec[1] /= v; vec[2] /= v; return *this; }

        vec3 operator/(float v) const
        { return vec3(vec[0] / v, vec[1] / v, vec[2] / v); }

        float length(void) const
        { return sqrtf(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]); }

        vec3 &normalize(void)
        { return *this /= length(); }

        vec3 normalized(void) const
        { return *this / length(); }

        vec3 operator^(const vec3 &ov) const
        { return vec3(vec[1] * ov[2] - vec[2] * ov[1], vec[2] * ov[0] - vec[0] * ov[2], vec[0] * ov[1] - vec[1] * ov[0]); }

        operator const float *(void) const
        { return vec; }

        operator float *(void)
        { return vec; }

              float &operator[](int i)       { return vec[i]; }
        const float &operator[](int i) const { return vec[i]; }
              float &x(void)                 { return vec[0]; }
        const float &x(void)           const { return vec[0]; }
              float &y(void)                 { return vec[1]; }
        const float &y(void)           const { return vec[1]; }
              float &z(void)                 { return vec[2]; }
        const float &z(void)           const { return vec[2]; }
};


static vec3 operator*(float f, const vec3 &v)
{ return v * f; }


class vec2
{
    private:
        float vec[2];


    public:
        vec2(void)
        { vec[0] = vec[1] = 0.f; }

        vec2(float x, float y)
        { vec[0] = x; vec[1] = y; }

        vec2(const vec2 &ov)
        { vec[0] = ov[0]; vec[1] = ov[1]; }

        vec2(const vec3 &ov)
        { vec[0] = ov[0]; vec[1] = ov[1]; }

        vec2(const vec4 &ov)
        { vec[0] = ov[0]; vec[1] = ov[1]; }

        vec2 &operator=(float v)
        { vec[0] = vec[1] = v; return *this; }

        vec2 &operator=(const vec2 &ov)
        { vec[0] = ov[0]; vec[1] = ov[1]; return *this; }

        vec2 operator-(void) const
        { return vec2(-vec[0], -vec[1]); }

        vec2 &operator+=(const vec2 &ov)
        { vec[0] += ov[0]; vec[1] += ov[1]; return *this; }

        vec2 operator+(const vec3 &ov) const
        { return vec2(vec[0] + ov[0], vec[1] + ov[1]); }

        vec2 &operator-=(const vec2 &ov)
        { vec[0] -= ov[0]; vec[1] -= ov[1]; return *this; }

        vec2 operator-(const vec3 &ov) const
        { return vec2(vec[0] - ov[0], vec[1] - ov[1]); }

        vec2 &operator*=(float v)
        { vec[0] *= v; vec[1] *= v; return *this; }

        vec2 operator*(float v) const
        { return vec2(vec[0] * v, vec[1] * v); }

        vec2 &operator/=(float v)
        { vec[0] /= v; vec[1] /= v; return *this; }

        vec2 operator/(float v) const
        { return vec2(vec[0] / v, vec[1] / v); }

        float length(void) const
        { return sqrtf(vec[0] * vec[0] + vec[1] * vec[1]); }

        vec2 &normalize(void)
        { return *this /= length(); }

        vec2 normalized(void) const
        { return *this / length(); }

        operator const float *(void) const
        { return vec; }

        operator float *(void)
        { return vec; }

              float &operator[](int i)       { return vec[i]; }
        const float &operator[](int i) const { return vec[i]; }
              float &x(void)                 { return vec[0]; }
        const float &x(void)           const { return vec[0]; }
              float &y(void)                 { return vec[1]; }
        const float &y(void)           const { return vec[1]; }
};


static vec2 operator*(float f, const vec2 &v)
{ return v * f; }

}

#endif
