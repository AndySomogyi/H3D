
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
/// \file FillProperties.cpp
/// \brief CPP file for FillProperties, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "FillProperties.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase FillProperties::database( "FillProperties", 
                                          &(newInstance<FillProperties>), 
                                          typeid( FillProperties ),
                                          &X3DAppearanceChildNode::database );

namespace FillPropertiesInternals {
  FIELDDB_ELEMENT( FillProperties, filled, INPUT_OUTPUT );
  FIELDDB_ELEMENT( FillProperties, hatchColor, INPUT_OUTPUT );
  FIELDDB_ELEMENT( FillProperties, hatched, INPUT_OUTPUT );
  FIELDDB_ELEMENT( FillProperties, hatchStyle, INPUT_OUTPUT );
}

FillProperties::FillProperties( 
                               Inst< DisplayList > _displayList,
                               Inst< SFBool >  _filled,
                               Inst< SFColor>  _hatchColor,
                               Inst< SFBool >  _hatched,
                               Inst< SFInt32>  _hatchStyle,
                               Inst< SFNode >  _metadata ) :
  X3DAppearanceChildNode( _displayList, _metadata ),
  filled    ( _filled     ),
  hatchColor( _hatchColor ),
  hatched   ( _hatched    ),
  hatchStyle( _hatchStyle ) {

  type_name = "FillProperties";
  database.initFields( this );
  
  filled->setValue( true );
  hatchColor->setValue( RGB( 1, 1, 1 ) );
  hatchStyle->setValue( 1 );
  hatched->setValue( true );

  filled->route( displayList );
  hatchColor->route( displayList );
  hatchStyle->route( displayList );
  hatched->route( displayList );

}

