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
/// \file SpiderMonkeyTypes.cpp
/// \brief Source file for SAI ECMAScript implementation using the SpiderMonkey
/// javascript engine.
///
//
//////////////////////////////////////////////////////////////////////////////

#include <H3D/SpiderMonkeyTypes.h>
#include <H3D/Field.h>
#include <H3D/X3DTypes.h>
#include <H3D/X3DFieldConversion.h>
#include <H3D/X3DTypeFunctions.h>
#include <H3D/SAIFunctions.h>

#ifdef HAVE_SPIDERMONKEY

using namespace H3D;
using namespace H3D::SpiderMonkey;

template< class FieldType >
const typename FieldType::value_type &getValueNoAccessCheck( FieldType *field ) {
  bool access = field->isAccessCheckOn();
  field->setAccessCheck( false );
  const typename FieldType::value_type &b = field->getValue();
  field->setAccessCheck( access );
  return b;
}

template< class FieldType >
void setValueNoAccessCheck( FieldType *field, 
			    const typename FieldType::value_type &v ) {
  bool access = field->isAccessCheckOn();
  field->setAccessCheck( false );
  field->setValue( v);
  field->setAccessCheck( access );
}

//////////////////////////////////////////////////////////
// SFVec3f object

JSBool SpiderMonkey::SFVec3f_construct(JSContext *cx, JSObject *obj, 
			 uintN argc, jsval *argv,
			 jsval *rval) {
  // check that we have enough arguments and that they are of
  // correct type.
  if( argc > 3 ||
      ( argc > 2 && !JSVAL_IS_NUMBER( argv[2] ) ) ||
      ( argc > 1 && !JSVAL_IS_NUMBER( argv[1] ) ) ||
      ( argc > 0 && !JSVAL_IS_NUMBER( argv[0] ) ) ) {
    return JS_FALSE;
  }

  Vec3f v( 0, 0, 0 );

  if( argc > 0 ) {
    jsdouble x;
    if(!JS_ValueToNumber( cx, argv[0], &x) ) return JS_FALSE;
    v.x = (H3DFloat) x;
  }

  if( argc > 1 ) {
    jsdouble y;
    if(!JS_ValueToNumber( cx, argv[1], &y) ) return JS_FALSE;
    v.y = (H3DFloat) y;
  }

  if( argc > 2 ) {
    jsdouble z;
    if(!JS_ValueToNumber( cx, argv[2], &z) ) return JS_FALSE;
    v.z = (H3DFloat) z;
  }
  
  // create return value
  SFVec3f *return_vec3f = new SFVec3f;
  return_vec3f->setValue( v );
  
  *rval = OBJECT_TO_JSVAL( SFVec3f_newInstance( cx, 
						return_vec3f,
						true ) ); 
  return JS_TRUE;
}

void SpiderMonkey::SFVec3f_finalize(JSContext *cx, JSObject *obj) {
  FieldObjectPrivate *private_data = 
    static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,obj));
  
  cerr << "Finalize SFVec3f: " << obj << " " << private_data << endl;
  
  // The prototype of SFColor does not have a private member
  if( private_data ) {
    SFVec3f* f = static_cast<SFVec3f *>(private_data->getField());
    
    // TODO: delete only those that need deletion
    //    if (f) delete f;
  }
}


 JSBool SpiderMonkey::SFVec3f_add(JSContext *cx, JSObject *obj, 
				  uintN argc, jsval *argv,
				  jsval *rval) {
   // check that this object is a SFVec3f_class 
   if (!JS_InstanceOf(cx, obj, &SFVec3fClass, argv))
      return JS_FALSE;

   // check that we have enough arguments and that they are of
   // correct type.
   if( argc != 1 || 
       !JSVAL_IS_OBJECT( argv[0] ) ||
       !JS_InstanceOf( cx, JSVAL_TO_OBJECT( argv[0] ), &SFVec3fClass, argv ) ) {
     return JS_FALSE;
   }

   FieldObjectPrivate *this_private_data = 
     static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,obj));
   FieldObjectPrivate *arg_private_data = 
     static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));

   SFVec3f* this_vec3f = static_cast<SFVec3f *>(this_private_data->getField());
   SFVec3f* arg_vec3f =  static_cast<SFVec3f *>(arg_private_data->getField());

   // make sure we have a value
   if (!this_vec3f || !arg_vec3f ) return JS_FALSE;
   
   // create return value
   SFVec3f *return_vec3f = new SFVec3f;
   return_vec3f->setValue( this_vec3f->getValue() + arg_vec3f->getValue() );
   
   *rval = OBJECT_TO_JSVAL( SFVec3f_newInstance( cx, 
						 return_vec3f,
						 true ) ); 
   return JS_TRUE;
 }

