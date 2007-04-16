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
/// \file VisabilitySensor.cpp
/// \brief CPP file for VisabilitySensor, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "VisibilitySensor.h"

using namespace H3D;

H3DNodeDatabase VisibilitySensor::database( 
        "VisibilitySensor", 
        &(newInstance<VisibilitySensor>), 
        typeid( VisibilitySensor ),
        &X3DEnvironmentalSensorNode::database 
        );


 
VisibilitySensor::VisibilitySensor( Inst< SFNode > _metadata ,
								Inst< SFVec3f > _center ,
								Inst< SFVec3f > _size ,
								Inst< SFTime > _enterTime ,
								Inst< SFTime > _exitTime ,
								Inst< SFBool > _enabled ,
								Inst< SFBool > _isActive ) :
								X3DEnvironmentalSensorNode( _metadata, 
                                            _center, 
                                            _enabled, 
                                            _size,
                                            _enterTime,
                                            _exitTime,
                                            _isActive ),
								setTime( new SetTime ){

  type_name = "VisibilitySensor";
  database.initFields( this );

  setTime->setOwner( this );

  isActive->route( setTime );
  
}
void VisibilitySensor::traverseSG( TraverseInfo &ti ) {
 
	if( enabled->getValue() )
	{
		
		X3DViewpointNode *vp = X3DViewpointNode::getActive();
		
		Vec3f loc_pos = vp->getFullPos();
		Rotation loc_orn = vp->getFullOrn();

		const Matrix4f &vp_forw = vp->accForwardMatrix->getValue();
		Vec3f vp_glob_pos =  vp_forw * loc_pos;
		
		Rotation vp_rot = 
		(Rotation)vp->accForwardMatrix->getValue().getRotationPart();

		Rotation vp_glob_rot = vp_rot * loc_orn;

			
		Vec3f size_diff = (size->getValue() - prev_size);
		Vec3f cent_diff = (center->getValue() - prev_center);
		Vec3f vp_pos_diff = (vp_glob_pos - prev_vp_glob_pos);
		
		Rotation orn_diff = -vp_glob_rot * prev_vp_glob_rot;


		if( size_diff.length()   >  Constants::f_epsilon   ||
			cent_diff.length()   >  Constants::f_epsilon   ||
			vp_pos_diff.length() >    Constants::f_epsilon ||
			orn_diff.angle       > Constants::f_epsilon )

		{

			
			const Matrix4f &vs_frw_m = ti.getAccForwardMatrix();

			H3DFloat xmin = center->getValue().x - size->getValue().x / 2.0;
			H3DFloat xmax = center->getValue().x + size->getValue().x / 2.0;
			H3DFloat ymin = center->getValue().y - size->getValue().y / 2.0;
			H3DFloat ymax = center->getValue().y + size->getValue().y / 2.0;
			H3DFloat zmin = center->getValue().z - size->getValue().z / 2.0;
			H3DFloat zmax = center->getValue().z + size->getValue().z / 2.0;
			
			Vec3f loc0(xmin, ymin, zmax);
			Vec3f loc1(xmax, ymin, zmax);
			Vec3f loc2(xmax, ymax, zmax);
			Vec3f loc3(xmin, ymax, zmax);
			Vec3f loc4(xmin, ymin, zmin);
			Vec3f loc5(xmax, ymin, zmin);
			Vec3f loc6(xmax, ymax, zmin);
			Vec3f loc7(xmin, ymax, zmin);

			Vec3f g0 = vs_frw_m * loc0;
			Vec3f g1 = vs_frw_m * loc1;
			Vec3f g2 = vs_frw_m * loc2;
			Vec3f g3 = vs_frw_m * loc3;
			Vec3f g4 = vs_frw_m * loc4;
			Vec3f g5 = vs_frw_m * loc5;
			Vec3f g6 = vs_frw_m * loc6;
			Vec3f g7 = vs_frw_m * loc7;
			
			GLuint queries[1];
			GLuint sampleCount;
			GLint available;
			GLuint bitsSupported;

			// check to make sure functionality is supported
			//glGetQueryiv(GL_QUERY_COUNTER_BITS_ARB, &bitsSupported);
			//if (bitsSupported == 0) {
			//     cout<<"query check is not supported"<<endl;
				// render scene without using occlusion queries
			//}

			glGenQueriesARB(1, queries);
	        
			// before this point, render major occluders
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDepthMask(GL_FALSE);
	        
			// also disable texturing and any fancy shaders
			glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[0]);
			// render bounding box for object i
			glBegin( GL_QUADS );
	  
				// +z
				//glNormal3f  ( 0, 0, 1 );

				glVertex3f  (g2.x, g2.y,g2.z);
				glVertex3f  (g3.x, g3.y,g3.z);
				glVertex3f  (g0.x, g0.y,g0.z);
				glVertex3f  (g1.x, g1.y,g1.z);

				  
				// -z
				//glNormal3f  ( 0, 0, -1 );

				glVertex3f  (g5.x, g5.y,g5.z);
				glVertex3f  (g4.x, g4.y,g4.z);
				glVertex3f  (g7.x, g7.y,g7.z);
				glVertex3f  (g6.x, g6.y,g6.z);
				
				// +y
				//glNormal3f  ( 0, 1, 0 );

				glVertex3f  (g2.x, g2.y,g2.z);
				glVertex3f  (g6.x, g6.y,g6.z);
				glVertex3f  (g7.x, g7.y,g7.z);
				glVertex3f  (g3.x, g3.y,g3.z);

				// -y
				//glNormal3f  ( 0, -1, 0 );

				glVertex3f  (g0.x, g0.y,g0.z);
				glVertex3f  (g4.x, g4.y,g4.z);
				glVertex3f  (g5.x, g5.y,g5.z);
				glVertex3f  (g1.x, g1.y,g1.z);

				// +x
				//glNormal3f  ( 1, 0, 0 );

				glVertex3f  (g2.x, g2.y,g2.z);
				glVertex3f  (g1.x, g1.y,g1.z);
				glVertex3f  (g5.x, g5.y,g5.z);
				glVertex3f  (g6.x, g6.y,g6.z);

				// -x
				//glNormal3f  ( -1, 0, 0 );

				glVertex3f  (g7.x, g7.y,g7.z);
				glVertex3f  (g4.x, g4.y,g4.z);
				glVertex3f  (g0.x, g0.y,g0.z);
				glVertex3f  (g3.x, g3.y,g4.z);
				
					
			glEnd();
			
			glEndQueryARB(GL_SAMPLES_PASSED_ARB);
	        
			glFlush();

			do {
				glGetQueryObjectivARB(queries[0],
									GL_QUERY_RESULT_AVAILABLE_ARB,
									&available);
			} while (!available);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthMask(GL_TRUE);
	        
			glGetQueryObjectuivARB(queries[0], GL_QUERY_RESULT_ARB,
									&sampleCount);

			if( isActive->getValue() )
			{
				if (sampleCount <= 0) {
	            
					isActive->setValue( false, id );
					//cout<<"The object exits the scene"<<endl;
				}

			
			}
			else
			{
				if (sampleCount > 0) {
	            
					isActive->setValue( true, id );
					//cout<<"The object enters the scene"<<endl;
				}
			}
		}

		prev_size = size->getValue();
		prev_center = center->getValue();
		prev_vp_glob_pos = vp_glob_pos;
		prev_vp_glob_rot = vp_glob_rot;
		
	}
  
}


