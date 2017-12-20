#include "Config.h"
#include <math.h>

#define LOG_TAG "QRM"
#include "Log.h"
#include "ErrorTools.h"

#include "QuarternionRotationMatrix.h"

#undef FILE_NUM
#define FILE_NUM 0xA100

void Quarternion2RotationMatrix(float q1, float q2, float q3, float q4, float* rm) {

#undef FUNC_CODE
#define FUNC_CODE 0x01

    float a00, a01, a02, a03, a11, a12, a13, a22, a23, a33;

    a00 = q1 * q1; a01 = q1 * q2; a02 = q1 * q3; a03 = q1 * q4;
    a11 = q2 * q2; a12 = q2 * q3; a13 = q2 * q4;
    a22 = q3 * q3; a23 = q3 * q4;
    a33 = q4 * q4;

    rm[0] = a33 + a00 - a11 - a22; rm[1] = 2.0f * (a01 - a23); rm[2] = 2.0f * (a02 + a13);
    rm[4] = 2.0f * (a01 + a23); rm[5] = a33 - a00 + a11 - a22; rm[6] = 2.0f * (a12 - a03);
    rm[8] = 2.0f * (a02 - a13); rm[9] = 2.0f * (a12 + a03); rm[10] = a33 - a00 - a11 + a22;
}

void RotationMatrix2Quarternion(float* rm, float*q, float* qbase) {

#undef FUNC_CODE
#define FUNC_CODE 0x02

    float tq[8] = { 0.0f };

    tq[3] = (float)sqrt(rm[0 * 4 + 0] + rm[1 * 4 + 1] + rm[2 * 4 + 2] + 1) / 2.0f;
    tq[7] = -tq[3];

    tq[0] = (rm[2 * 4 + 1] - rm[1 * 4 + 2]) / (4 * tq[3]);
    tq[4] = -tq[0];

    tq[1] = (rm[0 * 4 + 2] - rm[2 * 4 + 0]) / (4 * tq[3]);
    tq[5] = -tq[1];

    tq[2] = (rm[1 * 4 + 0] - rm[0 * 4 + 1]) / (4 * tq[3]);
    tq[6] = -tq[2];

    int ok = 1;
    int offset = 0;

    if (tq[0] * qbase[0] + tq[1] * qbase[1] + tq[2] * qbase[2] + tq[3] * qbase[3] > 0) {
        offset = 0;
        ok = 0;
    }
    if (ok && tq[4] * qbase[0] + tq[5] * qbase[1] + tq[6] * qbase[2] + tq[7] * qbase[3] > 0) {
        offset = 4;
        ok = 0;
    }
    q[0] = tq[offset + 0];     q[1] = tq[offset + 1];     q[2] = tq[offset + 2];     q[3] = tq[offset + 3];
}