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
/// \file OculusRiftSensor.h
/// \brief Header file for OculusRiftSensor.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __H3D_OCULUSRIFTSENSOR_H__
#define __H3D_OCULUSRIFTSENSOR_H__


#include <H3D/X3DSensorNode.h>
#include <list>
#include <H3D/SFVec3f.h>
#include <H3D/SFFloat.h>
#include <H3D/SFInt32.h>
#include <H3D/SFRotation.h>
#include <H3DUtil/Threads.h>
#include <H3D/OculusRiftHandler.h>

namespace H3D {

  /// \ingroup H3DNodes
  /// \class OculusRiftSensor
  /// \brief This is a X3DSensorNode for reading values from a Oculus Rift HMD.
  ///
  ///
  /// <b>Examples:</b>
  ///   - <a href="../../../H3DAPI/examples/All/OculusRiftSensor.x3d">OculusRiftSensor.x3d</a>
  ///     ( <a href="examples/OculusRiftSensor.x3d.html">Source</a> )
  ///
  /// \par Internal routes:
  /// \dotfile OculusRiftSensor.dot
  class H3DAPI_API OculusRiftSensor : public X3DSensorNode {
  public:

  
    /// This class is an AutoUpdate class that sets the field
    /// accumulatedRotation to its input value and also sets
    /// accumulatedYaw, accumulatedPitch and accumulatedRoll to
    /// a value dependent on input value
    ///
    class H3DAPI_API RecenterTrackingField: 
      public AutoUpdate< OnNewValueSField< SFBool > > {
      
    protected:
      virtual void onNewValue(const bool &new_value);
    };

    class H3DAPI_API MSAAEnabledField :
      public AutoUpdate< OnNewValueSField< SFBool > > {

    protected:
      virtual void onNewValue(const bool &new_value);
    };

    class H3DAPI_API PerformanceStatsField :
      public AutoUpdate< OnNewValueSField< SFString > > {

    protected:
      virtual void onNewValue(const string &new_value);
    };


   
  
    /// Constructor.
    OculusRiftSensor( 
                    Inst< SFBool                > _enabled  = 0,
                    Inst< SFNode                > _metadata = 0,
                    Inst< SFBool                > _isActive = 0,
                    Inst< SFVec3f               > _headPosition = 0,
                    Inst< SFRotation            > _headOrientation = 0,
                    Inst< PerformanceStatsField > _performanceStats = 0,
                    Inst< RecenterTrackingField > _recenterTracking = 0,
                    Inst< MSAAEnabledField      > _MSAAEnabled = 0
                    );

    ~OculusRiftSensor() {
    }

    /// Contains the current translation as reported by the device.
    ///
    /// <b>Access type: </b> outputOnly \n
    /// 
    /// \dotfile OculusRiftSensor_headPosition.dot
    auto_ptr< SFVec3f>             headPosition;

    /// Contains the current rotation around the x-axis as reported 
    /// by the device.
    ///
    /// <b>Access type: </b> outputOnly \n
    /// 
    /// \dotfile OculusRiftSensor_headOrientation.dot
    auto_ptr< SFRotation>          headOrientation;

    /// The sum of all instantTranslation values.
    ///
    /// <b>Access type: </b> outputOnly \n
    /// 
    /// \dotfile OculusRiftSensor_accumulatedTranslation.dot
    auto_ptr< PerformanceStatsField>  performanceStats;

    /// The sum of all instantYaw values.
    ///
    /// <b>Access type: </b> outputOnly \n
    /// 
    /// \dotfile OculusRiftSensor_accumulatedYaw.dot
    auto_ptr< RecenterTrackingField> recenterTracking;

    /// The sum of all instantPitch values.
    ///
    /// <b>Access type: </b> outputOnly \n
    /// 
    /// \dotfile OculusRiftSensor_accumulatedPitch.dot
    auto_ptr< MSAAEnabledField > MSAAEnabled;

  public:


    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

    static OculusRiftHandler *oculus;

  private:

    bool was_initialized;

    /// Transfers the values from the device communication thread to
    /// the scenegraph thread.
    void updateValues();
    
    struct H3DAPI_API Update
      : AutoUpdate<Field> {
      void update(){
        static_cast<OculusRiftSensor*>
          (owner)->updateValues();
      }
    };
    
    auto_ptr< Update > update;
  };
}

#endif