JSBool SpiderMonkey::SFVec3f_cross(JSContext *cx, JSObject *obj, 
				   uintN argc, jsval *argv,
				   jsval *rval) {
   cerr << "cross" << endl;
   return JS_TRUE;
}

JSBool SpiderMonkey::SFVec3f_divide(JSContext *cx, JSObject *obj, 
				    uintN argc, jsval *argv,
				    jsval *rval) {
   cerr << "divide" << endl;
   return JS_TRUE;
}

JSBool SpiderMonkey::SFVec3f_dot(JSContext *cx, JSObject *obj, 
				 uintN argc, jsval *argv,
				 jsval *rval) {
   cerr << "dot" << endl;
   return JS_TRUE;
}

JSBool SpiderMonkey::SFVec3f_length(JSContext *cx, JSObject *obj, 
				    uintN argc, jsval *argv,
				    jsval *rval) {
   cerr << "length" << endl;
   return JS_TRUE;
}
JSBool SpiderMonkey::SFVec3f_multiple(JSContext *cx, JSObject *obj, 
				      uintN argc, jsval *argv,
				      jsval *rval) {
   cerr << "multiple" << endl;
   return JS_TRUE;
}

JSBool SpiderMonkey::SFVec3f_negate(JSContext *cx, JSObject *obj, 
				    uintN argc, jsval *argv,
				    jsval *rval) {
   cerr << "negate" << endl;
   return JS_TRUE;
}

JSBool SpiderMonkey::SFVec3f_normalize(JSContext *cx, JSObject *obj, 
				       uintN argc, jsval *argv,
				       jsval *rval) {
   cerr << "normalize" << endl;
   return JS_TRUE;
}

JSBool SpiderMonkey::SFVec3f_subtract(JSContext *cx, JSObject *obj, 
				      uintN argc, jsval *argv,
				      jsval *rval) {
   cerr << "subtract" << endl;
   return JS_TRUE;
}

JSBool SpiderMonkey::SFVec3f_toString(JSContext *cx, JSObject *obj, 
				      uintN argc, jsval *argv,
				      jsval *rval) {
  // check that this object is a SFVec3f_class 
  if (!JS_InstanceOf(cx, obj, &SFVec3fClass, argv))
    return JS_FALSE;

 FieldObjectPrivate *private_data = 
   static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,obj));
 SFVec3f* this_vec3f = static_cast<SFVec3f *>(private_data->getField());

  // make sure we have a value
  if (!this_vec3f ) return JS_FALSE;

  // return string representation of the Vec3f
  const Vec3f &v = this_vec3f->getValue(); 
  stringstream s;
  s << v;
  string str = s.str();
  *rval = STRING_TO_JSVAL( JS_NewStringCopyN( cx,
					      (char *)str.c_str(),
					      str.length() ) );
  return JS_TRUE;
}

