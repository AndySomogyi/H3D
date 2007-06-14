//////////////////////////////////////////////////////////////////////////////
//    Copyright 2007, SenseGraphics AB
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
/// \file H3DWxFrame.cpp
/// \brief Implementation file for H3DWxFrame
///
//
//////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------

#include <H3DWxFrame.h>
#include <consoleDialog.h>
#include <vector>
#include <H3DWxWidgetsWindow.h>

#include <wx/wx.h>
#include <envini.h>

// ---------------------------------------------------------------------------
//  Includes (to open X3D files)
// ---------------------------------------------------------------------------

#include <VrmlParser.h>
#include <Group.h>
#include <Transform.h>
#include <Scene.h>
#include <KeySensor.h>
#include <MouseSensor.h>
#ifndef MACOSX
#include <SpaceWareSensor.h>
#endif
#include <Viewpoint.h>
#include <X3DViewpointNode.h>
#include <DeviceInfo.h>
#include <INIFile.h>
#include <ResourceResolver.h>
#include <Console.h>

#include <GraphicsCachingOptions.h>
#include <DebugOptions.h>
#include <HapticsOptions.h>
#include <GeometryBoundTreeOptions.h>
#include <OpenHapticsOptions.h>

#include <HapticsRenderers.h>
#include <HAPIHapticsRenderer.h>

using namespace std;
using namespace H3D;


//AutoRef< GlobalSettings > global_settings;

/*******************Required Class***********************/

H3D_API_EXCEPTION( QuitAPIException );

class QuitAPIField: public AutoUpdate< SFString > {
  virtual void update() {
    string s = static_cast< SFString * >(routes_in[0])->getValue();
    if( s[0] == 27 ) { 
      throw QuitAPIException();
    }
  }
};

/*******************Global Constants*********************/
static const wxChar *TITLE     = _T("Particle Systems - Demo ");
static const wxChar *AUTHOR    = _T("\nRamkumar Shankar\n\nCopyright 2007.  All Rights Reserved.");
static const wxChar *ABOUT     = _T("About");
static const wxChar *FILETYPES = _T( "x3d files|*.x3d|"
                                    "vrml files|*.wrl|"
                                     "All files|*.*"
                                   );

/*******************Constructor*********************/
H3DWxFrame::H3DWxFrame( wxWindow *_parent, wxWindowID _id,
                        const wxString& _title, const wxPoint& _pos,
                        const wxSize& _size, long _style,
                        const wxString& _name ):
