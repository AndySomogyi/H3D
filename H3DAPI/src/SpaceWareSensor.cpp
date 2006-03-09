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
/// \file SpaceWareSensor.cpp
/// \brief CPP file for SpaceWareSensor.
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "SpaceWareSensor.h"

#ifdef WIN32

// si.h requires us to specify an OS type when including it.
#ifdef WIN32
#define OS_WIN32
#endif

#ifdef HAVE_3DXWARE
#ifdef _MSC_VER
#pragma comment( lib, "siapp.lib" )
#pragma comment( lib, "spwmath.lib" )
#endif
extern "C" {
#include <si.h>
#include <siapp.h>
  extern  SpwRetVal SpwErrorVal;
}
#endif

#include <process.h>

#endif

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase SpaceWareSensor::database( 
                                          "SpaceWareSensor", 
                                          &(newInstance<SpaceWareSensor>),
                                          typeid( SpaceWareSensor ),
                                          &X3DSensorNode::database
                                          );

namespace SpaceWareSensorInternal {
  FIELDDB_ELEMENT( SpaceWareSensor, rawYaw, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, rawPitch, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, rawRoll, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, rawRotation, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, rawTranslation, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, instantTranslation, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, instantYaw, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, instantPitch, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, instantRoll, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, instantRotation, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, accumulatedTranslation, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, accumulatedYaw, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, accumulatedPitch, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, accumulatedRoll, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, accumulatedRotation, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, translationScale, INPUT_OUTPUT );
  FIELDDB_ELEMENT( SpaceWareSensor, rotationScale, INPUT_OUTPUT );
  FIELDDB_ELEMENT( SpaceWareSensor, buttons, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, latestButtonPress, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SpaceWareSensor, latestButtonRelease, OUTPUT_ONLY );

  H3D_API_EXCEPTION( CouldNotInitSpaceWare );
  H3D_API_EXCEPTION( CreateWindowError );
  H3D_API_EXCEPTION( WindowGetMessageError );

#ifdef HAVE_3DXWARE
#ifdef WIN32
  // Help function for getting a string of the current
  // error returned from GetLastError.
  string getWindowErrorMessage() {
    TCHAR buffer[80]; 
    DWORD dw = GetLastError(); 
    FormatMessage(
                  FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  dw,
                  0,
                  buffer,
                  80, NULL );
    return buffer;
  }

  // This is a std. Win32 function to handle various window events. We do
  // not handle any events at all.
  // \param hWnd handle to window   
  // \param msg message to process
  // \param wParam 32 bit msg parameter
  // \param lParam 32 bit msg parameter
  //
  LRESULT WINAPI HandleNTEvent ( HWND hWnd, 
                                 unsigned msg, 
                                 WPARAM wParam,
                                 LPARAM lParam ) {
    LONG addr;                /* address of our window */
    addr = GetClassLong(hWnd, 0);  /* get address */
    
    switch ( msg ) {
      //    case WM_CLOSE:
      //break;
      
    case WM_DESTROY :
      PostQuitMessage (0);
      return (0);
    }
    return DefWindowProc ( hWnd, msg, wParam, lParam );
  } 

