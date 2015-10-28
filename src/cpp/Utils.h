/*
 * Copyright 2015 
 * Hélène Perrier <helene.perrier@liris.cnrs.fr>
 * Jérémy Levallois <jeremy.levallois@liris.cnrs.fr>
 * David Coeurjolly <david.coeurjolly@liris.cnrs.fr>
 * Jacques-Olivier Lachaud <jacques-olivier.lachaud@univ-savoie.fr>
 * Jean-Philippe Farrugia <jean-philippe.farrugia@liris.cnrs.fr>
 * Jean-Claude Iehl <jean-claude.iehl@liris.cnrs.fr>
 * 
 * This file is part of ICTV.
 * 
 * ICTV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ICTV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ICTV.  If not, see <http://www.gnu.org/licenses/>
 */


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
