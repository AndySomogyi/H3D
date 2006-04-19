#ifndef ___H3DGLUE_GEOMETRY___
#define ___H3DGLUE_GEOMETRY___
 
 
 
#include "Math.hpp"
#include <vector>		//vector 
#include <float.h>	//FLT_MAX, FLT_EPSILON
#include "H3DTypes.h"
 
 
 
//UTILITY
template<typename C, typename T> bool Contains(const C& container, const T& item)
{
	for (C::const_iterator it = container.begin() ; it != container.end() ; it++)
    {
      if (*it == item) return true;
    }
	return false;
}
 


 
//---CLASS
//	Desc:	Axis Aligned Box
//----------------------->
class Box
{
public:
	Vector3 min;
	Vector3 max;
  bool collided;
  virtual void render( ) {
    glDisable( GL_LIGHTING );
    if( collided )
      glColor3f( 1, 0, 0 );
    else
      glColor3f( 1, 1, 0 );
    glBegin( GL_LINE_STRIP );
    glVertex3f( min.x, min.y, min.z );
    glVertex3f( min.x, max.y, min.z );
    glVertex3f( max.x, max.y, min.z );
    glVertex3f( max.x, min.y, min.z );
    glVertex3f( min.x, min.y, min.z );
    glEnd();

    glBegin( GL_LINE_STRIP );
    glVertex3f( min.x, min.y, max.z );
    glVertex3f( min.x, max.y, max.z );
    glVertex3f( max.x, max.y, max.z );
    glVertex3f( max.x, min.y, max.z );
    glVertex3f( min.x, min.y, max.z );
    glEnd();

    glBegin( GL_LINES );
    glVertex3f( min.x, min.y, max.z );
    glVertex3f( min.x, min.y, min.z );
    glVertex3f( max.x, min.y, max.z );
    glVertex3f( max.x, min.y, min.z );
    glVertex3f( min.x, max.y, max.z );
    glVertex3f( min.x, max.y, min.z );
    glVertex3f( max.x, max.y, max.z );
    glVertex3f( max.x, max.y, min.z );
    glEnd();
    glEnable( GL_LIGHTING );
  }
};
 
inline Vector3 LongestAxis(const Box& box)
{
  Vector3 dims(box.max.x-box.min.x,
               box.max.y-box.min.y,
               box.max.z-box.min.z);

  if (dims.x >= dims.y && dims.x >= dims.z) return Vector3(1,0,0);
  if (dims.y >= dims.x && dims.y >= dims.z) return Vector3(0,1,0);
  else													return Vector3(0,0,1);
}
Box BoundingBox(const Point3* pt,const unsigned int n)
{
	Box box;
	box.min = pt[0];
	box.max = pt[0];

	for(unsigned int i=1 ; i<n ; i++)
    {
      if (pt[i].x < box.min.x) box.min.x = pt[i].x;
      if (pt[i].y < box.min.y) box.min.y = pt[i].y;
      if (pt[i].z < box.min.z) box.min.z = pt[i].z;
		
      if (pt[i].x > box.max.x) box.max.x = pt[i].x;
      if (pt[i].y > box.max.y) box.max.y = pt[i].y;
      if (pt[i].z > box.max.z) box.max.z = pt[i].z;
    }
	return box;
}
 
 
 
class Sphere
{
public:
	Vector3 c;
	float r;
  bool collided;
  GLUquadricObj* gl_quadric;
public:
	Sphere(): gl_quadric( NULL ), collided( false ) {}
	Sphere(const Vector3& c_,float r_):c(c_),r(r_), 
                                     gl_quadric( NULL ), collided( false ) {}
  
  virtual void render( ) {
    if( !gl_quadric ) gl_quadric = gluNewQuadric();
    glDisable( GL_LIGHTING );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glTranslatef( c.x, c.y, c.z );
    if( collided )
      glColor3f( 1, 0, 0 );
    else
      glColor3f( 1, 1, 0 );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    gluSphere( gl_quadric, r, 10, 10 );
    glPopMatrix();
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glEnable( GL_LIGHTING );
  }
};
 
Sphere QuickBoundingSphere(const Point3* pt,const unsigned int n)
{
	Box box = BoundingBox(pt,n);
	Point3 c = 0.5 * (box.min + box.max);
	float r = 0;
	
  //	find largest square distance
	for(unsigned int i=0 ; i<n ; i++)
    {
      float d = Dot(pt[i]-c,pt[i]-c);
      if (d > r) r = d;
    }
	
	return Sphere(c, sqrt(r) );
} 
 
 
 
class IndexedTriangle
{
public:
  IndexedTriangle() {}
  IndexedTriangle( int _a, int _b, int _c ): a( _a ), b( _b ), c( _c ) {}
	int a,b,c;
};
class Triangle
{
public:
	Vector3 a,b,c;
	
public:
	Triangle(){}
	Triangle(const Vector3& a_, const Vector3& b_, const Vector3& c_) : a(a_),b(b_),c(c_) {}
};
 
