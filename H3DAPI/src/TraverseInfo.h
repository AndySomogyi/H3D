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
//
/// \file TraverseInfo.h
/// \brief Header file for TraverseInfo.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __TRAVERSEINFO_H__
#define __TRAVERSEINFO_H__

#include "H3DTypes.h"
#include "HapticForceEffect.h"
#include "HapticShape.h"
#include "AutoRefVector.h"
#include <sstream>
#include <stack>

using namespace std;

namespace H3D {
  // forward declarations
  class HapticShape;
  class H3DSurfaceNode;
  class H3DHapticsDevice;

  typedef AutoRefVector< HapticShape > HapticShapeVector;
  typedef AutoRefVector< HapticForceEffect > HapticEffectVector;

  /// TraverseInfo is a structure that is passed along when traversing the 
  /// scene graph. It contains information needed during the traversing, 
  /// including e.g. transform matrices, available haptics devices etc.
  /// Geometry nodes must register the shapes to render haptically
  /// by calls to the addHapticShape() function calls.
  /// 
  class H3DAPI_API TraverseInfo {
  public:
 
    /// Thrown when trying to get hold of a H3DHapticsDevice using an invalid
    /// device index.
    ///
    H3D_VALUE_EXCEPTION( int, InvalidHapticsDeviceIndex ); 

    /// Constructor.
    /// \param _haptics_devices The HapticDevice instances that are available
    /// to render HapticShapes on.
    /// 
    TraverseInfo( const vector< H3DHapticsDevice * > &_haptics_devices ) :
      current_surface( NULL ),
      haptics_devices( _haptics_devices ),
      haptic_shapes( _haptics_devices.size() ),
      haptic_effects( _haptics_devices.size() ),
      haptics_enabled( true ) {
      // put two unit matrices on the transform stack.
      transform_stack.push( TransformInfo( Matrix4f(), Matrix4f () ) );
    }

    /// Returns the index of the given H3DHapticsDevice. -1 is returned
    /// if the H3DHapticsDevice is not in the vector of haptics devices.
    inline int getHapticsDeviceIndex( H3DHapticsDevice *hd ) {
      int index = 0;
      for( vector< H3DHapticsDevice * >::iterator i = haptics_devices.begin();
           i != haptics_devices.end();
           i++, index++ ) {
        if( (*i) == hd ) return index;
      }
      return -1;
    } 

    /// Adds a HapticShape to be rendered by a H3DHapticsDevice.
    /// Shapes will only be added if hapticsEnabled() is true. 
    /// \param device_index The index of the H3DHapticsDevice to render the 
    /// shape on.
    /// \param shape The HapticShape to render.
    ///
    void addHapticShape( int device_index, HapticShape *shape );

    /// Adds a HapticShape to be rendered by a H3DHapticsDevice.
    /// Shapes will only be added if hapticsEnabled() is true. 
    /// \param hd The H3DHapticsDevice to render the shape on.
    /// \param shape The HapticShape to render.
    ///
    inline void addHapticShape( H3DHapticsDevice *hd, HapticShape *shape ) {
      int device_index = getHapticsDeviceIndex( hd );
      addHapticShape( device_index, shape );
    }
    
    /// Adds a HapticShape to be rendered by all H3DHapticsDevices.
    /// Shapes will only be added if hapticsEnabled() is true. 
    /// \param shape The HapticShape to render.
    ///
    void addHapticShapeToAll( HapticShape *shape );

    /// Get the HapticShapes that has been added for the H3DHapticsDevice
    /// with the given device_index.
    ///
    inline const HapticShapeVector &getHapticShapes(int device_index){
      if( device_index < 0 || device_index >= (int)haptics_devices.size() ) {
        stringstream s;
        s << "TraverseInfo only has " << (unsigned int) haptics_devices.size() 
          << " haptics device available. ";
        throw InvalidHapticsDeviceIndex( device_index,
                                         s.str(),
                                         H3D_FULL_LOCATION );
      }
      return haptic_shapes[ device_index ];
    }

    /// Get the HapticShapes that has been added for the given H3DHapticsDevice
    ///
    inline const HapticShapeVector 
      &getHapticShapes( H3DHapticsDevice * hd ) {
      int device_index = getHapticsDeviceIndex( hd );
      return getHapticShapes( device_index );
    }

    /// Adds a HapticForceEffect to be rendered by a H3DHapticsDevice.
    /// Effects will only be added if hapticsEnabled() is true. 
    /// \param device_index The index of the H3DHapticsDevice to render the 
    /// shape on.
    /// \param effect The HapticForceEffect to render.
    ///
    void addForceEffect( int device_index, HapticForceEffect *effect );

