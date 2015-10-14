#ifndef UTILS_H_
#define UTILS_H_

#include "QuaternionCamera.h"

#include "Vec.h"
#include "Transform.h"
#include "Parameters.h"

void rotateLocal_noTranslation(gk::Transform& m, gk::Vector axis, float angle);

void rotateGlobal_noTranslation(gk::Transform& m, gk::Vector axis, float angle);

void normalizePlane(float* plane);

void buildFrustum(float* frustum, gk::Matrix4x4 projection);

void writeMatrix(std::ofstream& file, const gk::Matrix4x4& mat);

void readMatrix(std::ifstream& file, gk::Matrix4x4& mat);

void write_viewPoint();

void load_viewPoint(char* file = 0);

void write_quatPoint(QuaternionCamera& qc);

void load_quatPoint(QuaternionCamera& qc, char* file = 0);

const std::string currentDateTime();

#endif
