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
/// \file H3DHapticsRendererNode.h
/// \brief Header file for H3DHapticsRendererNode
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __H3DHAPTICSRENDERERNODE_H__
#define __H3DHAPTICSRENDERERNODE_H__

// H3DApi includes
#include <H3DApi.h>
#include "Node.h"

// HAPI includes
#include <HAPIHapticsRenderer.h>

namespace H3D {

  /// \ingroup AbstractNodes
  /// \brief Base class for all haptic renderers, i.e. algorithms for generating
  /// forces based on geometries. 
  /// 
  /// All subclasses should set the renderer member.
  class H3DAPI_API H3DHapticsRendererNode: public Node {
  public:

    /// Constructor.
    H3DHapticsRendererNode() {}

    /// Get the HAPI haptics renderer used for this node.
    inline virtual HAPI::HAPIHapticsRenderer *getHapticsRenderer() {
      return renderer;
    }
  
    virtual string defaultXMLContainerField() {
      return "hapticsRenderer";
    }
    
  protected:
    HAPI::HAPIHapticsRenderer *renderer;
  };
}

#endif