JSBool
SpiderMonkey::SFVec3f_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  cerr << "Get Property SFVec3f" << endl;
  if (JSVAL_IS_INT(id)) {
    FieldObjectPrivate *private_data = 
      static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,obj));
    SFVec3f* this_vec3f = static_cast<SFVec3f *>(private_data->getField());

    const Vec3f &v = getValueNoAccessCheck( this_vec3f );

    switch (JSVAL_TO_INT(id)) {
    case SFVEC3F_X: {
      JS_NewNumberValue( cx, v.x, vp ); 
      break;
    }
    case SFVEC3F_Y: {
      JS_NewNumberValue( cx, v.y, vp ); 
      break;
    }
    case SFVEC3F_Z: {
      JS_NewNumberValue( cx, v.z, vp ); 
      break;
    }
    }
    return JS_TRUE;
  } else {
    // if we get here the property was not one of the ones defined in
    // sfvec3f_properties. It can be another property such as a function
    // so we check if the previous value of vp contains anything. On call 
    // of this function it contains the current value of the attribute.
    // If it is JSVAL_VOID the attribute does not exist.
    if( *vp == JSVAL_VOID ) {
      JSString *s = JSVAL_TO_STRING( id );
      JS_ReportError(cx, "Field object does not have property \"%s\".", JS_GetStringBytes( s ) );
      return JS_FALSE;
    } else {
      return JS_TRUE;
    }
  }
}

JSBool
SpiderMonkey::SFVec3f_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  cerr << "Set Property SFVec3f" << endl;
  if (JSVAL_IS_INT(id)) {
    FieldObjectPrivate *private_data = 
      static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,obj));
    SFVec3f* this_vec3f = static_cast<SFVec3f *>(private_data->getField());
    Vec3f v = this_vec3f->getValue();

    JSBool res = JS_TRUE;
    switch (JSVAL_TO_INT(id)) {
    case SFVEC3F_X: {
      jsdouble x;
      res = JS_ValueToNumber( cx, *vp, &x );
      v.x = (H3DFloat) x;
      break;
    }
    case SFVEC3F_Y: {
      jsdouble y;
      res = JS_ValueToNumber( cx, *vp, &y );
      v.y = (H3DFloat) y;
      break;
    }
    case SFVEC3F_Z: {
      jsdouble z;
      res = JS_ValueToNumber( cx, *vp, &z );
      v.z = (H3DFloat) z;
      break;
    }
    }
    if( !res ) {
      JS_ReportError(cx, "SFVec3f attribute must be convertable to Number" );
      return JS_FALSE;
    } else {
      setValueNoAccessCheck( this_vec3f, v );
      return JS_TRUE;
    }
  } else {
    // if we get here the property was not one of the ones defined in
    // browser_properties. It can be another property such as a function
    // so we check if the previous value of vp contains anything. On call 
    // of this function it contains the current value of the attribute.
    // If it is JSVAL_VOID the attribute does not exist.
    if( *vp == JSVAL_VOID ) {
      JSString *s = JSVAL_TO_STRING( id );
      JS_ReportError(cx, "Field object does not have property \"%s\".", JS_GetStringBytes( s ) );
      return JS_FALSE;
    } else {
      return JS_TRUE;
    }
  }
}


JSObject *SpiderMonkey::SFVec3f_newInstance( JSContext *cx,
					     SFVec3f *field,
					     bool internal_field ) {
  JSObject *js_field;

  js_field = JS_NewObject( cx, 
			   &SFVec3fClass, NULL, NULL );  

  JS_DefineProperties(cx, js_field, SFVec3f_properties );
  JS_DefineFunctions(cx, js_field, SFVec3f_functions );
  JS_SetPrivate(cx, js_field, (void *) new FieldObjectPrivate( field ) );
  return js_field;
}








//////////////////////////////////////////////////////////
// SFNode object

JSBool SpiderMonkey::SFNode_construct(JSContext *cx, JSObject *obj, 
			 uintN argc, jsval *argv,
			 jsval *rval) {
  // check that we have enough arguments and that they are of
  // correct type.
  if( argc != 0 ) return JS_FALSE;

  // create return value
  SFNode *return_node = new SFNode;
  return_node->setValue( NULL );
  
  *rval = OBJECT_TO_JSVAL( SFNode_newInstance( cx, 
					       return_node,
					       true ) ); 
  return JS_TRUE;
}

void SpiderMonkey::SFNode_finalize(JSContext *cx, JSObject *obj) {
  cerr << "Finalize SFNode" << endl;
  FieldObjectPrivate *private_data = 
    static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,obj));
  //    SFVec3f* f = static_cast<SFVec3f *>(private_data->getField());
  // TODO: delete only those that need deletion
  //    if (f) delete f;
}