wxFrame(_parent, _id, _title, _pos, _size, _style, _name )
{
 	scene.reset( new Scene );
	ks.reset ( new KeySensor );
	ms.reset ( new MouseSensor );
	#ifndef MACOSX
	//ss.reset (new SpaceWareSensor);
	#endif
	t.reset ( new Transform );
	default_stylus.reset (new Node);
	viewpoint.reset (NULL);
	device_info.reset (NULL);
	g.reset ( new Group );

	glwindow = new H3DWxWidgetsWindow(this);
	int width, height;
	GetClientSize(&width, &height);
	glwindow->width->setValue(width);
  glwindow->height->setValue(height);

  t->children->clear();
	//g->children->clear();

	//t->children->push_back( X3D::createVRMLFromString( "<Group />") );
	g->children->push_back( t.get() );
	scene->window->push_back( glwindow );
	scene->sceneRoot->setValue( g.get() );

	//scene->window->push_back( glwindow );

  wxString console_string = "Console";
  theConsole = new consoleDialog(this, wxID_ANY, console_string, wxDefaultPosition, 
                                     wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

  //H3DWxWidgetsWindow *glwindow = new H3DWxWidgetsWindow(theWxFrame);
  //Main Menu Bar
  menuBar  = (wxMenuBar *) NULL;

  //Top level menus
  fileMenu = (wxMenu *)	NULL;
  particleMenu = (wxMenu *) NULL;
  physicsMenu = (wxMenu *) NULL;
  emitterMenu = (wxMenu *) NULL;
  advancedMenu = (wxMenu *) NULL;
  helpMenu = (wxMenu *)	NULL;

  //Sub Menus
  geometryMenu = (wxMenu *) NULL;

  //Status Bar
  CreateStatusBar(2);

  /******************Menu Bar Items*****************/
  //File menu
  fileMenu = new wxMenu;
  fileMenu->Append(FRAME_OPEN,"&Open file...\tCtrl+O","Open a file");
  fileMenu->Append(FRAME_OPEN_URL, "&Open file from URL...", "Open a file from URL" );
  fileMenu->Append(FRAME_CLOSE, "&Close file","Close file");
  fileMenu->AppendSeparator();
  fileMenu->Append(FRAME_EXIT,"E&xit\tCtrl+X", "Exit");

  //Particle Geometry sub menu
  geometryMenu = new wxMenu;
  geometryMenu->AppendRadioItem(FRAME_QUAD,"Quad","Quad Geometry");
  geometryMenu->AppendRadioItem(FRAME_TRIANGLE,"Triangle","Triangle Geometry");
  geometryMenu->AppendRadioItem(FRAME_POINT,"Point","Point Geometry");
  geometryMenu->AppendRadioItem(FRAME_SPRITE,"Sprite","Sprite Geometry");
  geometryMenu->AppendRadioItem(FRAME_LINE,"Line","Line Geometry");

  //Particle menu
  particleMenu = new wxMenu;
  particleMenu->Append(FRAME_GEOMETRY,"Geometry", geometryMenu, "Select Particle Geometry");

  //Physics menu
  physicsMenu = new wxMenu;
  physicsMenu->Append(FRAME_BOUNDEDPHYSICSMODEL,"Bounded Physics Model", "Bounded Physics Model");
  physicsMenu->Append(FRAME_GRAVITYPHYSICSMODEL,"Gravity Physics Model", "Gravity Physics Model");
  physicsMenu->Append(FRAME_WINDPHYSICSMODEL,"Wind Physics Model", "Wind Physics Model");

  //Emitter Menu
  emitterMenu = new wxMenu;
  emitterMenu->Append(FRAME_CONEEMITTER,"Cone","Select Cone Emitter");
  emitterMenu->Append(FRAME_EXPLOSIONEMITTER,"Explosion","Select Explosion Emitter");
  emitterMenu->Append(FRAME_POINTEMITTER,"Point","Select Point Emitter");
  emitterMenu->Append(FRAME_POLYLINEEMITTER, "Polyline", "Select Polyline Emitter");
  emitterMenu->Append(FRAME_VOLUMEEMITTER, "Volume", "Select Volume Emitter");
  emitterMenu->Append(FRAME_SURFACEEMITTER, "Surface", "Select Surface Emitter");

  //Advanced Menu
  advancedMenu = new wxMenu;
  advancedMenu->Append(FRAME_CONSOLE, "Show Console", "Show the message console");
  
  //Help Menu
  helpMenu = new wxMenu;
  helpMenu->Append(FRAME_HELP, "Help");
  helpMenu->Append(FRAME_ABOUT, "About");

  //Install Menu Bar
  menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, "&File");
  menuBar->Append(particleMenu, "&Particle System");
  menuBar->Append(physicsMenu, "Phy&sics");
  menuBar->Append(emitterMenu, "&Emitter");
  menuBar->Append(advancedMenu, "&Advanced");
  menuBar->Append(helpMenu, "&Help");
  SetMenuBar(menuBar);

}

