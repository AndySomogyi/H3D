////////////////////////////////////////////////////////////////////////////////
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
/// \file PythonMethods.cpp
/// \brief Definitions of the methods used in the H3D module of the 
/// PythonScript node.
///
//
//////////////////////////////////////////////////////////////////////////////

#include "PythonMethods.h"
#include "DeviceInfo.h"
#include "Viewpoint.h"
#include "NavigationInfo.h"
#include "StereoInfo.h"
#include "X3DBackgroundNode.h" 
#include "X3DTypes.h"
#include "PythonTypes.h"
#include "MFNode.h"
#include "X3D.h"
#include "Scene.h"
#include <sstream>
#include <cctype>
using namespace H3D;
using namespace PythonInternals;

namespace H3D {
  namespace PythonInternals {

    H3D_VALUE_EXCEPTION( string, UnableToCreatePythonField );

    PyObject *H3D_module = 0;
    PyObject *H3D_dict = 0;
    PyObject *H3DInterface_module = 0;
    PyObject *H3DInterface_dict = 0;
    
    // Utility functions
    void insertFieldTypes( PyObject *dict ) {
      for( int i = 0; i <= X3DTypes::UNKNOWN_X3D_TYPE; i++ ) {
        // insert each type from the enumerated list into Python
        //cerr << X3DTypes::typeToString( (X3DTypes::X3DType)i ) <<  endl;
        string str = X3DTypes::typeToString( (X3DTypes::X3DType)i );
        for( string::iterator x = str.begin(); x != str.end(); x++ )
          *x = std::toupper( *x );
        PyDict_SetItem( dict, 
                        PyString_FromString( str.c_str() ), 
                        PyInt_FromLong( i ) );
      }
      
    }

    // returns a PyTuple containing python versions of the
    // given node and DEFNodes.
    PyObject *createX3DHelp( Node *n, X3D::DEFNodes *dn ) {
      // make a PyNode from the created node.
      PyObject* python_node = PyNode_FromNode( n );

      // make a python dict out of the DEFNodes.
      PyObject *py_def_map = PyDict_New();
      for( X3D::DEFNodes::const_iterator i = dn->begin(); 
           i != dn->end(); i++ ) {
        PyObject *node = PyNode_FromNode( (*i).second );
        PyDict_SetItemString( py_def_map, 
                              (*i).first.c_str(), 
                              node);
        Py_DECREF( node );
      }
      // create the tuple to return
      PyObject *res = Py_BuildValue( "(OO)", python_node, py_def_map );
      Py_DECREF( py_def_map );
      Py_DECREF( python_node );
      return res;
    }
    
    static PyMethodDef H3DMethods[] = {
      { "createField", pythonCreateField, 0 },
      { "setFieldValue", pythonSetFieldValue, 0 },
      { "getFieldValue", pythonGetFieldValue, 0 },
      { "routeField", pythonRouteField, 0 },
      { "routeFieldNoEvent", pythonRouteFieldNoEvent, 0 },
      { "unrouteField", pythonUnrouteField, 0 },
      { "createX3DFromURL", pythonCreateX3DFromURL, 0 },
      { "createX3DFromString", pythonCreateX3DFromString, 0 },
      { "createX3DNodeFromURL", pythonCreateX3DNodeFromURL, 0 },
      { "createX3DNodeFromString", pythonCreateX3DNodeFromString, 0 },
      { "getRoutesIn", pythonGetRoutesIn, 0 },
      { "getRoutesOut", pythonGetRoutesOut, 0 },
      { "getCurrentScenes", pythonGetCurrentScenes, 0 },
      { "getActiveDeviceInfo", pythonGetActiveDeviceInfo, 0 },
      { "getActiveViewpoint", pythonGetActiveViewpoint, 0 },
      { "getActiveNavigationInfo", pythonGetActiveNavigationInfo, 0 },
      { "getActiveStereoInfo", pythonGetActiveStereoInfo, 0 },
      { "getActiveBackground", pythonGetActiveBackground, 0 },
      { NULL, NULL }      
    };
    
    PyMODINIT_FUNC initH3D() {
      H3D_module = Py_InitModule( "H3D", H3DMethods );
      H3D_dict = PyModule_GetDict( H3D_module );

      PyNode::installType( H3D_module );     
      PyVec2f::installType( H3D_module );     
      PyVec2d::installType( H3D_module );     
      PyVec3f::installType( H3D_module );     
      PyVec3d::installType( H3D_module );     
      PyVec4f::installType( H3D_module );     
      PyVec4d::installType( H3D_module );     
      PyRGB::installType( H3D_module );     
      PyRGBA::installType( H3D_module );     
      PyRotation::installType( H3D_module );      
      PyQuaternion::installType( H3D_module );
      PyMatrix3f::installType( H3D_module );     
      PyMatrix4f::installType( H3D_module );
      PythonInternals::insertFieldTypes( H3D_dict );

      PythonInternals::H3DInterface_module = 
        PyImport_ImportModule( "H3DInterface" );
      if ( PythonInternals::H3DInterface_module == NULL ) {
        PyErr_Print();
        cerr << "PythonScript::initialiseParser() ERROR importing H3DInterface, check your PYTHONPATH" << endl;
        exit(-1); // SHOULD THROW AN ERROR!
      }

      PythonInternals::H3DInterface_dict = 
        PyModule_GetDict( PythonInternals::H3DInterface_module );

      if ( PythonInternals::H3DInterface_dict == NULL )
        PyErr_Print();
      PyObject *time = (PyObject*)fieldAsPythonObject( Scene::time );
      PyDict_SetItem( PythonInternals::H3DInterface_dict, 
                      PyString_FromString( "time" ), 
                      time );
      
    };
  


    void *fieldAsPythonObject( Field * f ) {
      PyObject *obj = PyDict_GetItemString( PythonInternals::H3DInterface_dict, 
                                            f->getTypeName().c_str() );
      if( ! obj || ! PyClass_Check( obj ) ) {
        PyErr_Print();
        throw UnableToCreatePythonField( "fieldAsPythonObject()", 
                                         f->classTypeName().c_str() );
      }
      // create the field instance with the owner flag set to zero.
      PyObject *py_field = PyInstance_New( obj, NULL, NULL );
      if ( !py_field ) {
        PyErr_Print();
        throw UnableToCreatePythonField( "fieldAsPythonObject()", "" );
      }
      PyObject *field_ptr = PyCObject_FromVoidPtr( f, NULL );
      PyObject_SetAttrString( py_field, "__fieldptr__", field_ptr );
      Py_DECREF( field_ptr);
      return (void*)py_field;      
    }


    double pyObjectToDouble( PyObject *v ) {
      if ( v ) {
        if ( PyFloat_Check( v ) )
          return PyFloat_AS_DOUBLE( v );
        if ( PyInt_Check( v ) )
          return PyInt_AS_LONG( v );
      }
      // TODO: else throw error
      return 0.f;
    }

    // Methods for the H3D Module:
    
