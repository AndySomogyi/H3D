#ifndef __MATHUTIL_H__
#define __MATHUTIL_H__

#include <H3DUtil/Vec3f.h>
#include <H3DUtil/Matrix4f.h>

namespace MathUtil
{
	using namespace H3DUtil;

	#define M_PI 3.14159265358979323846f
	#define M_PI_2 6.28318530718f

	/* The two functions below are from apitest. They were efficient and easy to understand. */
	/*
	Parameters

	eye 	Specifies the position of the eye point.
	up		Specifies the direction of the up vector.
	lookAt 	Specifies the position of the reference point.
	*/
	H3D::Matrix4f CreateLookAt(Vec3f eye, Vec3f lookAt, Vec3f up)
	{
		Vec3f z_axis = eye - lookAt;
		z_axis.normalize();

		Vec3f x_axis = z_axis.crossProduct(up); 
		x_axis.normalize();

		Vec3f y_axis = z_axis.crossProduct(x_axis);
		y_axis.normalize();

		float x_offset = -x_axis.dotProduct(eye);
		float y_offset = -y_axis.dotProduct(eye);
		float z_offset = -z_axis.dotProduct(eye);
		
		/*
		Matrix4f r
		(
			x_axis.x, x_axis.y, x_axis.z, x_offset,
			y_axis.x, y_axis.y, y_axis.z, y_offset,
			z_axis.x, z_axis.y, z_axis.z, z_offset,
			0.0f,     0.0f,     0.0f,     1.0f
		);
		*/

		Matrix4f r
			(
			x_axis.x, y_axis.x, z_axis.x,	0.0f,
			x_axis.y, y_axis.y, z_axis.y,	0.0f,
			x_axis.z, y_axis.z, z_axis.z,	0.0f,
			x_offset, y_offset, z_offset,   1.0f
			);
		
		return r;
	}

	/* https://unspecified.wordpress.com/2012/06/21/calculating-the-gluperspective-matrix-and-other-opengl-matrix-maths/ */
	Matrix4f CreatePerspectiveProjection(float fovY, float aspectRatio, float zNear, float zFar)
	{
		float height = 1.0f / tan(fovY*2.0f);
		float width = height / aspectRatio;

		Matrix4f m(
			width, 0.0f, 0.0f, 0.0f,
			0.0f, height,	0.0f,	0.0f,
			0.0f, 0.0f, -(zFar + zNear) / (zFar - zNear), -1.0f,
			0.0f, 0.0f, -(2 * zNear * zNear) / (zFar - zNear),	0.0f
		);

		return m;
	}

	float Radians(float degrees)
	{
		return degrees * (M_PI/360.0);
	}
}

#endif