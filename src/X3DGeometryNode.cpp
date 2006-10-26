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
/// \file X3DGeometryNode.cpp
/// \brief CPP file for X3DGeometryNode, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "X3DGeometryNode.h"
#include "GlobalSettings.h"

#ifdef USE_HAPTICS
#include "OpenHapticsOptions.h"
#include <HAPIHapticShape.h>
#include "DeviceInfo.h"
#include "FeedbackBufferGeometry.h"
#include <HapticTriangleSet.h>
#include "HLDepthBufferShape.h"
#include "HLFeedbackShape.h"
#endif

using namespace H3D;

H3DNodeDatabase X3DGeometryNode::database( "X3DGeometryNode", 
                                           NULL,
                                           typeid( X3DGeometryNode ) );

namespace X3DGeometryNodeInternals {
  FIELDDB_ELEMENT( X3DGeometryNode, isTouched, OUTPUT_ONLY );
  FIELDDB_ELEMENT( X3DGeometryNode, force, OUTPUT_ONLY );
  FIELDDB_ELEMENT( X3DGeometryNode, contactPoint, OUTPUT_ONLY );
  FIELDDB_ELEMENT( X3DGeometryNode, contactNormal, OUTPUT_ONLY );
  FIELDDB_ELEMENT( X3DGeometryNode, options, INPUT_OUTPUT );
}

X3DGeometryNode::X3DGeometryNode( 
                                 Inst< SFNode>  _metadata,
                                 Inst< SFBound > _bound,
                                 Inst< DisplayList > _displayList,
                                 Inst< MFBool > _isTouched,
                                 Inst< MFVec3f > _force,
                                 Inst< MFVec3f > _contactPoint,
                                 Inst< MFVec3f > _contactNormal,
                                 Inst< SFBoundTree > _boundTree ) :
  X3DChildNode( _metadata ),
  H3DBoundedObject( _bound ),
  H3DDisplayListObject( _displayList ),
  isTouched( _isTouched ),
  force( _force ),
  contactPoint( _contactPoint ),
  contactNormal( _contactNormal ),
  boundTree( _boundTree ),
  options( new MFOptionsNode ),
  use_culling( false ),
  allow_culling( true ),
  cull_face( GL_BACK ) {

  type_name = "X3DGeometryNode";
  
  displayList->setOwner( this );
  bound->setOwner( this );
  boundTree->setName( "boundTree" );
  boundTree->setOwner( this );
  database.initFields( this );

  displayList->route( boundTree );
}


int X3DGeometryNode::getHapticShapeId( unsigned int index ) {

  if( index >= haptic_shape_ids.size() ) {
    for( size_t i = haptic_shape_ids.size(); i <= index; i++ ) {
      int shape_id = HAPI::HAPIHapticShape::genShapeId();
      haptic_shape_ids.push_back( shape_id );
    }
  }
  return haptic_shape_ids[ index ];
}

X3DGeometryNode::~X3DGeometryNode() {/*
  for( ShapeIdMap::iterator entry = hl_shape_ids.begin();
       entry != hl_shape_ids.end();
       entry++ )
  for( vector< HLuint>::iterator i = (*entry).second.begin();
       i != (*entry).second.end();
       i++ ) {
    HLuint hl_shape_id = *i; 
    hlDeleteShapes( hl_shape_id, 1 );
    hlRemoveEventCallback( HL_EVENT_MOTION, 
                           hl_shape_id,
                           HL_CLIENT_THREAD,
                           &motionCallback );
    hlRemoveEventCallback( HL_EVENT_TOUCH, 
                           hl_shape_id,
                           HL_CLIENT_THREAD,
                           &touchCallback );
    hlRemoveEventCallback( HL_EVENT_UNTOUCH, 
                           hl_shape_id,
                           HL_CLIENT_THREAD,
                           &untouchCallback );
  }*/
}