JSBool
SpiderMonkey::SFNode_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{

  if( JSVAL_IS_STRING( id ) ) {
    JSString *s = JSVAL_TO_STRING( id );
    string field_name = JS_GetStringBytes( s );

    cerr << "Get Property SFNode: " << field_name << " " << obj << endl;
    FieldObjectPrivate *private_data = 
      static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,obj));
    SFNode* node_field = static_cast<SFNode *>(private_data->getField());
    Node *n = node_field->getValue();
    Field *f = NULL;
    if( n ) f = n->getField( field_name );
    if( f ) *vp = jsvalFromField( cx, f, false );
  }

  return JS_TRUE;
}

JSBool
SpiderMonkey::SFNode_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  if( JSVAL_IS_STRING( id ) ) {
    JSString *s = JSVAL_TO_STRING( id );
    string field_name = JS_GetStringBytes( s );
    cerr << "Set Property SFNode: " << field_name << " " << obj << endl;
    FieldObjectPrivate *private_data = 
      static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,obj));
    SFNode* node_field = static_cast<SFNode *>(private_data->getField());
    Node *n = node_field->getValue();
    Field *f = NULL;
    if( n ) f = n->getField( field_name );
    if( f ) return setFieldValueFromjsval( cx, f, *vp );
  }
  return JS_TRUE;
}


JSObject *SpiderMonkey::SFNode_newInstance( JSContext *cx,
					    SFNode *field,
					    bool internal_field,
					    JSObject *obj ) {
  JSObject *js_field;

  js_field = JS_NewObject( cx, 
			   &SFNodeClass, NULL, NULL );  

  //  JS_DefineProperties(cx, js_field, SFVec3f_properties );
  //JS_DefineFunctions(cx, js_field, SFVec3f_functions );
  JS_SetPrivate(cx, js_field, (void *) new FieldObjectPrivate( field ) );
  return js_field;
}


//////////////////////////////////////////////////////////
// SFColor object

JSBool SpiderMonkey::SFColor_construct(JSContext *cx, JSObject *obj, 
				       uintN argc, jsval *argv,
				       jsval *rval) {
  // check that we have enough arguments and that they are of
  // correct type.
  if( argc > 3 ||
      ( argc > 2 && !JSVAL_IS_NUMBER( argv[2] ) ) ||
      ( argc > 1 && !JSVAL_IS_NUMBER( argv[1] ) ) ||
      ( argc > 0 && !JSVAL_IS_NUMBER( argv[0] ) ) ) {
    return JS_FALSE;
  }

  RGB v( 0, 0, 0 );

  if( argc > 0 ) {
    jsdouble r;
    if(!JS_ValueToNumber( cx, argv[0], &r) ) return JS_FALSE;
    v.r = (H3DFloat) r;
  }

  if( argc > 1 ) {
    jsdouble g;
    if(!JS_ValueToNumber( cx, argv[1], &g) ) return JS_FALSE;
    v.g = (H3DFloat) g;
  }

  if( argc > 2 ) {
    jsdouble b;
    if(!JS_ValueToNumber( cx, argv[2], &b) ) return JS_FALSE;
    v.b = (H3DFloat) b;
  }
  
  // create return value
  SFColor *return_color = new SFColor;
  return_color->setValue( v );
  
  *rval = OBJECT_TO_JSVAL( SFColor_newInstance( cx, 
						return_color,
						true ) ); 
  return JS_TRUE;
}

void SpiderMonkey::SFColor_finalize(JSContext *cx, JSObject *obj) {
  FieldObjectPrivate *private_data = 
    static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,obj));
  cerr << "Finalize SFColor:" << obj << " " << private_data << endl;
  
  // The prototype of SFColor does not have a private member
  if( private_data ) {
    SFColor* f = static_cast<SFColor *>(private_data->getField());
    // TODO: delete only those that need deletion
    //    if (f) delete f;
  }
}


