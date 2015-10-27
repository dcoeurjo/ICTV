#include "QuaternionCamera.h"

#include <cmath>
#include <fstream>

float Quaternion::length() const
{
	return sqrt( _w*_w + _v.x*_v.x + _v.y*_v.y + _v.z*_v.z);
}

Quaternion Quaternion::normalize() const
{
	return Quaternion( _v / length(), _w / length() );
}

Quaternion Quaternion::conjugate() const
{
	return Quaternion( _v * -1, _w );
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
	float ax = _v.x;
	float ay = _v.y;
	float az = _v.z;
	float aw = _w;

	float bx = q.v().x;
	float by = q.v().y;
	float bz = q.v().z;
	float bw = q.w();


	float w2 = aw*bw - ax*bx - ay*by - az*bz;
	Vector v2(
	           aw*bx + ax*bw + ay*bz - az*by,
	           aw*by - ax*bz + ay*bw + az*bx,
	           aw*bz + ax*by - ay*bx + az*bw
	           );

	return Quaternion(v2, w2);
}

void Quaternion::rotateAround(float angle, Vector axis)
{
	Quaternion temp(axis.x * sin(angle/2), axis.y * sin(angle/2), axis.z * sin(angle/2), cos(angle/2));
	Quaternion quat_view (_v, 0);

	Quaternion result = temp.conjugate() * quat_view * temp;
	result = result.normalize();

	_v.x = result.v().x;
	_v.y = result.v().y;
	_v.z = result.v().z;
}

Transform QuaternionCamera::getMatrix()
{
	/*Quaternion v2 = view.normalize();
	
	float x = v2.v().x();
	float y = v2.v().y();
	float z = v2.v().z();
	float w = v2.w();

	Matrix44 rot = Matrix44(
	                1 - y*y - 2*z*z , 2*x*y + 2*w*z , 2*x*z + 2*w*y , 0,
					2*x*y - 2*w*z , 1 - 2*x*x - 2*z*z , 2*y*z + 2*w*x , 0,
					2*x*z + 2*w*y , 2*y*z - 2*w*x , 1 - 2*x*x - 2*y*y ,	0,
					0,   0,   0,   1
	                );
	return rot * Translate(position);*/
    
	Vector aim = view.v();
	Vector f = Normalize(aim);

	Vector s = Cross(f, Normalize(up.v()));
	Vector u = Cross(Normalize(s), f);



	Transform M ( Matrix4x4(
	                s.x, s.y, s.z, 0,
	                u.x, u.y, u.z, 0,
	                -f.x, -f.y, -f.z, 0,
	                0, 0, 0, 1
	                ) );
	
	return Transform(M) * Translate(position * -1);
}

void QuaternionCamera::write(std::ofstream& file)
{
	file << position.x << " " <<  position.y << " " << position.z << std::endl;
	file << view.v().x << " " << view.v().y << " " << view.v().z << " " << view.w() << std::endl;
	file << up.v().x << " " << up.v().y << " " << up.v().z << " " << up.w() << std::endl;
}

void QuaternionCamera::read(std::ifstream& file)
{
	file >> position[0] >> position[1] >> position[2];
	
	float v[4];
	file >> v[0] >> v[1] >> v[2] >> v[3];
	view = Quaternion(v);
	
	file >> v[0] >> v[1] >> v[2] >> v[3];
	up = Quaternion(v);
}



void QuaternionCamera::moveForward(float mvt)
{
	position = position + Normalize(view.v()) * (mvt);
}

void QuaternionCamera::moveBackward(float mvt)
{
	position = position - Normalize(view.v()) * (mvt);
}

void QuaternionCamera::rotate(float ax, float ay)
{
	Vector axis = Normalize( Cross(Normalize(view.v()), Vector(0, 1, 0)) );
	
	view.rotateAround(ax/1000.0, Vector(0, 1, 0));
	view.rotateAround(ay/1000.0, axis);
	
	up.rotateAround(ax/1000.0, Vector(0, 1, 0));
	up.rotateAround(ay/1000.0, axis);
}



/*
void QuaternionCamera::moveWithKeyboard(GLWindow* w, float speed)
{
	if(w->isKeyPressed(GLFW_KEY_UP))
    {
        if (  w->isKeyPressed(GLFW_KEY_LEFT_CONTROL)  )
            position.z( position.z() - speed );
        else
            position.y( position.y() + speed );
    }
    if(w->isKeyPressed(GLFW_KEY_DOWN))
    {
        if (  w->isKeyPressed(GLFW_KEY_LEFT_CONTROL)  )
            position.z( position.z() + speed );
        else
            position.y( position.y() - speed );
    }
    if(w->isKeyPressed(GLFW_KEY_RIGHT))
        position.x( position.x() + speed );
    if(w->isKeyPressed(GLFW_KEY_LEFT))
        position.x( position.x() - speed );
}

void QuaternionCamera::moveWithKeyboardAndMouse(GLWindow* w, float speed)
{	
	Vector3 axis = CrossProduct(view.v().normalize(), Vector3(0, 1, 0)).normalize();
	
	if(w->isKeyPressed(GLFW_KEY_Z))
		position = position + view.v().normalize() * (speed);
	if(w->isKeyPressed(GLFW_KEY_S))
		position = position - view.v().normalize() * (speed);

	if (w->isClikedLeft())
        {
            double x, y;
            w->getMouseMove(&x, &y);
            
            view.rotateAround(x/1000.f, Vector3(0, 1, 0));
            view.rotateAround(y/1000.f, axis);
            
            up.rotateAround(x/1000.f, Vector3(0, 1, 0));
            up.rotateAround(y/1000.f, axis);
        }
}
*/