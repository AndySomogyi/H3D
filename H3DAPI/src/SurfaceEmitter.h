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
/// \file SurfaceEmitter.h
/// \brief Header file for SurfaceEmitter, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __SURFACEEMITTER_H__
#define __SURFACEEMITTER_H__

#include <X3DParticleEmitterNode.h>
#include <SFVec3f.h>

namespace H3D {

  /// \ingroup X3DNodes
  /// \class SurfaceEmitter
  /// \brief The SurfaceEmitter node is an emitter that generates particles 
  /// from the surface of an object. 
  /// New particles are generated by first randomly choosing a face on the
  /// tessellated geometry and then a random position on that face. Particles
  /// are generated with an initial direction of the normal to that point
  /// (including any normal averaging due to normalPerVertex and creaseAngle
  /// field settings). If the surface is indicated as not being solid
  /// (solid field set to FALSE), randomly choose from which side of the 
  /// surface to emit, negating the normal direction when generating from
  /// the back side. Only valid geometry shall be used.
  ///
  /// The surface field specifies the geometry to be used as the emitting
  /// surface.
  class H3DAPI_API SurfaceEmitter : public X3DParticleEmitterNode {
  public:
    
    typedef TypedSFNode< X3DGeometryNode > SFGeometryNode;

    /// Constructor.
    SurfaceEmitter( Inst< SFNode  > _metadata     = 0,
                    Inst< SFFloat > _speed        = 0,
                    Inst< SFFloat > _variation    = 0,
                    Inst< SFFloat > _mass         = 0,
                    Inst< SFFloat > _surfaceArea  = 0,
                    Inst< SFGeometryNode > _surface = 0 );

    virtual void generateParticles( ParticleSystem *ps,
                                    H3DTime last_time,
                                    H3DTime now,
                                    std::list< Particle > &particles );

    /// The surface field specifies the geometry to be used as the
    /// emitting surface.
    ///
    /// <b>Access type:</b> inputOutput \n
    auto_ptr< SFGeometryNode > surface;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;
  };
}

#endif
