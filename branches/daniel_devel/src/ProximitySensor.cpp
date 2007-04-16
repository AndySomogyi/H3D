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
/// \file ProximitySensor.cpp
/// \brief CPP file for ProximitySensor, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "ProximitySensor.h"

using namespace H3D;

H3DNodeDatabase ProximitySensor::database( 
        "ProximitySensor", 
        &(newInstance<ProximitySensor>), 
        typeid( ProximitySensor ),
        &X3DEnvironmentalSensorNode::database 
        );

namespace ProximitySensorInternals {
  FIELDDB_ELEMENT( ProximitySensor, centerOfRotation_changed, OUTPUT_ONLY );
  FIELDDB_ELEMENT( ProximitySensor, orientation_changed, OUTPUT_ONLY );
  FIELDDB_ELEMENT( ProximitySensor, position_changed, OUTPUT_ONLY );
}

ProximitySensor::ProximitySensor( Inst< SFNode > _metadata ,
								Inst< SFVec3f > _center ,
								Inst< SFBool > _enabled ,
								Inst< SFVec3f > _size ,
								Inst< SFTime > _enterTime ,
								Inst< SFTime > _exitTime ,
								Inst< SFBool > _isActive ,
								Inst< SFVec3f > _centerOfRotation_changed,
								Inst< SFRotation > _orientation_changed,
								Inst< SFVec3f > _position_changed) :

								centerOfRotation_changed(
									_centerOfRotation_changed),
								X3DEnvironmentalSensorNode( _metadata ,_center,
									_enabled, _size, _enterTime, _exitTime,
									_isActive),
								orientation_changed( _orientation_changed ),
								position_changed( _position_changed ),
								setTime( new SetTime ){

  type_name = "ProximitySensor";
  database.initFields( this );

  setTime->setOwner( this );

  isActive->route( setTime );

  centerOfRotation_changed->setValue( Vec3f( 0, 0, 0 ), id );
  orientation_changed->setValue( Rotation( Vec3f( 0, 0, 0 ) ) , id );
  position_changed->setValue( Vec3f( 0, 0, 0 ) , id );
  
}
void ProximitySensor::traverseSG( TraverseInfo &ti ) {
 
	if( enabled->getValue() )
	{
		
		X3DViewpointNode *VP = X3DViewpointNode::getActive();
		
		Vec3f LP = VP->getFullPos();
		Vec3f COR = VP->centerOfRotation->getValue();
		Rotation LO = VP->getFullOrn();

		const Matrix4f &MVP = VP->accForwardMatrix->getValue();
		Vec3f GP =  MVP * LP;
		Vec3f GCOR =  MVP * COR;
		
		Rotation VP_rot = 
		(Rotation)VP->accForwardMatrix->getValue().getRotationPart();

		Rotation VP_G_rot = VP_rot * LO;

			
		Vec3f size_diff = (size->getValue() - prev_size);
		Vec3f cent_diff = (center->getValue() - prev_center);
		Vec3f vp_pos_diff = (GP - prev_vp_pos);
		Vec3f vp_cor_diff = (GCOR - prev_vp_cor);
	
		Rotation orn_diff = -VP_G_rot * prev_vp_orn;


		if( size_diff.length()   >  Constants::f_epsilon   ||
			cent_diff.length()   >  Constants::f_epsilon   ||
			vp_pos_diff.length() >    Constants::f_epsilon ||
			vp_cor_diff.length() >    Constants::f_epsilon ||
			orn_diff.angle       > Constants::f_epsilon )

		{
			const Matrix4f &accInvMatrix = ti.getAccInverseMatrix();
			Vec3f VP_PR =  accInvMatrix * GP;
			Vec3f VP_COR_PR =  accInvMatrix * GCOR;

			Rotation PS_rot = 
				(Rotation)ti.getAccInverseMatrix().getRotationPart(); 


			Rotation VP_PR_rot = PS_rot * VP_G_rot;
			
			H3DFloat cx = center->getValue().x;
			H3DFloat cy = center->getValue().y;
			H3DFloat cz = center->getValue().z;

			H3DFloat x = size->getValue().x / 2.0;
			H3DFloat y = size->getValue().y / 2.0;
			H3DFloat z = size->getValue().z / 2.0;

			if ( isActive->getValue() )
			{
				if ( VP_PR.x < cx - x || VP_PR.x > cx + x ||
					VP_PR.y < cy - y || VP_PR.y > cy + y ||
					VP_PR.z < cz - z || VP_PR.z > cz + z ) 
				{				
					// Viewpoint exits from the proximity sensor
					isActive->setValue( false , id);					
									
				
				}	
				else
				{
				
					position_changed->setValue( Vec3f( VP_PR - 
							center->getValue() ) , id );
					orientation_changed->setValue( VP_PR_rot , id );
					
					NavigationInfo *ni = NavigationInfo::getActive();
					bool containLookat = false;

					vector< string > navigation_types = ni->type->getValue();
					
					for(unsigned int i = 0; i<navigation_types.size(); i++) {
						if( navigation_types[i] == "LOOKAT" ||
							navigation_types[i] == "ANY") 
								containLookat = true;
					}
					if( containLookat )
					{
						centerOfRotation_changed->setValue( Vec3f( VP_COR_PR  
							- center->getValue() ) , id );
						//cerr<<centerOfRotation_changed->getValue()<<endl;
					}

				}
			}
			else
			{
				if( VP_PR.x > cx - x && VP_PR.x < cx + x &&
					VP_PR.y > cy - y && VP_PR.y < cy + y &&
					VP_PR.z > cz - z && VP_PR.z < cz + z )
				{	
					isActive->setValue( true , id );
										
					position_changed->setValue( Vec3f( VP_PR 
						- center->getValue() ) , id );
					orientation_changed->setValue( VP_PR_rot , id );

					NavigationInfo *ni = NavigationInfo::getActive();
					bool containLookat = false;

					vector< string > navigation_types = ni->type->getValue();
					for(unsigned int i = 0; i<navigation_types.size(); i++ ){
						if( navigation_types[i] == "LOOKAT" ||
							navigation_types[i] == "ANY") 
								containLookat = true;
					}
					if( containLookat )
						centerOfRotation_changed->setValue( Vec3f( VP_COR_PR
						- center->getValue() ) , id );
				}

			}
		}

		prev_size = size->getValue();
		prev_center = center->getValue();
		prev_vp_pos = GP;
		prev_vp_orn = VP_G_rot;
		prev_vp_cor = GCOR;
		
	}
  
}