inline Triangle FetchTriangle(const IndexedTriangle& tri, const Vector3* vertex)
{
	return Triangle(vertex[tri.a],vertex[tri.b],vertex[tri.c]);
}
 
class IndexedTriangleGeometry
{
private:
	std::vector<Vector3> vertex;
	std::vector<IndexedTriangle> triangle;
	std::vector<Vector3> normal;
	std::vector<IndexedTriangle> adjacency;
		
public:
	IndexedTriangleGeometry(){}
	IndexedTriangleGeometry(unsigned int vertexCount, unsigned int triangleCount)
	{
		SetSize(vertexCount,triangleCount);
	}
  IndexedTriangleGeometry( const vector< H3D::Vec3f > &coords, const vector< int > &index ) {
    vertex.resize( coords.size() );
    triangle.resize( index.size() / 3 );
    for( unsigned int i = 0; i < coords.size(); i++ ) {
      vertex[i] = Vector3( coords[i].x, coords[i].y, coords[i].z ); 
    }

    for( unsigned int i = 0; i < index.size(); i+=3 ) {
      triangle[i/3] = IndexedTriangle( index[i], index[i+1], index[i+2] ); 
    }
  } 
	
public:
	void SetSize(unsigned int vertexCount, unsigned int triangleCount)
	{
		vertex.resize(vertexCount);
		triangle.resize(triangleCount);
		
		normal.resize(vertexCount);
		adjacency.resize(triangleCount);
	}
	
public:
	unsigned int IndexCount() const
	{
		return TriangleCount() * 3;
	}
	unsigned int TriangleCount() const
	{
		return (unsigned int) triangle.size();
	}
	unsigned int VertexCount() const
	{
		return (unsigned int) vertex.size();
	}
	Triangle FetchTriangle(unsigned int i) const
	{
		return ::FetchTriangle(triangle[i], VertexPointer() );
	}
	Triangle FetchTriangleNormals(unsigned int i) const
	{
		return ::FetchTriangle(triangle[i], NormalPointer() );
	}
	
  unsigned int AdjacentTriangle(unsigned int i, unsigned int edge) const
	{
		return (&adjacency[i].a)[edge];
	}
	
public:
	int* IndexPointer() const
	{
		return const_cast<int*>(&triangle[0].a);
	}
	
  Point3* VertexPointer() const
	{
		return const_cast<Vector3*>(&vertex[0]);
	}
	
  Vector3* NormalPointer() const
	{
		return const_cast<Vector3*>(&normal[0]);
	}
	int* AdjacencyPointer() const
	{
		return const_cast<int*>(&adjacency[0].a);
	}
};
 
 
 
Vector3 TriangleNormal(const Point3& a, const Point3& b, const Point3& c)
{
	//	Note: triangles are defined clockwise for left handed systems
	return Normalize(Cross(b-a, c-a));
}
float SquareDistancePointSegment(const Point3& p, const Point3& a, const Point3& b)
{
	Vector3 ab = b - a;
	Vector3 ap = p - a;
	Vector3 bp = p - b;
	
	float e = Dot(ap,ab);
	if (e<0) return Dot(ap,ap);
	
	float f = Dot(ab,ab);
	if (e>=f) return Dot(bp,bp);
	
	return Dot(ap,ap) - e*e/f;
}
float DistancePointTrianglePlane(const Point3& p, const Point3& a, const Point3& b, const Point3& c)
{
	return Dot( TriangleNormal(a,b,c) , p-a);
}
Vector3 BarycentricCoords(const Point3& p, const Point3& a, const Point3& b, const Point3& c)
{
	Vector3 v0 = b-a;
	Vector3 v1 = c-a;
	Vector3 v2 = p-a;

	float d00 = Dot(v0,v0);
	float d01 = Dot(v0,v1);
	float d11 = Dot(v1,v1);
	float d20 = Dot(v2,v0);
	float d21 = Dot(v2,v1);
	float den = d00*d11-d01*d01;

	float v = (d11*d20 - d01*d21) / den;
	float w = (d00*d21 - d01*d20) / den;
	float u = 1.0f - v - w;

	return Vector3(u,v,w);
} 
Vector3 OutOfPlaneBarycentricCoordinates(const Point3& p, const Point3& a,const Point3& b, const Point3& c, const Vector3& na, const Vector3& nb, const Vector3& nc, float threshold = FLT_EPSILON,unsigned int maxIterations = 7)
{
	Point3 p0 = a;
	Point3 p1 = b;
	Point3 p2 = c;
	
	float h = 0;
	float lastDist = FLT_MAX;
	
	for(unsigned int i=0 ; i<maxIterations ; i++)
    {
      float dist = Dot( Normalize(Cross(p1-p0,p2-p0)) , p-p0);
		
      if (Abs(dist) < threshold) break;
      if (Abs(dist)>Abs(lastDist)) break;
		
      h+= dist;
      p0 = a + h * na;
      p1 = b + h * nb;
      p2 = c + h * nc;
		
      lastDist = dist;
    }
	
	return BarycentricCoords(p,p0,p1,p2);
}
float AngleBetweenVectors(const Vector3& u,const Vector3& v)
{
	return acos( Clamp( Dot(u,v)/(Length(u)*Length(v)),-1.0f,1.0f) );
}
float AngleOfTriangle(const Vector3& a,const Vector3& b, const Vector3& c)
{
	return AngleBetweenVectors(b-a,c-a);
}
 
 
//---ADJACENCY
#define H3DGLUE_INVALID_INDEX	-1
 
