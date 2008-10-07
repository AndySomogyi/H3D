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
/// \file PythonMethods.h
/// \brief Header file for methods used in the H3D module of the 
/// PythonScript node.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __PYTHONMETHODS_H__
#define __PYTHONMETHODS_H__
#include <H3D/Field.h>
#include <H3D/X3DTypes.h>
#include <H3D/X3DFieldConversion.h>
#include <H3D/X3DTypeFunctions.h>

#ifdef HAVE_PYTHON
#if defined(_MSC_VER)
// undefine _DEBUG since we want to always link to the release version of
// python and pyconfig.h automatically links debug version if _DEBUG is
// defined.
#ifdef _DEBUG
#define _DEBUG_UNDEFED
#undef _DEBUG
#endif
#endif
#if defined(__APPLE__) && defined(__MACH__)
#include <Python/Python.h>
#else
#include <Python.h>
#endif
#if defined(_MSC_VER)
// redefine _DEBUG if it was undefed
#ifdef _DEBUG_UNDEFED
#define _DEBUG
#endif
#endif

namespace H3D {
  namespace PythonInternals {
    PyMODINIT_FUNC initH3D();
    
    double pyObjectToDouble( PyObject *v );

    void fieldDestructor( void *f );

    PyObject *fieldAsPythonObject( Field * f, bool destruct=false );

    PyObject *pythonCreateField( PyObject *self, PyObject *args );
    
    PyObject *pythonSetFieldValueFromObject( Field *field_ptr, PyObject *v );

    PyObject *pythonSetFieldValue( PyObject *self, PyObject *args );

    PyObject *pythonGetFieldValue( PyObject *self, PyObject *arg );

    PyObject *pythonRouteField( PyObject *self, PyObject *args );

    PyObject *pythonRouteFieldNoEvent( PyObject *self, PyObject *args );

    PyObject *pythonUnrouteField( PyObject *self, PyObject *args );

    PyObject *pythonGetCPtr( PyObject *self, PyObject *arg  );

    PyObject* pythonCreateX3DFromURL( PyObject *self, PyObject *arg );

    PyObject* pythonCreateX3DFromString( PyObject *self, PyObject *arg );

    PyObject* pythonCreateX3DNodeFromURL( PyObject *self, PyObject *arg );

    PyObject* pythonCreateX3DNodeFromString( PyObject *self, PyObject *arg );

    PyObject* pythonCreateVRMLFromURL( PyObject *self, PyObject *arg );

    PyObject* pythonCreateVRMLFromString( PyObject *self, PyObject *arg );

    PyObject* pythonCreateVRMLNodeFromURL( PyObject *self, PyObject *arg );

    PyObject* pythonCreateVRMLNodeFromString( PyObject *self, PyObject *arg );

    PyObject* pythonGetRoutesIn( PyObject *self, PyObject *arg );

    PyObject* pythonGetRoutesOut( PyObject *self, PyObject *arg );

    PyObject* pythonGetCurrentScenes( PyObject *self, PyObject *arg );

    PyObject* pythonGetActiveDeviceInfo( PyObject *self, PyObject *arg );

    PyObject* pythonGetActiveBindableNode( PyObject *self, PyObject *arg );

    PyObject* pythonGetActiveViewpoint( PyObject *self, PyObject *arg );

    PyObject* pythonGetActiveNavigationInfo( PyObject *self, PyObject *arg );

    PyObject* pythonGetActiveStereoInfo( PyObject *self, PyObject *arg );

    PyObject* pythonGetActiveBackground( PyObject *self, PyObject *arg );

    PyObject* pythonGetActiveFog( PyObject *self, PyObject *arg );

    PyObject* pythonGetActiveGlobalSettings( PyObject *self, PyObject *arg );

    PyObject* pythonEraseElementFromMField( PyObject *self, PyObject *arg );

    PyObject* pythonPushBackElementInMField( PyObject *self, PyObject *arg );

    PyObject* pythonMFieldClear( PyObject *self, PyObject *arg );

    PyObject* pythonMFieldBack( PyObject *self, PyObject *arg );

    PyObject* pythonMFieldFront( PyObject *self, PyObject *arg );

    PyObject* pythonMFieldEmpty( PyObject *self, PyObject *arg );

    PyObject* pythonMFieldPopBack( PyObject *self, PyObject *arg );

    PyObject* pythonTouchField( PyObject *self, PyObject *arg );

    PyObject* pythonResolveURLAsFile( PyObject *self, PyObject *arg );
    
  }

    
  /// 
  /// An exception thrown when a field is of the wrong type
  /// when it is checked. Internal to the H3D::TypedFieldCheck classes.
  /// 
  H3D_VALUE_EXCEPTION( string, PythonInvalidFieldType );

