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

#ifndef QUATERNIONCAMERA_H_
#define QUATERNIONCAMERA_H_

#include "Geometry.h"
#include "Transform.h"

using namespace gk;

class Quaternion
{
	private:
		Vector _v;
		float _w;
	public:
		Quaternion() { _w = 0; _v = Vector(0, 0, -1); }
		Quaternion(float i, float j, float k, float l) { _v = Vector(i, j ,k); _w = l; }
		Quaternion(Vector n, float l) { _v = n; _w = l; }
		Quaternion(float* t) { _v = Vector(t[0], t[1], t[2]); _w = t[3]; }
		Quaternion(const Quaternion& q) { _v = q.v(); _w = q.w(); }

		Vector v() const { return _v; }
		float w() const { return _w; } 

		float length() const;
		Quaternion normalize() const;
		Quaternion conjugate() const;

		void rotateAround(float angle, Vector axis);

		Quaternion operator* (const Quaternion&) const;
};

class QuaternionCamera
{
	private:
		Vector position;
                Quaternion up;
		Quaternion view;
		
	public:
                QuaternionCamera() { position = Vector(0, 0, 0); up = Quaternion(0, 1, 0, 0); }
		Transform getMatrix();
		void setFromMatrix(Transform& m);

		Vector getPosition() { return position; }
		void setTo( Vector v ) { position = v; }
		
		void moveForward(float mvt);
		void moveBackward(float mvt);
		void rotate(float ax, float ay);
		//void moveWithKeyboard(GLWindow* w, float speed=1.0);
		//void moveWithKeyboardAndMouse(GLWindow* w, float speed=1.0);
		void write(std::ofstream& file);
		void read(std::ifstream& file);
};

#endif