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
//////////////////////////////////////////////////////////////////////////////

#ifndef __X3DTYPES_H__
#define __X3DTYPES_H__

#include "FieldTemplates.h"
#include "SFNode.h"
#include "MFNode.h"

namespace H3D {
  namespace X3DTypes {
    enum X3DType {
      SFFLOAT,      MFFLOAT,
      SFDOUBLE,     MFDOUBLE,
      SFTIME,       MFTIME,
      SFINT32,      MFINT32,
      SFVEC2F,      MFVEC2F,
      SFVEC2D,      MFVEC2D,
      SFVEC3F,      MFVEC3F,
      SFVEC3D,      MFVEC3D,
      SFBOOL,       MFBOOL,
      SFSTRING,     MFSTRING,
      SFNODE,       MFNODE,
      SFCOLOR,      MFCOLOR,
      SFCOLORRGBA,  MFCOLORRGBA,
      SFROTATION,   MFROTATION,
      SFMATRIX3F,   MFMATRIX3F,
      SFMATRIX4F,   MFMATRIX4F,
      UNKNOWN_X3D_TYPE 
    };


    inline const char* typeToString( X3DType t ) {
      switch( t ) {
      case     SFFLOAT: return "SFFloat";
      case     MFFLOAT: return "MFFloat";
      case    SFDOUBLE: return "SFDouble";
      case    MFDOUBLE: return "MFDouble";
      case      SFTIME: return "SFTime";
      case      MFTIME: return "MFTime";
      case     SFINT32: return "SFInt32";
      case     MFINT32: return "MFInt32";
      case     SFVEC2F: return "SFVec2f";
      case     MFVEC2F: return "MFVec2f";
      case     SFVEC2D: return "SFVec2d";
      case     MFVEC2D: return "MFVec2d";
      case     SFVEC3F: return "SFVec3f";
      case     MFVEC3F: return "MFVec3f";
      case     SFVEC3D: return "SFVec3d";
      case     MFVEC3D: return "MFVec3d";
      case      SFBOOL: return "SFBool";
      case      MFBOOL: return "MFBool";
      case    SFSTRING: return "SFString";
      case    MFSTRING: return "MFString";
      case      SFNODE: return "SFNode";
      case      MFNODE: return "MFNode";
      case     SFCOLOR: return "SFColor";
      case     MFCOLOR: return "MFColor";
      case SFCOLORRGBA: return "SFColorRGBA";
      case MFCOLORRGBA: return "MFColorRGBA";
      case  SFROTATION: return "SFRotation";
      case  MFROTATION: return "MFRotation";
      case  SFMATRIX3F: return "SFMatrix3f";
      case  MFMATRIX3F: return "MFMatrix3f";
      case  SFMATRIX4F: return "SFMatrix4f";
      case  MFMATRIX4F: return "MFMatrix4f";
      case UNKNOWN_X3D_TYPE: 
      default:return "UNKNOWN_X3D_TYPE";
      };
    }

