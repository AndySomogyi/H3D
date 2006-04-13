#ifndef ___H3DGLUE_MATH___
#define ___H3DGLUE_MATH___
 
 
 
#include <math.h>		//abs
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
};
typedef Vector3 Point3;
 
 
 
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
 
 
 
#endif//___H3DGLUE_MATH___