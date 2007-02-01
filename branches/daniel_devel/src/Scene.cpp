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
/// \file Scene.cpp
/// \brief CPP file for Scene, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "Scene.h"
#include "TimeStamp.h"
#include "DeviceInfo.h"
#include <GL/glew.h>
#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "PeriodicUpdate.h"
#include "GLUTWindow.h"
#include "X3DShapeNode.h"

#include "X3DPointingDeviceSensorNode.h"

using namespace H3D;


/////////////////////////////////////////////////////////////////////////////
//
// Initialize static members
//

HAPI::MutexLock Scene::callback_lock;
Scene::CallbackList Scene::callbacks;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase Scene::database( 
                                "Scene", 
                                &(newInstance<Scene>), 
                                typeid( Scene ) );

namespace SceneInternals {
  FIELDDB_ELEMENT( Scene, sceneRoot, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Scene, window, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Scene, frameRate, OUTPUT_ONLY );
}


set< Scene* > Scene::scenes;
SFTime *Scene::time = new SFTime( TimeStamp() );
Scene::EventSink *Scene::eventSink = new EventSink;

namespace SceneInternal {
  void idle() {
    for( set< Scene * >::iterator i = Scene::scenes.begin();
         i != Scene::scenes.end();
         i++ ) {
      if( (*i)->isActive() )
        (*i)->idle();
    }
  }
}


void Scene::idle() {
  // calculate and set frame rate
  TimeStamp t;
  frameRate->setValue( 1 / ( t - last_time ), id );
  last_time = t;
  time->setValue( t, id );

  DeviceInfo *di = DeviceInfo::getActive();
  if( di ) {
    vector< H3DHapticsDevice * > hds;
    // update the values for all H3DHapticsDevices
    for( DeviceInfo::MFDevice::const_iterator d = di->device->begin();
         d != di->device->end();
         d++ ) {
      H3DHapticsDevice *hd = static_cast< H3DHapticsDevice * >( *d );
      if( hd->initialized->getValue() ) { 
        hd->preRender();
        hd->updateDeviceValues();
      }
      hds.push_back( hd );
    }

    X3DPointingDeviceSensorNode::clearGeometryNodes();
    // traverse the scene graph to collect the HapticObject instances to render.
    TraverseInfo *ti = new TraverseInfo( hds );
    X3DChildNode *c = static_cast< X3DChildNode * >( sceneRoot->getValue() );
    if( c )
      c->traverseSG( *ti );

    X3DPointingDeviceSensorNode::updateX3DPointingDeviceSensors( c );

    /// traverse the stylus of all haptics devices
    DeviceInfo *di = DeviceInfo::getActive();
    if( di ) {
      for( DeviceInfo::MFDevice::const_iterator i = di->device->begin();
           i != di->device->end(); i++ ) {
        H3DHapticsDevice *hd = static_cast< H3DHapticsDevice * >( *i );
        Node *stylus = hd->stylus->getValue();
        if( stylus ) {
          stylus->traverseSG( *ti );
        }
      }
    }
      
    // render the HapticShapes and HapticForceEffets in the TraverseInfo 
    // instance on the H3DHapticsDevices.
    unsigned int nr_devices = (unsigned int) ti->getHapticsDevices().size();
    for( unsigned int i = 0; i < nr_devices; i++ ) {
      H3DHapticsDevice *hd = ti->getHapticsDevice( i );
      if( hd->initialized->getValue() ) {
        for( unsigned int l = 0; l < ti->nrLayers(); l++ ) {
          ti->setCurrentLayer( l );
          hd->renderShapes( ti->getHapticShapes( i ), l );
        }
        hd->renderEffects( ti->getForceEffects( i ) );
        hd->postRender();
      }
    }

    // remove the TraverseInfo instance from the last loop. TraverseInfo 
    // instances must be kept alive until its HapticShapes and 
    // HAPIForceEffects are not rendered anymore, which in this case is
    // one scenegraph loop.
    if( last_traverseinfo )
      delete last_traverseinfo;
    last_traverseinfo = ti;
  } else {
    // no HapticDevices exist, but we still have to traverse the scene-graph.
    // Haptics is disabled though to avoid unnecessary calculations.
    TraverseInfo *ti = new TraverseInfo( vector< H3DHapticsDevice * >() );
    ti->disableHaptics();
    X3DPointingDeviceSensorNode::clearGeometryNodes();
    X3DChildNode *c = static_cast< X3DChildNode *>( sceneRoot->getValue() );
    if( c )
      c->traverseSG( *ti );
    X3DPointingDeviceSensorNode::updateX3DPointingDeviceSensors( c );
    // remove the TraverseInfo instance from the last loop. TraverseInfo 
    // instances must be kept alive until its HapticShapes and 
    // HAPIForceEffects are not rendered anymore, which in this case is 
    // one scenegraph loop.
    if( last_traverseinfo )
      delete last_traverseinfo;
    last_traverseinfo = ti;
  }
  
  // call window's render function
  for( MFWindow::const_iterator w = window->begin(); 
       w != window->end(); w++ ) {
    H3DWindowNode *window = static_cast< H3DWindowNode * >(*w);
    window->setMultiPassTransparency( 
                       last_traverseinfo->getMultiPassTransparency() );
    window->render( static_cast< X3DChildNode * >( sceneRoot->getValue() ) );
  }

  // update the eventSink
  eventSink->upToDate();

  callback_lock.lock();
  // execute callbacks
  for( CallbackList::iterator i = callbacks.begin();
       i != callbacks.end(); ) {
    CallbackCode c = (*i).first( (*i).second );
    if( c == CALLBACK_DONE ) {
      i = callbacks.erase( i );
    } else {
      i++;
    }
  }
  callback_lock.unlock();
}



/////////////////////////////////////////////////////////////////////////////
//
// Scene member functions
//

Scene::Scene( Inst< SFChildNode >  _sceneRoot,
              Inst< MFWindow    >  _window,
              Inst< SFFloat     >  _frameRate ) :
  sceneRoot( _sceneRoot ),
  window   ( _window    ),
  frameRate( _frameRate ),
  active( true ),
  last_traverseinfo( NULL )	{
  
  scenes.insert( this );
  
  type_name = "Scene";
  database.initFields( this );
  Scene::eventSink->setName( "Scene::eventSink" );
  Scene::time->setName( "Scene::time" );
  time->setAccessType( Field::OUTPUT_ONLY );
  frameRate->setValue( 0, id );
}

Scene::~Scene() {
  scenes.erase( this );

  if( last_traverseinfo )
    delete last_traverseinfo;
}

void Scene::mainLoop() {
  GLUTWindow::initGLUT();
  glutIdleFunc( SceneInternal::idle );
  glutMainLoop();
}

void Scene::EventSink::update() {
  for( unsigned int i = 0; i < routes_in.size(); i++ ) {
    if( PeriodicUpdateField *pf = 
        dynamic_cast< PeriodicUpdateField * >( routes_in[i] ) ) {
      if( pf->timeToUpdate() ) routes_in[i]->upToDate();
    } else {
      routes_in[i]->upToDate();
    }
  }
}
