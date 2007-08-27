//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2007, SenseGraphics AB
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
/// \file RefCountMField.h 
/// \brief Contains the RefCountMField class.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __REFCOUNTMFIELD_H__
#define __REFCOUNTMFIELD_H__

#include <MField.h>
#include <MFNodeAutoRefVector.h>

namespace H3D {

  /// NodeVector is a vector of nodes that manages reference counting
  /// on the Nodes added to or removed from it.
  /// 
  typedef AutoRefVector< Node > NodeVector;
  
  /// RefCountMField is almost like any MField but it encapsulates a
  /// vector of RefCoundtedClass pointers. Reference counting on the
  /// encapsulated RefCountedClass instances are upheld when changes
  /// to the values are made.
  /// \ingroup FieldTemplateModifiers
  template< class RefClass >
  class RefCountMField: 
    public MFieldBase< RefClass *, 
                       FieldInternals::MFNodeAutoRefVector< RefClass >,
                       Field > {
  public:   
    
    typedef MFieldBase< RefClass *, 
                        FieldInternals::MFNodeAutoRefVector< RefClass >,
                        Field > BaseFieldType;

    /// Thrown if the index given to getValueByIndex() is outside the 
    /// boundaries.
    H3D_VALUE_EXCEPTION( typename BaseFieldType::size_type, InvalidIndex );


    /// Default constructor. Creates an empty RefCountMField.
    RefCountMField() {
      this->value.owner = this;
    }

    /// Creates an RefCountMField with space reserved for size nodes.
    RefCountMField( typename FieldInternals::MFNodeAutoRefVector< RefClass >::size_type sz ) : 
      MFieldBase< RefClass *, 
                  FieldInternals::MFNodeAutoRefVector< RefClass >,
                  Field >( sz ){
      this->value.owner = this;
    }
    
    ~RefCountMField() {
      this->value.clear();
    };

    /// Get the value of the MField.
    inline virtual const NodeVector &getValue( int id = 0 ) {
      this->checkAccessTypeGet( id );
      this->upToDate();
      return this->value;
    }

    /// Get the value of the MField.
    inline virtual RefClass * getValueByIndex( 
                                              typename BaseFieldType::size_type i,
                                              int id = 0 ) {
      this->checkAccessTypeGet( id );
      this->upToDate();
      if( i < 0 || i >= this->value.size() ) {
        stringstream s;
        s << "Trying to access value outside the bounds of field "
          << this->getFullName() << ". Field has size " << this->value.size()
          << ". ";
        throw InvalidIndex( i, s.str(), H3D_FULL_LOCATION );
      }
      return this->value[i];    
    }
    
    /// TEMPORARILY REMOVED: causes INTERNAL_COMPILER_ERROR
    /// Get the value of an element of the MField.
    /// \param i The index of the element.
    ///
    ///inline virtual const Type &getValue( size_type i );
    
    inline void setValue( typename FieldInternals::MFNodeAutoRefVector< RefClass >::size_type i,
                          const typename FieldInternals::MFNodeAutoRefVector< RefClass >::value_type &v,
                          int id = 0 ) {
      this->checkAccessTypeSet( id );
      this->value.set( i, v );
      // generate an event.
      this->startEvent();                                  
    }
    
    /// Set the value of the field.
    /// \param The new value.
    inline virtual void setValue( const AutoRefVector< RefClass > &v,
                                  int id = 0  ) {
      this->checkAccessTypeSet( id );
      this->value = v;
      this->startEvent();
    }
    
    /// Set the value of the field.
    /// \param The new value.
    inline virtual void setValue( const vector< RefClass * > &v, int id = 0 ) {
      this->checkAccessTypeSet( id );
      this->value = v;
      this->startEvent();
    }


    /// Swaps the contents of two vectors.
    inline virtual void swap( vector< RefClass * > &v, int id = 0 ) {
      this->checkAccessTypeSet( id );
      this->checkAccessTypeGet( id );
      this->upToDate();
      this->value.swap( v );
      this->startEvent();
    }

    /// Erase the first element equal to a.
    inline virtual void erase( RefClass *a, int id = 0 ) {
      this->checkAccessTypeSet( id );
      this->upToDate();
      this->value.erase( a );
    } 
    
    /// Returns a string name for this field type i.e. SFNode
    virtual string getTypeName() {
      return classTypeName();
    }
    
    /// Returns a string name for this field type i.e. SFNode
    static string classTypeName() {
      return "RefCountMField";
    }
    
  protected:
    friend class FieldInternals::MFNodeAutoRefVector<RefClass>;
    
    /// Make the field up to date given that an event has occured.
    inline virtual void update() {
      this->value = static_cast< RefCountMField* >(this->event.ptr)->getValue();
    }

    virtual RefClass *preOnAdd( RefClass *n ) {
      return n;
    }

    virtual RefClass *preOnRemove( RefClass *n ) {
      return n;
    }
    
    /// This function will be called when values of SFNode
    /// changes. As soon as a Node is added onAdd will
    /// be called on the added value. 
    /// \param n The added value.
    ///
    virtual void onAdd( RefClass *n ) {
#ifdef REF_COUNT_DEBUG
      Console(1) << "RefCountMField::onAdd( " << getFullName() << " = " << this 
                 << ", n = " << n << ") " << endl;
#endif
    }
    
    /// This function will be called when values of SFNode
    /// changes. As soon as a Node is removed onRemove will
    /// be called on the removed value. 
    /// \param n The removed value.
    ///
    virtual void onRemove( RefClass *n ) {
#ifdef REF_COUNT_DEBUG
      Console(1) << "RefCountMField::onRemove( " << getFullName() << " = " 
                 << this << ", n = " << n << ") " << endl;
#endif
    }
  };

}

#endif