bool PointsAreOrdered(const Point3& a, const Point3& b)
{
	if (a.x != b.x) return b.x > a.x;
	if (a.y != b.y) return b.y > a.y;
	return b.z > a.z;	
}
bool EdgesCoincide(const Point3& Ai, const Point3& Bi, const Point3& Aj, const Point3& Bj)
{
	if ( PointsAreOrdered(Ai,Bi) ^ PointsAreOrdered(Aj,Bj) ) 
		return ( (Ai == Bj) && (Bi == Aj) );	//not in the same order, cross compare
	else
		return ( (Ai == Aj) && (Bi == Bj) );	//same order, direct compare
}
void ComputeTriangleAdjacency(const Point3* vertex, unsigned int vertexCount, const unsigned int* index,const unsigned int indexCount, unsigned int* const adjacency)
{
  //	init to no adj
	for(unsigned int i=0 ; i<indexCount ; i++) adjacency[i] = H3DGLUE_INVALID_INDEX;

  //	for each triangle couple (i,j)...
	for(unsigned int i=0 ; i<indexCount/3 ; i++) for(unsigned int j=i+1 ; j<indexCount/3 ; j++)
    {
      //	...and for each edge couple (e,f) between the two triangles
      for(unsigned int e=0 ; e<3 ; e++) for(unsigned int f=0 ; f<3 ; f++)
        {
          //	fetch edge's vertices
          const Point3& Ai = vertex[ index[i*3+ e     ] ];
          const Point3& Bi = vertex[ index[i*3+(e+1)%3] ];

          const Point3& Aj = vertex[ index[j*3+ f     ] ];
          const Point3& Bj = vertex[ index[j*3+(f+1)%3] ];

          //	if edges coincide in space...
          if ( EdgesCoincide(Ai,Bi,Aj,Bj) == true)
            {
              //	record adjacency
              adjacency[i*3+e] = j;	
              adjacency[j*3+f] = i;
            }
        }
    }
}
void ComputeTriangleNormals(const Point3* vertex, unsigned int vertexCount, const unsigned int* index,const unsigned int indexCount, Vector3* const normal)
{
  //	init
	for(unsigned int i=0 ; i<vertexCount ; i++) normal[i] = Vector3(0,0,0);

  //	for each triangle
	for(unsigned int i=0 ; i<indexCount/3 ; i++)
    {
      Vector3 n = TriangleNormal(vertex[index[i*3+0]],vertex[index[i*3+1]],vertex[index[i*3+2]]);
		
      normal[index[i*3+0]] += AngleOfTriangle(vertex[index[i*3+0]],vertex[index[i*3+1]],vertex[index[i*3+2]]) * n;
      normal[index[i*3+1]] += AngleOfTriangle(vertex[index[i*3+1]],vertex[index[i*3+2]],vertex[index[i*3+0]]) * n;
      normal[index[i*3+2]] += AngleOfTriangle(vertex[index[i*3+2]],vertex[index[i*3+0]],vertex[index[i*3+1]]) * n;
    }
	
	for(unsigned int i=0 ; i<vertexCount ; i++) normal[i] = Normalize(normal[i]);
}
unsigned int FindMaxVertexDegree(const unsigned int* index,const unsigned int indexCount,unsigned int vertexCount)
{
	std::vector< std::vector<unsigned int> > neighbours(vertexCount);
	
  //	for each triangle...
	for(unsigned int i=0 ; i<indexCount/3 ; i++)
    {
      unsigned int a = index[i*3+0];
      unsigned int b = index[i*3+1];
      unsigned int c = index[i*3+2];

      //	put b,c in a
      if (!Contains(neighbours[a],b)) neighbours[a].push_back(b);
      if (!Contains(neighbours[a],c)) neighbours[a].push_back(c);

      //	put a,c in b
      if (!Contains(neighbours[b],a)) neighbours[b].push_back(a);
      if (!Contains(neighbours[b],c)) neighbours[b].push_back(c);

      //	put a,b in c
      if (!Contains(neighbours[c],a)) neighbours[c].push_back(a);
      if (!Contains(neighbours[c],b)) neighbours[c].push_back(b);
    }
	
  //	find max...
	unsigned int maxDegree = (unsigned int) neighbours[0].size();
	for(unsigned int i=1 ; i<vertexCount ; i++)
    {
      unsigned int degree = (unsigned int) neighbours[i].size();
      if (degree > maxDegree) maxDegree = degree;
    }
	return maxDegree;
}
 
 
#endif//___H3DGLUE_GEOMETRY___
