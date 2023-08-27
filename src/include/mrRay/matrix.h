#ifndef MR_RAY_MATRIX_H
#define MR_RAY_MATRIX_H

#include "mrRay/namespace.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

class Mat4
{
public:
    Mat4(
        double m00, double m01, double m02, double m03, double m10, double m11,
        double m12, double m13, double m20, double m21, double m22, double m23,
        double m30, double m31, double m32, double m33)
    {
        _data[0] = m00;
        _data[1] = m01;
        _data[2] = m02;
        _data[3] = m03;
        _data[4] = m10;
        _data[5] = m11;
        _data[6] = m12;
        _data[7] = m13;
        _data[8] = m20;
        _data[9] = m21;
        _data[10] = m22;
        _data[11] = m23;
        _data[12] = m30;
        _data[13] = m31;
        _data[14] = m32;
        _data[15] = m33;
    }

    double operator[](int index) const { return _data[index]; }

private:
    double _data[16];
};

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif // MR_RAY_MATRIX_H