void FillProperties::render() {
  if( filled->getValue() )
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  else 
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  if( hatched->getValue() ) {
 
    int hatch_style = hatchStyle->getValue();
    if( hatch_style < 1 || hatch_style > 6 ) {
      Console(3) << "Warning: hatchStyle " << hatch_style 
                 << " not supported. Skipping hatches (in " 
                 << getName() << "). "<< endl;
    } else {
      const RGB &c = hatchColor->getValue();
      glColor3f( c.r, c.g, c.b );
      glDisable( GL_LIGHTING );
      glEnable (GL_POLYGON_STIPPLE);

      if( hatch_style == 1 ) {
        // horizontal, equally spaced lines
        GLubyte horizontal_lines[] = {
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
        };
        glPolygonStipple (horizontal_lines);
      } else if( hatch_style == 2 ) {
        // vertical, equally spaced lines
        GLubyte vertical_lines[] = {
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
        glPolygonStipple (vertical_lines);
      } else if( hatch_style == 3 ) {
        // positive slope, equally spaced lines
        GLubyte positive_slope[] = {
          0x11, 0x11, 0x11, 0x11, 0x88, 0x88, 0x88, 0x88,
          0x44, 0x44, 0x44, 0x44, 0x22, 0x22, 0x22, 0x22,
          0x11, 0x11, 0x11, 0x11, 0x88, 0x88, 0x88, 0x88,
          0x44, 0x44, 0x44, 0x44, 0x22, 0x22, 0x22, 0x22,
          0x11, 0x11, 0x11, 0x11, 0x88, 0x88, 0x88, 0x88,
          0x44, 0x44, 0x44, 0x44, 0x22, 0x22, 0x22, 0x22,
          0x11, 0x11, 0x11, 0x11, 0x88, 0x88, 0x88, 0x88,
          0x44, 0x44, 0x44, 0x44, 0x22, 0x22, 0x22, 0x22,
          0x11, 0x11, 0x11, 0x11, 0x88, 0x88, 0x88, 0x88,
          0x44, 0x44, 0x44, 0x44, 0x22, 0x22, 0x22, 0x22,
          0x11, 0x11, 0x11, 0x11, 0x88, 0x88, 0x88, 0x88,
          0x44, 0x44, 0x44, 0x44, 0x22, 0x22, 0x22, 0x22,
          0x11, 0x11, 0x11, 0x11, 0x88, 0x88, 0x88, 0x88,
          0x44, 0x44, 0x44, 0x44, 0x22, 0x22, 0x22, 0x22,
          0x11, 0x11, 0x11, 0x11, 0x88, 0x88, 0x88, 0x88,
          0x44, 0x44, 0x44, 0x44, 0x22, 0x22, 0x22, 0x22,
          0x11, 0x11, 0x11, 0x11, 0x88, 0x88, 0x88, 0x88,
          0x44, 0x44, 0x44, 0x44, 0x22, 0x22, 0x22, 0x22 };
        glPolygonStipple (positive_slope);
      } else if( hatch_style == 4 ) {
        // negative slope, equally spaced lines
        GLubyte negative_slope[] = {
          0x88, 0x88, 0x88, 0x88, 0x11, 0x11, 0x11, 0x11,
          0x22, 0x22, 0x22, 0x22, 0x44, 0x44, 0x44, 0x44,
          0x88, 0x88, 0x88, 0x88, 0x11, 0x11, 0x11, 0x11,
          0x22, 0x22, 0x22, 0x22, 0x44, 0x44, 0x44, 0x44,    
          0x88, 0x88, 0x88, 0x88, 0x11, 0x11, 0x11, 0x11,
          0x22, 0x22, 0x22, 0x22, 0x44, 0x44, 0x44, 0x44,    
          0x88, 0x88, 0x88, 0x88, 0x11, 0x11, 0x11, 0x11,
          0x22, 0x22, 0x22, 0x22, 0x44, 0x44, 0x44, 0x44,    
          0x88, 0x88, 0x88, 0x88, 0x11, 0x11, 0x11, 0x11,
          0x22, 0x22, 0x22, 0x22, 0x44, 0x44, 0x44, 0x44,    
          0x88, 0x88, 0x88, 0x88, 0x11, 0x11, 0x11, 0x11,
          0x22, 0x22, 0x22, 0x22, 0x44, 0x44, 0x44, 0x44,    
          0x88, 0x88, 0x88, 0x88, 0x11, 0x11, 0x11, 0x11,
          0x22, 0x22, 0x22, 0x22, 0x44, 0x44, 0x44, 0x44,    
          0x88, 0x88, 0x88, 0x88, 0x11, 0x11, 0x11, 0x11,
          0x22, 0x22, 0x22, 0x22, 0x44, 0x44, 0x44, 0x44,    
          0x88, 0x88, 0x88, 0x88, 0x11, 0x11, 0x11, 0x11,
          0x22, 0x22, 0x22, 0x22, 0x44, 0x44, 0x44, 0x44 };
        glPolygonStipple (negative_slope);
      } else if( hatch_style == 5 ) {
        // horizontal/vertical crosshatch
        GLubyte hv_cross_hatch[] = {
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0xff, 0xff, 0xff, 0xff,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0xff, 0xff, 0xff, 0xff,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0xff, 0xff, 0xff, 0xff,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0xff, 0xff, 0xff, 0xff,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0xff, 0xff, 0xff, 0xff,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0xff, 0xff, 0xff, 0xff,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0xff, 0xff, 0xff, 0xff,
          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
          0x11, 0x11, 0x11, 0x11, 0xff, 0xff, 0xff, 0xff
        };
        glPolygonStipple (hv_cross_hatch);
      } else if( hatch_style == 6 ) {
        // positive slope/negative slope crosshatch
        GLubyte slope_cross_hatch[] = {
          0x88, 0x88, 0x88, 0x88, 0x55, 0x55, 0x55, 0x55,
          0x22, 0x22, 0x22, 0x22, 0x55, 0x55, 0x55, 0x55,
          0x88, 0x88, 0x88, 0x88, 0x55, 0x55, 0x55, 0x55,
          0x22, 0x22, 0x22, 0x22, 0x55, 0x55, 0x55, 0x55,
          0x88, 0x88, 0x88, 0x88, 0x55, 0x55, 0x55, 0x55,
          0x22, 0x22, 0x22, 0x22, 0x55, 0x55, 0x55, 0x55,
          0x88, 0x88, 0x88, 0x88, 0x55, 0x55, 0x55, 0x55,
          0x22, 0x22, 0x22, 0x22, 0x55, 0x55, 0x55, 0x55,
          0x88, 0x88, 0x88, 0x88, 0x55, 0x55, 0x55, 0x55,
          0x22, 0x22, 0x22, 0x22, 0x55, 0x55, 0x55, 0x55,
          0x88, 0x88, 0x88, 0x88, 0x55, 0x55, 0x55, 0x55,
          0x22, 0x22, 0x22, 0x22, 0x55, 0x55, 0x55, 0x55,
          0x88, 0x88, 0x88, 0x88, 0x55, 0x55, 0x55, 0x55,
          0x22, 0x22, 0x22, 0x22, 0x55, 0x55, 0x55, 0x55,
          0x88, 0x88, 0x88, 0x88, 0x55, 0x55, 0x55, 0x55,
          0x22, 0x22, 0x22, 0x22, 0x55, 0x55, 0x55, 0x55,
          0x88, 0x88, 0x88, 0x88, 0x55, 0x55, 0x55, 0x55,
          0x22, 0x22, 0x22, 0x22, 0x55, 0x55, 0x55, 0x55
        };
        glPolygonStipple (slope_cross_hatch);
      }
    }
  }
}

  
