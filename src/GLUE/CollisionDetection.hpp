#ifndef ___H3DGLUE_COLLISION_DETECTION___
#define ___H3DGLUE_COLLISION_DETECTION___
 
 
 
#include "Math.hpp"
#include "Geometry.hpp"
#include <stack>			//stack
#include <crtdbg.h>		//assertions
 
 
 
//BASIC INTERSECTION TESTS
bool IntersectSegmentTriangle(const Point3& p, const Point3& q, const Point3& a, const Point3& b, const Point3& c, Vector3& barycentric, Point3& i)
{
//	Note: triangles are defined clockwise for left handed systems
	Vector3 pq = q - p;
	Vector3 pa = a - p;
	Vector3 pb = b - p;
	Vector3 pc = c - p;

	float u = ScalarTriple(pq,pc,pb);
	if (u < 0) return false;
	float v = ScalarTriple(pq,pa,pc);
	if (v < 0) return false;
	float w = ScalarTriple(pq,pb,pa);
	if (w < 0) return false;

	if(u+v+w == 0) return false;

	barycentric.x = u / (u+v+w);
	barycentric.y = v / (u+v+w);
	barycentric.z = w / (u+v+w);

	i = barycentric.x * a + barycentric.y * b + barycentric.z * c;

//	segment test
	Vector3 pi = i - p;
	float d = Dot(pi,pq)/Dot(pq,pq);
	if (d >= 0 && d <= 1) return true;
	else return false;
}
bool IntersectSegmentSphere(const Point3& p, const Point3& q, const Point3& s, float r)
{
	return SquareDistancePointSegment(s,p,q) <= r*r;
}
bool IntersectSegmentBox(const Point3& p, const Point3& q, const Point3& min, const Point3& max)
{
	Point3 c = 0.5f*(min+max);
	Vector3 e = max-c;
	Point3 m = 0.5f*(p+q);
	Vector3 d = q-m;

	m = m-c;

	float adx = Abs(d.x);
	if (Abs(m.x) > e.x + adx) return false;
	float ady = Abs(d.y);
	if (Abs(m.y) > e.y + ady) return false;
	float adz = Abs(d.z);
	if (Abs(m.z) > e.z + adz) return false;

//	bias with an epsilon to increase robustness when segment is parallel to a box plane...
	float epsilon = Dot(e,e)/100000;
	adx += epsilon;
	ady += epsilon;
	adz += epsilon;

	if (Abs(m.y*d.z - m.z*d.y) > e.y*adz + e.z*ady) return false;
	if (Abs(m.z*d.x - m.x*d.z) > e.x*adz + e.z*adx) return false;
	if (Abs(m.x*d.y - m.y*d.x) > e.x*ady + e.y*adx) return false;

	return true;
}
bool IntersectSegmentOrientedBox(const Point3& p, const Point3& q, const OrientedBox& obox)
{
	Point3 rp = obox.Rotation() * (p-obox.center);
	Point3 rq = obox.Rotation() * (q-obox.center);
	
	return IntersectSegmentBox(rp,rq,-obox.halfsize,+obox.halfsize);
}
 
 
 
