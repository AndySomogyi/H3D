#ifndef __MATHUTIL_H__
#define __MATHUTIL_H__

#include <H3DUtil/Vec3f.h>
#include <H3DUtil/Matrix4f.h>

namespace MathUtil
{
	using namespace H3DUtil;

	/* http://msdn.microsoft.com/en-us/library/windows/desktop/bb281711%28v=vs.85%29.aspx
	zaxis = normal(cameraPosition - cameraTarget)
	xaxis = normal(cross(cameraUpVector, zaxis))
	yaxis = cross(zaxis, xaxis)

	*/
	//Create a row-major lookat(world-to-view) matrix.
	H3D::Matrix4f CreateLookAt(Vec3f eye, Vec3f lookAt, Vec3f up) {
		Vec3f zAxis = eye - lookAt;
		zAxis.normalizeSafe();

		up.normalizeSafe();
		Vec3f xAxis = up.crossProduct(zAxis);
		xAxis.normalizeSafe();

		Vec3f yAxis = zAxis.crossProduct(xAxis);
		yAxis.normalizeSafe();

		Matrix4f r
			(
			xAxis.x,		xAxis.y,		xAxis.z,		-xAxis.dotProduct(eye),
			yAxis.x,		yAxis.y,		yAxis.z,		-yAxis.dotProduct(eye),
			zAxis.x,		zAxis.y,		zAxis.z,		-zAxis.dotProduct(eye),
			0.0f,			0.0f,			0.0f,			1.0f
			);

		return r;
	}

	//http://stackoverflow.com/questions/3498581/in-opengl-what-is-the-simplest-way-to-create-a-perspective-view-using-only-open
	/* 
	//NOTE: THIS IS GLSL CODE. GLSL uses column major, H3D uses row major. We'll transpose when sending this into the shader later.
	mat4 CreatePerspectiveMatrix(in float fov, in float aspect, in float near, in float far)
	{
	mat4 m = mat4(0.0);

	float angle = (fov / 180.0f) * PI;
	float f = 1.0f / tan( angle * 0.5f );

	//Note, matrices are accessed like 2D arrays in C.
	//They are column major, i.e m[y][x]

	m[0][0] = f / aspect;
	m[1][1] = f;
	m[2][2] = (far + near) / (near - far);
	m[2][3] = -1.0f;
	m[3][2] = (2.0f * far*near) / (near - far);

	return m;
	}
	*/

	/*
	Create row-major perspective projection matrix
	*/
	Matrix4f CreatePerspectiveProjection(float fovY, float aspectRatio, float zNear, float zFar)
	{
		float angle = (fovY/180.0f) * static_cast<float>(H3D::Constants::pi); //Unnecessary cast, but removes compile warning.
		float f = 1.0f / tan(angle*0.5f);

		float nearMinusFar = (zNear-zFar);
		float nearPlusFar = (zFar+zNear);
		float twoNearTimesFar = (2.0f * (zFar*zNear));

		Matrix4f m(
			f/aspectRatio,	0.0f,	0.0f,							0.0f,
			0.0f,			f,		0.0f,							0.0f,
			0.0f,			0.0f,	nearPlusFar/nearMinusFar,		twoNearTimesFar/nearMinusFar,
			0.0f,			0.0f,	-1.0f,	0.0f
			);

		return m;
	}

	float DegreesToRadians(float degrees) {
		return degrees * static_cast<float>(H3D::Constants::pi/180.0); //Unnecessary cast, but removes compile warning.
	}

	float RadiansToDegrees(float radians) {
		return radians * static_cast<float>(180.0/H3D::Constants::pi); //Unnecessary cast, but removes compile warning.
	}
}

#endif