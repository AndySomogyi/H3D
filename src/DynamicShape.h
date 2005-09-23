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
/// \file Dynamicshape.h
/// \brief Header file for Dynamicshape, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __DYNAMICSHAPE_H__
#define __DYNAMICSHAPE_H__

#include "MatrixTransform.h"
#include "PeriodicUpdate.h"
#include "RK4.h"

namespace H3D {

  /// \ingroup Groups
  /// \class Dynamicshape
  /// \brief The Dynamicshape node is a Shape container that has basic
  /// properties for defining rigid body motion. This includes
  /// a position, orientation, mass, etc.
  ///
  class H3DAPI_API DynamicShape : public MatrixTransform {
  public:

    /// Time dependant field to perform the RK4 integration for determining
    /// the motion of the dynamic transform
    /// routes_in[0] time field
    ///
    class H3DAPI_API SFMotion: 
      public  TypedField< H3D::SFVec3f, Types< SFTime > >  {
    public:
      
      SFMotion() {
      }
      
      H3DTime last_t;
    protected:
      /// Update the matrix from the fields in the Transform node.
      virtual void update();

      virtual void updateState( LMState &state, H3DTime dt );
    };

    /// Specialize the SFMatrix4f to update the matrix from the
    /// fields in the Transform node.
    /// routes_in[0] orientation field
    /// routes_in[1] position field
    ///
    class H3DAPI_API SFMatrix4f: 
      public TypedField< H3D::SFMatrix4f,
                         Types< SFRotation, SFVec3f > >{
    protected:
 
      /// Update the matrix from the fields in the Dynamicshape node.
      virtual void update();
    };

     /// Constructor.
    DynamicShape( Inst< MFChild    > _addChildren              = 0,
                  Inst< MFChild    > _removeChildren           = 0,
                  Inst< MFChild    > _children                 = 0,
                  Inst< SFNode     > _metadata                 = 0,
                  Inst< SFBound    > _bound                    = 0,
                  Inst< SFVec3f    > _bboxCenter               = 0,
                  Inst< SFVec3f    > _bboxSize                 = 0,
                  Inst< SFTransformedBound > _transformedBound = 0,    
                  Inst< SFMatrix4f > _matrix                   = 0,
                  Inst< SFMatrix4f > _accumulatedForward       = 0,
                  Inst< SFMatrix4f > _accumulatedInverse       = 0,
                  Inst< SFVec3f          > _position         = 0,
                  Inst< SFRotation       > _orientation      = 0,
                  Inst< SFVec3f          > _velocity         = 0,
                  Inst< SFVec3f          > _momentum         = 0,
                  Inst< SFVec3f          > _force            = 0,
                  Inst< SFVec3f          > _angularVelocity  = 0,
                  Inst< SFVec3f          > _angularMomentum  = 0, 
                  Inst< SFRotation       > _spin             = 0,
                  Inst< SFVec3f          > _torque           = 0,
                  Inst< SFFloat          > _mass             = 0,
                  Inst< SFMatrix3f       > _inertia          = 0,
                  Inst< SFMotion         > _motion           = 0 );


    virtual H3DNodeDatabase *getDatabase() { return &database; }

    /// Specifies the position of the shape
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Vec3f( 0, 0, 0 ) \n
    auto_ptr<    SFVec3f    >  position;

    /// Specifies the orientation of the shape
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Rotation( 0, 0, 1, 0 ) \n
    auto_ptr<    SFRotation >  orientation;

    /// Specifies the velocity in m/s
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Vec3f( 0, 0, 0 ) \n
    auto_ptr<    SFVec3f    >  velocity;
    
    /// Specifies the momentum
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Vec3f( 0, 0, 0 ) \n
    auto_ptr<    SFVec3f    >  momentum;
    
    /// Specifies the force currently acting on the dynamic
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Vec3f( 0, 0, 0 ) \n
    auto_ptr<    SFVec3f    >  force;

    /// Specifies the angular velocity of the dynamic
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Vec3f( 0, 0, 0 ) \n
    auto_ptr<    SFVec3f    >  angularVelocity;

    /// Specifies the angular momentum of the dynamic
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Vec3f( 0, 0, 0 ) \n
    auto_ptr<    SFVec3f    >  angularMomentum;

    /// Specifies the spin of the dynamic
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Vec3f( 0, 0, 0 ) \n
    auto_ptr<    SFRotation    >  spin;

    /// Specifies the torque force currently acting on the dynamic
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Vec3f( 0, 0, 0 ) \n
    auto_ptr<    SFVec3f    >  torque;

    /// Specifies the mass, in kg
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Vec3f( 0, 0, 0 ) \n
    auto_ptr<    SFFloat    >  mass;

    /// Specifies the inertia tensor
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Matrix3f( 1, 0, 0, 0, 1, 0, 0, 0, 1 ) \n
    auto_ptr<    SFMatrix3f  >  inertia;

    /// Dynamic update algorithm
    auto_ptr<    SFMotion    >  motion;
    
    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;
  };
}

#endif