  /// The function used as the thread for reading data from
  /// the SpceWare device.
  unsigned __stdcall spaceWareThread( void* data ) {
    try {
      // the creator of this thread
      SpaceWareSensor *space_ware_sensor = 
        static_cast< SpaceWareSensor * >( data );

      // Platform-specific device data.
      SiOpenData open_data; 
      // 3DxWare device handle.
      SiHdl hdl;   
      
      sint32_t err = SiInitialize();
      if ( err != SPW_NO_ERROR ) {
        char * s = SpwErrorString( err );
        if( s )
          throw CouldNotInitSpaceWare( s );
        else 
          throw CouldNotInitSpaceWare( "Probably the driver is not installed." );
        return 1;
      }
    
      WNDCLASS wndclass;        /* our own instance of the window class */
      HINSTANCE   hInst;        /* handle to our instance */
    
      hInst = NULL;             /* init handle */
    
      wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
      wndclass.lpfnWndProc   =(WNDPROC)SpaceWareSensorInternal::HandleNTEvent ;
      wndclass.cbClsExtra    = 8 ;
      wndclass.cbWndExtra    = 0 ;
      wndclass.hInstance     = hInst;
      wndclass.hIcon         = NULL;
      wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
      wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
      wndclass.lpszMenuName  = NULL ;
      wndclass.lpszClassName = "SpaceWare";
  
      // Register the display windows class to be used for subsequent calls to 
      // CreateWindow.
      if( !RegisterClass (&wndclass)  ) {
        throw CreateWindowError( getWindowErrorMessage(),
                                 H3D_FULL_LOCATION );  
      }
    
      // Create a window to use for getting messages from the SpaceWare device.
      HWND hwnd = CreateWindow ( "SpaceWare",           /*Window class name*/
                                 "SpaceWareSensor",     /*Window caption*/
                                 WS_OVERLAPPEDWINDOW, //WS_BORDER | WS_ICONIC, /*Window Style*/
                                 0,0, 10, 10,
                                 NULL,           /*parent window handle*/
                                 NULL,             /*window menu handle*/
                                 hInst,    /*program instance handle*/
                                 NULL);            /*creation parameters*/
      if( hwnd == NULL ) {
        throw CreateWindowError( getWindowErrorMessage(),
                                 H3D_FULL_LOCATION );  
      }
    
      // init Window platform specific data for a call to SiOpen
      SiOpenWinInit( &open_data, hwnd );
    
      // open the SpaceWare device
      hdl = SiOpen( "H3D API", SI_ANY_DEVICE, SI_NO_MASK,
                    SI_EVENT, &open_data );
      if( !hdl ) {
        string s = SpwErrorString( SpwErrorVal );
        throw CouldNotInitSpaceWare( s );
        return 1;
      }

      BOOL ret;
      MSG msg;
      SiGetEventData event_data;  /* Platform-specific event data */
      SiSpwEvent si_event;        /* 3DxWare data event */

      // main message loop
      while( ret = GetMessage (&msg, NULL, 0, 0) ) {
        if (ret == -1) {
          throw WindowGetMessageError( getWindowErrorMessage(),
                                       H3D_FULL_LOCATION );
        }
      
        // init Window platform specific data for a call to SiGetEvent
        SiGetEventWinInit (&event_data, msg.message, msg.wParam,
                           msg.lParam);

        if (SiGetEvent (hdl, 0, &event_data, &si_event) == SI_IS_EVENT) {
          // the event is a 3DxWare event
          switch (si_event.type) {
          case SI_MOTION_EVENT: {
            const SiSpwData & data = si_event.u.spwData;
            // we have to negate the z component since H3D API
            // and the SpaceWare have different coordinate systems.
            space_ware_sensor->thread_translation =
              Vec3f( (H3DFloat)data.mData[ SI_TX ],
                     (H3DFloat)data.mData[ SI_TY ],
                     (H3DFloat)-data.mData[ SI_TZ ] );
            // convert to radians
            space_ware_sensor->thread_rotation = 
              Vec3f( (H3DFloat)data.mData[SI_RX],
                     (H3DFloat)data.mData[SI_RY],
                     (H3DFloat)-data.mData[SI_RZ] ) * ( Constants::pi / 180 );
            space_ware_sensor->thread_motion_event = true;
            break;
          }
          case SI_ZERO_EVENT:
            break;
    
          case SI_BUTTON_EVENT: {
            const SiSpwData & data = si_event.u.spwData;
            SpaceWareSensor::ButtonData bd( data.bData.pressed,
                                            data.bData.released,
                                            data.bData.current );
            space_ware_sensor->thread_buttons.push_back( bd );
            break;
          }
    
          default:
            break;
          } 
        } else {
          // the event is not a 3DxWare event so we dispatch it
          TranslateMessage (&msg);
          DispatchMessage (&msg);
        }
      }
    }
    catch (const Exception::H3DException &e) {
      Console(4) << "Warning: SpaceWareSensor error. Node will be unusable."<<endl;
      Console(4) << e << endl;
    }
    return 1;
  }
#endif // WIN32
#endif // HAVE_3DXWARE
}