  struct PythonFieldBase {
    PythonFieldBase( void *_python_field ) :
      python_field( _python_field ),
      python_update( 0 ),
      python_typeinfo( 0 ),
      python_opttypeinfo( 0 ){}
    
    void *python_field;
    void *python_update;
    void *python_typeinfo;
    void *python_opttypeinfo;
  };
  
  template< class F >
  struct PythonField : public F, PythonFieldBase {
    
    PythonField( void *_python_field ) : 
      PythonFieldBase( _python_field ) {
    }
    virtual void update() {
      if ( python_update ) {
        PyErr_Clear();
        PyObject *args = PyTuple_New(1);
        PyObject *f = PythonInternals::fieldAsPythonObject( this->event.ptr, false );
        PyTuple_SetItem( args, 0, f );
        PyObject *r = PyEval_CallObject( static_cast< PyObject * >(python_update), 
                                         args );
        ////const char *value = PyString_AsString( PyObject_Repr( r ) );
        Py_DECREF( args );
        if( r == Py_None ) {
          Console(3) << "Warning: update()-function for Python defined field of type " 
                     << this->getFullName() << " does not return a value. "<< endl;
          Py_DECREF( r );
        } else if ( r ) {
          if( !PythonInternals::pythonSetFieldValueFromObject( this, r ) ) {
            Console(3) << "Warning: invalid return value from update()-function"
                       << " for Python defined field of type " 
                       << this->getFullName() << endl;
          }
          Py_DECREF( r );
        } else {
          PyErr_Print();
        }        
      } else {
        F::update();
      }
    }
    
    /// Function for checking that a field is of a correct type 
    /// given type specification according to the arguments specified
    /// during the construction of the Python field class.
    /// \param f The field to check the type of
    /// \param index The index of the field in the template
    /// type list.
    /// \throws PythonInvalidFieldType
    void checkFieldType( Field *f, int index ) {
      if( python_typeinfo == 0 && python_opttypeinfo == 0 ) {
        F::checkFieldType( f, index );
        return;
      }
      int arg_size = -1;      
      if( python_typeinfo )
        arg_size = PyTuple_Size( static_cast< PyObject * >(python_typeinfo) );

      if ( index >= arg_size ) {
        int opt_arg_size = -1;
        if( python_opttypeinfo )
          opt_arg_size = PyTuple_Size( static_cast< PyObject * >(python_opttypeinfo) );
        if( opt_arg_size > 0 ) {
          PyObject *type_info = 
            PyTuple_GetItem( static_cast< PyObject * >(python_opttypeinfo),
                             0 );
          PyObject *type_id = PyObject_GetAttrString( type_info, "type" );
          int type_int = PyInt_AsLong( type_id );
          if ( f->getTypeName().compare( 
           X3DTypes::typeToString( (X3DTypes::X3DType)type_int ) ) != 0 ) {
            ostringstream err;
            err << "Bad input, expected " 
                << X3DTypes::typeToString( (X3DTypes::X3DType)type_int )  
                << " got " << f->getTypeName() << " for route" << index;
            throw H3D::PythonInvalidFieldType( err.str(), "", 
                                               H3D_FULL_LOCATION );
      }
        } else {
          ostringstream err;
          err << "Too many inputs, expected " << arg_size+1;
          throw H3D::PythonInvalidFieldType( err.str(), "", 
                                             H3D_FULL_LOCATION );
        }
      } else {
      PyObject *type_info = 
        PyTuple_GetItem( static_cast< PyObject * >(python_typeinfo),
                         index );
      PyObject *type_id = PyObject_GetAttrString( type_info, "type" );
      int type_int = PyInt_AsLong( type_id );
      //if( ! t || ! PyInt_Check( t ) )

      // check that field type is of correct type. The comparison
      // with UNKNOWN_X3D_TYPE is to allow Python fields to accept
      // Field as input, thus allowing any field type to be routed
      // to it.
      if ( f->getTypeName().compare( 
           X3DTypes::typeToString( (X3DTypes::X3DType)type_int ) ) != 0 &&
           strcmp( X3DTypes::typeToString( (X3DTypes::X3DType)type_int ),
                   "UNKNOWN_X3D_TYPE" ) != 0 ) {
        ostringstream err;
        err << "Bad input, expected " 
            << X3DTypes::typeToString( (X3DTypes::X3DType)type_int )  
            << " got " << f->getTypeName() << " for route" << index;
        throw H3D::PythonInvalidFieldType( err.str(), "", 
                                           H3D_FULL_LOCATION );
      }
      }
    }
  };
}

#endif // HAVE_PYTHON
#endif