//TEMPLATES & SPECIALIZATIONS 
template<typename OUTSHAPE, typename INSHAPE> OUTSHAPE BoundingShape(const INSHAPE* item, unsigned int itemCount)
{
	return OUTSHAPE(item,itemCount);
}
template<> Box BoundingShape(const Triangle* item, unsigned int itemCount)
{
	return BoundingBox( &item[0].a , itemCount*3 );
}
template<> Sphere BoundingShape(const Triangle* item, unsigned int itemCount)
{
	return QuickBoundingSphere( &item[0].a , itemCount*3 );
}
template<> OrientedBox BoundingShape(const Triangle* item, unsigned int itemCount)
{
	return BoundingOrientedBox( &item[0].a , itemCount*3 );
}
template<typename SHAPE> Point3 Representative(const SHAPE& shape)
{
	return (Point3)shape;
}
template<> Point3 Representative(const Triangle& triangle)
{
	return (1.0f/3.0f)*(triangle.a + triangle.b + triangle.c);
}
template<typename SHAPE> Vector3 LongestAxis(const SHAPE& shape)
{
	return Vector3(1,0,0);
}
template<typename SHAPE> bool IntersectSegmentShape(const Point3& p, const Point3& q, const SHAPE& shape)
{
	//no implicit version
}
template<> bool IntersectSegmentShape(const Point3& p, const Point3& q, const Box& box)
{
	return IntersectSegmentBox(p,q,box.min,box.max);
} 
template<> bool IntersectSegmentShape(const Point3& p, const Point3& q, const Sphere& sphere)
{
	return IntersectSegmentSphere(p,q,sphere.c,sphere.r);
} 
template<> bool IntersectSegmentShape(const Point3& p, const Point3& q, const OrientedBox& box)
{
	return IntersectSegmentOrientedBox(p,q,box);
} 
 
 
 
//BOUNDING TREE
template<typename SHAPE> class BoundingShapesTree
{
public:

  virtual void render( int depth, const IndexedTriangleGeometry& mesh ) {
    if( root ) root->render( depth, mesh );
  }
  
	class Node
	{
	public:
		union {
			struct {SHAPE shape;};
			unsigned int data;
		};
		Node* left;
		Node* right;
		
	public:
		Node()
		{
			left = NULL;
			right = NULL;
		}
		~Node()
		{
			if (left) delete left;
			if (right) delete right;
		}

    virtual void render( int depth, const IndexedTriangleGeometry& mesh ) {
      if( !IsLeaf() ) {
        if( depth == 0 ) {
          shape.render();
        } else {
          if( left ) left->render( depth - 1, mesh );
          if( right ) right->render( depth - 1, mesh );
        }
      } else {
        glDisable( GL_LIGHTING );
        glColor3f( 0, 0, 1 );
        Triangle t = mesh.FetchTriangle(data);
        glBegin( GL_LINE_STRIP );
        glVertex3f( t.a.x, t.a.y, t.a.z );
        glVertex3f( t.b.x, t.b.y, t.b.z );
        glVertex3f( t.c.x, t.c.y, t.c.z );
        glVertex3f( t.a.x, t.a.y, t.a.z );
        glEnd();
        glEnable( GL_LIGHTING );
      }
    }
		
	public:
		bool IsLeaf() const
		{
			return ( (left == NULL) && (right == NULL) );
		}
	};
	
public:
	Node* root;

public:
	BoundingShapesTree()
	{
		root = NULL;
	}
	~BoundingShapesTree()
	{
		if (root) delete root;
	}
}; 
 
 
//BOUNDING TREE CONSTRUCTION
template<typename SHAPE> class Subtree
{
public:
	typename BoundingShapesTree<SHAPE>::Node* node;
	std::vector<unsigned int> triangles;
};

