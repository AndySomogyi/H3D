
//////////////////////////////////////////////////////////////////////////////
//    H3D API.   Copyright 2004, Daniel Evestedt, Mark Dixon
//    All Rights Reserved
//
/// \file HLDepthBufferShape.cpp
/// \brief cpp file for HLDepthBufferShape
///
//
//////////////////////////////////////////////////////////////////////////////
#include "H3DApi.h"
#ifdef USE_HAPTICS
#include "HLDepthBufferShape.h"
#include "GL/glew.h"
#include "X3DChildNode.h"

using namespace H3D;
#ifdef HAVE_OPENHAPTICS

void HLDepthBufferShape::hlRender( HAPI::HAPIHapticsDevice *hd,
                                   HLuint hl_shape_id ) {
#ifdef HAVE_OPENHAPTICS
  X3DGeometryNode *geometry = static_cast< X3DGeometryNode *>( userdata );
  HAPI::OpenHapticsRenderer::HLSurface *s = 
    dynamic_cast< HAPI::OpenHapticsRenderer::HLSurface * >( surface );
  if( s ) { //&& closeEnoughToBound( hd->proxyPosition->getValue(),  
    //hd->getPreviousProxyPosition(),
    //(Matrix4f)transform.inverse(), 
    //geometry ) ) {
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
    glLoadMatrixd( vt );
    s->hlRender();

    hlTouchableFace( HL_FRONT_AND_BACK );
    Matrix3d m3 = m.getScaleRotationPart();

    hlTouchableFace( touchable_face );
    if( use_haptic_camera )
      hlEnable( HL_HAPTIC_CAMERA_VIEW );
    else
      hlDisable( HL_HAPTIC_CAMERA_VIEW );

    if( use_adaptive_viewport )
      hlEnable( HL_ADAPTIVE_VIEWPORT );
    else
      hlDisable( HL_ADAPTIVE_VIEWPORT );

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

    bool previous_allow = geometry->allowingCulling();
    geometry->allowCulling( false );
    hlBeginShape( HL_SHAPE_DEPTH_BUFFER, hl_shape_id );
    glClear( GL_DEPTH_BUFFER_BIT );
//    geometry->hlRender( hd, (Matrix4f)transform );
    geometry->displayList->callList();
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
#endif
