//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2013, SenseGraphics AB
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
/// \file PlaybackDevice.h
/// \brief Header file for PlaybackDevice.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __PLAYBACKDEVICE_H__
#define __PLAYBACKDEVICE_H__

#include <H3D/X3DUrlObject.h>
#include <H3D/H3DHapticsDevice.h>

namespace H3D {

  class H3DAPI_API PlaybackDevice: 
    public X3DUrlObject,
    public H3DHapticsDevice {
  public:

    class OnPlay : public OnNewValueSField < AutoUpdate < SFBool > > {
      virtual void onNewValue( const bool& new_value );
    };

    class OnSeekToTime : public OnNewValueSField < AutoUpdate < SFTime > > {
      virtual void onNewValue( const H3DTime& new_value );
    };

    class OnPlaybackSpeed : public OnNewValueSField < AutoUpdate < SFFloat > > {
      virtual void onNewValue( const H3DFloat& new_value );
    };

    /// Constructor.
    PlaybackDevice( 
        Inst< MFString        >  _url                   = 0,
        Inst< SFVec3f         > _devicePosition         = 0,
        Inst< SFRotation      > _deviceOrientation      = 0,
        Inst< TrackerPosition > _trackerPosition        = 0,
        Inst< TrackerOrientation > _trackerOrientation  = 0,
        Inst< PosCalibration  > _positionCalibration    = 0,
        Inst< OrnCalibration  > _orientationCalibration = 0,
        Inst< SFVec3f         > _proxyPosition          = 0,
        Inst< WeightedProxy   > _weightedProxyPosition  = 0,     
        Inst< SFFloat         > _proxyWeighting         = 0,
        Inst< SFBool          > _mainButton             = 0,
        Inst< SFBool          > _secondaryButton       = 0,
        Inst< SFInt32         > _buttons                = 0,
        Inst< SFVec3f         > _force                  = 0,
        Inst< SFVec3f         > _torque                 = 0,
        Inst< SFInt32         > _inputDOF               = 0,
        Inst< SFInt32         > _outputDOF              = 0,
        Inst< SFInt32         > _hapticsRate            = 0,
        Inst< SFInt32         > _desiredHapticsRate     = 0,
        Inst< SFNode          > _stylus                 = 0,
        Inst< SFHapticsRendererNode > _hapticsRenderer  = 0,
        Inst< MFVec3f         > _proxyPositions         = 0,
        Inst< SFBool          > _followViewpoint        = 0,
        Inst< OnPlay          > _play                   = 0,
        Inst< SFBool          > _binary                 = 0,
        Inst< OnSeekToTime    > _seekToTime             = 0,
        Inst< OnPlaybackSpeed > _playbackSpeed          = 0,
        Inst< SFTime          > _playbackTime           = 0,
        Inst< SFBool          > _playing                = 0
        );

    virtual ~PlaybackDevice ();

    virtual void initialize ();

    virtual void updateDeviceValues ();

    /// Start and stop the playback. 
    ///
    /// If a value of true is received and a valid url has been
    /// specified then playback will begin. If a value of false is
    /// recieved then the playback is paused and can be resumed with
    /// a subsiquent true event.
    ///
    /// To start the playback from the begining again, set seekToTime
    /// to zero first.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> false \n
    ///
    auto_ptr < OnPlay > play;

    /// If true, then the recording pointed to by url is assumed to contain
    /// binary data. Otherwise it is assumed to contain text data.
    auto_ptr< SFBool > binary;

    /// Move playback to the specified time.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> 0.0 \n
    ///
    auto_ptr < OnSeekToTime > seekToTime;

    /// Speed of playback. A value of 1 corresponds to the original 
    /// recording speed.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> 1.0 \n
    ///
    auto_ptr < OnPlaybackSpeed > playbackSpeed;

    /// Current playback time.
    ///
    /// Indicates the current playback time in the recording.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// <b>Default value:</b> 0.0 \n
    ///
    auto_ptr < SFTime > playbackTime;

    /// True if the device is currently playing back values. False
    /// if idle, or playback has ended.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// <b>Default value:</b> false \n
    ///
    auto_ptr < SFBool > playing;

    /// Node database entry
    static H3DNodeDatabase database;

  protected:

    auto_ptr < Field > playback_url_changed;

    std::string tmp_filename;
  };
}

#endif