JSBool SpiderMonkey::SFColor_getHSV(JSContext *cx, JSObject *obj, 
				    uintN argc, jsval *argv,
				    jsval *rval) {
  cerr <<  "SFColor_getHSV" << endl;
  return JS_TRUE;
}

JSBool SpiderMonkey::SFColor_setHSV(JSContext *cx, JSObject *obj, 
				    uintN argc, jsval *argv,
				    jsval *rval) {
  cerr << "SFColor_setHSV" << endl;
  return JS_TRUE;
}

JSBool SpiderMonkey::SFColor_toString(JSContext *cx, JSObject *obj, 
				      uintN argc, jsval *argv,
				      jsval *rval) {
  // check that this object is a SFColor_class 
  if (!JS_InstanceOf(cx, obj, &SFColorClass, argv))
    return JS_FALSE;

 FieldObjectPrivate *private_data = 
   static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,obj));
 SFColor* this_color = static_cast<SFColor *>(private_data->getField());

  // make sure we have a value
  if (!this_color ) return JS_FALSE;

  // return string representation of the Vec3f
  const RGB &rgb = this_color->getValue(); 
  stringstream s;
  s << rgb;
  string str = s.str();
  *rval = STRING_TO_JSVAL( JS_NewStringCopyN( cx,
					      (char *)str.c_str(),
					      str.length() ) );
  return JS_TRUE;
}

JSBool SpiderMonkey::SFColor_getProperty(JSContext *cx, 
					 JSObject *obj, 
					 jsval id, jsval *vp) {
  if (JSVAL_IS_INT(id)) {
    FieldObjectPrivate *private_data = 
      static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,obj));
    SFColor* this_color = static_cast<SFColor *>(private_data->getField());

    const RGB &rgb = getValueNoAccessCheck( this_color );

    switch (JSVAL_TO_INT(id)) {
    case SFCOLOR_R: {
      JS_NewNumberValue( cx, rgb.r, vp ); 
      break;
    }
    case SFCOLOR_G: {
      JS_NewNumberValue( cx, rgb.g, vp ); 
      break;
    }
    case SFCOLOR_B: {
      JS_NewNumberValue( cx, rgb.b, vp ); 
      break;
    }
    }
    return JS_TRUE;
  } else {
    // if we get here the property was not one of the ones defined in
    // sfcolor_properties. It can be another property such as a function
    // so we check if the previous value of vp contains anything. On call 
    // of this function it contains the current value of the attribute.
    // If it is JSVAL_VOID the attribute does not exist.
    if( *vp == JSVAL_VOID ) {
      JSString *s = JSVAL_TO_STRING( id );
      JS_ReportError(cx, "Field object does not have property \"%s\".", JS_GetStringBytes( s ) );
	return JS_FALSE;
      } else {
	return JS_TRUE;
      }
    }
}

JSBool
SpiderMonkey::SFColor_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  cerr << "Set Property SFColor" << endl;
  if (JSVAL_IS_INT(id)) {
    FieldObjectPrivate *private_data = 
      static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,obj));
    SFColor* this_color = static_cast<SFColor *>(private_data->getField());
    RGB v = this_color->getValue();
    
    JSBool res = JS_TRUE;
    switch (JSVAL_TO_INT(id)) {
    case SFCOLOR_R: {
      jsdouble r;
      res = JS_ValueToNumber( cx, *vp, &r );
      v.r = (H3DFloat) r;
      break;
    }
    case SFCOLOR_G: {
      jsdouble g;
      res = JS_ValueToNumber( cx, *vp, &g );
      v.g = (H3DFloat) g;
      break;
    }
    case SFCOLOR_B: {
      jsdouble b;
      res = JS_ValueToNumber( cx, *vp, &b );
      v.b = (H3DFloat) b;
      break;
    }
    }
    if( !res ) {
      JS_ReportError(cx, "SFColor attribute must be convertable to Number" );
      return JS_FALSE;
    } else {
      setValueNoAccessCheck( this_color, v );
      return JS_TRUE;
    }
  } else {
    // if we get here the property was not one of the ones defined in
    // sfcolor_properties. It can be another property such as a function
    // so we check if the previous value of vp contains anything. On call 
    // of this function it contains the current value of the attribute.
    // If it is JSVAL_VOID the attribute does not exist.
    if( *vp == JSVAL_VOID ) {
      JSString *s = JSVAL_TO_STRING( id );
      JS_ReportError(cx, "Field object does not have property \"%s\".", JS_GetStringBytes( s ) );
      return JS_FALSE;
    } else {
      return JS_TRUE;
    }
  }
}


