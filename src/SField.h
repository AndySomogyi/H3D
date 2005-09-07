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
// \file SField.h Contains the SField template class.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __SFIELD_H__
#define __SFIELD_H__

#include "X3DFieldConversion.h"
#include "TypedField.h"
#include "Node.h"

namespace H3D {

  /// \defgroup FieldTemplateModifiers Field template modifiers.
  /// Template classes for modifying the behaviour of Field classes.
 
  //
  /// Template class that adds the Field mechanisms to an encapsulated value
  /// of specified type.
  /// 
  /// \param Type The type of the value.
  /// \ingroup FieldTemplateModifiers
  template< class Type > 
  class SField: public TypedField< ParsableField,
                                   void,
                                   AnyNumber< SField< Type> > > {
  public:   
    /// The type of the value member. 
    typedef Type value_type;

    /// Constructor.
    SField() {}
        
    /// Constructor.
    SField( const Type &_value ) {
      value = _value;
    }

    /// Set the value of the field.
    inline virtual void setValue( const Type &v, int id = 0 );
    /// Get the value of the field.
    inline virtual const Type &getValue( int id = 0 );
        
    /// Set the value of the field given a string. By default
    /// we try to parse the values according to the X3D/XML 
    /// specification.
    ///
    inline virtual void setValueFromString( const string &s ) {
      this->setValue( X3D::X3DStringToValue< Type >( s ) ); 
    }

    /// Get the value of the field as a string.
    inline virtual string getValueAsString( const string& separator = " " ) {
      stringstream s;
      s << getValue();
      return s.str();
    }
  
    /// Returns a string name for this field type e.g. SFInt32
    virtual string getTypeName() {
      return classTypeName();
    }

    /// Returns a string name for this field type e.g. SFInt32
	static string classTypeName() { return typeid( SField< Type > ).name(); }

  protected:
    /// Make the field up to date given that an event has occured.
    inline virtual void update();

    /// The encapsulated value.
    Type value;

  };

  template< class Type  >
  void SField< Type >::update() {
#ifdef DEBUG
    cerr << "SField< " << typeid( Type ).name() 
         << " >(" << this->name << ")::update()" << endl;
#endif
    if( this->owner )
      this->value = 
        static_cast< SField<Type>* >
        (this->event.ptr)->getValue( this->owner->id );
    else 
      this->value = 
        static_cast< SField<Type>* >(this->event.ptr)->getValue();
  }

  template< class Type >
  void SField<Type>::setValue( const Type &v, int id ) {
#ifdef DEBUG
    cerr << "SField< " << typeid( Type ).name() 
         << " >(" << this->name << ")::setValue()" << endl;
#endif
    // check that we have the correct access type
    this->checkAccessTypeSet( id );
    
    value = v;
    // reset the event pointer since we want to ignore any pending
    // events when the field is set to a new value.
    this->event.ptr = NULL;
    // generate an event.
    this->startEvent();
  }

  template< class Type >
  const Type &SField<Type>::getValue( int id ) {
#ifdef DEBUG
    cerr << "SField< " << typeid( Type ).name() 
         << " >(" <<  this->name << ")::getValue()" << endl;
#endif
    // check that we have the correct access type
    this->checkAccessTypeGet( id );

    // check that the field is up-to-date first
    this->upToDate();
    return value;
  }
}
#endif