/*******************Event Table*********************/
BEGIN_EVENT_TABLE(H3DWxFrame, wxFrame)
	EVT_MENU (FRAME_EXIT, H3DWxFrame::OnExit)
	EVT_MENU (FRAME_OPEN, H3DWxFrame::OnOpenFile)
  EVT_MENU (FRAME_OPEN_URL, H3DWxFrame::OnOpenFileURL)
	EVT_MENU (FRAME_CLOSE, H3DWxFrame::OnCloseFile)
	EVT_MENU (FRAME_CONSOLE, H3DWxFrame::ShowConsole)
	EVT_MENU_HIGHLIGHT (FRAME_SELECTION, H3DWxFrame::GetSelection)
	EVT_MENU (FRAME_ABOUT, H3DWxFrame::OnAbout)
	EVT_MENU (FRAME_HELP, H3DWxFrame::OnHelp)
  EVT_MENU_RANGE (FRAME_QUAD, FRAME_LINE, H3DWxFrame::OnParticleGeometry)
  EVT_MENU (FRAME_PARTICLECONTROL, H3DWxFrame::ToggleParticles)
  EVT_MENU (FRAME_PARTICLECREATION, H3DWxFrame::ParticleGeneration)
  EVT_MENU (FRAME_POINTEMITTER, H3DWxFrame::PointEmitterSettingsDialog)
  EVT_MENU (FRAME_POLYLINEEMITTER, H3DWxFrame::PolylineEmitterSettingsDialog)
  EVT_MENU (FRAME_VOLUMEEMITTER, H3DWxFrame::VolumeEmitterSettingsDialog)
  EVT_MENU (FRAME_CONEEMITTER, H3DWxFrame::ConeEmitterSettingsDialog)
  EVT_MENU (FRAME_EXPLOSIONEMITTER, H3DWxFrame::ExplosionEmitterSettingsDialog)
  EVT_MENU (FRAME_SURFACEEMITTER, H3DWxFrame::SurfaceEmitterSettingsDialog)
  EVT_MENU (FRAME_BOUNDEDPHYSICSMODEL, H3DWxFrame::BoundedPhysicsModelSettingsDialog)
  EVT_MENU (FRAME_GRAVITYPHYSICSMODEL, H3DWxFrame::GravityPhysicsModelSettingsDialog)
  EVT_MENU (FRAME_WINDPHYSICSMODEL, H3DWxFrame::WindPhysicsModelSettingsDialog)
  EVT_CLOSE(H3DWxFrame::OnWindowExit)
END_EVENT_TABLE()

/*******************Event Table*********************/


/*******************Member Functions*********************/

