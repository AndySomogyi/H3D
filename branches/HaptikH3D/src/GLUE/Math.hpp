#ifndef ___H3DGLUE_MATH___
#define ___H3DGLUE_MATH___
 
 
 
#include <math.h>	//abs
#include <stdlib.h>	//random numbers
 
 
class Vector2
{
public:
	float x,y;
	
public:
	Vector2(){}
	Vector2(float x_, float y_):x(x_),y(y_){}
};
 
 
class Vector3
{
public:
	float x,y,z;
	
public:
	Vector3(){}
	Vector3(float x_, float y_, float z_):x(x_),y(y_),z(z_){}
	
public:
	Vector3& operator/=(float f)
	{
		x/= f; y/=f; z/= f;
		return(*this);
	}
};
typedef Vector3 Point3;
 
 
class Matrix3x3
{
public:
	float e[3][3];
	
public:
	Matrix3x3(){}
	Matrix3x3(float e00_,float e01_,float e02_,
				 float e10_,float e11_,float e12_,
				 float e20_,float e21_,float e22_)
	{
		e[0][0] = e00_; e[0][1] = e01_; e[0][2] = e02_;
		e[1][0] = e10_; e[1][1] = e11_; e[1][2] = e12_;
		e[2][0] = e20_; e[2][1] = e21_; e[2][2] = e22_;
	}
	Matrix3x3(const Vector3& r0,const Vector3& r1,const Vector3& r2)
	{
		e[0][0] = r0.x; e[0][1] = r0.y; e[0][2] = r0.z;
		e[1][0] = r1.x; e[1][1] = r1.y; e[1][2] = r1.z;
		e[2][0] = r2.x; e[2][1] = r2.y; e[2][2] = r2.z;
	}


};
 
 
 
Matrix3x3 Identity3x3()	{return Matrix3x3(1,0,0,0,1,0,0,0,1);}
Matrix3x3 Transpose(const Matrix3x3& M)
{
	return Matrix3x3(M.e[0][0],M.e[1][0],M.e[2][0],
						  M.e[0][1],M.e[1][1],M.e[2][1],
						  M.e[0][2],M.e[1][2],M.e[2][2]);
}
 
 
 
//MAT METHOD
inline Vector3 operator*(const Vector3& v, const Matrix3x3& M)
{
	return Vector3(
		v.x*M.e[0][0] + v.y*M.e[1][0] + v.z*M.e[2][0],
		v.x*M.e[0][1] + v.y*M.e[1][1] + v.z*M.e[2][1],
		v.x*M.e[0][2] + v.y*M.e[1][2] + v.z*M.e[2][2]	);
}
inline Vector3 operator*(const Matrix3x3& M, const Vector3& v)
{
	return Vector3(
		M.e[0][0]*v.x + M.e[0][1]*v.y + M.e[0][2]*v.z,
		M.e[1][0]*v.x + M.e[1][1]*v.y + M.e[1][2]*v.z,
		M.e[2][0]*v.x + M.e[2][1]*v.y + M.e[2][2]*v.z	);
}
inline Matrix3x3 operator*(const Matrix3x3& A,const Matrix3x3& B)
{
	return Matrix3x3(
		A.e[0][0]*B.e[0][0] + A.e[0][1]*B.e[1][0] + A.e[0][2]*B.e[2][0],
		A.e[0][0]*B.e[0][1] + A.e[0][1]*B.e[1][1] + A.e[0][2]*B.e[2][1],
		A.e[0][0]*B.e[0][2] + A.e[0][1]*B.e[1][2] + A.e[0][2]*B.e[2][2],

		A.e[1][0]*B.e[0][0] + A.e[1][1]*B.e[1][0] + A.e[1][2]*B.e[2][0],
		A.e[1][0]*B.e[0][1] + A.e[1][1]*B.e[1][1] + A.e[1][2]*B.e[2][1],
		A.e[1][0]*B.e[0][2] + A.e[1][1]*B.e[1][2] + A.e[1][2]*B.e[2][2],

		A.e[2][0]*B.e[0][0] + A.e[2][1]*B.e[1][0] + A.e[2][2]*B.e[2][0],
		A.e[2][0]*B.e[0][1] + A.e[2][1]*B.e[1][1] + A.e[2][2]*B.e[2][1],
		A.e[2][0]*B.e[0][2] + A.e[2][1]*B.e[1][2] + A.e[2][2]*B.e[2][2]	);
}
 
 
 