    PyObject *pythonCreateField( PyObject *self, PyObject *args ) {
      if( !args || !PyTuple_Check( args ) || PyTuple_Size( args ) != 2  ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.createField( Field, int auto_update )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }
      PyObject *field = PyTuple_GetItem( args, 0 );
      PyObject *autoupdateobj = PyTuple_GetItem( args, 1 );
      bool autoupdate = PyInt_AsLong( autoupdateobj ) != 0;
      
      if( field && PyInstance_Check( field ) ) {
        //PyObject_SetAttrString( field, "__fieldtype__", field );

        PyObject *fieldtype = PyObject_GetAttrString( field, "type" );
        int field_type = PyInt_AsLong( fieldtype );
        Field *f;

        if ( autoupdate ) {
          switch( field_type ) {
          case X3DTypes::SFFLOAT: 
            f = new PythonField< AutoUpdate< SFFloat > >( field ); break;
          case X3DTypes::MFFLOAT: 
            f = new PythonField< AutoUpdate< MFFloat > >( field ); break;
          case X3DTypes::SFDOUBLE:
            f = new PythonField< AutoUpdate< SFDouble > >( field ); break;
          case X3DTypes::MFDOUBLE:
            f = new PythonField< AutoUpdate< MFDouble > >( field ); break;
          case X3DTypes::SFTIME:  
            f = new PythonField< AutoUpdate< SFTime > >( field ); break;
          case X3DTypes::MFTIME:  
            f = new PythonField< AutoUpdate< MFTime > >( field ); break;
          case X3DTypes::SFINT32: 
            f = new PythonField< AutoUpdate< SFInt32 > >( field ); break;
          case X3DTypes::MFINT32: 
            f = new PythonField< AutoUpdate< MFInt32 > >( field ); break;
          case X3DTypes::SFVEC2F: 
            f = new PythonField< AutoUpdate< SFVec2f > >( field ); break;
          case X3DTypes::MFVEC2F: 
            f = new PythonField< AutoUpdate< MFVec2f > >( field ); break;
          case X3DTypes::SFVEC2D: 
            f = new PythonField< AutoUpdate< SFVec2d > >( field ); break;
          case X3DTypes::MFVEC2D: 
            f = new PythonField< AutoUpdate< MFVec2d > >( field ); break;
          case X3DTypes::SFVEC3F:
            f = new PythonField< AutoUpdate< SFVec3f > >( field ); break;
          case X3DTypes::MFVEC3F: 
            f = new PythonField< AutoUpdate< MFVec3f > >( field ); break;
          case X3DTypes::SFVEC3D: 
            f = new PythonField< AutoUpdate< SFVec3f > >( field ); break;
          case X3DTypes::MFVEC3D: 
            f = new PythonField< AutoUpdate< MFVec3d > >( field ); break;
          case X3DTypes::SFBOOL:  
            f = new PythonField< AutoUpdate< SFBool > >( field ); break;
          case X3DTypes::MFBOOL:  
            f = new PythonField< AutoUpdate< MFBool > >( field ); break;
          case X3DTypes::SFSTRING:
            f = new PythonField< AutoUpdate< SFString > >( field ); break;
          case X3DTypes::MFSTRING:
            f = new PythonField< AutoUpdate< MFString > >( field ); break;
          case X3DTypes::SFNODE:
            f = new PythonField< AutoUpdate< SFNode > >( field ); break;
          case X3DTypes::MFNODE:
            f = new PythonField< AutoUpdate< MFNode > >( field ); break;
          case X3DTypes::SFCOLOR:   
            f = new PythonField< AutoUpdate< SFColor > >( field ); break;
          case X3DTypes::MFCOLOR:   
            f = new PythonField< AutoUpdate< MFColor > >( field ); break;
          case X3DTypes::SFCOLORRGBA:    
            f = new PythonField< AutoUpdate< SFColorRGBA > >( field ); break;
          case X3DTypes::MFCOLORRGBA:    
            f = new PythonField< AutoUpdate< MFColorRGBA > >( field ); break;
          case X3DTypes::SFROTATION:
            f = new PythonField< AutoUpdate< SFRotation > >( field ); break;
          case X3DTypes::MFROTATION:
            f = new PythonField< AutoUpdate< MFRotation > >( field ); break;
          case X3DTypes::SFMATRIX3F:
            f = new PythonField< AutoUpdate< SFMatrix3f > >( field ); break;
          case X3DTypes::MFMATRIX3F:
            f = new PythonField< AutoUpdate< MFMatrix3f > >( field ); break;
          case X3DTypes::SFMATRIX4F:
            f = new PythonField< AutoUpdate< SFMatrix4f > >( field ); break;
          case X3DTypes::MFMATRIX4F:
            f = new PythonField< AutoUpdate< MFMatrix4f > >( field ); break;
          case X3DTypes::UNKNOWN_X3D_TYPE: 
          default: f = 0; //THROW ERROR
          };
          
        } else {
          switch( field_type ) {
          case X3DTypes::SFFLOAT: 
            f = new PythonField< SFFloat >( field ); break;
          case X3DTypes::MFFLOAT: 
            f = new PythonField< MFFloat >( field ); break;
          case X3DTypes::SFDOUBLE:
            f = new PythonField< SFDouble >( field ); break;
          case X3DTypes::MFDOUBLE:
            f = new PythonField< MFDouble >( field ); break;
          case X3DTypes::SFTIME:  
            f = new PythonField< SFTime >( field ); break;
          case X3DTypes::MFTIME:  
            f = new PythonField< MFTime >( field ); break;
          case X3DTypes::SFINT32: 
            f = new PythonField< SFInt32 >( field ); break;
          case X3DTypes::MFINT32: 
            f = new PythonField< MFInt32 >( field ); break;
          case X3DTypes::SFVEC2F: 
            f = new PythonField< SFVec2f >( field ); break;
          case X3DTypes::MFVEC2F: 
            f = new PythonField< MFVec2f >( field ); break;
          case X3DTypes::SFVEC2D: 
            f = new PythonField< SFVec2d >( field ); break;
          case X3DTypes::MFVEC2D: 
            f = new PythonField< MFVec2d >( field ); break;
          case X3DTypes::SFVEC3F: 
            f = new PythonField< SFVec3f >( field ); break;
          case X3DTypes::MFVEC3F: 
            f = new PythonField< MFVec3f >( field ); break;
          case X3DTypes::SFVEC3D: 
            f = new PythonField< SFVec3f >( field ); break;
          case X3DTypes::MFVEC3D: 
            f = new PythonField< MFVec3d >( field ); break;
          case X3DTypes::SFBOOL:  
            f = new PythonField< SFBool >( field ); break;
          case X3DTypes::MFBOOL:  
            f = new PythonField< MFBool >( field ); break;
          case X3DTypes::SFSTRING:
            f = new PythonField< SFString >( field ); break;
          case X3DTypes::MFSTRING:
            f = new PythonField< MFString >( field ); break;
          case X3DTypes::SFNODE:
            f = new PythonField< SFNode >( field ); break;
          case X3DTypes::MFNODE:
            f = new PythonField< MFNode >( field ); break;
          case X3DTypes::SFCOLOR:   
            f = new PythonField< SFColor >( field ); break;
          case X3DTypes::MFCOLOR:   
            f = new PythonField< MFColor >( field ); break;
          case X3DTypes::SFCOLORRGBA:    
            f = new PythonField< SFColorRGBA >( field ); break;
          case X3DTypes::MFCOLORRGBA:    
            f = new PythonField< MFColorRGBA >( field ); break;
          case X3DTypes::SFROTATION:
            f = new PythonField< SFRotation >( field ); break;
          case X3DTypes::MFROTATION:
            f = new PythonField< MFRotation >( field ); break;
          case X3DTypes::SFMATRIX3F:
            f = new PythonField< SFMatrix3f >( field ); break;
          case X3DTypes::MFMATRIX3F:
            f = new PythonField< MFMatrix3f >( field ); break;
          case X3DTypes::SFMATRIX4F:
            f = new PythonField< SFMatrix4f >( field ); break;
          case X3DTypes::MFMATRIX4F:
            f = new PythonField< MFMatrix4f >( field ); break;
          case X3DTypes::UNKNOWN_X3D_TYPE: 
          default: f = 0; //THROW ERROR
          };
          
        }
        
        PyObject *pfield = PyCObject_FromVoidPtr( f, 0 );
        PyObject_SetAttrString( field, "__fieldptr__", pfield );
        // field now holds a reference to pfield so we can remove the extra reference
        // from the all to PyCObject_FromVoidPtr()
        Py_DECREF( pfield );
        PyObject *update = PyObject_GetAttrString( field, "update" );
        if( update ) {
          if( PyMethod_Check( update ) ) {
            dynamic_cast< PythonFieldBase* >(f)->python_update = update;
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "Symbol 'update' must be a method!" );
            return 0;
          }
        } else PyErr_Clear();
  
        PyObject *typeinfo = PyObject_GetAttrString( field, "__type_info__" );
        if( typeinfo ) {
          if( PyTuple_Check( typeinfo ) ) {
            dynamic_cast< PythonFieldBase* >(f)->python_typeinfo = typeinfo;
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "Symbol '__type_info__' must be a tuple!" );
            return NULL;
          }
        } else PyErr_Clear();
        
