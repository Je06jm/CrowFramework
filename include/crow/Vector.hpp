#ifndef CROW_VECTOR_HPP
#define CROW_VECTOR_HPP

#include <cmath>

namespace crow {

    template <typename T=double>
    struct Vector2 {
        union {
            T x;
            T u;
        };

        union {
            T y;
            T v;
        };

        inline constexpr Vector2() : x{0}, y{0} {}
        inline constexpr Vector2(T x, T y) : x{x}, y{y} {}
        inline constexpr Vector2(const Vector2& v) : x{v.x}, y{v.y} {}
        
        inline constexpr Vector2& operator=(const Vector2& v) {
            x = v.x;
            y = v.y;
            return *this;
        }

        inline constexpr friend Vector2 operator+(const Vector2& lhs, const Vector2& rhs) {
            return {lhs.x + rhs.x, lhs.y + rhs.y};
        }

        inline constexpr Vector2& operator+=(const Vector2& v) {
            x += v.x;
            y += v.y;
            return *this;
        }

        inline constexpr friend Vector2 operator+(const Vector2& lhs, T rhs) {
            return {lhs.x + rhs, lhs.y + rhs};
        }

        inline constexpr Vector2& operator+=(T s) {
            x += s;
            y += s;
            return *this;
        }

        inline constexpr friend Vector2 operator-(const Vector2& lhs, const Vector2& rhs) {
            return {lhs.x - rhs.x, lhs.y - rhs.y};
        }

        inline constexpr Vector2& operator-=(const Vector2& v) {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        inline constexpr friend Vector2 operator-(const Vector2& lhs, T rhs) {
            return {lhs.x - rhs, lhs.y - rhs};
        }

        inline constexpr Vector2& operator-=(T s) {
            x -= s;
            y -= s;
            return *this;
        }

        inline constexpr friend Vector2 operator*(const Vector2& lhs, const Vector2& rhs) {
            return {lhs.x * rhs.x, lhs.y * rhs.y};
        }

        inline constexpr Vector2& operator*=(const Vector2& v) {
            x *= v.x;
            y *= v.y;
            return *this;
        }

        inline constexpr friend Vector2 operator*(const Vector2& lhs, T rhs) {
            return {lhs.x * rhs, lhs.y * rhs};
        }

        inline constexpr Vector2& operator*=(T s) {
            x *= s;
            y *= s;
            return *this;
        }

        inline constexpr friend Vector2 operator/(const Vector2& lhs, const Vector2& rhs) {
            return {lhs.x / rhs.x, lhs.y / rhs.y};
        }

        inline constexpr Vector2& operator/=(const Vector2& v) {
            x /= v.x;
            y /= v.y;
            return *this;
        }

        inline constexpr friend Vector2 operator/(const Vector2& lhs, T rhs) {
            return {lhs.x / rhs, lhs.y / rhs};
        }

        inline constexpr Vector2& operator/=(T s) {
            x /= s;
            y /= s;
            return *this;
        }

        inline constexpr friend Vector2 operator%(const Vector2& lhs, const Vector2& rhs) {
            return {modf(lhs.x, rhs.x), modf(lhs.y, rhs.y)};
        }

        inline constexpr Vector2& operator%=(const Vector2& v) {
            x = modf(x, v.x);
            y = modf(y, v.y);
            return *this;
        }

        inline constexpr friend Vector2 operator%(const Vector2& lhs, T rhs) {
            return {modf(lhs.x, rhs), modf(lhs.y, rhs)};
        }

        inline constexpr Vector2& operator%=(T s) {
            x = modf(x, s);
            y = modf(y, s);
            return *this;
        }

        inline constexpr Vector2& operator-() {
            x = -x;
            y = -y;
            return *this;
        }

        inline constexpr T& operator[](size_t index) {
            T* arr[] = { &x, &y };
            return *arr[index];
        }

        inline constexpr T operator[](size_t index) const {
            T* arr[] = { &x, &y };
            return *arr[index];
        }


        inline constexpr double LengthSquared() const {
            return x * x + y * y;
        }

        inline constexpr double Length() const {
            return sqrt(LengthSquared());
        }

        
        inline constexpr Vector2 Normalize() const {
            return *this / Length();
        }

        
        inline constexpr double Dot(const Vector2& v) const {
            return x * v.x + y * v.y;
        }

        inline constexpr Vector2 DirectionTo(const Vector2& v) const {
            return (v - *this).Normalize();
        }

        inline constexpr double DistanceToSquared(const Vector2& v) const {
            double x = this->x - v.x;
            double y = this->y - v.y;
            return x * x + y * y;
        }

        inline constexpr double DistanceTo(const Vector2& v) const {
            return sqrt(DistanceToSquared(v));
        }

        static constexpr Vector2 UP = {0, 1};
        static constexpr Vector2 RIGHT = {1, 0};
    };

