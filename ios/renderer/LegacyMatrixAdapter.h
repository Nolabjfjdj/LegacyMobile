#pragma once

#include "MatrixStacks.h"

namespace ios_renderer {

class LegacyMatrixAdapter {
public:
    void MatrixMode(int mode);
    void MatrixSetIdentity();
    void MatrixTranslate(float x, float y, float z);
    void MatrixRotate(float angleRadians, float x, float y, float z);
    void MatrixScale(float x, float y, float z);
    void MatrixPerspective(float fovyDegrees, float aspect, float zNear, float zFar);
    void MatrixOrthogonal(float left, float right, float bottom, float top, float zNear, float zFar);
    void MatrixPop();
    void MatrixPush();
    void MatrixMult(float* matrix16);
    const float* MatrixGet(int mode) const;
    void Set_matrixDirty();

    bool matrixDirty() const;
    const MatrixStacks& stacks() const;

private:
    static ios_renderer::MatrixMode decodeMode(int mode);

    MatrixStacks stacks_;
    bool matrixDirty_ = true;
};

}