        //initField( (PyInstanceObject*) field );
        Py_INCREF( Py_None );
        return Py_None;
      } else {
        PyErr_SetString( PyExc_ValueError, 
                         "H3D.createField() failed due to invalid input!" );
        return NULL;
        // THROW APPROPRIATE ERROR
      }
      Py_INCREF(Py_None);
      return Py_None;
    }
    
    
    PyObject *pythonSetFieldValueFromObject( Field *field_ptr, PyObject *v ) {
      if ( field_ptr ) {
        // Try to set the field value, depending on the type of the field
        if ( field_ptr->getTypeName().compare( "SFFloat" ) == 0 ) {
          static_cast<SFFloat*>(field_ptr)->setValue
            ( (H3DFloat)PythonInternals::pyObjectToDouble( v ) );
        } 
        else if ( field_ptr->getTypeName().compare( "MFFloat" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFFloat value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< H3DFloat > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) 
            fv[i] = (H3DFloat)PythonInternals::pyObjectToDouble( PyList_GET_ITEM( v, i ) );
          static_cast<MFFloat*>(field_ptr)->swap(fv);         
        } 
        else if ( field_ptr->getTypeName().compare( "SFDouble" ) == 0 ) {
          static_cast<SFDouble*>(field_ptr)->setValue( PyFloat_AsDouble( v ) );
        } 
        else if ( field_ptr->getTypeName().compare( "MFDouble" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFDouble value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< double > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) 
            fv[i] = PythonInternals::pyObjectToDouble( PyList_GET_ITEM( v, i ) );
          static_cast<MFDouble*>(field_ptr)->swap(fv);         
        }
        else if ( field_ptr->getTypeName().compare( "SFTime" ) == 0 ) {
          static_cast<SFTime*>(field_ptr)->setValue( PyFloat_AsDouble( v ) );
        } 
        else if ( field_ptr->getTypeName().compare( "MFTime" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFTime value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< H3DDouble > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) 
            fv[i] = PythonInternals::pyObjectToDouble( PyList_GET_ITEM( v, i ) );
          static_cast<MFTime*>(field_ptr)->swap(fv);         
        }
        else if ( field_ptr->getTypeName().compare( "SFInt32" ) == 0 ) {
          static_cast<SFInt32*>(field_ptr)->setValue( PyInt_AsLong( v ) );
        } 
        else if ( field_ptr->getTypeName().compare( "MFInt32" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFInt32 value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< int > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) 
            fv[i] = (int)PythonInternals::pyObjectToDouble( PyList_GET_ITEM( v, i ) );
          static_cast<MFInt32*>(field_ptr)->swap(fv);         
        }
        else if ( field_ptr->getTypeName().compare( "SFVec2f" ) == 0 ) {
          if ( PyVec2f_Check( v ) ) {
            PyVec2f *value = (PyVec2f*)( v );
            static_cast<SFVec2f*>(field_ptr)->setValue( *value );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFVec2f value must be a Vec2f" );
            return 0;
          }
        }
        else if ( field_ptr->getTypeName().compare( "MFVec2f" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFVec2f value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< Vec2f > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            if ( PyVec2f_Check( PyList_GET_ITEM( v, i ) ) ) {
              PyVec2f *value = (PyVec2f*)( PyList_GET_ITEM( v, i ) );
              fv[i] = *value;
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFVec2f value must be a list of Vec2f" );
              return 0;
            }
          }
          static_cast<MFVec2f*>(field_ptr)->swap(fv);         
        }
        else if ( field_ptr->getTypeName().compare( "SFVec2d" ) == 0 ) {
          if ( PyVec2d_Check( v ) ) {
            PyVec2d *value = (PyVec2d*)( v );
            static_cast<SFVec2d*>(field_ptr)->setValue( *value );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFVec2d value must be a Vec2d" );
            return 0;
          }
        }
        else if ( field_ptr->getTypeName().compare( "MFVec2d" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFVec2d value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< Vec2d > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            if ( PyVec2d_Check( PyList_GET_ITEM( v, i ) ) ) {
              PyVec2d *value = (PyVec2d*)( PyList_GET_ITEM( v, i ) );
              fv[i] = *value;
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFVec2d value must be a list of Vec2d" );
              return 0;
            }
          }
          static_cast<MFVec2d*>(field_ptr)->swap(fv);         
        }
        else if ( field_ptr->getTypeName().compare( "SFVec3f" ) == 0 ) {
          if ( PyVec3f_Check( v ) ) {
            PyVec3f *value = (PyVec3f*)( v );
            static_cast<SFVec3f*>(field_ptr)->setValue( *value );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFVec3f value must be a Vec3f" );
            return 0;
          }
        }
        else if ( field_ptr->getTypeName().compare( "MFVec3f" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFVec3f value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< Vec3f > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            if ( PyVec3f_Check( PyList_GET_ITEM( v, i ) ) ) {
              PyVec3f *value = (PyVec3f*)( PyList_GET_ITEM( v, i ) );
              fv[i] = *value;
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFVec3f value must be a list of Vec3f" );
              return 0;
            }
          }
          static_cast<MFVec3f*>(field_ptr)->swap(fv);         
        }
        else if ( field_ptr->getTypeName().compare( "SFVec3d" ) == 0 ) {
          if ( PyVec3d_Check( v ) ) {
            PyVec3d *value = (PyVec3d*)( v );
            static_cast<SFVec3d*>(field_ptr)->setValue( *value );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFVec3d value must be a Vec3d" );
            return 0;
          }
        }
        else if ( field_ptr->getTypeName().compare( "MFVec3d" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFVec3d value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< Vec3d > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            if ( PyVec3d_Check( PyList_GET_ITEM( v, i ) ) ) {
              PyVec3d *value = (PyVec3d*)( PyList_GET_ITEM( v, i ) );
              fv[i] = *value;
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFVec3d value must be a list of Vec3d" );
              return 0;
            }
          }
          static_cast<MFVec3d*>(field_ptr)->swap(fv);         
        }
        else if ( field_ptr->getTypeName().compare( "SFVec4f" ) == 0 ) {
          if ( PyVec4f_Check( v ) ) {
            PyVec4f *value = (PyVec4f*)( v );
            static_cast<SFVec4f*>(field_ptr)->setValue( *value );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFVec4f value must be a Vec4f" );
            return 0;
          }
        }
        else if ( field_ptr->getTypeName().compare( "MFVec4f" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFVec4f value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< Vec4f > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            if ( PyVec4f_Check( PyList_GET_ITEM( v, i ) ) ) {
              PyVec4f *value = (PyVec4f*)( PyList_GET_ITEM( v, i ) );
              fv[i] = *value;
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFVec4f value must be a list of Vec4f" );
              return 0;
            }
          }
          static_cast<MFVec4f*>(field_ptr)->swap(fv);         
        }
        else if ( field_ptr->getTypeName().compare( "SFVec4d" ) == 0 ) {
          if ( PyVec4d_Check( v ) ) {
            PyVec4d *value = (PyVec4d*)( v );
            static_cast<SFVec4d*>(field_ptr)->setValue( *value );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFVec4d value must be a Vec4d" );
            return 0;
          }
        }
        else if ( field_ptr->getTypeName().compare( "MFVec4d" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFVec4d value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< Vec4d > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            if ( PyVec4d_Check( PyList_GET_ITEM( v, i ) ) ) {
              PyVec4d *value = (PyVec4d*)( PyList_GET_ITEM( v, i ) );
              fv[i] = *value;
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFVec4d value must be a list of Vec4d" );
              return 0;
            }
          }
          static_cast<MFVec4d*>(field_ptr)->swap(fv);         
        }        
        else if ( field_ptr->getTypeName().compare( "SFBool" ) == 0 ) {
          if( PyInt_Check( v ) ) {
            bool value = PyInt_AsLong( v ) != 0;
            static_cast<SFBool*>(field_ptr)->setValue( value );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFBool value must be an integer representing a boolean" );
            return 0;
          }
        } 
        else if ( field_ptr->getTypeName().compare( "MFBool" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFBool value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< bool > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            if ( PyInt_Check( PyList_GET_ITEM( v, i ) ) ) {
              bool value = PyInt_AsLong( PyList_GET_ITEM( v, i ) ) != 0;
              fv[i] = value;
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFBool value must be a list of integers representing boolean values" );
              return 0;
            }
          }
          static_cast<MFBool*>(field_ptr)->swap(fv);         
        }
        else if ( field_ptr->getTypeName().compare( "SFString" ) == 0 ) {
          if ( PyString_Check( v ) ) {
            char *value = PyString_AsString( v );
            static_cast<SFString*>(field_ptr)->setValue( value );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFString value must be a string" );
            return 0;
          }
        } 
        else if ( field_ptr->getTypeName().compare( "MFString" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFString value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< string > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            if ( PyString_Check( PyList_GET_ITEM( v, i ) ) ) {
              char *value = PyString_AsString( PyList_GET_ITEM( v, i ) );
              fv[i] = value;
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFString value must be a list of strings" );
              return 0;
              
            }
          }
          static_cast<MFString*>(field_ptr)->swap(fv);         
        }
        else if ( field_ptr->getTypeName().compare( "SFNode" ) == 0 ) {
          if ( PyNode_Check( v ) ) {
            PyNode *value = (PyNode*)( v );
            static_cast<SFNode*>(field_ptr)->setValue( value->nodePtr() );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFNode value must be a Node type" );
            return 0;            
          }
        }
        else if ( field_ptr->getTypeName().compare( "MFNode" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFNode value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< Node* > fv( n ); 
          //fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            if ( PyNode_Check( PyList_GET_ITEM( v, i ) ) ) {
              PyNode *value = (PyNode*)( PyList_GET_ITEM( v, i ) );
              fv[i] = value->nodePtr();
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFNode value must be a list of Node types" );
              return 0;            
            }
          }
 
          static_cast<MFNode*>(field_ptr)->setValue( fv );         
        }
        else if ( field_ptr->getTypeName().compare( "SFColor" ) == 0 ) {
          if ( PyRGB_Check( v ) ) {
            PyRGB *value = (PyRGB*)( v );
            static_cast<SFColor*>(field_ptr)->setValue( *value );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFColor value must be a RGB" );
            return 0;
          }
        }
        else if ( field_ptr->getTypeName().compare( "MFColor" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFColor value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< RGB > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            if ( PyRGB_Check( PyList_GET_ITEM( v, i ) ) ) {
              PyRGB *value = (PyRGB*)( PyList_GET_ITEM( v, i ) );
              fv[i] = *value;
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFColor value must be a list of RGB" );
              return 0;
            }
          }
        }
        else if ( field_ptr->getTypeName().compare( "SFColorRGBA" ) == 0 ) {
          if ( PyRGBA_Check( v ) ) {
            PyRGBA *value = (PyRGBA*)( v );
            static_cast<SFColorRGBA*>(field_ptr)->setValue( *value );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFColorRGBA value must be a RGBA" );
            return 0;
          }
        }
        else if ( field_ptr->getTypeName().compare( "MFColorRGBA" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFColorRGBA value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< RGBA > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            if ( PyRGBA_Check( PyList_GET_ITEM( v, i ) ) ) {
              PyRGBA *value = (PyRGBA*)( PyList_GET_ITEM( v, i ) );
              fv[i] = *value;
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFColorRGBA value must be a list of RGBA" );
              return 0;
            }
          }
        }
        else if ( field_ptr->getTypeName().compare( "SFRotation" ) == 0 ) {
          if ( PyRotation_Check(v) ) {
            PyRotation *value = (PyRotation*)( v );
            static_cast<SFRotation*>(field_ptr)->setValue( *value );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFRotation value must be of Rotation type" );
            return 0;            
          }
        }
        else if ( field_ptr->getTypeName().compare( "MFRotation" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFRotation value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< Rotation > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            if ( PyRotation_Check( PyList_GET_ITEM( v, i ) ) ) {
              PyRotation *value = (PyRotation*)( PyList_GET_ITEM( v, i ) );
              fv[i] = *value;
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFRotation value must be a list of Rotations" );
              return 0;            
            }
          }
          static_cast<MFRotation*>(field_ptr)->swap(fv);         
        }
        /* No SF/MFQuaternion yet
           else if ( field_ptr->getTypeName().compare( "SFQuaternion" ) == 0 ) {
           if ( PyQuaternion_Check(v) ) {
           PyQuaternion *value = (PyQuaternion*)( v );
           static_cast<SFQuaternion*>(field_ptr)->setValue( *value );
           } else {
           PyErr_SetString( PyExc_ValueError, 
           "SFQuaternion value must be of Quaternion type" );
           return 0;            
           }
           }
           else if ( field_ptr->getTypeName().compare( "MFQuaternion" ) == 0 ) {
           if( ! v || ! PyList_Check( v ) ) {
           PyErr_SetString( PyExc_ValueError, 
           "MFQuaternion value must be a list" );
           return 0;
           }
           int n = PyList_GET_SIZE( v );
           vector< Quaternion > fv; 
           fv.resize( n );
           for ( int i=0; i < n; i++ ) {
           if ( PyQuaternion_Check( PyList_GET_ITEM( v, i ) ) ) {
           PyQuaternion *value = (PyQuaternion*)( PyList_GET_ITEM( v, i ) );
           fv[i] = *value;
           } else {
           PyErr_SetString( PyExc_ValueError, 
           "MFQuaternion value must be a list of Quaternions" );
           return 0;            
           }
           }
           static_cast<MFQuaternion*>(field_ptr)->swap(fv);         
           }
        */
        else if ( field_ptr->getTypeName().compare( "SFMatrix3f" ) == 0 ) {
          if ( PyMatrix3f_Check(v) ) {
            SFMatrix3f *f = static_cast<SFMatrix3f*>(field_ptr);
            f->setValue( PyMatrix3f_AsMatrix3f( v ) );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFMatrix3f value must be of Matrix3f type" );
            return 0;            
          }
        }
        else if ( field_ptr->getTypeName().compare( "MFMatrix3f" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFMatrix3f value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< Matrix3f > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            PyObject *o = PyList_GET_ITEM( v, i );
            if ( PyMatrix3f_Check( o ) ) {
              fv[i] = PyMatrix3f_AsMatrix3f( o );
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFMatrix3f value must be a list of Matrix3f" );
              return 0;
            }
          }
          static_cast<MFMatrix3f*>(field_ptr)->swap(fv); 
        }
        else if ( field_ptr->getTypeName().compare( "SFMatrix4f" ) == 0 ) {
          if ( PyMatrix4f_Check(v) ) {
            SFMatrix4f *f = static_cast<SFMatrix4f*>(field_ptr);
            f->setValue( PyMatrix4f_AsMatrix4f( v ) );
          } else {
            PyErr_SetString( PyExc_ValueError, 
                             "SFMatrix4f value must be of Matrix4f type" );
            return 0;            
          }

        }
        else if ( field_ptr->getTypeName().compare( "MFMatrix4f" ) == 0 ) {
          if( ! v || ! PyList_Check( v ) ) {
            PyErr_SetString( PyExc_ValueError, 
                             "MFMatrix3f value must be a list" );
            return 0;
          }
          int n = PyList_GET_SIZE( v );
          vector< Matrix4f > fv; 
          fv.resize( n );
          for ( int i=0; i < n; i++ ) {
            PyObject *o = PyList_GET_ITEM( v, i );
            if ( PyMatrix4f_Check( o ) ) {
              fv[i] = PyMatrix4f_AsMatrix4f( o );
            } else {
              PyErr_SetString( PyExc_ValueError, 
                               "MFMatrix4f value must be a list of Matrix4f" );
              return 0;
            }
          }
          static_cast<MFMatrix4f*>(field_ptr)->swap(fv); 
        }

      } else {
        PyErr_SetString( PyExc_ValueError, 
                         "Error: not a valid Field instance in call to H3D.setFieldValue( self, value )" );
        return 0;       
      }     
      
      Py_INCREF(Py_None);
      return Py_None; 
    }

    PyObject *pythonSetFieldValue( PyObject *self, PyObject *args ) {
      if( !args || ! PyTuple_Check( args ) || PyTuple_Size( args ) != 2  ) {
        PyErr_SetString( PyExc_ValueError, 
                         "Invalid argument(s) to function H3D.setFieldValue( self, value )" );
        return 0;
      }
  
      PyObject *field = PyTuple_GetItem( args, 0 );
      if( ! PyInstance_Check( field ) ) {
        PyErr_SetString( PyExc_ValueError, 
                         "Invalid Field type given as argument to H3D.setFieldValue( self, value )" );
        return 0;
      }
      PyObject *py_field_ptr = PyObject_GetAttrString( field, "__fieldptr__" );
      if( !py_field_ptr ) {
        PyErr_SetString( PyExc_ValueError, 
                         "Python object not a Field type. Make sure that if you \
have defined an __init__ function in a specialized field class, you \
call the base class __init__ function." );
        return 0;
      }
      Field *field_ptr = static_cast< Field * >
        ( PyCObject_AsVoidPtr( py_field_ptr ) );
  
      PyObject *v = PyTuple_GetItem( args, 1 );
      //const char *value = PyString_AsString( PyObject_Repr( v ) );
      //ParsableField *pf = static_cast< ParsableField* >(field_ptr);
      return PythonInternals::pythonSetFieldValueFromObject( field_ptr, v );
    }
    

    PyObject *pythonGetFieldValue( PyObject *self, PyObject *arg ) {
      if(!arg || ! PyInstance_Check( arg ) ) {
        PyErr_SetString( PyExc_ValueError, 
                         "Invalid argument(s) to function H3D.getFieldValue( self )" );
        return 0;
      }
      
      PyObject *py_field_ptr = PyObject_GetAttrString( arg, "__fieldptr__" );
      if( !py_field_ptr ) {
        PyErr_SetString( PyExc_ValueError, 
                         "Python object not a Field type. Make sure that if you \
have defined an __init__ function in a specialized field class, you \
call the base class __init__ function." );
        return 0;
      }
      //cerr << "pythonGetFieldValue() ptr=" << py_field_ptr << endl;
      
      Field *field_ptr = static_cast< Field * >
        ( PyCObject_AsVoidPtr( py_field_ptr ) );
      
      //cerr << "  field type name = " << field_ptr->getTypeName() << endl;
      
      if ( field_ptr->getTypeName().compare( "SFFloat" ) == 0 ) {
        return PyFloat_FromDouble( static_cast< SFFloat* >
                                   ( field_ptr )->getValue() );        
      } else if ( field_ptr->getTypeName().compare( "MFFloat" ) == 0 ) {
        MFFloat *mfield = static_cast< MFFloat* >( field_ptr );
        PyObject *list = PyList_New( (int)mfield->size() );
        //cerr << "   MFFloat - size = " << mfield->size() << endl;
        for( size_t i = 0; i < mfield->size(); i++ ) {
          PyObject *v = PyFloat_FromDouble( (*mfield)[i] );
          PyList_SetItem( list, i, v );
        }
        return list;
      } 
      else if ( field_ptr->getTypeName().compare( "SFDouble" ) == 0 ) {
        return PyFloat_FromDouble( static_cast< SFDouble* >
                                   ( field_ptr )->getValue() );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFDouble" ) == 0 ) {
        MFDouble *mfield = static_cast< MFDouble* >( field_ptr );
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          PyObject *v = PyFloat_FromDouble( (*mfield)[i] );
          PyList_SetItem( list, i, v );
        }
        return list;
      }
      else if ( field_ptr->getTypeName().compare( "SFTime" ) == 0 ) {
        return PyFloat_FromDouble( static_cast< SFTime* >
                                   ( field_ptr )->getValue() );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFTime" ) == 0 ) {
        MFTime *mfield = static_cast< MFTime* >( field_ptr );
        PyObject *list = PyList_New( (int) mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          PyObject *v = PyFloat_FromDouble( (*mfield)[i] );
          PyList_SetItem( list, i, v );
        }
        return list;
      }
      else if ( field_ptr->getTypeName().compare( "SFInt32" ) == 0 ) {
        return PyInt_FromLong( static_cast< SFInt32* >
                               ( field_ptr )->getValue() );        
      } 
      else if ( field_ptr->getTypeName().compare( "MFInt32" ) == 0 ) {
        MFInt32 *mfield = static_cast< MFInt32* >( field_ptr );
        PyObject *list = PyList_New( (int) mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          PyObject *v = PyInt_FromLong( (*mfield)[i] );
          PyList_SetItem( list, i, v );
        }
        return list;
      }
      else if ( field_ptr->getTypeName().compare( "SFVec2f" ) == 0 ) {
        Vec2f a = static_cast< SFVec2f* >( field_ptr )->getValue();
        return PyVec2f_FromVec2f( a );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFVec2f" ) == 0 ) {
        MFVec2f *mfield = static_cast< MFVec2f* >( field_ptr );
        vector<Vec2f> v = mfield->getValue();
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          Vec2f a = v[i];
          PyList_SetItem( list, i, PyVec2f_FromVec2f( a ) );
        }
        return list;

      } 
      else if ( field_ptr->getTypeName().compare( "SFVec2d" ) == 0 ) {
        Vec2d a = static_cast< SFVec2d* >( field_ptr )->getValue();
        return PyVec2d_FromVec2d( a );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFVec2d" ) == 0 ) {
        MFVec2d *mfield = static_cast< MFVec2d* >( field_ptr );
        vector<Vec2d> v = mfield->getValue();
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          Vec2d a = v[i];
          PyList_SetItem( list, i, PyVec2d_FromVec2d( a ) );
        }
        return list;        
      } 
      else if ( field_ptr->getTypeName().compare( "SFVec3f" ) == 0 ) {
        Vec3f a = static_cast< SFVec3f* >( field_ptr )->getValue();
        return PyVec3f_FromVec3f( a );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFVec3f" ) == 0 ) {
        MFVec3f *mfield = static_cast< MFVec3f* >( field_ptr );
        vector<Vec3f> v = mfield->getValue();
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          Vec3f a = v[i];
          PyList_SetItem( list, i, PyVec3f_FromVec3f( a ) );
        }
        return list;
      } 
      else if ( field_ptr->getTypeName().compare( "SFVec3d" ) == 0 ) {
        Vec3d a = static_cast< SFVec3d* >( field_ptr )->getValue();
        return PyVec3d_FromVec3d( a );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFVec3d" ) == 0 ) {
        MFVec3d *mfield = static_cast< MFVec3d* >( field_ptr );
        vector<Vec3d> v = mfield->getValue();
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          Vec3d a = v[i];
          PyList_SetItem( list, i, PyVec3d_FromVec3d( a ) );
        }
        return list;
      } 
      else if ( field_ptr->getTypeName().compare( "SFVec4f" ) == 0 ) {
        Vec4f a = static_cast< SFVec4f* >( field_ptr )->getValue();
        return PyVec4f_FromVec4f( a );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFVec4f" ) == 0 ) {
        MFVec4f *mfield = static_cast< MFVec4f* >( field_ptr );
        vector<Vec4f> v = mfield->getValue();
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          Vec4f a = v[i];
          PyList_SetItem( list, i, PyVec4f_FromVec4f( a ) );
        }
        return list;
      } 
      else if ( field_ptr->getTypeName().compare( "SFVec4d" ) == 0 ) {
        Vec4d a = static_cast< SFVec4d* >( field_ptr )->getValue();
        return PyVec4d_FromVec4d( a );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFVec4d" ) == 0 ) {
        MFVec4d *mfield = static_cast< MFVec4d* >( field_ptr );
        vector<Vec4d> v = mfield->getValue();
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          Vec4d a = v[i];
          PyList_SetItem( list, i, PyVec4d_FromVec4d( a ) );
        }
        return list;
      } 
      else if ( field_ptr->getTypeName().compare( "SFBool" ) == 0 ) {
        return PyInt_FromLong( static_cast< SFBool* >
                               ( field_ptr )->getValue() );
      } 
      else if ( field_ptr->getTypeName().compare( "MFBool" ) == 0 ) {
        MFBool *mfield = static_cast< MFBool* >( field_ptr );
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          PyObject *v = PyInt_FromLong( (*mfield)[i] );
          PyList_SetItem( list, i, v );
        }
        return list;
      }
      else if ( field_ptr->getTypeName().compare( "SFString" ) == 0 ) {
        return PyString_FromString( static_cast< SFString* >
                                    ( field_ptr )->getValue().c_str() );        
      } 
      else if ( field_ptr->getTypeName().compare( "MFString" ) == 0 ) {
        MFString *mfield = static_cast< MFString* >( field_ptr );
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          PyObject *v = PyString_FromString( (*mfield)[i].c_str() );
          PyList_SetItem( list, i, v );
        }
        return list;
      }
      else if ( field_ptr->getTypeName().compare( "SFNode" ) == 0 ) {
        Node* n = static_cast< SFNode* >( field_ptr )->getValue();
        return PyNode_FromNode( n );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFNode" ) == 0 ) {
        MFNode *mfield = static_cast< MFNode* >( field_ptr );
        AutoRefVector<Node> v = mfield->getValue();
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          Node *n = v[i];
          PyObject *o = PyNode_FromNode( n );
          PyList_SetItem( list, i, o );
        }
        return list;
      }
      else if ( field_ptr->getTypeName().compare( "SFColor" ) == 0 ) {
        RGB a = static_cast< SFColor* >( field_ptr )->getValue();
        return PyRGB_FromRGB( a );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFColor" ) == 0 ) {
        MFColor *mfield = static_cast< MFColor* >( field_ptr );
        vector<RGB> v = mfield->getValue();
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          RGB a = v[i];
          PyList_SetItem( list, i, PyRGB_FromRGB( a ) );
        }
        return list;
      } 
      else if ( field_ptr->getTypeName().compare( "SFColorRGBA" ) == 0 ) {
        RGBA a = static_cast< SFColorRGBA* >( field_ptr )->getValue();
        return PyRGBA_FromRGBA( a );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFColorRGBA" ) == 0 ) {
        MFColorRGBA *mfield = static_cast< MFColorRGBA* >( field_ptr );
        vector<RGBA> v = mfield->getValue();
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          RGBA a = v[i];
          PyList_SetItem( list, i, PyRGBA_FromRGBA( a ) );
        }
        return list;
      } 
      else if ( field_ptr->getTypeName().compare( "SFRotation" ) == 0 ) {
        Rotation a = static_cast< SFRotation* >( field_ptr )->getValue();
        return PyRotation_FromRotation( a );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFRotation" ) == 0 ) {
        MFRotation *mfield = static_cast< MFRotation* >( field_ptr );
        vector<Rotation> v = mfield->getValue();
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          Rotation a = v[i];
          PyList_SetItem( list, i, PyRotation_FromRotation( a ) );
        }
        return list;
      } 
      else if ( field_ptr->getTypeName().compare( "SFMatrix3f" ) == 0 ) {
        Matrix3f a = static_cast< SFMatrix3f* >( field_ptr )->getValue();
        return PyMatrix3f_FromMatrix3f( a );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFMatrix3f" ) == 0 ) {
        MFMatrix3f *mfield = static_cast< MFMatrix3f* >( field_ptr );
        vector<Matrix3f> v = mfield->getValue();
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          Matrix3f a = v[i];
          PyList_SetItem( list, i, PyMatrix3f_FromMatrix3f( a ) );
        }
        return list;
      } 
      else if ( field_ptr->getTypeName().compare( "SFMatrix4f" ) == 0 ) {
        Matrix4f a = static_cast< SFMatrix4f* >( field_ptr )->getValue();
        return PyMatrix4f_FromMatrix4f( a );       
      } 
      else if ( field_ptr->getTypeName().compare( "MFMatrix4f" ) == 0 ) {
        MFMatrix4f *mfield = static_cast< MFMatrix4f* >( field_ptr );
        vector<Matrix4f> v = mfield->getValue();
        PyObject *list = PyList_New( (int)mfield->size() );
        for( size_t i = 0; i < mfield->size(); i++ ) {
          Matrix4f a = v[i];
          PyList_SetItem( list, i, PyMatrix4f_FromMatrix4f( a ) );
        }
        return list;
      } 
      
      Py_INCREF( Py_None );
      return Py_None;
    }



    // help function for pythonRouteField and pythonRouteFieldNoEvent.
    PyObject *pythonUnrouteField( PyObject *self, 
                                  PyObject *args  ) {
      if( !args || !PyTuple_Check( args ) || PyTuple_Size( args ) != 2  ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.unrouteField( fromField, toField )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }

      PyObject *from_field = PyTuple_GetItem( args, 0 );
      PyObject *to_field = PyTuple_GetItem( args, 1 );
      if( !PyInstance_Check( from_field ) ) {
        ostringstream err;
        err << "Invalid Field type given as fromField argument to H3D.unrouteField( fromField, toField )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }
      if( !PyInstance_Check( to_field ) ) {
        ostringstream err;
        err << "Invalid Field type given as toField argument to H3D.unrouteField( fromField, toField )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }
      PyObject *py_from_field_ptr = PyObject_GetAttrString( from_field, 
                                                            "__fieldptr__" );
      PyObject *py_to_field_ptr = PyObject_GetAttrString( to_field, 
                                                          "__fieldptr__" );
      if( !py_from_field_ptr || !py_to_field_ptr ) {
        PyErr_SetString( PyExc_ValueError, 
                         "Python object not a Field type. Make sure that if you \
have defined an __init__ function in a specialized field class, you \
call the base class __init__ function." );
        return 0;
      }
      Field *from_field_ptr = static_cast< Field * >
        ( PyCObject_AsVoidPtr( py_from_field_ptr ) );
      Field *to_field_ptr = static_cast< Field * >
        ( PyCObject_AsVoidPtr( py_to_field_ptr ) );
      
      if( from_field_ptr == 0 ) {
        ostringstream err;
        err << "Source not a Field class in H3D.unrouteField( fromField, toField )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;       
      }
      if( to_field_ptr == 0 ) {
        ostringstream err;
        err << "Destination not a Field class in call to H3D.unrouteField( fromField, toField )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;       
      }
      try {
        from_field_ptr->unroute( to_field_ptr );
      } catch ( H3D::Exception::H3DException &e ) {
        ostringstream errstr;
        errstr << e << ends;
        PyErr_SetString( PyExc_ValueError, errstr.str().c_str() );
        return NULL;
      }
      
      Py_INCREF(Py_None);
      return Py_None; 
    }

    // help function for pythonRouteField and pythonRouteFieldNoEvent.
    PyObject *pythonRouteFieldHelp( PyObject *self, 
                                    PyObject *args, 
                                    bool send_event  ) {
      if( !args || !PyTuple_Check( args ) || PyTuple_Size( args ) != 2  ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.routeField( fromField, toField )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }

      PyObject *from_field = PyTuple_GetItem( args, 0 );
      PyObject *to_field = PyTuple_GetItem( args, 1 );
      if( !PyInstance_Check( from_field ) ) {
        ostringstream err;
        err << "Invalid Field type given as fromField argument to H3D.routeField( fromField, toField )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }
      if( !PyInstance_Check( to_field ) ) {
        ostringstream err;
        err << "Invalid Field type given as toField argument to H3D.routeField( fromField, toField )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }
      PyObject *py_from_field_ptr = PyObject_GetAttrString( from_field, 
                                                            "__fieldptr__" );
      PyObject *py_to_field_ptr = PyObject_GetAttrString( to_field, 
                                                          "__fieldptr__" );
      if( !py_from_field_ptr || !py_to_field_ptr ) {
        PyErr_SetString( PyExc_ValueError, 
                         "Python object not a Field type. Make sure that if you \
have defined an __init__ function in a specialized field class, you \
call the base class __init__ function." );
        return 0;
      }
      Field *from_field_ptr = static_cast< Field * >
        ( PyCObject_AsVoidPtr( py_from_field_ptr ) );
      Field *to_field_ptr = static_cast< Field * >
        ( PyCObject_AsVoidPtr( py_to_field_ptr ) );
      
      if( from_field_ptr == 0 ) {
        ostringstream err;
        err << "Source not a Field class in H3D.routeField( fromField, toField )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;       
      }
      if( to_field_ptr == 0 ) {
        ostringstream err;
        err << "Destination not a Field class in call to H3D.routeField( fromField, toField )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;       
      }
      try {
        if( send_event )
          from_field_ptr->route( to_field_ptr );
        else
          from_field_ptr->routeNoEvent( to_field_ptr );
      } catch ( H3D::Exception::H3DException &e ) {
        ostringstream errstr;
        errstr << e << ends;
        PyErr_SetString( PyExc_ValueError, errstr.str().c_str() );
        return NULL;
      }
      
      Py_INCREF(Py_None);
      return Py_None; 
    }
    
    PyObject* pythonCreateX3DFromURL( PyObject *self, PyObject *arg ) {
      if( !arg || !PyString_Check( arg ) ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.createX3DFromURL( filename )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }
      char *filename = PyString_AsString( arg );
      X3D::DEFNodes dm;
      Node *n = X3D::createX3DFromURL( filename, &dm );
      return PythonInternals::createX3DHelp( n, &dm );
    }

    PyObject* pythonCreateX3DFromString( PyObject *self, PyObject *arg ) {
      if( !arg || !PyString_Check( arg ) ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.createX3DFromString( s )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }
      char *s = PyString_AsString( arg );
      X3D::DEFNodes dm;
      Node *n = X3D::createX3DFromString( s, &dm );
      return PythonInternals::createX3DHelp( n, &dm );
    }

    PyObject* pythonCreateX3DNodeFromURL( PyObject *self, PyObject *arg ) {
      if( !arg || !PyString_Check( arg ) ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.createX3DFromURL( filename )" << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }
      char *filename = PyString_AsString( arg );
      X3D::DEFNodes dm;
      AutoRef< Node > n = X3D::createX3DNodeFromURL( filename, &dm );
      return PythonInternals::createX3DHelp( n.get(), &dm );
    }

    PyObject* pythonCreateX3DNodeFromString( PyObject *self, PyObject *arg ) {
      if( !arg || !PyString_Check( arg ) ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.createX3DFromString( s )"
            << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }
      char *s = PyString_AsString( arg );
      X3D::DEFNodes dm;
      AutoRef< Node > n = X3D::createX3DNodeFromString( s, &dm );
      return PythonInternals::createX3DHelp( n.get(), &dm );
    }

    PyObject* pythonGetRoutesIn( PyObject *self, PyObject *arg ) {
      if( !arg || ! PyInstance_Check( arg ) ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.getRoutesIn( Field f )."
            << " Requires one argument of type Field. "
            << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }

      PyObject *py_field_ptr = PyObject_GetAttrString( arg, "__fieldptr__" );
      if( !py_field_ptr ) {
        PyErr_SetString( PyExc_ValueError, 
             "Python object not a Field type. Make sure that if you \
have defined an __init__ function in a specialized field class, you \
call the base class __init__ function." );
        return 0;
      }
      //cerr << "pythonGetFieldValue() ptr=" << py_field_ptr << endl;
      
      Field *field_ptr = static_cast< Field * >
        ( PyCObject_AsVoidPtr( py_field_ptr ) );
      Field::FieldVector routes_in =  field_ptr->getRoutesIn();
      PyObject *retval = PyTuple_New( routes_in.size() );

      for( unsigned int i = 0; i < routes_in.size() ; i++ ) {
        PyTuple_SetItem( retval, i, 
           (PyObject *)PythonInternals::fieldAsPythonObject( routes_in[i] ) );
      }
      return retval;
    }

    // function for returning a tuple of the Fields that the field given
    // as arg is routed to.
    PyObject* pythonGetRoutesOut( PyObject *self, PyObject *arg ) {
      if( !arg || ! PyInstance_Check( arg ) ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.getRoutesOut( Field f )."
            << " Requires one argument of type Field. "
            << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }

      PyObject *py_field_ptr = PyObject_GetAttrString( arg, "__fieldptr__" );
      if( !py_field_ptr ) {
        PyErr_SetString( PyExc_ValueError, 
             "Python object not a Field type. Make sure that if you \
have defined an __init__ function in a specialized field class, you \
call the base class __init__ function." );
        return 0;
      }

      Field *field_ptr = static_cast< Field * >
        ( PyCObject_AsVoidPtr( py_field_ptr ) );
      Field::FieldSet routes_out =  field_ptr->getRoutesOut();
      PyObject *retval = PyTuple_New( routes_out.size() );

      Field::FieldSet::iterator fi = routes_out.begin();
      unsigned int i = 0;
      for( ; fi != routes_out.end(); i++,fi++ ) {
        PyTuple_SetItem( retval, i, 
            (PyObject *) PythonInternals::fieldAsPythonObject( *fi ) );
      }
      return retval;
    }

    PyObject *pythonRouteField( PyObject *self, 
                                PyObject *args ) {
      return pythonRouteFieldHelp( self, args, true );
    }

    PyObject *pythonRouteFieldNoEvent( PyObject *self, 
                                       PyObject *args ) {
      return pythonRouteFieldHelp( self, args, false );
    }

    PyObject* pythonGetCurrentScenes( PyObject *self, PyObject *arg ) {
      if( arg ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.getCurrentScenes()."
            << "Function does not take any arguments."
            << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }
      PyObject *scenes = PyTuple_New( Scene::scenes.size() );
      unsigned int counter = 0;
      for( set< Scene * >::const_iterator i = Scene::scenes.begin(); 
           i != Scene::scenes.end(); i++, counter++ ) {
        PyObject *node = PyNode_FromNode( *i );
        PyTuple_SetItem( scenes, 
                         counter, 
                         node );
      }      
      return scenes;
    }

    PyObject* pythonGetActiveDeviceInfo( PyObject *self, PyObject *arg ) {
      if( arg ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.getActiveDeviceInfo()."
            << "Function does not take any arguments."
            << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }

      return PyNode_FromNode( DeviceInfo::getActive() );
    }
    
    PyObject* pythonGetActiveViewpoint( PyObject *self, PyObject *arg ) {
     if( arg ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.getActiveViewpoint()."
            << "Function does not take any arguments."
            << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }

      return PyNode_FromNode( Viewpoint::getActive() );
    }

    PyObject* pythonGetActiveNavigationInfo( PyObject *self, PyObject *arg ) {
     if( arg ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.getActiveNavigationInfo()."
            << "Function does not take any arguments."
            << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }

      return PyNode_FromNode( NavigationInfo::getActive() );
    }

    PyObject* pythonGetActiveStereoInfo( PyObject *self, PyObject *arg ) {
     if( arg ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.getActiveStereoInfo()."
            << "Function does not take any arguments."
            << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }

      return PyNode_FromNode( StereoInfo::getActive() );
    }

    PyObject* pythonGetActiveBackground( PyObject *self, PyObject *arg ) {
     if( arg ) {
        ostringstream err;
        err << "Invalid argument(s) to function H3D.getActiveBackground()."
            << "Function does not take any arguments."
            << ends;
        PyErr_SetString( PyExc_ValueError, err.str().c_str() );
        return 0;
      }

      return PyNode_FromNode( X3DBackgroundNode::getActive() );
    }

  }
}
