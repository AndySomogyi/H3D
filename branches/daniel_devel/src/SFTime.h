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
//
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __SFTIME_H__
#define __SFTIME_H__

#include <SField.h>
#include <SFDouble.h>

namespace H3D {

  /// \ingroup FieldTypes
  /// The SFTime field contains a single time value.
  class H3DAPI_API SFTime: public TypedField< SFDouble, void, 
                                              AnyNumber< SFTime > > {
  public:
    SFTime() {}
    SFTime( const H3DTime &_value ) { value = _value; }
    virtual string getTypeName() { return classTypeName(); }
    static string classTypeName() { return "SFTime"; }
    virtual X3DTypes::X3DType getX3DType() { return X3DTypes::SFTIME; }
  };
}

#endif