HAPI::HAPIHapticShape *X3DGeometryNode::getOpenGLHapticShape( H3DSurfaceNode *_surface,
                                                    const Matrix4f &_transform,
                                                    HLint _nr_vertices ) {
  int type = -1;
  bool adaptive_viewport = true;
  bool camera_view = true;
  HLenum touchable_face;
  
  if( usingCulling() ) {
      if( getCullFace() == GL_FRONT ) touchable_face = HL_BACK;
      else touchable_face = HL_FRONT;
  } else {
      touchable_face = HL_FRONT_AND_BACK;
  }

  OpenHapticsOptions *openhaptics_options = NULL;

  getOptionNode( openhaptics_options );

  if( !openhaptics_options ) {
    GlobalSettings *default_settings = GlobalSettings::getActive();
    if( default_settings ) {
      default_settings->getOptionNode( openhaptics_options );
    }
  }

  if( openhaptics_options ) {
    const string &shape = openhaptics_options->GLShape->getValue();
    if( shape == "FEEDBACK_BUFFER" ) {
      type = 0;
    } else if( shape == "DEPTH_BUFFER" ) {
      type = 1;
    } else {
      Console(4) << "Warning: Invalid OpenHaptics GLShape type: "
                 << shape 
                 << ". Must be \"FEEDBACK_BUFFER\" or \"DEPTH_BUFFER\" "
                 << "(in \"" << getName() << "\")" << endl;
    }
    
    const string &face = openhaptics_options->touchableFace->getValue();
    if( face == "FRONT" ) touchable_face = HL_FRONT;
    else if( face == "BACK" ) touchable_face = HL_BACK;
    else if( face == "FRONT_AND_BACK" ) touchable_face = HL_FRONT_AND_BACK;
    else if( face == "AS_GRAPHICS" ) {
// default values are the same as graphics
    } else {
      Console(4) << "Warning: Invalid default OpenHaptics touchable face: "
                 << face 
                 << ". Must be \"FRONT\", \"BACK\" or \"FRONT_AND_BACK\" "
                 << "(in active OpenHapticsSettings node\")" << endl;
    }

    adaptive_viewport = openhaptics_options->useAdaptiveViewport->getValue();
    camera_view = openhaptics_options->useHapticCameraView->getValue();
  }

  if( type == 1 ) {
    return new HLDepthBufferShape( this,
                                   _surface,
                                   _transform,
                                   touchable_face,
                                   camera_view,
                                   adaptive_viewport );
  } else {
    return new HLFeedbackShape( this,
                                _surface,
                                _transform,
                                _nr_vertices,
                                touchable_face,
                                camera_view );
  }
}

void X3DGeometryNode::DisplayList::callList( bool build_list ) {
    
  X3DGeometryNode *geom = 
    static_cast< X3DGeometryNode * >( owner );
    
  GLboolean culling_enabled;
  glGetBooleanv( GL_CULL_FACE, &culling_enabled );

  GLint cull_face;
  glGetIntegerv( GL_CULL_FACE_MODE, &cull_face );

  if( geom->usingCulling() && geom->allowingCulling() ) {
    glEnable( GL_CULL_FACE );
  } else {
    glDisable( GL_CULL_FACE );
  }

  glCullFace( geom->getCullFace() );

  BugWorkaroundDisplayList::callList( build_list );

  // restore previous values for culling
  if( culling_enabled ) glEnable( GL_CULL_FACE );
  else glDisable( GL_CULL_FACE );
  
  glCullFace( cull_face );

}


/// The HAPIBoundTree constructs a 
void X3DGeometryNode::SFBoundTree::update() { 
  X3DGeometryNode *geometry = static_cast< X3DGeometryNode * >( getOwner() );
  FeedbackBufferGeometry fbg( geometry, Matrix4f() );
  value = new HAPI::Bounds::AABBTree( fbg.triangles );
}

