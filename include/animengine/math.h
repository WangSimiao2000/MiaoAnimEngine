#pragma once

namespace animengine {

struct Vec3 {
    float x{}, y{}, z{};

    friend Vec3 operator+(Vec3 a, Vec3 b) {
        return {a.x + b.x, a.y + b.y, a.z + b.z};
    }

    friend Vec3 operator-(Vec3 a, Vec3 b) {
        return {a.x - b.x, a.y - b.y, a.z - b.z};
    }

    friend Vec3 operator*(Vec3 a, float s) {
        return {a.x * s, a.y * s, a.z * s};
    }
};

struct Color {
    float r{}, g{}, b{}, a{1.0f};

    friend Color operator+(Color x, Color y) {
        return {x.r + y.r, x.g + y.g, x.b + y.b, x.a + y.a};
    }

    friend Color operator-(Color x, Color y) {
        return {x.r - y.r, x.g - y.g, x.b - y.b, x.a - y.a};
    }

    friend Color operator*(Color x, float s) {
        return {x.r * s, x.g * s, x.b * s, x.a * s};
    }
};

struct Quat {
    float x{}, y{}, z{}, w{1.0f};
};

[[nodiscard]] Quat lerp(const Quat& a, Quat b, float t);  // slerp (b is by value: it may be negated for the shortest arc)

}  // namespace animengine