//VEC METHOD
inline Vector3 operator-(const Vector3& u, const Vector3& v)
{
	return Vector3(u.x-v.x,u.y-v.y,u.z-v.z);
}
inline Vector3 operator+(const Vector3& u, const Vector3& v)
{
	return Vector3(u.x+v.x,u.y+v.y,u.z+v.z);
}
inline Vector3 operator*(const float k, const Vector3& u)
{
	return Vector3(k*u.x,k*u.y,k*u.z);
}
inline Vector3 operator/(const Vector3& u, const float& f)
{
	return Vector3(u.x/f,u.y/f,u.z/f);
}
inline bool operator==(const Vector3& u,const Vector3& v)
{
	return ( (u.x == v.x) && (u.y == v.y) && (u.z == v.z) );
}
inline void operator+=(Vector3& u, const Vector3& v)
{
	u = u+v;
}
inline Vector3 operator-(const Vector3& u)
{
	return Vector3(-u.x,-u.y,-u.z);
}
inline Vector3 operator+(const Vector3& u)
{
	return u;
}
 
 
 
//RANDOMS
inline bool RandomBool()
{
	return rand()%1 == 0;
}
inline float RandomFloat(float min = 0.0f,float max = 1.0f)
{
	float random = rand()/float(RAND_MAX);
	return min + random*(max-min);
}
 
 
 
//GENERIC MATH
template<typename T> void Swap(T& a,T& b)
{
	T temp(a);
	a = b;
	b = temp;
}
template<typename T> T Clamp(const T& value,const T& min, const T& max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}
template<typename T> unsigned int IndexOfMin(const T* array,unsigned int n)
{
	unsigned int min = 0;
	for(unsigned int i=0 ; i<n ; i++) if (array[i] < array[min]) min = i;
	return min;
}
template<typename T> unsigned int IndexOfMax(const T* array,unsigned int n)
{
	unsigned int max = 0;
	for(unsigned int i=0 ; i<n ; i++) if (array[i] > array[max]) max = i;
	return max;
}
template<typename T> const T& Min(const T& a,const T& b)
{
	return a>b ? b : a;
}
template<typename T> const T& Max(const T& a,const T& b)
{
	return a<b ? b : a;
}
template<typename T> const T& Max(const T* p,unsigned int n)
{
	unsigned int max = 0;
	for(unsigned int i=0 ; i<n ; i++) if (p[i] > p[max]) max = i;
	return p[max];
}
template<typename T> T Abs(const T& x)
{
	return abs(x);
} 
template<typename T> T SquareRoot(const T& x)
{
	return sqrt(x);
}
 
 
 
//VEC MATH
inline float Dot(const Vector3& u, const Vector3& v)
{
	return u.x*v.x + u.y*v.y + u.z*v.z;
}
inline Vector3 Cross(const Vector3& u, const Vector3& v)
{
	return Vector3(u.y*v.z-u.z*v.y , -u.x*v.z+u.z*v.x , u.x*v.y-u.y*v.x);
} 
inline float ScalarTriple(const Vector3& u, const Vector3& v, const Vector3& w)
{
	//return Dot(u,Cross(v,w));
	return u.x*(v.y*w.z-v.z*w.y) + u.y*(-v.x*w.z+v.z*w.x) + u.z*(v.x*w.y-v.y*w.x);
}
inline float Length(const Vector3& u)
{
	return sqrt(Dot(u,u));
}
inline Vector3 Normalize(const Vector3& u)
{
	return u / Length(u);
}
inline Vector3 RandomVector3(float min,float max)
{
	return Vector3(RandomFloat(min,max),RandomFloat(min,max),RandomFloat(min,max));
}
 
 
template<typename T> void ComputeSortingArray(const T* array,unsigned int n, unsigned int * index)
{
//	initialize
	for(unsigned int i=0 ; i<n ; i++) index[i] = i;
	
//	do a bubble sort on indices
	for(unsigned int i=0 ; i<n-1 ; i++) for(unsigned int j=i+1 ; j<n ; j++)
	{
		if (array[index[i]] < array[index[j]]) Swap(index[i],index[j]);
	}
}
 
 
 
#endif//___H3DGLUE_MATH___