    template <typename T=double>
    struct Vector3 {
        union {
            T x;
            T r;
        };

        union {
            T y;
            T g;
        };

        union {
            T z;
            T b;
        };

        inline constexpr Vector3() : x{0}, y{0}, z{0} {}
        inline constexpr Vector3(T x, T y, T z) : x{x}, y{y}, z{z} {}
        inline constexpr Vector3(const Vector3& v) : x{v.x}, y{v.y}, z{v.z} {}
        
        inline constexpr Vector3& operator=(const Vector3& v) {
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }

        inline constexpr friend Vector3 operator+(const Vector3& lhs, const Vector3& rhs) {
            return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
        }

        inline constexpr Vector3& operator+=(const Vector3& v) {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        inline constexpr friend Vector3 operator+(const Vector3& lhs, T rhs) {
            return {lhs.x + rhs, lhs.y + rhs, lhs.z + rhs};
        }

        inline constexpr Vector3& operator+=(T s) {
            x += s;
            y += s;
            z += s;
            return *this;
        }

        inline constexpr friend Vector3 operator-(const Vector3& lhs, const Vector3& rhs) {
            return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
        }

        inline constexpr Vector3& operator-=(const Vector3& v) {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        inline constexpr friend Vector3 operator-(const Vector3& lhs, T rhs) {
            return {lhs.x - rhs, lhs.y - rhs, lhs.z - rhs};
        }

        inline constexpr Vector3& operator-=(T s) {
            x -= s;
            y -= s;
            z -= s;
            return *this;
        }

        inline constexpr friend Vector3 operator*(const Vector3& lhs, const Vector3& rhs) {
            return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
        }

        inline constexpr Vector3& operator*=(const Vector3& v) {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            return *this;
        }

        inline constexpr friend Vector3 operator*(const Vector3& lhs, T rhs) {
            return {lhs.x * rhs, lhs.y * rh, lhs.z * rhs};
        }

        inline constexpr Vector3& operator*=(T s) {
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        inline constexpr friend Vector3 operator/(const Vector3& lhs, const Vector3& rhs) {
            return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z};
        }

        inline constexpr Vector3& operator/=(const Vector3& v) {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            return *this;
        }

        inline constexpr friend Vector3 operator/(const Vector3& lhs, T rhs) {
            return {lhs.x / rhs, lhs.y / rhs, lhs.z / rhs};
        }

        inline constexpr Vector3& operator/=(T s) {
            x /= s;
            y /= s;
            z /= s;
            return *this;
        }

        inline constexpr friend Vector3 operator%(const Vector3& lhs, const Vector3& rhs) {
            return {modf(lhs.x, rhs.x), modf(lhs.y, rhs.y), modf(lhs.z, rhs.z)};
        }

        inline constexpr Vector3& operator%=(const Vector3& v) {
            x = modf(x, v.x);
            y = modf(y, v.y);
            z = modf(z, v.z);
            return *this;
        }

        inline constexpr friend Vector3 operator%(const Vector3& lhs, T rhs) {
            return {modf(lhs.x, rhs), modf(lhs.y, rhs), modf(lhs.z, rhs)};
        }

        inline constexpr Vector3& operator%=(T s) {
            x = modf(x, s);
            y = modf(y, s);
            z = modf(z, s);
            return *this;
        }

        inline constexpr Vector2& operator-() {
            x = -x;
            y = -y;
            z = -z;
            return *this;
        }

        inline constexpr T& operator[](size_t index) {
            T* arr[] = {&x, &y, &z};
            return *arr[index];
        }

        inline constexpr T operator[](size_t index) const {
            T* arr[] = {&x, &y, &z};
            return *arr[index];
        }


        inline constexpr double LengthSquared() const {
            return x * x + y * y + z * z;
        }

        inline constexpr double Length() const {
            return sqrt(LengthSquared());
        }

        
        inline constexpr Vector3 Normalize() const {
            return *this / Length();
        }

        
        inline constexpr double Dot(const Vector3& v) const {
            return x * v.x + y * v.y + z * v.z;
        }

        inline constexpr Vector3 DirectionTo(const Vector3& v) const {
            return (v - *this).Normalize();
        }

        inline constexpr double DistanceToSquared(const Vector3& v) const {
            double x = this->x - v.x;
            double y = this->y - v.y;
            double z = this->z - v.z;
            return x * x + y * y + z * z;
        }

        inline constexpr double DistanceTo(const Vector3& v) const {
            return sqrt(DistanceToSquared(v));
        }

        inline constexpr Vector3 Cross(const Vector3& v) const {
            return {
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
            }
        }
    };

    template <typename T=double>
    struct Vector4 {
        union {
            T x;
            T r;
        };

        union {
            T y;
            T g;
        };

        union {
            T z;
            T b;
        };

        union {
            T w;
            T a;
        }

        inline constexpr Vector4() : x{0}, y{0}, z{0}, w{0} {}
        inline constexpr Vector4(T x, T y, T z, T w) : x{x}, y{y}, z{z}, w{w} {}
        inline constexpr Vector4(const Vector4& v) : x{v.x}, y{v.y}, z{v.z}, w{w} {}
        
        inline constexpr Vector4& operator=(const Vector4& v) {
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
            return *this;
        }

        inline constexpr friend Vector4 operator+(const Vector4& lhs, const Vector4& rhs) {
            return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w};
        }

        inline constexpr Vector4& operator+=(const Vector4& v) {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }

        inline constexpr friend Vector4 operator+(const Vector4& lhs, T rhs) {
            return {lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs};
        }

        inline constexpr Vector4& operator+=(T s) {
            x += s;
            y += s;
            z += s;
            w += s;
            return *this;
        }

        inline constexpr friend Vector4 operator-(const Vector4& lhs, const Vector4& rhs) {
            return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w};
        }

