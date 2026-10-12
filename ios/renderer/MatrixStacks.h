#pragma once

#include <array>
#include <cstddef>
#include <vector>

namespace ios_renderer {

using Matrix4 = std::array<float, 16>;

enum class MatrixMode {
    ModelView,
    Projection,
    Texture
};

class MatrixStacks {
public:
    MatrixStacks();

    void setMode(MatrixMode mode);
    MatrixMode mode() const;
    void loadIdentity();
    void translate(float x, float y, float z);
    void rotate(float radians, float x, float y, float z);
    void scale(float x, float y, float z);
    void perspective(float fovyDegrees, float aspect, float zNear, float zFar);
    void orthographic(float left, float right, float bottom, float top, float zNear, float zFar);
    void multiply(const float* matrix16);
    void push();
    bool pop();
    const Matrix4& current() const;
    const Matrix4& get(MatrixMode mode) const;
    std::size_t depth(MatrixMode mode) const;

    static Matrix4 identity();
    static Matrix4 multiply(const Matrix4& left, const Matrix4& right);

private:
    using Stack = std::vector<Matrix4>;

    Stack& activeStack();
    const Stack& stackFor(MatrixMode mode) const;
    Stack& stackFor(MatrixMode mode);

    MatrixMode mode_;
    Stack modelView_;
    Stack projection_;
    Stack texture_;
};

}
