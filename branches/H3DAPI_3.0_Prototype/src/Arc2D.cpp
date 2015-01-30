//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2014, SenseGraphics AB
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
/// \file Arc2D.cpp
/// \brief CPP file for Arc2D, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include <H3D/Arc2D.h>

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase Arc2D::database( "Arc2D", 
	&(newInstance<Arc2D>), 
	typeid( Arc2D ),
	&X3DGeometryNode::database );

namespace Arc2DInternals {
	FIELDDB_ELEMENT( Arc2D, endAngle, INPUT_OUTPUT );
	FIELDDB_ELEMENT( Arc2D, startAngle, INPUT_OUTPUT );
	FIELDDB_ELEMENT( Arc2D, radius, INPUT_OUTPUT );
}


Arc2D::Arc2D( Inst< SFNode      > _metadata,
	Inst< SFBound     > _bound,
	Inst< DisplayList > _displayList,
	Inst< MFBool      > _isTouched,
	Inst< MFVec3f     > _force,
	Inst< MFVec3f     > _contactPoint,
	Inst< MFVec3f     > _contactNormal,
	Inst< SFFloat     > _endAngle,
	Inst< SFFloat     > _startAngle,
	Inst< SFFloat     > _radius ):
X3DGeometryNode( _metadata, _bound, _displayList, _isTouched,
	_force, _contactPoint, _contactNormal ),
	endAngle( _endAngle ),
	startAngle( _startAngle ),
	radius( _radius ),
	dirtyFlag(new Field) {
		type_name = "Arc2D";
		database.initFields( this );

		endAngle->setValue( (H3DFloat) Constants::pi/2 );
		startAngle->setValue( 0.0f );
		radius->setValue( 1.0f );

		radius->route( bound );

		endAngle->route( dirtyFlag );
		startAngle->route( dirtyFlag );
		radius->route( dirtyFlag );
		renderable.renderMode = GL_LINE_STRIP;
		renderable.VBOs.resize(ATTRIBUTE_COUNT);
}

void Arc2D::DisplayList::callList( bool build_list ) {
	//Arc2D *arc = static_cast< Arc2D * >( owner );
	//
	//glPushAttrib( GL_CURRENT_BIT );
	//
	//float v[4];
	//glGetMaterialfv( GL_FRONT, GL_EMISSION, v );
	//glColor3f( v[0], v[1], v[2] );
	//
	//X3DGeometryNode::DisplayList::callList( build_list );
	//
	//glPopAttrib();
}

void Arc2D::render() {
	//// Save the old state of GL_LIGHTING 
	//GLboolean lighting_enabled;
	//glGetBooleanv( GL_LIGHTING, &lighting_enabled );
	//glDisable( GL_LIGHTING );

	//H3DFloat start_angle = startAngle->getValue();
	//H3DFloat end_angle = endAngle->getValue();
	//H3DFloat r = radius->getValue();

	//if( start_angle == end_angle ) {
	//	start_angle = 0.f;
	//	end_angle = (H3DFloat)Constants::pi * 2;
	//}

	//H3DFloat theta, angle_increment;
	//H3DFloat x, y;
	//H3DFloat nr_segments = nrLines();

	//angle_increment = (H3DFloat) Constants::pi*2 / nr_segments;

	///*
	//In this class we'll instead have a traverseSG that has the functionality of callList and render combined. We'll have an 
	//"if(vbo == 0) { build vbo using the code used between glBegin and glEnd }" The VBO will be contained in one of the objects that 
	//*/

	//glBegin( GL_LINE_STRIP );

	//for ( theta = start_angle; 
	//	theta < end_angle;
	//	theta += angle_increment ) {
	//		x = r * H3DCos(theta);
	//		y = r * H3DSin(theta);

	//		glVertex2f (x, y);
	//}

	//theta = end_angle;
	//x = r * H3DCos(theta);
	//y = r * H3DSin(theta);

	//glVertex2f(x, y);

	//glEnd ();

	//// reenable lighting if it was enabled before
	//if( lighting_enabled ) {
	//	// glEnable( GL_LIGHTING );
	//}
}

void H3D::Arc2D::traverseSG(TraverseInfo& ti) {
	//// Save the old state of GL_LIGHTING 
	//GLboolean lighting_enabled;
	//glGetBooleanv( GL_LIGHTING, &lighting_enabled );
	//glDisable( GL_LIGHTING );

	//If any of the fields have changed, we want to rebuild the VBO
	if(!dirtyFlag->isUpToDate()) {		
		buildVBO(ti);

		renderable.transform = ti.getAccForwardMatrix();
		renderable.totalRenderStateHandle = ti.getRenderer()->insertNewTotalRenderState(ti.getCurrentRenderstate());
		ti.getRenderer()->insertNewRenderable(&renderable);

		dirtyFlag->upToDate();
	}

	//// reenable lighting if it was enabled before
	//if( lighting_enabled ) {
	//	// glEnable( GL_LIGHTING );
	//}
}

void H3D::Arc2D::buildVBO(TraverseInfo &ti) {
	//ti.getCurrentMaterial->getDiffuseColor

	Vec3f colorVal(1.0f, 1.0f, 1.0f);

	H3DFloat start_angle = startAngle->getValue();
	H3DFloat end_angle = endAngle->getValue();
	H3DFloat r = radius->getValue();

	if( start_angle == end_angle ) {
		start_angle = 0.f;
		end_angle = (H3DFloat)Constants::pi * 2;
	}

	H3DFloat theta, angle_increment;
	H3DFloat nr_segments = static_cast<float>(nrLines());

	angle_increment = (H3DFloat) Constants::pi*2 / nr_segments;

	std::vector<Vec2f> positions;
	Vec2f position;

	//Calculate individual vertex positions then create and insert them
	for ( theta = start_angle; theta < end_angle;
		theta += angle_increment ) {
			position.x = r * H3DCos(theta);
			position.y = r * H3DSin(theta);

			positions.push_back(position);
	}

	//And one closing vertex
	theta = end_angle;
	position.x = r * H3DCos(theta);
	position.y = r * H3DSin(theta);

	positions.push_back(position);

	//Create as many elements as position and fill with colorVal
	std::vector<Vec3f> colors(positions.size(), colorVal);

	//Build the individual attributes
	renderable.VBOs[COORDINATE].buildBufferObjectSingleAttribute(GL_FLOAT, static_cast<void*>(&positions[0]), positions.size()*sizeof(Vec2f), positions.size(), GL_FALSE, COORDINATE);
	renderable.VBOs[COLOR].buildBufferObjectSingleAttribute(GL_FLOAT, static_cast<void*>(&colors[0]), colors.size()*sizeof(Vec3f), colors.size(), GL_FALSE, COLOR);
}