bool H3DWxFrame::loadFile( const string &filename) {
  
  char *r = getenv( "H3D_ROOT" );

  string h3d_root = r ? r : ""; 
  
  INIFile ini_file( h3d_root + "/settings/h3dload.ini" );
  
  //Clear existing data
  t->children->clear();
  viewpoint.reset (NULL);

  settings_path = 
    GET_ENV_INI_DEFAULT( "H3D_DISPLAY",
                         h3d_root + "/settings/display/",
                         "display","type",
                         h3d_root + "/settings/common/" );
  
  common_path =  h3d_root + "/settings/common/";

  deviceinfo_file =
    GET_ENV_INI_DEFAULT_FILE( ini_file, "H3D_DEFAULT_DEVICEINFO",
                              settings_path + "/device/",
                              common_path + "/device/",
                              "haptics device","device" );

  stylus_file =
    GET_ENV_INI_DEFAULT_FILE( ini_file, "H3D_STYLUS",
                              common_path + "/stylus/",
                              common_path + "/stylus/",
                              "haptics device","stylus" );
  
  viewpoint_file =
    GET_ENV_INI_DEFAULT_FILE( ini_file, "H3D_DEFAULT_VIEWPOINT",
                              settings_path + "/viewpoint/",
                              common_path + "/viewpoint/",
                              "graphical", "viewpoint" );

  render_mode = GET4( "H3D_RENDERMODE",
                             "graphical", "rendermode",
                             (string)"MONO" );
  
  bool fullscreen    = GET_BOOL("graphical", "fullscreen", false);
  if( char *buffer = getenv("H3D_FULLSCREEN") ) {
    if (strcmp( buffer, "TRUE" ) == 0 ){
      fullscreen = true; }
    else if (strcmp( buffer, "FALSE" ) == 0 ){
      fullscreen = false; }
    else
      Console(4) << "Invalid valid value \"" << buffer 
                 << "\" on environment "
                 << "variable H3D_FULLSCREEN. Must be TRUE or FALSE. "
                 << endl;
  }
  
  bool mirrored      = GET_BOOL("graphical", "mirrored", false);
  if( char *buffer = getenv("H3D_MIRRORED") ) {
    if (strcmp( buffer, "TRUE" ) == 0 ){
      mirrored = true; }
    else if (strcmp( buffer, "FALSE" ) == 0 ){
      mirrored = false; }
    else
      Console(4) << "Invalid valid value \"" << buffer 
                 << "\" on environment "
                 << "variable H3D_MIRRORED. Must be TRUE or FALSE. "<< endl;
  }

  bool use_space_mouse = false;

  // Loading X3D file and setting up VR environment ---
  
  try {
#ifndef MACOSX
    //if( use_space_mouse ) ss.reset( new SpaceWareSensor );
#endif
    X3D::DEFNodes dn;
//    KeyRotation *kr = new KeyRotation;
    QuitAPIField *quit_api = new QuitAPIField;

    if (!DeviceInfo::getActive()) {    
    if( deviceinfo_file.size() ){
      try {
        device_info = X3D::createX3DNodeFromURL( deviceinfo_file );
      } catch( const Exception::H3DException &e ) {
        Console(3) << "Warning: Could not create default DeviceInfo node "
                   << "from file \"" << deviceinfo_file << "\": "
                   << e << endl;
      }
    }

    di = DeviceInfo::getActive();
    hdev = (H3DHapticsDevice *) NULL;

    if( di && !(di->device->empty())) {
      hdev = di->device->getValueByIndex(0);
    }

    if( di && stylus_file.size() ) {
      try {
        default_stylus = X3D::createX3DNodeFromURL( stylus_file );
      } catch( const Exception::H3DException &e ) {
        Console( 4 ) << "Warning: Could not create default stylus "
                     << "from file \"" << stylus_file << "\": "
                     << e << endl;
      }
      
      for( DeviceInfo::MFDevice::const_iterator i = di->device->begin();
           i != di->device->end(); i++ ) {
        H3DHapticsDevice *d = static_cast< H3DHapticsDevice * >(*i);
        if( !d->stylus->getValue() )
          d->stylus->setValue( default_stylus );
      }
    }
    }

    DeviceInfo::DeviceInfoList DEVlist = DeviceInfo::getAllDeviceInfos();
    int devcount = DEVlist.size();
    Console (3) << devcount << endl;

    Console(3) << "Loading " << filename << endl;
    if ( filename.size() > 4 && 
         filename.find( ".wrl", filename.size()-5 ) != string::npos )
      t->children->push_back( X3D::createVRMLFromURL( filename, 
                                                      &dn ) );
      else
        t->children->push_back( X3D::createX3DFromURL( filename, 
                                                       &dn ) );

    ParticleSystem::ParticleSystemList PSList = ParticleSystem::getAllParticleSystems();

	  ParticleSystem::ParticleSystemList::iterator psiterator = PSList.begin();
  	//for (psiterator = PSList.begin(); psiterator != PSList.end(); psiterator++) {
    if(!PSList.empty()) {
      PS = PSList.front();
      if (PS->enabled->getValue()) {
        ParticleStatus = true;
        particleMenu->Insert(0, FRAME_PARTICLECONTROL, "Remove particles", "Stop and remove all particles on screen");
        particleMenu->Insert(1, FRAME_PARTICLECREATION, "Stop particle creation", "Do not generate new particles");
        particleMenu->InsertSeparator(2);
      }
      else {
        ParticleStatus = false;
        particleMenu->Insert(0, FRAME_PARTICLECONTROL, "Display particles", "Start and display all particles on screen");
        particleMenu->Insert(1, FRAME_PARTICLECREATION, "Start particle creation", "Resume generation of new particles");
        particleMenu->InsertSeparator(2);
      }
    }
    else {
      wxMessageDialog PSnotfound ( this, "Particle System node not found.\nCertain menus will be disabled.", "Particle System", wxOK);
      PSnotfound.ShowModal();
      menuBar->EnableTop(1, false);
    }

    pointEmitterSettings = new PointEmitterDialog(this, PS, hdev);
    polylineEmitterSettings = new PolylineEmitterDialog(this, PS, hdev);
    volumeEmitterSettings = new VolumeEmitterDialog(this, PS, hdev);
    coneEmitterSettings = new ConeEmitterDialog(this, PS, hdev);
    explosionEmitterSettings = new ExplosionEmitterDialog(this, PS, hdev);
    surfaceEmitterSettings =  new SurfaceEmitterDialog(this, PS, hdev);
    //}

	/****************************Device Info****************************/
	//Enable Device Menu

#ifndef MACOSX
    if( use_space_mouse )
      g->children->push_back(ss.get());
#endif    
    // create a Viewpoint if it does not exist.
    if( !Viewpoint::getActive() && viewpoint_file.size() ) {
      try {
        viewpoint = X3D::createX3DNodeFromURL( viewpoint_file );
      } catch( const Exception::H3DException &e ) {
        Console(3) << "Warning: Could not create default Viewpoint node "
                   << "from file \"" << viewpoint_file << "\": "
                   << e << endl;
      }
    }
    
    if( X3DBindableNode::getStack( "DeviceInfo" ).size() > 1 ) {
      device_info.reset( NULL );
    }

  	this->glwindow->fullscreen->setValue( fullscreen );
    this->glwindow->mirrored->setValue( mirrored );
    this->glwindow->renderMode->setValue( render_mode );

	
    scene->window->push_back( glwindow );
    scene->sceneRoot->setValue( g.get() );
  }
  catch (const Exception::H3DException &e) {
    wxMessageBox(e.message.c_str(), "Error", wxOK | wxICON_EXCLAMATION);
    return false;
  }
  return true;
}