void X3DGeometryNode::traverseSG( TraverseInfo &ti ) {
  X3DChildNode::traverseSG( ti );
  
  const vector< H3DHapticsDevice * > &devices = ti.getHapticsDevices();

  for( unsigned int i = 0; i < devices.size(); i++ ) {

    H3DHapticsDevice *hd = devices[i];
  
    //displayList->breakCache();

    vector< HAPI::Bounds::Triangle > tris;
    tris.reserve( 200 );
    Vec3f scale = ti.getAccInverseMatrix().getScalePart();
    boundTree->getValue()->getTrianglesWithinRadius( ti.getAccInverseMatrix() * hd->proxyPosition->getValue() * 1e3,
                                                     15 * H3DMax( scale.x, H3DMax( scale.y, scale.z ) ),
                                                     tris );
    
    //if( tris.size() > 0 )
    //cerr << tris.size() << endl;

    int ii = tris.size();    
    if( ti.hapticsEnabled() && ti.getCurrentSurface() && ii > 0 ) {

      //cerr << ii;// << endl;
      HAPI::HapticTriangleSet * tri_set = new HAPI::HapticTriangleSet( tris ,
                                               this,
                                               ti.getCurrentSurface(),
                                               Matrix4f( 1e3, 0, 0, 0,
                                                         0, 1e3, 0, 0,
                                                         0, 0, 1e3, 0,
                                                         0, 0, 0, 1 ) *(ti.getAccForwardMatrix() *
                                               Matrix4f( 1e-3, 0, 0, 0,
                                                         0, 1e-3, 0, 0,
                                                         0, 0, 1e-3, 0,
                                                         0, 0, 0, 1 )));
      
      // if we have an OpenHapticsOptions node, then set the OpenHaptics options in
      /// the HapticTriangleSet we just created.
      OpenHapticsOptions *openhaptics_options = NULL;
      getOptionNode( openhaptics_options );
      if( !openhaptics_options ) {
        GlobalSettings *default_settings = GlobalSettings::getActive();
        if( default_settings ) {
          default_settings->getOptionNode( openhaptics_options );
        }
      }

      if( openhaptics_options ) {
        HAPI::OpenHapticsRenderer::OpenHapticsOptions::ShapeType type;
        bool adaptive_viewport;
        bool camera_view;

        const string &shape = openhaptics_options->GLShape->getValue();
        if( shape == "FEEDBACK_BUFFER" ) {
          type = HAPI::OpenHapticsRenderer::OpenHapticsOptions::FEEDBACK_BUFFER;
        } else if( shape == "DEPTH_BUFFER" ) {
          type = HAPI::OpenHapticsRenderer::OpenHapticsOptions::DEPTH_BUFFER;
        } else {
          Console(4) << "Warning: Invalid OpenHaptics GLShape type: "
                     << shape 
                     << ". Must be \"FEEDBACK_BUFFER\" or \"DEPTH_BUFFER\" "
                     << "(in \"" << getName() << "\")" << endl;
        }
        
        adaptive_viewport = openhaptics_options->useAdaptiveViewport->getValue();
        camera_view = openhaptics_options->useHapticCameraView->getValue();
        tri_set->addRenderOption( 
                new HAPI::OpenHapticsRenderer::OpenHapticsOptions( type,
                                                                   adaptive_viewport,
                                                                   camera_view ) );
      }
      
      ti.addHapticShape( i, tri_set );
    }
  }
}

