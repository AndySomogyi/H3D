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
/// \file H3DDynamicFieldsObject.h
/// \brief Header file for H3DDynamicFieldsObject.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __H3DDYNAMICFIELDSOBJECT_H__
#define __H3DDYNAMICFIELDSOBJECT_H__

#include <H3D/H3DNodeDatabase.h>
#include <H3DUtil/AutoPtrVector.h>
#include <H3D/Node.h>

namespace H3D {
  /// \ingroup AbstractInterface
  /// \class H3DDynamicFieldsObject
  /// \brief This abstract interface class is the base class for all classes
  /// that specify arbitrary fields.
  /// 
  class H3DAPI_API H3DDynamicFieldsObjectBase {
  public:

    /// Constructor
    H3DDynamicFieldsObjectBase() {}

    /// Destructor. Virtual to make H3DDynamicFieldsObjectBase a polymorphic type.
    virtual ~H3DDynamicFieldsObjectBase() {
    }

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

        DynamicFieldsMap::const_iterator j = object_dynamic_fields.find( name );
        if ( field && j == object_dynamic_fields.end() ) {
          field->setName( name );
          field->setAccessType( access );
          object_dynamic_fields.insert( std::pair< string, Field* >( name , field ) );
          return true;
        }

        return false;

    }

    /// Remove a field from the Node.
    /// \param name The name of the field to remove.
    /// \returns true on success false otherwise.
    inline virtual bool removeField ( const string& _name ) {

      DynamicFieldsMap::iterator j = object_dynamic_fields.find( _name );
      if ( j != object_dynamic_fields.end() ) {
        Field* f= j->second;
        if ( f ) {
          object_dynamic_fields.erase ( j );
          delete f;
          return true;
        }
      }
      return false;

    }

    /// Get a field from the dynamics object. Use this one if the instance is not a Node
    /// otherwise use getField instead.
    /// \param name The name of the field to get.
    /// \returns Field on success NULL otherwise.
    inline Field *getObjectField ( const string& _name ) const {
      DynamicFieldsMap::const_iterator j = object_dynamic_fields.find( _name );
      if ( j != object_dynamic_fields.end() ) {
        Field* f= j->second;
        return f;
      }
      return NULL;
    }

    /// Remove all dynamic fields that have been previously added.
    virtual void clearFields() {
      for( DynamicFieldsMap::iterator i = object_dynamic_fields.begin(); 
        i != object_dynamic_fields.end(); ++i ) {
          if( i->second )
            delete i->second;
      }
      object_dynamic_fields.clear();
    }

  protected:
    typedef map< string, Field* > DynamicFieldsMap;
    DynamicFieldsMap object_dynamic_fields;
  };


  /// \ingroup AbstractInterface
  /// \class H3DDynamicFieldsObject
  /// \brief This abstract interface class is the base class for all node
  /// types that specify arbitrary fields i.e. fields that can be 
  /// defined e.g. from X3D as in a prototype declaration.
  /// 
  class H3DAPI_API H3DDynamicFieldsObject : public H3DDynamicFieldsObjectBase {
  public:
    /// Constructor.
    /// \deprecated Use the one without explicit database pointer input ( the default )
    /// The argument to this constructor will be ignored.
    H3DDynamicFieldsObject( H3DNodeDatabase *_database ):
      H3DDynamicFieldsObjectBase(), database( NULL ), inherited_node( NULL ) {}

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
        if( n ) {
          if( !database.get() ) {
            H3DNodeDatabase *parent_db = H3DNodeDatabase::lookupTypeId( typeid( *n ) );
            database.reset( new H3DNodeDatabase( n, parent_db ) );
          }
          if( !database->getField( n, name ) ) {
            // Set the placeholder to the node address.
            inherited_node = n;
            field->setOwner( n );
            field->setName( name );
            field->setAccessType( access );
            database->addField( new DynamicFieldDBElement( database.get(),
              name,
              access,
              field ) );
            dynamic_fields.push_back( field );
            return true;
          }
        }
        return false;
    }
    
    /// Remove a field from the Node.
    /// \param name The name of the field to remove.
    /// \returns true on success false otherwise.
    inline virtual bool removeField ( const string& _name ) {
      if ( database.get() ) {
        Field* f= inherited_node->getField ( _name );
        AutoPtrVector< Field >::iterator i= find ( dynamic_fields.begin(), dynamic_fields.end(), f );
    
        if ( f && i != dynamic_fields.end() ) {
          if ( database->removeField ( _name, inherited_node ) ) {
            dynamic_fields.erase ( i );
            return true;
          }
        }
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

    /// Remove all dynamic fields that have been previously added.
    virtual void clearFields() {
      if( database.get() )
        database->clearDynamicFields(inherited_node);
      dynamic_fields.clear();
    }

  protected:
    auto_ptr< H3DNodeDatabase > database;
    AutoPtrVector< Field > dynamic_fields;
    // Holds a pointer to the node instance to which dynamic fields
    // belong to. Note that this pointer should only be used for pointer
    // comparasion and never for access to any node members.
    // The reason for using this pointer instead of this is because
    // H3DDynamicFieldsObject is used with multiple inheritance and
    // H3DDynamicFieldsObject is not a node class. This could result
    // in a destructor sequence for a node deleting the node base
    // class before the H3DDynamicFieldsObject base class which would
    // result in an invalid value for dynamic_cast< Node * >(this) 
    // and the database is then not cleaned up properly.
    Node * inherited_node;
  public:

    /// Constructor
    H3DDynamicFieldsObject():
        H3DDynamicFieldsObjectBase(), database( NULL ), inherited_node( NULL ) {}
  };
}

#endif