    /// Adds a HapticForceEffect to be rendered by a H3DHapticsDevice.
    /// Shapes will only be added if hapticsEnabled() is true. 
    /// \param hd The H3DHapticsDevice to render the shape on.
    /// \param effect The HapticForceEffect to render.
    ///
    inline void addForceEffect( H3DHapticsDevice *hd, 
                                HapticForceEffect *effect ) {
      int device_index = getHapticsDeviceIndex( hd );
      addForceEffect( device_index, effect );
    }
    
    /// Adds a HapticForceEffect to be rendered by all H3DHapticsDevices.
    /// Effects will only be added if hapticsEnabled() is true. 
    /// \param effect The HapticForceEffect to render.
    ///
    void addForceEffectToAll( HapticForceEffect *effect );
		
    /// Get the HapticForceEffects that has been added for the H3DHapticsDevice
    /// with the given device_index.
    ///
    inline const HapticEffectVector 
      &getForceEffects( int device_index){
      if( device_index < 0 || device_index >= (int)haptics_devices.size() ) {
        stringstream s;
        s << "TraverseInfo only has " << (unsigned int)haptics_devices.size() 
          << " haptics device available. ";
        throw InvalidHapticsDeviceIndex( device_index,
                                         s.str(),
                                         H3D_FULL_LOCATION );
      }
      return haptic_effects[ device_index ];
    }
    
    /// Get the HapticForceEffects that has been added for the given 
    /// H3DHapticsDevice
    inline const HapticEffectVector
      &getForceEffects( H3DHapticsDevice * hd ) {
      int device_index = getHapticsDeviceIndex( hd );
      return getForceEffects( device_index );
    }
    
    /// Sets the surface that is active in the current traversal.
    inline void setCurrentSurface( H3DSurfaceNode *s ) {
      current_surface = s;
    }

    /// Get the surface that is active in the current traversal.
    inline H3DSurfaceNode * getCurrentSurface() {
      return current_surface;
    }
    
    inline H3DHapticsDevice *getHapticsDevice( int device_index ) {
      return haptics_devices[ device_index ];
    }

    /// Get the haptics devices that are available.
    inline const vector< H3DHapticsDevice * > &getHapticsDevices() {
      return haptics_devices;
    }

    /// When called, future calls to addHapticShapes() and addForceEffects() 
    /// functions will not add any shapes until enableHaptics() is called.
    inline void disableHaptics() {
      haptics_enabled = false;
    }

    /// When called, the addHapticShapes() and addForceEffects() functions
    /// calls will add HapticShapes to render. Haptics is enabled by default.
    inline void enableHaptics() {
      haptics_enabled = true;
    }

    /// Check whether haptics is enabled or not. If disabled the
    /// addHapticsShapes() functions will have no effect. Geometry
    /// nodes that add shapes should check that haptics is enabled 
    /// before doing any calculations deciding what to render. This is
    /// because any HapticShape created will not be rendered anyway
    /// so no computer resources should be used creating them.
    /// 
    inline bool hapticsEnabled() {
      return haptics_enabled;
    }

    /// This function should be called when we move into a nested
    /// coordinate space with the transformation matrices between
    /// the coordinate spaces.
    /// \param forward Transformation matrix between the space we
    /// move into to the previous space.
    /// \param inverse The inverse of forward.
    ///
    inline void pushMatrices( const Matrix4f &forward,
		       const Matrix4f &inverse ) {
      const TransformInfo &top = transform_stack.top();
      transform_stack.push( TransformInfo( top.acc_frw * forward,
                                           inverse * top.acc_inv ) );
    }

    /// This function should be called when we move out from a 
    /// nested coordinate space
    inline void popMatrices() {
      transform_stack.pop();
    }

    /// Get the accumulated forward transformation matrix, i.e. the
    /// transformation matrix from the local space at the current 
    /// traversal position to global space.
    inline const Matrix4f &getAccForwardMatrix() {
      return transform_stack.top().acc_frw;
    }

    /// Get the accumulated inverse transformation matrix, i.e. the
    /// transformation matrix from global space to the local space 
    /// at the current traversal position.
    inline const Matrix4f &getAccInverseMatrix() {
      return transform_stack.top().acc_inv;
    }

  protected:
    class TransformInfo {
    public:
      TransformInfo( const Matrix4f &_acc_frw,
                     const Matrix4f &_acc_inv ):
        acc_frw( _acc_frw ),
        acc_inv( _acc_inv ) {}
      Matrix4f acc_frw, acc_inv;
    };
    stack< TransformInfo > transform_stack;
    
    H3DSurfaceNode *current_surface;
    vector< H3DHapticsDevice * > haptics_devices;
    vector< HapticShapeVector > haptic_shapes;
    vector< HapticEffectVector > haptic_effects;
    bool haptics_enabled;
  };

};

#endif