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
/// \file H3DImageLoaderNode.h
/// \brief Header file for H3DImageLoaderNode, the abstract base
/// class for all image loaders for different file formats.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __H3DIMAGELOADERNODE_H__
#define __H3DIMAGELOADERNODE_H__

#include "Image.h"
#include "Node.h"

namespace H3D {

  /// \ingroup AbstractNodes
  /// \class H3DImageLoaderNode
  /// \brief H3DImageLoaderNode is a virtual base class for classes that read
  /// an image file format such as PNG or JPEG and construct an Image class
  /// with the data from the file.
  ///
  /// Each subclass must define the loadImage () function. 
  /// It is used by e.g. the ImageTexture node.
  ///
  class H3DAPI_API H3DImageLoaderNode : public Node {
  public:
    /// Constructor.
    H3DImageLoaderNode() {
      type_name = "H3DImageLoaderNode";
    }

    /// Pure virtual function to load an Image from a url.
    /// \returns An Image * with the image data loaded from the
    /// url.
    ///
    virtual Image *loadImage( const string &url ) = 0;
    /// TODO: Implement 
    ////virtual Image *loadImage( const istream &is ) = 0;
    
    /// Returns the default xml containerField attribute value.
    /// For this node it is "imageLoader".
    ///
    virtual string defaultXMLContainerField() {
      return "imageLoader";
    }
  };
}

#endif
