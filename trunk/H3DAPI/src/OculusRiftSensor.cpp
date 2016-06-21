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
/// \file OculusRiftSensor.cpp
/// \brief CPP file for OculusRiftSensor.
///
//
//
//////////////////////////////////////////////////////////////////////////////


#include <H3D/OculusRiftSensor.h>

#include <H3D/Scene.h>

using namespace H3D;
#ifdef HAVE_LIBOVR
OculusRiftHandler * OculusRiftSensor::oculus = NULL;
#endif

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase OculusRiftSensor::database( 
                                          "OculusRiftSensor", 
                                          &(newInstance<OculusRiftSensor>),
                                          typeid( OculusRiftSensor ),
                                          &X3DSensorNode::database
                                          );

namespace OculusRiftSensorInternal {
  FIELDDB_ELEMENT(OculusRiftSensor, headOrientation, OUTPUT_ONLY);
  FIELDDB_ELEMENT(OculusRiftSensor, headPosition, OUTPUT_ONLY);
  FIELDDB_ELEMENT(OculusRiftSensor, performanceStats, INPUT_OUTPUT);
  FIELDDB_ELEMENT(OculusRiftSensor, recenterTracking, INPUT_OUTPUT);
  FIELDDB_ELEMENT(OculusRiftSensor, MSAAEnabled, INPUT_OUTPUT);
}



OculusRiftSensor::OculusRiftSensor( 
              Inst< SFBool                > _enabled,
              Inst< SFNode                > _metadata,
              Inst< SFBool                > _isActive,
              Inst< SFVec3f               > _headPosition,
              Inst< SFRotation            > _headOrientation,
              Inst< PerformanceStatsField > _performanceStats,
              Inst< RecenterTrackingField > _recenterTracking,
              Inst< MSAAEnabledField      > _MSAAEnabled ) :
  X3DSensorNode( _enabled, _metadata, _isActive ),
  headPosition( _headPosition ),
  headOrientation( _headOrientation ),
  performanceStats( _performanceStats ),
  recenterTracking( _recenterTracking ),
  MSAAEnabled( _MSAAEnabled ),
  update( new Update ) {

  type_name = "OculusRiftSensor";

  database.initFields( this );
  
  update->setOwner(this);
  Scene::time->routeNoEvent(update);
  
  MSAAEnabled->setValue(true, id);
  recenterTracking->setValue(false, id);

  performanceStats->addValidValue("OFF");
  performanceStats->addValidValue("PERF_SUMMARY");
  performanceStats->addValidValue("LATENCY_TIMING");
  performanceStats->addValidValue("APP_RENDER_TIMING");
  performanceStats->addValidValue("COMP_RENDER_TIMING");
  performanceStats->addValidValue("VERSION_INFO");

  performanceStats->setValue("OFF", id);
}

void OculusRiftSensor::updateValues(){
#ifdef HAVE_LIBOVR

  bool active = oculus && oculus->isInitialized();

  if( isActive->getValue() != active )
    isActive->setValue( active, id );



  if( active && enabled->getValue() ) {
    const ovrTrackingState &tracking_state = oculus->getTrackingState();
    const ovrEyeRenderDesc &left_eye_desc = oculus->getLeftEyeRenderDesc();
    const ovrEyeRenderDesc &right_eye_desc = oculus->getRightEyeRenderDesc();
    const ovrHmdDesc &hmd_desc = oculus->getHMDDesc();

    headPosition->setValue(Vec3f(tracking_state.HeadPose.ThePose.Position.x,
                                 tracking_state.HeadPose.ThePose.Position.y,
                                 tracking_state.HeadPose.ThePose.Position.z), id);

    headOrientation->setValue(Rotation( Quaternion( tracking_state.HeadPose.ThePose.Orientation.x,
                                                    tracking_state.HeadPose.ThePose.Orientation.y,
                                                    tracking_state.HeadPose.ThePose.Orientation.z,
                                                    tracking_state.HeadPose.ThePose.Orientation.w ) ), id );
  }
#endif
}

void OculusRiftSensor::PerformanceStatsField::onNewValue(const string &new_value) {
#ifdef HAVE_LIBOVR
  if (OculusRiftSensor::oculus) {
    if (new_value == "PERF_SUMMARY") OculusRiftSensor::oculus->showPerfStats(ovrPerfHud_PerfSummary);
    else if (new_value == "LATENCY_TIMING") OculusRiftSensor::oculus->showPerfStats(ovrPerfHud_LatencyTiming);
    else if (new_value == "APP_RENDER_TIMING") OculusRiftSensor::oculus->showPerfStats(ovrPerfHud_AppRenderTiming);
    else if (new_value == "COMP_RENDER_TIMING") OculusRiftSensor::oculus->showPerfStats(ovrPerfHud_CompRenderTiming);
    else if (new_value == "VERSION_INFO") OculusRiftSensor::oculus->showPerfStats(ovrPerfHud_VersionInfo);
    else {
      if (new_value != "OFF") {
        Console(LogLevel::Error) << "Warning: Invalid performanceStats value: \"" << new_value
          << "\". Using \"OFF\" instead(in OculusRiftSensor node). " << endl;
      }
      OculusRiftSensor::oculus->showPerfStats(ovrPerfHud_Off);
    }
  }
#endif
}

void OculusRiftSensor::RecenterTrackingField::onNewValue(const bool &new_value) {
#ifdef HAVE_LIBOVR  
  if (new_value && OculusRiftSensor::oculus) oculus->recenterTracking();
#endif
}
void OculusRiftSensor::MSAAEnabledField::onNewValue(const bool &new_value) {
#ifdef HAVE_LIBOVR
  if (OculusRiftSensor::oculus) oculus->setMSAA( new_value );
#endif
}