        inline constexpr Vector4& operator-=(const Vector4& v) {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }

        inline constexpr friend Vector4 operator-(const Vector4& lhs, T rhs) {
            return {lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs};
        }

        inline constexpr Vector4& operator-=(T s) {
            x -= s;
            y -= s;
            z -= s;
            w -= s;
            return *this;
        }

        inline constexpr friend Vector4 operator*(const Vector4& lhs, const Vector4& rhs) {
            return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w};
        }

        inline constexpr Vector4& operator*=(const Vector4& v) {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            w *= v.w;
            return *this;
        }

        inline constexpr friend Vector4 operator*(const Vector4& lhs, T rhs) {
            return {lhs.x * rhs, lhs.y * rh, lhs.z * rhs, lhs.w * rhs};
        }

        inline constexpr Vector4& operator*=(T s) {
            x *= s;
            y *= s;
            z *= s;
            w *= s;
            return *this;
        }

        inline constexpr friend Vector4 operator/(const Vector4& lhs, const Vector4& rhs) {
            return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w};
        }

        inline constexpr Vector4& operator/=(const Vector4& v) {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            w /= v.w;
            return *this;
        }

        inline constexpr friend Vector4 operator/(const Vector4& lhs, T rhs) {
            return {lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs};
        }

        inline constexpr Vector4& operator/=(T s) {
            x /= s;
            y /= s;
            z /= s;
            w /= s;
            return *this;
        }

        inline constexpr friend Vector4 operator%(const Vector4& lhs, const Vector4& rhs) {
            return {modf(lhs.x, rhs.x), modf(lhs.y, rhs.y), modf(lhs.z, rhs.z), modf(lhs.w, rhs.w)};
        }

        inline constexpr Vector4& operator%=(const Vector4& v) {
            x = modf(x, v.x);
            y = modf(y, v.y);
            z = modf(z, v.z);
            w = modf(w, v.w);
            return *this;
        }

        inline constexpr friend Vector4 operator%(const Vector4& lhs, T rhs) {
            return {modf(lhs.x, rhs), modf(lhs.y, rhs), modf(lhs.z, rhs), modf(lhs.w, rhs)};
        }

        inline constexpr Vector4& operator%=(T s) {
            x = modf(x, s);
            y = modf(y, s);
            z = modf(z, s);
            w = modf(w, s);
            return *this;
        }

        inline constexpr Vector4& operator-() {
            x = -x;
            y = -y;
            z = -z;
            w = -w;
            return *this;
        }

        inline constexpr T& operator[](size_t index) {
            T* arr[] = {&x, &y, &z, &w};
            return *arr[index];
        }

        inline constexpr T operator[](size_t index) const {
            T* arr[] = {&x, &y, &z, &w};
            return *arr[index];
        }


        inline constexpr double LengthSquared() const {
            return x * x + y * y + z * z + w * w;
        }

        inline constexpr double Length() const {
            return sqrt(LengthSquared());
        }

        
        inline constexpr Vector4 Normalize() const {
            return *this / Length();
        }

        
        inline constexpr double Dot(const Vector4& v) const {
            return x * v.x + y * v.y + z * v.z + w * v.w;
        }

        inline constexpr Vector4 DirectionTo(const Vector4& v) const {
            return (v - *this).Normalize();
        }

        inline constexpr double DistanceToSquared(const Vector4& v) const {
            double x = this->x - v.x;
            double y = this->y - v.y;
            double z = this->z - v.z;
            double w = this->w - v.w;
            return x * x + y * y + z * z + w * w;
        }

        inline constexpr double DistanceTo(const Vector4& v) const {
            return sqrt(DistanceToSquared(v));
        }
    };

    using Vector2f = Vector2<float>;
    using Vector2i = Vector2<int>;

    using Vector3f = Vector3<float>;
    using Vector3i = Vector3<int>;

    using Vector4f = Vector4<float>;
    using Vector4i = Vector4<int>;

}

#endif