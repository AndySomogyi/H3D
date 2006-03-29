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
/// \file H3DDynamicFieldsObject.h
/// \brief Header file for H3DDynamicFieldsObject.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __H3DDYNAMICFIELDSOBJECT_H__
#define __H3DDYNAMICFIELDSOBJECT_H__

#include "H3DNodeDatabase.h"
#include "AutoPtrVector.h"
#include "Node.h"

namespace H3D {
  /// \ingroup AbstractInterface
  /// \class H3DDynamicFieldsObject
  /// \brief This abstract interface class is the base class for all node
  /// types that specify arbitrary fields i.e. fields that can be 
  /// defined e.g. from X3D as in a prototype declaration.
  /// 
  class H3DAPI_API H3DDynamicFieldsObject {
  public:
    /// Constructor. 
    H3DDynamicFieldsObject( H3DNodeDatabase *_database ):
      database( _database ) {}

    /// Destructor. Virtual to make H3DDynamicFieldsObject a polymorphic type.
    virtual ~H3DDynamicFieldsObject() {}

    /// Add a field to the Node. 
    /// \param name The name of the field.
    /// \param access The access type of the field.
    /// \param field The field to add to the node. The ownership of the 
    /// Field pointer is handed over to the Node and will be destructed
    /// when the Node is destructed.
    /// \returns true on success, false otherwise.
    inline virtual bool addField( const string &name,
                                  const Field::AccessType &access,
                                  Field *field ) {
      Node *n = dynamic_cast< Node * >( this );
      if( n && !database->getField( n, name.c_str() ) ) {
        field->setOwner( n );
        field->setName( name );
        database->addField( new DynamicFieldDBElement( database,
                                                       name.c_str(),
                                                       access,
                                                       field ) );
        dynamic_fields.push_back( field );
        return true;
      }
      return false;
    }
    
    typedef AutoPtrVector< Field >::const_iterator field_iterator;

    /// Get an iterator to the first of the dynamic fields.
    inline field_iterator firstField() {
      return dynamic_fields.begin();
    }

    /// Get an iterator pointing to the end of the dynamic fields.
    inline field_iterator endField() {
      return dynamic_fields.end();
    }

    /// Remove a field from the Node.
    /// \param name The name of the field to remove.
    /// \returns true on success false otherwise.
    //    bool removeField( const string &name );

  protected:
    H3DNodeDatabase *database;
    AutoPtrVector< Field > dynamic_fields;
  };
}

#endif