template<typename SHAPE> void Build(BoundingShapesTree<SHAPE>& tree, const IndexedTriangleGeometry& mesh)
{
	std::stack< Subtree<SHAPE> > stack;
	
//	build a starting set with all triangles
	std::vector<unsigned int> triangles;
	triangles.resize( mesh.TriangleCount() );
	for(unsigned int i=0 ; i<triangles.size() ; i++) triangles[i] = i;

//	add the starting subtree to stack...
	Subtree<SHAPE> start;
	start.node = tree.root = new BoundingShapesTree<SHAPE>::Node();
	start.triangles = triangles;
	stack.push( start );
	
	
//	BUILD TREE
	while( !stack.empty() )
	{
		std::vector<unsigned int> triangle = stack.top().triangles;
		BoundingShapesTree<SHAPE>::Node* node = stack.top().node;
		stack.pop();
		
		std::vector<Triangle> temp;
		temp.resize( triangle.size() );
		for (unsigned int i=0 ; i<temp.size() ; i++) temp[i] = mesh.FetchTriangle(triangle[i]);
		
	//	build bounding shape
		node->shape = BoundingShape<SHAPE>( &temp[0],(unsigned int)temp.size() );
		
		if (triangle.size() == 1 )
		{
		//	build a leaf
			node->data = triangle[0];
			continue;
		}
		
		
    //	DIVIDE SUBSET AND RECURSE
    //	longest axis
		Vector3 axis = LongestAxis(node->shape);

    //	middle point of current set
		Point3 mid(0,0,0);
    float test = 0;
    for(unsigned int i=0 ; i<triangle.size() ; i++) {
       mid = mid + Representative( temp[i] );
       test = test + temp[i].a.x;
    }
		mid = (1.0f/triangle.size()) * mid;	//divide by N
    
    //	build subsets based on axis/middle point
		std::vector<unsigned int> left;
		std::vector<unsigned int> right;
		for(unsigned int i=0 ; i<triangle.size() ; i++)
		{
			if ( Dot(Representative(temp[i])-mid, axis) < 0 )
				left.push_back(triangle[i]);
			else
				right.push_back(triangle[i]);
		}
		
    //	sanity check... sometimes current subset cannot be divided by longest axis: change axis or just half
		if ( (left.size() == 0) || (right.size() == 0) )
		{
			left.clear();
			right.clear();
			for (unsigned int i=0 ; i<triangle.size()/2 ; i++) left.push_back(triangle[i]);
			for (unsigned int i=triangle.size()/2 ; i<triangle.size() ; i++) right.push_back(triangle[i]);
		}
		
		
		//	do recurse
		if ( left.size() != 0)
		{
			node->left = new BoundingShapesTree<SHAPE>::Node;
			
			Subtree<SHAPE> subtree;
			subtree.node = node->left;
			subtree.triangles = left;
			stack.push( subtree );
		}
		
		if ( right.size() != 0)
		{
			node->right = new BoundingShapesTree<SHAPE>::Node;
			
			Subtree<SHAPE> subtree;
			subtree.node = node->right;
			subtree.triangles = right;
			stack.push( subtree );
		}
	}
} 
 
 
//BOUNDING TREE INTERSECTION
class IntersectionQueryResult
{
public:
	Point3 intersection;
	unsigned int triangleIndex;
	Vector3 barycentric;
	
public:
	IntersectionQueryResult()
	{
	//	guards
		triangleIndex = -1;
		intersection = Point3(FLT_MAX,FLT_MAX,FLT_MAX);
		barycentric = Vector3(0,0,0);
	}
};
template<typename SHAPE> bool Intersect(const Point3& p, const Point3& q, typename BoundingShapesTree<SHAPE>::Node* node, const IndexedTriangleGeometry& mesh, IntersectionQueryResult& result)
{
	if ( node->IsLeaf() )
	{
		Triangle t = mesh.FetchTriangle(node->data);
		Vector3 barycentric;
		Point3 intersection;

		if (IntersectSegmentTriangle(p,q,t.a,t.b,t.c,barycentric,intersection))
		{
			//	eventually update results if nearer or never recorded...
			if ( (result.triangleIndex == -1) || (Dot(q-p,intersection-p) < Dot(q-p,result.intersection-p)) )
			{
				result.intersection = intersection;
				result.barycentric = barycentric;
				result.triangleIndex = node->data;
			}
			return true;
		}
		else return false;
	}
	else 
	{
		if ( IntersectSegmentShape(p,q,node->shape) )
		{
			bool overlap = false;
      node->shape.collided = true;
			
			if (node->left) overlap |= Intersect<SHAPE>(p,q,node->left,mesh,result);
			if (node->right) overlap |= Intersect<SHAPE>(p,q,node->right,mesh,result);
			
			return overlap;
		}
		else return false;
	}
}

template<typename SHAPE> 
void Clear( typename BoundingShapesTree<SHAPE>::Node* node) {
  node->shape.collided = false;
  if( !node->IsLeaf() ) {
    Clear<SHAPE>(node->left);
    Clear<SHAPE>(node->right);
  }
}
 
 
#endif//___H3DGLUE_COLLISION_DETECTION___
