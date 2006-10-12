//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004, SenseGraphics AB
//
//    This file is part of H3D API.
//
//    H3D API is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    H3D API is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with H3D API; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    A commercial license is also available. Please contact us at 
//    www.sensegraphics.com for more information.
//
//
/// \file Billboard.cpp
/// \brief CPP file for Billboard, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "FeedbackBufferGeometry.h"

using namespace H3D;

FeedbackBufferGeometry::FeedbackBufferGeometry( X3DGeometryNode *_geometry,
                                                const Matrix4f &_transform,
                                                int _nr_vertices  ):
  nr_vertices( _nr_vertices ) {
  if( nr_vertices == -1 ) nr_vertices = 3000000;
  GLfloat *buffer = new GLfloat[ nr_vertices ];
  glFeedbackBuffer( nr_vertices, GL_3D, buffer );
  
  glRenderMode( GL_FEEDBACK );
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  //glScaled( 1000, 1000, 1000 ); 
  //glScaled( 1e-3, 1e-3, 1e-3 ); 
  

  const Matrix4f &m = _transform;
  GLdouble vt[] = { m[0][0], m[1][0], m[2][0], 0,
                    m[0][1], m[1][1], m[2][1], 0,
                    m[0][2], m[1][2], m[2][2], 0,
                    m[0][3], m[1][3], m[2][3], 1 };
  
  //glMultMatrixd( vt ); 

  GLdouble mm[16] = { 1,0,0,0,
                      0,1,0,0,
                      0,0,1,0,  
                      0,0,-0.15,1 };
  //glLoadMatrixd( mm );

  BoxBound *bound = 
    dynamic_cast< BoxBound * >( _geometry->bound->getValue() );
  

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  if( bound ) {
    glLoadIdentity();
    const Vec3f &center = bound->center->getValue();
    const Vec3f &half_size = bound->size->getValue() / 2;
    Vec3f llf = center - half_size;
    Vec3f urn = center + half_size;
    glOrtho( llf.x - 0.01, urn.x + 0.01, llf.y -0.01, urn.y + 0.01, -urn.z-0.01, -llf.z+0.01 );
    //glOrtho( -0.5, 0.5, -0.5, 0.5, 0, 10 );
  }
  
  glDisable(GL_CULL_FACE);
glDisable(GL_LIGHTING);


  GLdouble mv[16];
  glGetDoublev( GL_MODELVIEW_MATRIX, mv );
  
  GLdouble pm[16];
  glGetDoublev( GL_PROJECTION_MATRIX, pm );
  Matrix4d proj(  pm[0], pm[4], pm[8], pm[12],
                  pm[1], pm[5], pm[9], pm[13],
                  pm[2], pm[6], pm[10], pm[14],
                  pm[3], pm[7], pm[11], pm[15] );
  
  //cerr << proj << endl;
  
  GLint vp[4];
  glGetIntegerv( GL_VIEWPORT, vp );
  
  _geometry->render();
  GLint nr_values = glRenderMode( GL_RENDER );
  GLuint err = glGetError();
  if( err != GL_NO_ERROR ) {
    Console(4) << "OpenGL error in glCallList() Error: \"" << gluErrorString( err ) << endl;
  }
  //cerr << nr_values << endl;
  Vec3f p;
  unsigned int vertex_index = 0;
  
  for( GLint i = 0; i < nr_values; ) {
    switch( (int)buffer[i++] ) {
    case( GL_POINT_TOKEN ): {
      cerr << "Point: ";
      i+= parseVertex( buffer, i, p );
      break;
    }
    case( GL_LINE_TOKEN ): {
      cerr << "Line: "; 
      i+= parseVertex( buffer, i, p );
      i+= parseVertex( buffer, i, p );
      break;
    }
    case( GL_POLYGON_TOKEN ): {
      int nr_vertices =  (int)buffer[i];
      i++;
      Vec3d v0, v1, v2; 
      if( nr_vertices != 3 ) { 
        cerr << "Too Many vertices: " << nr_vertices << endl;
      }
      //cerr << "Polygon: " << nr_vertices << endl;
      i+= parseVertex( buffer, i, p );
      gluUnProject( p.x, p.y, p.z, mv, pm, vp, &v0.x, &v0.y, &v0.z );
      i+= parseVertex( buffer, i, p );
      gluUnProject( p.x, p.y, p.z, mv, pm, vp, &v1.x, &v1.y, &v1.z );
      i+= parseVertex( buffer, i, p );
      gluUnProject( p.x, p.y, p.z, mv, pm, vp, &v2.x, &v2.y, &v2.z );
      triangles.push_back( HAPI::Bounds::Triangle( v0 *1e3, v1*1e3, v2*1e3 )) ;
      //cerr << v0 << " " << v1 << " " << v2 << endl;
      break;
    }
    case( GL_BITMAP_TOKEN ): cerr << "Bitmap: "; break;
    case( GL_DRAW_PIXEL_TOKEN ): cerr << "Draw pixel: "; break;
    case( GL_COPY_PIXEL_TOKEN ): cerr << "Copy pixel: "; break;
    case( GL_PASS_THROUGH_TOKEN ): cerr << "Pass through: "; break;
    default: cerr << "Default" << endl;
    };
  }

  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
  delete buffer;
}
    
int FeedbackBufferGeometry::parseVertex( GLfloat *buffer, int index, Vec3f &p ) {
  p = Vec3f( buffer[index], buffer[index+1], buffer[index+2] );
  return 3;
}