    inline X3DType stringToType( const char *t ) {
      if( strcmp( t, "SFFloat" ) == 0 ) return SFFLOAT;
      else if( strcmp( t, "MFFloat" ) == 0 ) return MFFLOAT;
      else if( strcmp( t, "SFDouble" ) == 0 ) return SFDOUBLE;
      else if( strcmp( t, "MFDouble" ) == 0 ) return MFDOUBLE;
      else if( strcmp( t, "SFTime" ) == 0 ) return SFTIME;
      else if( strcmp( t, "MFTime" ) == 0 ) return MFTIME;
      else if( strcmp( t, "SFInt32" ) == 0 ) return SFINT32;
      else if( strcmp( t, "MFInt32" ) == 0 ) return MFINT32;
      else if( strcmp( t, "SFVec2f" ) == 0 ) return SFVEC2F;
      else if( strcmp( t, "MFVec2f" ) == 0 ) return MFVEC2F;
      else if( strcmp( t, "SFVec3f" ) == 0 ) return SFVEC3F;
      else if( strcmp( t, "MFVec3f" ) == 0 ) return MFVEC3F;
      else if( strcmp( t, "SFVec2d" ) == 0 ) return SFVEC2D;
      else if( strcmp( t, "MFVec2d" ) == 0 ) return MFVEC2D;
      else if( strcmp( t, "SFVec3d" ) == 0 ) return SFVEC3D;
      else if( strcmp( t, "MFVec3d" ) == 0 ) return MFVEC3D;
      else if( strcmp( t, "SFBool" ) == 0 ) return SFBOOL;
      else if( strcmp( t, "MFBool" ) == 0 ) return MFBOOL;
      else if( strcmp( t, "SFString" ) == 0 ) return SFSTRING;
      else if( strcmp( t, "MFString" ) == 0 ) return MFSTRING;
      else if( strcmp( t, "SFNode" ) == 0 ) return SFNODE;
      else if( strcmp( t, "MFNode" ) == 0 ) return MFNODE;
      else if( strcmp( t, "SFColor" ) == 0 ) return SFCOLOR;
      else if( strcmp( t, "MFColor" ) == 0 ) return MFCOLOR;
      else if( strcmp( t, "SFColorRGBA" ) == 0 ) return SFCOLORRGBA;
      else if( strcmp( t, "MFColorRGBA" ) == 0 ) return MFCOLORRGBA;
      else if( strcmp( t, "SFRotation" ) == 0 ) return SFROTATION;
      else if( strcmp( t, "MFRotation" ) == 0 ) return MFROTATION;
      else if( strcmp( t, "SFMatrix3f" ) == 0 ) return SFMATRIX3F;
      else if( strcmp( t, "MFMatrix3f" ) == 0 ) return MFMATRIX3F;
      else if( strcmp( t, "SFMatrix4f" ) == 0 ) return SFMATRIX4F;
      else if( strcmp( t, "MFMatrix4f" ) == 0 ) return MFMATRIX4F;
      else return UNKNOWN_X3D_TYPE;
    };

    inline Field *newFieldInstance( X3DTypes::X3DType t ) {
      switch( t ) {
      case X3DTypes::SFFLOAT: 
        return new SFFloat;
      case X3DTypes::MFFLOAT: 
        return new MFFloat;
      case X3DTypes::SFDOUBLE:
        return new SFDouble;
      case X3DTypes::MFDOUBLE:
        return new MFDouble;
      case X3DTypes::SFTIME:  
        return new SFTime;
      case X3DTypes::MFTIME:  
        return new MFTime;
      case X3DTypes::SFINT32: 
        return new SFInt32;
      case X3DTypes::MFINT32: 
        return new MFInt32;
      case X3DTypes::SFVEC2F: 
        return new SFVec2f;
      case X3DTypes::MFVEC2F: 
        return new MFVec2f;
      case X3DTypes::SFVEC2D: 
        return new SFVec2d;
      case X3DTypes::MFVEC2D: 
        return new MFVec2d;
      case X3DTypes::SFVEC3F: 
        return new SFVec3f;
      case X3DTypes::MFVEC3F: 
        return new MFVec3f;
      case X3DTypes::SFVEC3D: 
        return new SFVec3f;
      case X3DTypes::MFVEC3D: 
        return new MFVec3d;
      case X3DTypes::SFBOOL:  
        return new SFBool;
      case X3DTypes::MFBOOL:  
        return new MFBool;
      case X3DTypes::SFSTRING:
        return new SFString;
      case X3DTypes::MFSTRING:
        return new MFString;
      case X3DTypes::SFNODE:
        return new SFNode;
      case X3DTypes::MFNODE:
        return new MFNode;
      case X3DTypes::SFCOLOR:   
        return new SFColor;
      case X3DTypes::MFCOLOR:   
        return new MFColor;
      case X3DTypes::SFCOLORRGBA:    
        return new SFColorRGBA;
      case X3DTypes::MFCOLORRGBA:    
        return new MFColorRGBA;
      case X3DTypes::SFROTATION:
        return new SFRotation;
      case X3DTypes::MFROTATION:
        return new MFRotation;
      case X3DTypes::SFMATRIX3F:
        return new SFMatrix3f;
      case X3DTypes::MFMATRIX3F:
        return new MFMatrix3f;
      case X3DTypes::SFMATRIX4F:
        return new SFMatrix4f;
      case X3DTypes::MFMATRIX4F:
        return new MFMatrix4f;
      case X3DTypes::UNKNOWN_X3D_TYPE: 
      default: return NULL;
      }
    };

    inline Field *newFieldInstance( const char *t ) {
      return newFieldInstance( stringToType( t ) );
    };
      
  }
}

#endif
