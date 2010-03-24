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
/// \file Scene.cpp
/// \brief CPP file for Scene, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include <H3D/Scene.h>
#include <H3DUtil/TimeStamp.h>
#include <H3D/DeviceInfo.h>
#include <GL/glew.h>
#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#ifdef FREEGLUT
#include <GL/freeglut.h>
#endif
#include <H3D/PeriodicUpdate.h>
#include <H3D/GLUTWindow.h>
#include <H3D/X3DShapeNode.h>

#include <H3D/X3DGroupingNode.h>
#include <H3D/ProfilesAndComponents.h>
#include <H3D/H3DNavigation.h>
#include <H3D/NavigationInfo.h>
#include <H3D/H3DMultiPassRenderObject.h>
#include <H3D/Anchor.h>
#include <H3D/DirectionalLight.h>

using namespace H3D;


/////////////////////////////////////////////////////////////////////////////
//
// Initialize static members
//

H3DUtil::MutexLock Scene::callback_lock;
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
auto_ptr< SFTime > Scene::time(new SFTime( TimeStamp() ) );
auto_ptr< Scene::EventSink > Scene::eventSink(new EventSink);

//SFTime *Scene::time = new SFTime( TimeStamp() );
//Scene::EventSink *Scene::eventSink = new EventSink;

namespace SceneInternal {

  void idle() {
    try {
      for( set< Scene * >::iterator i = Scene::scenes.begin();
	   i != Scene::scenes.end();
	   i++ ) {
	if( (*i)->isActive() )
	  (*i)->idle();
      }
    }
    catch (const Exception::QuitAPI &) {
#ifdef FREEGLUT
      glutLeaveMainLoop();
#else
      // normal glut does not support exiting main loop. Have to quit 
      // using exit. This will not call any destructors.
      exit(1);
#endif 
    }
    
    catch (const Exception::H3DException &e) {
      Console(4) << e << endl;
#ifdef FREEGLUT
      glutLeaveMainLoop();
#else
      // normal glut does not support exiting main loop. Have to quit 
      // using exit. This will not call any destructors.
      exit(1);
#endif 
    }
  }
}


void Scene::idle() {
  // calculate and set frame rate
  TimeStamp t;
  frameRate->setValue( 1.0f / (H3DFloat)( t - last_time ), id );
  last_time = t;
  time->setValue( t, id );

  H3DMultiPassRenderObject::resetCounters();
  shadow_caster->object->clear();
  shadow_caster->light->clear();

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

    // traverse the scene graph to collect the HapticObject instances to render.
    TraverseInfo *ti = new TraverseInfo( hds );

    ti->setUserData( "ShadowCaster", shadow_caster.get() );

    X3DChildNode *c = static_cast< X3DChildNode * >( sceneRoot->getValue() );
    if( c ) {
      c->traverseSG( *ti );
    }

    /// traverse the stylus of all haptics devices
    DeviceInfo *di = DeviceInfo::getActive();
    if( di ) {
      for( DeviceInfo::MFDevice::const_iterator i = di->device->begin();
           i != di->device->end(); i++ ) {
        H3DHapticsDevice *hd = static_cast< H3DHapticsDevice * >( *i );
        Node *stylus = hd->stylus->getValue();
        if( stylus ) {
          const Vec3f &pos = hd->weightedProxyPosition->getValue();
          const Rotation &rot = hd->trackerOrientation->getValue();
          Matrix4f m(rot);
          m[0][3] = pos.x;
          m[1][3] = pos.y;
          m[2][3] = pos.z;
          (*ti).pushMatrices( m,
                           m.inverse() );
          stylus->traverseSG( *ti );
          (*ti).popMatrices();
        }
      }
    }
      
    // render the HapticShapes and HapticForceEffets in the TraverseInfo 
    // instance on the H3DHapticsDevices.
    unsigned int nr_devices = (unsigned int) ti->getHapticsDevices().size();
    for( unsigned int i = 0; i < nr_devices; i++ ) {
      H3DHapticsDevice *hd = ti->getHapticsDevice( i );
      // nr_of_layers is the biggest of the nr of layers in this traverseinfo
      // and last_traverseinfo.
      unsigned int nr_of_layers = ti->nrLayers();
      if( last_traverseinfo && last_traverseinfo->nrLayers() > nr_of_layers )
        nr_of_layers = last_traverseinfo->nrLayers();
      if( hd->initialized->getValue() ) {
        for( unsigned int l = 0; l < nr_of_layers; l++ ) {
          if( l < ti->nrLayers() ) {
            ti->setCurrentLayer( l );
            hd->renderShapes( ti->getHapticShapes( i ), l );
          } else {
            // This layer no longer exists, set the shapes to an empty vector
            // to remove from haptics loop.
            HapticShapeVector tmp_vector;
            hd->renderShapes( tmp_vector, l );
          }
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
    ti->setUserData( "ShadowCaster", shadow_caster.get() );
    ti->disableHaptics();
    X3DChildNode *c = static_cast< X3DChildNode *>( sceneRoot->getValue() );
    if( c ) {
      c->traverseSG( *ti );
    }
    // remove the TraverseInfo instance from the last loop. TraverseInfo 
    // instances must be kept alive until its HapticShapes and 
    // HAPIForceEffects are not rendered anymore, which in this case is 
    // one scenegraph loop.
    if( last_traverseinfo )
      delete last_traverseinfo;
    last_traverseinfo = ti;
  }

  // add the head light to shadow casting nodes if it is active.
  if( !shadow_caster->object->empty() ) {
    bool head_light = true;
    NavigationInfo *ni = NavigationInfo::getActive();
    if( ni ) {
      head_light = 
        ni->headlightShadows->getValue() && 
        ni->headlight->getValue();
    }

    if( head_light ) {
      X3DViewpointNode *vp = X3DViewpointNode::getActive();
      Vec3f direction = Vec3f( 0, 0, -1 );
      if( vp ) {
	direction = 
	  vp->accForwardMatrix->getValue().getRotationPart() * 
	  (vp->totalOrientation->getValue() * Vec3f( 0, 0, -1 ));
      }
      DirectionalLight *light = new DirectionalLight();
      light->direction->setValue( direction );
      shadow_caster->light->push_back( light );
    }
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

  Anchor::replaceSceneRoot( this );
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
  last_traverseinfo( NULL ),
  SAI_browser( this ),
  shadow_caster( new ShadowCaster ) {

  scenes.insert( this );
  
  shadow_caster->algorithm->setValue( "ZFAIL" );

  type_name = "Scene";
  database.initFields( this );
  Scene::eventSink->setName( "Scene::eventSink" );
  Scene::time->setName( "Scene::time" );
  time->setAccessType( Field::OUTPUT_ONLY );
  frameRate->setValue( 0, id );
  ThreadBase::setThreadName( ThreadBase::getMainThreadId(), 
			     "H3D API Main Thread" );
  Scene::time->route( eventSink );
}

Scene::~Scene() {
  scenes.erase( this );

  if( last_traverseinfo )
    delete last_traverseinfo;
#ifdef HAVE_XERCES
  ProfilesAndComponents::destroy();
#endif
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

void Scene::loadSceneRoot( const string &url ) {
  SAI::SAIScene *scene = new SAI::SAIScene;
  // TODO: fill out scene with all values
  scene->root_node.reset( X3D::createX3DFromURL( url, 
						 &scene->named_nodes, 
						 &scene->exported_nodes, 
						 &scene->protos ) );
  SAI_browser.replaceWorld( scene );
}

void Scene::setSceneRoot( SAI::SAIScene *scene ) {
  SAI_browser.replaceWorld( scene );
}