#if 0
void X3DGeometryNode::TriangleSet::hlRender( H3DHapticsDevice *hd ) {
  return;

  X3DGeometryNode *geometry = static_cast< X3DGeometryNode * >( userdata );
  int type = -1;
  bool adaptive_viewport = true;
  bool camera_view = true;
  HLenum touchable_face;
  
  if( geometry->usingCulling() ) {
    if( geometry->getCullFace() == GL_FRONT ) touchable_face = HL_BACK;
    else touchable_face = HL_FRONT;
  } else {
    touchable_face = HL_FRONT_AND_BACK;
  }

  OpenHapticsOptions *openhaptics_options = NULL;

  geometry->getOptionNode( openhaptics_options );

  if( !openhaptics_options ) {
    GlobalSettings *default_settings = GlobalSettings::getActive();
    if( default_settings ) {
      default_settings->getOptionNode( openhaptics_options );
    }
  }

  if( openhaptics_options ) {
    const string &shape = openhaptics_options->GLShape->getValue();
    if( shape == "FEEDBACK_BUFFER" ) {
      type = 0;
    } else if( shape == "DEPTH_BUFFER" ) {
      type = 1;
    } else {
      Console(4) << "Warning: Invalid OpenHaptics GLShape type: "
                 << shape 
                 << ". Must be \"FEEDBACK_BUFFER\" or \"DEPTH_BUFFER\" "
                 << "(in \"" << getName() << "\")" << endl;
    }
    
    const string &face = openhaptics_options->touchableFace->getValue();
    if( face == "FRONT" ) touchable_face = HL_FRONT;
    else if( face == "BACK" ) touchable_face = HL_BACK;
    else if( face == "FRONT_AND_BACK" ) touchable_face = HL_FRONT_AND_BACK;
    else if( face == "AS_GRAPHICS" ) {
// default values are the same as graphics
    } else {
      Console(4) << "Warning: Invalid default OpenHaptics touchable face: "
                 << face 
                 << ". Must be \"FRONT\", \"BACK\" or \"FRONT_AND_BACK\" "
                 << "(in active OpenHapticsSettings node\")" << endl;
    }

    adaptive_viewport = openhaptics_options->useAdaptiveViewport->getValue();
    camera_view = openhaptics_options->useHapticCameraView->getValue();
  }

#ifdef HAVE_OPENHAPTICS
  HLSurface *s = dynamic_cast< HLSurface * >( surface );
  if( s /*&& closeEnoughToBound( hd->proxyPosition->getValue(), 
                               hd->getPreviousProxyPosition(), 
                               (Matrix4f) transform.inverse(), 
                               geometry ) */) {
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
#if HL_VERSION_MAJOR_NUMBER >= 2
    hlPushAttrib( HL_MATERIAL_BIT | HL_TOUCH_BIT );
#endif
    const Matrix4d &m = transform;
    GLdouble vt[] = { m[0][0], m[1][0], m[2][0], 0,
                     m[0][1], m[1][1], m[2][1], 0,
                     m[0][2], m[1][2], m[2][2], 0,
                     m[0][3], m[1][3], m[2][3], 1 };
    glLoadIdentity();
    glScaled( 1e-3, 1e-3, 1e-3 );
    glMultMatrixd( vt );
    s->hlRender( hd );
    hlTouchableFace( touchable_face );
    if( camera_view )
      hlEnable( HL_HAPTIC_CAMERA_VIEW );
    else
      hlDisable( HL_HAPTIC_CAMERA_VIEW );

    if( adaptive_viewport )
      hlEnable( HL_ADAPTIVE_VIEWPORT );
    else
      hlDisable( HL_ADAPTIVE_VIEWPORT );

    Matrix3d m3 = m.getScaleRotationPart();
    GLint front_face;

    bool negative_scaling = 
      ( ( m3.getRow( 0 ) % m3.getRow( 1 ) ) * m3.getRow(2) ) < 0;
    glGetIntegerv( GL_FRONT_FACE, &front_face );
    // if front_face == GL_CW then the GLWindow we render to is mirrored.
    // front_face has to be flipped each time a negative scaling is applied
    // in order for normals to be correct. 
    if( front_face == GL_CW ) {
      if( !negative_scaling )
        glFrontFace( GL_CCW );
    } else if( negative_scaling )
      glFrontFace( GL_CW );

    hlHinti( HL_SHAPE_FEEDBACK_BUFFER_VERTICES, triangles.size() * 3 );

    bool previous_allow = geometry->allowingCulling();
    geometry->allowCulling( false );

    if( type == 1 ) hlBeginShape( HL_SHAPE_DEPTH_BUFFER, getShapeId( hd ) );
    else hlBeginShape( HL_SHAPE_FEEDBACK_BUFFER, getShapeId( hd ) );
    
    glMatrixMode( GL_MODELVIEW );
    //glScalef( 1e-3f, 1e-3f, 1e-3f );
    glBegin( GL_TRIANGLES );
    for( unsigned int i = 0; i < triangles.size(); i++ ) {
      HAPI::Bounds::Triangle &t = triangles[i];
      glVertex3d( t.a.x, t.a.y, t.a.z );
      glVertex3d( t.b.x, t.b.y, t.b.z );
      glVertex3d( t.c.x, t.c.y, t.c.z );
    }
    glEnd();
    //geometry->hlRender( hd, (Matrix4f)transform );

    hlEndShape();

    geometry->allowCulling( previous_allow );
    
    glFrontFace( front_face );
#if HL_VERSION_MAJOR_NUMBER >= 2
    hlPopAttrib();
#endif
glPopMatrix();

  }
#endif

}
#endif
