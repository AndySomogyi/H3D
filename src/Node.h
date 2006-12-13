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
/// \file Node.h
/// \brief Node base class
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __NODE_H__
#define __NODE_H__

#include "Instantiate.h"
#include <H3DNodeDatabase.h>
#include "RefCountedClass.h"
#ifdef USE_HAPTICS
#include "TraverseInfo.h"
#endif
#include "Console.h"

//#include "FieldTemplates.h"

using namespace std;

namespace H3D {

  /// \defgroup Nodes Scene-graph nodes.
  ///
  /// \class Node
  /// \brief Node is the base class for all classes that can be part of
  /// the H3D scene-graph.
  class H3DAPI_API Node: public RefCountedClass {
  public:
 
    /// Constructor.
    Node();
    
    /// Destructor.
    virtual ~Node() {};

    template< class N >
    static Node *newInstance() { return new N; };

    /// the render() function is used for the depth-first rendering traversal 
    /// of the scene-graph. 
    virtual void render()     { };

#ifdef USE_HAPTICS
		/// traverseSG is called onve per scenegraph loop on the scene in order to
    /// traverse the scenegraph. During this traversal things can be updated
    /// in the node and HapticObject instances to be rendered should be added 
    /// to the TraverseInfo object.
    /// \param ti The TraverseInfo object containing information about the
    /// traversal.
    /// 
    virtual void traverseSG( TraverseInfo &ti ) {}
#endif

    /// Detect intersection between a line from "from" to "to"
    /// and a Node.
    virtual bool lineIntersect( 
      const Vec3f &from, 
      const Vec3f &to,    
      vector< HAPI::Bounds::IntersectionInfo > &result,
      bool global,
      vector< X3DGeometryNode * > &theGeometry,
      vector< H3DInt32 > &theGeometryIndex ) {
      return false;
    }
    
    /// Returns the default xml containerField attribute value.
    /// For this node it is "children".
    ///
    virtual string defaultXMLContainerField() {
      return "children";
    }
    
    /// return a pointer to the field specified by name within this instance 
    virtual Field *getField( const string &name );
    
    friend class Field;
    template <class Type>
    friend class SField;
    template <class Type>
    friend class MField;
    template<class RefClass, class BaseField>
    friend class RefCountSField;

  protected:
    static H3DNodeDatabase database;
    int id;
    static int nr_nodes_created;
  };

};

#endif