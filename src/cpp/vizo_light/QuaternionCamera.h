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