//Clear data when closing file
void H3DWxFrame::clearData () {
  t->children->clear();
	viewpoint.reset (NULL);
}


//Open a file
void H3DWxFrame::OnOpenFileURL(wxCommandEvent & event) {
   auto_ptr< wxTextEntryDialog > text_dialog( new wxTextEntryDialog ( this,
													   "Enter the location of the file here",
													   "Open file from URL",
													   "") );
   if( text_dialog->ShowModal() == wxID_OK ) {
     string s(text_dialog->GetValue());
     clearData();
     loadFile( s );
   }
}

void H3DWxFrame::OnOpenFile(wxCommandEvent & event)
{
	auto_ptr< wxFileDialog > openFileDialog( new wxFileDialog ( this,
													   "Open file",
													   GetCurrentPath(),
													   "",
													   FILETYPES,
													   wxOPEN,
													   wxDefaultPosition) );

	//Open an X3D file
	if (openFileDialog->ShowModal() == wxID_OK) {
	  SetCurrentFilename(openFileDialog->GetFilename());	
	  SetCurrentPath(openFileDialog->GetDirectory());
    SetStatusText(GetCurrentFilename(), 0);
    SetStatusText(openFileDialog->GetDirectory(),1);
#ifdef WIN32
    wxString wx_filename = currentPath + "\\" + currentFilename;
#else
    wxString wx_filename = currentPath + "/" + currentFilename;
#endif
    string filename(wx_filename);
    clearData();
    loadFile( filename );
  }
}

//Close File
void H3DWxFrame::OnCloseFile(wxCommandEvent & event) {
  //clearData();
  t->children->clear();
	SetStatusText("Open a file...", 0);
  SetStatusText("",1);
}


//About dialog
void H3DWxFrame::OnAbout(wxCommandEvent & event)
{ wxString t = TITLE;

  t.append( AUTHOR );
  
  wxMessageDialog aboutDialog ( this, t, ABOUT, wxOK);
  aboutDialog.ShowModal();
}

//Particle Geometry event
void H3DWxFrame::OnParticleGeometry(wxCommandEvent & event)
{
  wxString geometry;
  switch ( event.GetId() ) {
    case FRAME_QUAD:
      geometry = "QUAD";
      break;
		case FRAME_TRIANGLE:
			geometry = "TRIANGLE";
			break;
		case FRAME_POINT:
  		geometry = "POINT";
			break;
		case FRAME_SPRITE:
			geometry = "SPRITE";
			break;
    case FRAME_LINE:
      geometry = "LINE";
      break;
	}
  PS->geometryType->setValue(geometry.c_str());
}

//Toggle Particles
void H3DWxFrame::ToggleParticles(wxCommandEvent & event)
{
  if (ParticleStatus) {
    PS->enabled->setValue(false);
    ParticleStatus = false;
    particleMenu->Destroy(FRAME_PARTICLECONTROL);
    particleMenu->Insert(0, FRAME_PARTICLECONTROL, "Display Particles", "Start and display all particles on screen");
  }
  else {
    PS->enabled->setValue(true);
    ParticleStatus = true;
    particleMenu->Destroy(FRAME_PARTICLECONTROL);
    particleMenu->Insert(0, FRAME_PARTICLECONTROL, "Remove Particles", "Stop and remove all particles on screen");
  }
}

//Toggle Particle Generation
void H3DWxFrame::ParticleGeneration(wxCommandEvent & event)
{
  /*if (ParticleStatus) {
    PS->createParticles->setValue(false);
    ParticleStatus = false;
    particleMenu->Destroy(FRAME_PARTICLECREATION);
    particleMenu->Insert(1, FRAME_PARTICLECREATION, "Start particle creation", "Resume generation of new particles");
  }
  else {
    PS->createParticles->setValue(true);
    ParticleStatus = true;
    particleMenu->Destroy(FRAME_PARTICLECREATION);
    particleMenu->Insert(1, FRAME_PARTICLECREATION, "Stop particle creation", "Do not generate new particles");
  }*/

}