SpaceWareSensor::SpaceWareSensor( 
              Inst< SFBool                > _enabled,
              Inst< SFNode                > _metadata,
              Inst< SFBool                > _isActive,
              Inst< SFVec3f               > _rawTranslation,
              Inst< SFRotation            > _rawYaw,
              Inst< SFRotation            > _rawPitch,
              Inst< SFRotation            > _rawRoll,
              Inst< SFRotation            > _rawRotation,
              Inst< ScaleTranslation      > _instantTranslation,
              Inst< ScaleRotation         > _instantYaw,
              Inst< ScaleRotation         > _instantPitch,
              Inst< ScaleRotation         > _instantRoll,
              Inst< ScaleRotation         > _instantRotation,
              Inst< AccumulateTranslation > _accumulatedTranslation,
              Inst< AccumulateRotation    > _accumulatedYaw,
              Inst< AccumulateRotation    > _accumulatedPitch,
              Inst< AccumulateRotation    > _accumulatedRoll,
              Inst< AccumulateRotation    > _accumulatedRotation,
              Inst< SFFloat               > _translationScale,
              Inst< SFFloat               > _rotationScale,
              Inst< SFInt32               > _buttons,
              Inst< SFInt32               > _latestButtonPress,
              Inst< SFInt32               > _latestButtonRelease ) :
  X3DSensorNode( _enabled, _metadata, _isActive ),
  rawTranslation( _rawTranslation ),
  rawYaw( _rawYaw ),
  rawPitch( _rawPitch ),
  rawRoll( _rawRoll ),
  rawRotation( _rawRotation ),
  instantTranslation( _instantTranslation ),
  instantYaw( _instantYaw ),
  instantPitch( _instantPitch ),
  instantRoll( _instantRoll ),
  instantRotation( _instantRotation ),
  accumulatedTranslation( _accumulatedTranslation ),
  accumulatedYaw( _accumulatedYaw ),
  accumulatedPitch( _accumulatedPitch ),
  accumulatedRoll( _accumulatedRoll ),
  accumulatedRotation( _accumulatedRotation ),
  translationScale( _translationScale ),
  rotationScale( _rotationScale ),
  buttons( _buttons ),
  latestButtonPress( _latestButtonPress ),
  latestButtonRelease( _latestButtonRelease ),
  thread_motion_event( false ) {

  type_name = "SpaceWareSensor";

  database.initFields( this );

  rawTranslation->setValue( Vec3f( 0, 0, 0 ), id );
  rawYaw->setValue( Rotation( 1, 0, 0, 0 ), id  );
  rawPitch->setValue( Rotation( 1, 0, 0, 0 ), id  );
  rawRoll->setValue( Rotation( 1, 0, 0, 0 ), id  );
  rawRotation->setValue( Rotation( 1, 0, 0, 0 ), id  );

  translationScale->setValue( 1 );
  rotationScale->setValue( 1 );

  buttons->setValue( 0, id  );
  latestButtonPress->setValue( 0, id  );
  latestButtonRelease->setValue( 0, id  );

  rawTranslation->route( instantTranslation, id  );
  translationScale->route( instantTranslation, id  );
  instantTranslation->route( accumulatedTranslation, id  );

  rawYaw->route( instantYaw, id  );
  rotationScale->route( instantYaw, id  );
  instantYaw->route( accumulatedYaw, id  );

  rawPitch->route( instantPitch, id  );
  rotationScale->route( instantPitch, id  );
  instantPitch->route( accumulatedPitch, id  );

  rawRoll->route( instantRoll, id  );
  rotationScale->route( instantRoll, id  );
  instantRoll->route( accumulatedRoll, id  );

  rawRotation->route( instantRotation, id  );
  rotationScale->route( instantRotation, id  );
  instantRotation->route( accumulatedRotation, id  );

#ifndef HAVE_3DXWARE
#ifdef WIN32
  Console(4) << "Warning: H3D API compiled without 3Dxware. SpaceWareSensor node "
             << "will be unusable." << endl;
#else
  Console(4) << "Warning: SpaceWareSensor only supported on Windows platform." 
             << endl;
#endif
#endif
}

SpaceWareSensor::~SpaceWareSensor() {
#ifdef HAVE_3DXWARE
#ifdef WIN32
  CloseHandle( thread_handle );
#endif
#endif
}

void SpaceWareSensor::initialize() {
  X3DSensorNode::initialize();
#ifdef HAVE_3DXWARE
#ifdef WIN32
  thread_handle =
    (HANDLE) _beginthreadex( NULL, 0,
                             SpaceWareSensorInternal::spaceWareThread,
                             this,
                             0, 0 );
#endif
#endif
  
}

void SpaceWareSensor::traverseSG( TraverseInfo &ti ) {
  using namespace SpaceWareSensorInternal;
  // TODO: lock


  // transfer  the motion data if an event has occured since
  // last scenegraph loop
  if( thread_motion_event ) {
    rawTranslation->setValue( thread_translation, id  );
    Vec3f rot = thread_rotation;
    rawYaw->setValue( Rotation( 1, 0, 0, rot.x ), id  );
    rawPitch->setValue( Rotation( 0, 1, 0, rot.y ), id  );
    rawRoll->setValue( Rotation( 0, 0, 1, rot.z ), id  );

    // the length of the rot vector is the angle to rotate
    // and the direction of it is the axis to rotate around.
    H3DFloat a = rot.length();
    if( rot * rot > Constants::f_epsilon ) {
      rot.normalize();
    } else {
      rot = Vec3f( 1, 0, 0 );
    }
    rawRotation->setValue( Rotation( rot, a ), id  );
    thread_motion_event = false;
  }
  
  // transfer button data
  vector< ButtonData > button_data;
  button_data.swap( thread_buttons );
  for( vector<ButtonData >::iterator i = button_data.begin();
       i != button_data.end();
       i++ ) {
    buttons->setValue( (*i).current_state, id  );
    int mask = 1;
    int b;
    for( b = 0; b <=32; b++, mask *= 2 ) {
      if( mask & (*i).pressed ) {
        latestButtonPress->setValue( b, id  );
      }  
    }

    mask = 1; 
    for( b = 0; b <=32; b++, mask *= 2 ) {
      if( mask & (*i).released ) {
        latestButtonRelease->setValue( b, id  );
      }  
    }
  }
}