JSObject *SpiderMonkey::SFColor_newInstance( JSContext *cx,
					     SFColor *field,
					     bool internal_field ) {
  JSObject *js_field;

  js_field = JS_NewObject( cx, 
			   &SFColorClass, NULL, NULL );  

  JS_DefineProperties(cx, js_field, SFColor_properties );
  JS_DefineFunctions(cx, js_field, SFColor_functions );
  JS_SetPrivate(cx, js_field, (void *) new FieldObjectPrivate( field ) );
  cerr << "SFColor_newInstance: " << js_field << " " << field->getName() << " " << JS_GetPrivate( cx, js_field ) <<endl;
  return js_field;
}



























































bool SpiderMonkey::insertH3DTypes( JSContext *cx, JSObject *obj ) {
  // TODO: FIX!!
  JSObject *proto = JS_InitClass( cx, obj, NULL, 
				  &SFVec3fClass, SFVec3f_construct, 3, 
				  SFVec3f_properties, SFVec3f_functions, NULL, NULL );
  proto = JS_InitClass( cx, obj, NULL, 
			&SFNodeClass, SFNode_construct, 3, 
			NULL, SFNode_functions, NULL, NULL );
  proto = JS_InitClass( cx, obj, NULL, 
			&SFColorClass, SFColor_construct, 3, 
			SFColor_properties, SFColor_functions, NULL, NULL );
  return true;
}

jsval SpiderMonkey::jsvalFromField( JSContext *cx, 
				    Field *field, bool make_copy ) {
  JSObject *object = JSObjectFromField( cx, field, make_copy );
  if( object ) return OBJECT_TO_JSVAL( object );
  else {
    X3DTypes::X3DType x3d_type = field->getX3DType();
    switch( x3d_type ) { 
      /*    case X3DTypes::SFDOUBLE: { 
      break;
    } 
    case X3DTypes::SFTIME: { 
      break;
    } 
    case X3DTypes::SFINT32: { 
      break;
      }*/
    case X3DTypes::SFBOOL: { 
      SFBool *f = static_cast< SFBool * >( field );
      bool b = getValueNoAccessCheck( f );
      return BOOLEAN_TO_JSVAL( b );
    }
    default:
      return JSVAL_VOID;
    }
  }
}

JSBool SpiderMonkey::setFieldValueFromjsval( JSContext *cx, 
					     Field *field, 
					     jsval value ) {
  X3DTypes::X3DType field_x3d_type = field->getX3DType();
  switch( field_x3d_type ) { 
  case X3DTypes::SFBOOL: { 
    SFBool *f = static_cast< SFBool * >( field );
    JSBool b;
    if( JS_ValueToBoolean( cx, value, &b ) ) {
      bool bool_value = JSVAL_TO_BOOLEAN( b ); 
      setValueNoAccessCheck( f, b );
      return JS_TRUE;
    } else {
      JS_ReportError(cx, "Value not convertable to Boolean" );
      return JS_FALSE;
    }
  }
  case X3DTypes::SFCOLOR: { 
    SFColor *f = static_cast< SFColor * >( field );

    if( JSVAL_IS_OBJECT( value ) ) {
      JSObject *js_object = JSVAL_TO_OBJECT( value );
      
      FieldObjectPrivate *private_data = 
	static_cast<FieldObjectPrivate *>(JS_GetPrivate(cx,js_object));

      Field *value_field = private_data->getField();
      if( value_field->getX3DType() == X3DTypes::SFCOLOR ) {
	const RGB &c = 
	  getValueNoAccessCheck( static_cast< SFColor * >( value_field ) );
	setValueNoAccessCheck( f, c ); 
	return JS_TRUE;
      } 
    }

    JS_ReportError(cx, "Invalid type in assignment. Expecting SFColor." );
    return JS_FALSE;
  }
  default:
    JS_ReportError(cx, "Field has invalid/unsupported X3DType." );
    return JS_FALSE;
  }
}


