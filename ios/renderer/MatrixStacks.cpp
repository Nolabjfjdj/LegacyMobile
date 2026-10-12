#include "MatrixStacks.h"

#include <cmath>
#include <stdexcept>

namespace ios_renderer {
namespace {

constexpr float kPi = 3.14159265358979323846f;

Matrix4 makeTranslation(float x, float y, float z) {
    Matrix4 m = MatrixStacks::identity();
    m[12] = x;
    m[13] = y;
    m[14] = z;
    return m;
}

Matrix4 makeScale(float x, float y, float z) {
    Matrix4 m = MatrixStacks::identity();
    m[0] = x;
    m[5] = y;
    m[10] = z;
    return m;
}

}

MatrixStacks::MatrixStacks()
    : mode_(MatrixMode::ModelView),
      modelView_{identity()},
      projection_{identity()},
      texture_{identity()} {}

Matrix4 MatrixStacks::identity() {
    return Matrix4{1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f};
}

Matrix4 MatrixStacks::multiply(const Matrix4& a, const Matrix4& b) {
    Matrix4 result{};
    for (std::size_t column = 0; column < 4; ++column) {
        for (std::size_t row = 0; row < 4; ++row) {
            for (std::size_t k = 0; k < 4; ++k) {
                result[column * 4 + row] += a[k * 4 + row] * b[column * 4 + k];
            }
        }
    }
    return result;
}

void MatrixStacks::setMode(MatrixMode mode) { mode_ = mode; }
MatrixMode MatrixStacks::mode() const { return mode_; }

MatrixStacks::Stack& MatrixStacks::stackFor(MatrixMode mode) {
    switch (mode) {
        case MatrixMode::ModelView: return modelView_;
        case MatrixMode::Projection: return projection_;
        case MatrixMode::Texture: return texture_;
    }
    throw std::invalid_argument("Unknown matrix mode");
}

const MatrixStacks::Stack& MatrixStacks::stackFor(MatrixMode mode) const {
    switch (mode) {
        case MatrixMode::ModelView: return modelView_;
        case MatrixMode::Projection: return projection_;
        case MatrixMode::Texture: return texture_;
    }
    throw std::invalid_argument("Unknown matrix mode");
}

MatrixStacks::Stack& MatrixStacks::activeStack() { return stackFor(mode_); }

void MatrixStacks::loadIdentity() { activeStack().back() = identity(); }

void MatrixStacks::translate(float x, float y, float z) {
    activeStack().back() = multiply(activeStack().back(), makeTranslation(x, y, z));
}

void MatrixStacks::scale(float x, float y, float z) {
    activeStack().back() = multiply(activeStack().back(), makeScale(x, y, z));
}

void MatrixStacks::rotate(float radians, float x, float y, float z) {
    const float length = std::sqrt(x * x + y * y + z * z);
    if (length == 0.0f) return;
    x /= length;
    y /= length;
    z /= length;
    const float c = std::cos(radians);
    const float s = std::sin(radians);
    const float t = 1.0f - c;
    Matrix4 r = identity();
    r[0] = t * x * x + c;
    r[4] = t * x * y - s * z;
    r[8] = t * x * z + s * y;
    r[1] = t * x * y + s * z;
    r[5] = t * y * y + c;
    r[9] = t * y * z - s * x;
    r[2] = t * x * z - s * y;
    r[6] = t * y * z + s * x;
    r[10] = t * z * z + c;
    activeStack().back() = multiply(activeStack().back(), r);
}

void MatrixStacks::perspective(float fovyDegrees, float aspect, float zNear, float zFar) {
    if (!(fovyDegrees > 0.0f && fovyDegrees < 180.0f) || aspect <= 0.0f ||
        zNear <= 0.0f || zFar <= zNear) {
        throw std::invalid_argument("Invalid perspective projection parameters");
    }
    const float f = 1.0f / std::tan(fovyDegrees * kPi / 360.0f);
    Matrix4 p{};
    p[0] = f / aspect;
    p[5] = f;
    p[10] = (zFar + zNear) / (zNear - zFar);
    p[11] = -1.0f;
    p[14] = (2.0f * zFar * zNear) / (zNear - zFar);
    activeStack().back() = multiply(activeStack().back(), p);
}

void MatrixStacks::orthographic(float left, float right, float bottom, float top,
                                float zNear, float zFar) {
    if (left == right || bottom == top || zNear == zFar) {
        throw std::invalid_argument("Invalid orthographic projection parameters");
    }
    Matrix4 o = identity();
    o[0] = 2.0f / (right - left);
    o[5] = 2.0f / (top - bottom);
    o[10] = -2.0f / (zFar - zNear);
    o[12] = -(right + left) / (right - left);
    o[13] = -(top + bottom) / (top - bottom);
    o[14] = -(zFar + zNear) / (zFar - zNear);
    activeStack().back() = multiply(activeStack().back(), o);
}

void MatrixStacks::multiply(const float* matrix16) {
    if (matrix16 == nullptr) throw std::invalid_argument("Matrix pointer must not be null");
    Matrix4 other{};
    for (std::size_t i = 0; i < other.size(); ++i) other[i] = matrix16[i];
    activeStack().back() = multiply(activeStack().back(), other);
}

void MatrixStacks::push() { activeStack().push_back(activeStack().back()); }

bool MatrixStacks::pop() {
    Stack& stack = activeStack();
    if (stack.size() <= 1) return false;
    stack.pop_back();
    return true;
}

const Matrix4& MatrixStacks::current() const { return stackFor(mode_).back(); }
const Matrix4& MatrixStacks::get(MatrixMode mode) const { return stackFor(mode).back(); }
std::size_t MatrixStacks::depth(MatrixMode mode) const { return stackFor(mode).size(); }

}
