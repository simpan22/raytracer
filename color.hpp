#pragma once

struct Color {
    double r;
    double g;
    double b;
};

Color color_clamped(const Color &a) {
    Color c;
    c.r = a.r <= 1.0f ? a.r : 1.0f;
    c.g = a.g <= 1.0f ? a.g : 1.0f;
    c.b = a.b <= 1.0f ? a.b : 1.0f;

    c.r = c.r >= 0.0f ? c.r : 0.0f;
    c.g = c.g >= 0.0f ? c.g : 0.0f;
    c.b = c.b >= 0.0f ? c.b : 0.0f;
    return c;
}

Color operator+(const Color &a, const Color &b) {
    return {a.r + b.r, a.g + b.g, a.b + b.b};
}

Color operator-(const Color &a, const Color &b) {
    return {a.r - b.r, a.g - b.g, a.b - b.b};
}

Color operator/(const Color &a, const double b) {
    return {a.r/b, a.g/b, a.b/b};
}

Color operator*(const Color &a, const double b) {
    return {a.r*b, a.g*b, a.b*b};
}

Color color_linear_interpolate(const Color& a, const Color& b, const double c) {
    return {
        a.r*c + b.r*(1.0f - c),
        a.g*c + b.g*(1.0f - c),
        a.b*c + b.b*(1.0f - c)
    };
}