#define RETURN_NEW_INSTANCE( field_type, new_instance_func ) \
field_type *f = static_cast< field_type * >( field ); \
if( make_copy ) {				      \
  f = new field_type; \
  f->setValue( static_cast< field_type * >(field)->getValue() );\
}\
return new_instance_func( cx, f, make_copy );

JSObject *SpiderMonkey::JSObjectFromField( JSContext *cx, 
					   Field *field, bool make_copy ) {
  X3DTypes::X3DType x3d_type = field->getX3DType();
  switch( x3d_type ) { 
  case X3DTypes::SFFLOAT: { 
    //SFFloat *f = new SFFloat;
    //f->setValue( static_cast< SFFloat * >(f)->getValue() );
    //return 
  } 
  case X3DTypes::SFDOUBLE: { 
    break;
  } 
  case X3DTypes::SFTIME: { 
    break;
  } 
  case X3DTypes::SFINT32: { 
    break;
  } 
  case X3DTypes::SFVEC2F: { 
    break;
  } 
  case X3DTypes::SFVEC2D: { 
    break;
  } 
  case X3DTypes::SFVEC3F: { 
    RETURN_NEW_INSTANCE( SFVec3f, SFVec3f_newInstance );
  } 
  case X3DTypes::SFVEC3D: { 
  } 
  case X3DTypes::SFVEC4F: { 
  } 
  case X3DTypes::SFVEC4D: { 
  } 
  case X3DTypes::SFBOOL: { 
  } 
  case X3DTypes::SFSTRING: { 
    break;
  } 
  case X3DTypes::SFNODE: { 
    RETURN_NEW_INSTANCE( SFNode, SFNode_newInstance );
  } 
  case X3DTypes::SFCOLOR: { 
    RETURN_NEW_INSTANCE( SFColor, SFColor_newInstance );
  } 
  case X3DTypes::SFCOLORRGBA: { 
  } 
  case X3DTypes::SFROTATION: { 
  } 
  case X3DTypes::SFQUATERNION: { 
  }
  case X3DTypes::SFMATRIX3F: { 
  } 
  case X3DTypes::SFMATRIX4F: { 
  } 
  case X3DTypes::SFMATRIX3D: { 
  } 
  case X3DTypes::SFMATRIX4D: { 
  } 
  case X3DTypes::MFFLOAT: { 
  } 
  case X3DTypes::MFDOUBLE: { 
  } 
  case X3DTypes::MFTIME: { 
  } 
  case X3DTypes::MFINT32: { 
  } 
  case X3DTypes::MFVEC2F: { 
  } 
  case X3DTypes::MFVEC2D: { 
  } 
  case X3DTypes::MFVEC3F: { 
  } 
  case X3DTypes::MFVEC3D: { 
  } 
  case X3DTypes::MFVEC4F: { 
  } 
  case X3DTypes::MFVEC4D: { 
  } 
  case X3DTypes::MFBOOL: { 
  } 
  case X3DTypes::MFSTRING: { 
  } 
  case X3DTypes::MFNODE: { 
  } 
  case X3DTypes::MFCOLOR: { 
  } 
  case X3DTypes::MFCOLORRGBA: { 
  } 
  case X3DTypes::MFROTATION: { 
  } 
  case X3DTypes::MFQUATERNION: { 
  } 
  case X3DTypes::MFMATRIX3F: { 
  } 
  case X3DTypes::MFMATRIX4F: { 
  } 
  case X3DTypes::MFMATRIX3D: { 
  }
  case X3DTypes::MFMATRIX4D: { 
  }
  } 

  return NULL;
}
#endif // HAVE_SPIDERMONKEY

