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
/// \file Circle2D.cpp
/// \brief CPP file for Circle2D, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include <H3D/Circle2D.h>

#include <H3D/Renderer.h>

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase Circle2D::database( "Circle2D", 
	&(newInstance<Circle2D>), 
	typeid( Circle2D ),
	&X3DGeometryNode::database );

namespace Circle2DInternals {
	FIELDDB_ELEMENT( Circle2D, radius, INPUT_OUTPUT );
}


Circle2D::Circle2D( Inst< SFNode      > _metadata,
	Inst< SFBound     > _bound,
	Inst< DisplayList > _displayList,
	Inst< MFBool      > _isTouched,
	Inst< MFVec3f     > _force,
	Inst< MFVec3f     > _contactPoint,
	Inst< MFVec3f     > _contactNormal,
	Inst< SFFloat     > _radius ):
X3DGeometryNode( _metadata, _bound, _displayList, _isTouched,
	_force, _contactPoint, _contactNormal ),
	radius( _radius ) {

		type_name = "Circle2D";
		database.initFields( this );

		radius->setValue( 1.f );
		radius->route( bound );
		radius->route( dirtyFlag );

		renderable.renderMode = GL_LINE_STRIP;
		renderable.VBOs.resize(ATTRIBUTE_COUNT);
}


void Circle2D::DisplayList::callList( bool build_list ) {
	//Circle2D *circle = 
	//	static_cast< Circle2D * >( owner );
	//
	//glPushAttrib( GL_CURRENT_BIT );
	//float v[4];
	//glGetMaterialfv( GL_FRONT, GL_EMISSION, v );
	//glColor3f( v[0], v[1], v[2] );
	//
	//X3DGeometryNode::DisplayList::callList( build_list );
	//
	//glPopAttrib();
}

void Circle2D::render() {
	// Save the old state of GL_LIGHTING 
	//GLboolean lighting_enabled;
	//glGetBooleanv( GL_LIGHTING, &lighting_enabled );
	//glDisable( GL_LIGHTING );


}

void H3D::Circle2D::traverseSG(TraverseInfo &ti) {
	X3DGeometryNode::traverseSG(ti);

	if(!dirtyFlag->isUpToDate()) {
		buildVBO(ti);

		renderable.transform = ti.getAccForwardMatrix();
		renderable.totalRenderStateHandle = ti.getRenderer()->insertNewTotalRenderState(ti.getCurrentRenderstate());
		ti.getRenderer()->insertNewRenderable(&renderable);

		dirtyFlag->upToDate();
	}
}

void H3D::Circle2D::buildVBO(TraverseInfo &ti)
{
	//This is supposed to be fetched from some form of material struct thing in traverseinfo. For now it's white.
	Vec3f color(1.0f, 1.0f, 1.0f);

	H3DFloat theta, angle_increment;
	H3DFloat nr_segments = static_cast<float>(nrLines());
	angle_increment = (H3DFloat)Constants::pi*2 / nr_segments;
	H3DFloat r = radius->getValue();

	std::vector<Vec2f> positions;

	// draw a circle with lines
	H3DFloat x, y;
	//glBegin( GL_LINE_STRIP );
	int i = 0;
	for ( ; i < nr_segments; ++i ) {
		theta = i * angle_increment;
		x = r * H3DCos(theta);
		y = r * H3DSin(theta);
		positions.push_back(Vec2f (x, y));
	}

	theta = 0;
	x = r * H3DCos(theta);
	y = r * H3DSin(theta);

	positions.push_back(Vec2f (x, y));
	//glEnd ();

	// reenable lighting if it was enabled before
	//if( lighting_enabled ) {
	//	// glEnable( GL_LIGHTING );
	//}

	std::vector<Vec3f> colors(positions.size(), color);

	//Build the individual attributes
	renderable.VBOs[COORDINATE].buildBufferObjectSingleAttribute(GL_FLOAT, &positions[0],	positions.size()*sizeof(Vec2f), positions.size(), GL_FALSE, COORDINATE);
	renderable.VBOs[COLOR].buildBufferObjectSingleAttribute(GL_FLOAT,	&colors[0],	colors.size()*sizeof(Vec3f), colors.size(), GL_FALSE, COLOR);
}

