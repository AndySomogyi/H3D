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
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __ISTREAMINPUTSTREAM_H__
#define __ISTREAMINPUTSTREAM_H__
#include <xercesc/util/BinInputStream.hpp>
#include <iostream>

using namespace std;
XERCES_CPP_NAMESPACE_USE

namespace H3D {
  namespace X3D {
    /// This class implements the BinInputStream interface to read
    /// from a istream
    class IStreamInputStream : public BinInputStream {
    public :
      /// Constructor.
      IStreamInputStream ( istream &_is ) : is( _is ), total_count( 0 ) {}
      
      virtual unsigned int curPos() const;
      
      virtual unsigned int readBytes( XMLByte* const toFill,
                                      const unsigned int maxToRead );
      
    protected :
      //BinFileInputStream(const BinFileInputStream&);
      // BinFileInputStream& operator=(const BinFileInputStream&);   
  
      istream &is;
      unsigned int total_count;
      
    };
  }
}

#endif
