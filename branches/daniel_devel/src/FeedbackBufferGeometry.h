
//////////////////////////////////////////////////////////////////////////////
//    H3D API.   Copyright 2004, Daniel Evestedt, Mark Dixon
//    All Rights Reserved
//
/// \file FeedbackBufferGeometry.h
/// \brief Header file for FeedbackBufferGeometry.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __FEEDBACKBUFFERGEOMETRY_H__
#define __FEEDBACKBUFFERGEOMETRY_H__

#include "X3DGeometryNode.h"
#include <CollisionObjects.h>

namespace H3D {
  class FeedbackBufferGeometry {
  public:
    FeedbackBufferGeometry( X3DGeometryNode *_geometry,
                            H3DSurfaceNode *_surface,
                            const Matrix4f &_transform,
                            int _nr_vertices = -1 );
    
    int parseVertex( GLfloat *buffer, int index, Vec3f &p );
    
        vector< Bounds::Triangle > triangles;
  protected:
    /// A upper bound on the number of triangles that will be rendered.
    /// Negative values will use the system default value.
    int nr_vertices;


  };
}

#endif