//Point Emitter settings
void H3DWxFrame::PointEmitterSettingsDialog(wxCommandEvent & event)
{
  /*pointEmitter = (PointEmitter *) NULL;
  pointEmitter = new PointEmitter;
  PS->emitter->setValue(pointEmitter);*/
  pointEmitterSettings->Show();
}

//Polyline Emitter settings
void H3DWxFrame::PolylineEmitterSettingsDialog(wxCommandEvent & event)
{
  polylineEmitterSettings->Show();
}

//Volume Emitter settings
void H3DWxFrame::VolumeEmitterSettingsDialog(wxCommandEvent & event)
{
  volumeEmitterSettings->Show();
}

//Cone Emitter settings
void H3DWxFrame::ConeEmitterSettingsDialog(wxCommandEvent & event)
{
  coneEmitterSettings->Show();
}

//Explosion Emitter settings
void H3DWxFrame::ExplosionEmitterSettingsDialog(wxCommandEvent & event)
{
  explosionEmitterSettings->Show();
}

//Surface Emitter settings
void H3DWxFrame::SurfaceEmitterSettingsDialog(wxCommandEvent & event)
{
  surfaceEmitterSettings->Show();
}

//Bounded Physics Model settings
void H3DWxFrame::BoundedPhysicsModelSettingsDialog(wxCommandEvent & event)
{
  boundedPhysicsModelSettings = new BoundedPhysicsModelDialog(this, PS);
  boundedPhysicsModelSettings->Show();
}

//Wind Physics Model settings
void H3DWxFrame::WindPhysicsModelSettingsDialog(wxCommandEvent & event)
{
  windPhysicsModelSettings = new WindPhysicsModelDialog(this, PS);
  windPhysicsModelSettings->Show();
}

//Gravity Physics Model settings
void H3DWxFrame::GravityPhysicsModelSettingsDialog(wxCommandEvent & event)
{
  gravityPhysicsModelSettings = new GravityPhysicsModelDialog(this, PS);
  gravityPhysicsModelSettings->Show();
}

//Help event
void H3DWxFrame::OnHelp(wxCommandEvent & event)
{

}

//Show console event
void H3DWxFrame::ShowConsole(wxCommandEvent & event)
{
  theConsole->Show();
}

//Gets Menu Selections
void H3DWxFrame::GetSelection (wxMenuEvent & event)
{
	selection = event.GetMenuId();
}

//Exit via menu
void H3DWxFrame::OnExit (wxCommandEvent & event)
{
	Close(true);
}

//Exit via window manager
void H3DWxFrame::OnWindowExit (wxCloseEvent & event) 
{
  Destroy();
}

/*******************Standard trivial functions*********************/
//Get current filename
wxString H3DWxFrame::GetCurrentFilename()
{
 return currentFilename;
}

//Set current filename
void H3DWxFrame::SetCurrentFilename(wxString n)
{
 currentFilename = n;
}

//Get current path
wxString H3DWxFrame::GetCurrentPath()
{
 return currentPath;
}

//Set current path
void H3DWxFrame::SetCurrentPath(wxString n)
{
 currentPath = n;
}

/*******************Create Geometries for use with emitters and physics models*********************/
void H3DWxFrame::CreateGeometries()
{
  ////Box
  box = (Box *) NULL;
  box = new Box;
  box->size->setValue(Vec3f(0.2, 0.2, 0.2));
  box->solid->setValue(true);

  ////Cone
  cone = (Cone *) NULL;
  cone = new Cone;
  cone->bottom->setValue(true);
  cone->bottomRadius->setValue(0.1);
  cone->height->setValue(0.2);
  cone->side->setValue(true);
  cone->solid->setValue(true);

  ////Cylinder
  cylinder = (Cylinder *) NULL;
  cylinder = new Cylinder;
  cylinder->bottom->setValue(true);
  cylinder->height->setValue(0.2);
  cylinder->radius->setValue(0.1);
  cylinder->side->setValue(true);
  cylinder->solid->setValue(true);
  cylinder->top->setValue(true);

  ////Sphere
  sphere = (Sphere *) NULL;
  sphere = new Sphere;
  sphere->radius->setValue(0.1);
  sphere->solid->setValue(true);
}