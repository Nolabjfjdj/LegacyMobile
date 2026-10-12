#include "LegacyMatrixAdapter.h"

#include <stdexcept>

namespace ios_renderer {

MatrixMode LegacyMatrixAdapter::decodeMode(int mode) {
    switch (mode) {
        case 0: return MatrixMode::ModelView;
        case 1: return MatrixMode::Projection;
        case 2: return MatrixMode::Texture;
        default: throw std::invalid_argument("Unsupported legacy matrix mode");
    }
}

void LegacyMatrixAdapter::MatrixMode(int mode) {
    stacks_.setMode(decodeMode(mode));
}

void LegacyMatrixAdapter::MatrixSetIdentity() {
    stacks_.loadIdentity();
    matrixDirty_ = true;
}

void LegacyMatrixAdapter::MatrixTranslate(float x, float y, float z) {
    stacks_.translate(x, y, z);
    matrixDirty_ = true;
}

void LegacyMatrixAdapter::MatrixRotate(float angleRadians, float x, float y, float z) {
    stacks_.rotate(angleRadians, x, y, z);
    matrixDirty_ = true;
}

void LegacyMatrixAdapter::MatrixScale(float x, float y, float z) {
    stacks_.scale(x, y, z);
    matrixDirty_ = true;
}

void LegacyMatrixAdapter::MatrixPerspective(float fovyDegrees, float aspect, float zNear, float zFar) {
    stacks_.perspective(fovyDegrees, aspect, zNear, zFar);
    matrixDirty_ = true;
}

void LegacyMatrixAdapter::MatrixOrthogonal(float left, float right, float bottom, float top,
                                           float zNear, float zFar) {
    stacks_.orthographic(left, right, bottom, top, zNear, zFar);
    matrixDirty_ = true;
}

void LegacyMatrixAdapter::MatrixPop() {
    if (stacks_.pop()) matrixDirty_ = true;
}

void LegacyMatrixAdapter::MatrixPush() {
    stacks_.push();
}

void LegacyMatrixAdapter::MatrixMult(float* matrix16) {
    stacks_.multiply(matrix16);
    matrixDirty_ = true;
}

const float* LegacyMatrixAdapter::MatrixGet(int mode) const {
    return stacks_.get(decodeMode(mode)).data();
}

void LegacyMatrixAdapter::Set_matrixDirty() {
    matrixDirty_ = true;
}

bool LegacyMatrixAdapter::matrixDirty() const { return matrixDirty_; }
const MatrixStacks& LegacyMatrixAdapter::stacks() const { return stacks_; }

}
