// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------

#include "H3DWxFrame.h"
#include "consoleDialog.h"
#include <vector>
#include "H3DWxWidgetsWindow.h"
//#include <wx/string.h>
#include <wx/wx.h>
#include "envini.h"

// ---------------------------------------------------------------------------
//  Includes (to open X3D files)
// ---------------------------------------------------------------------------
//#include <fstream>
//#include "X3D.h"
//#include <string.h>

//#include <iostream>

#include "VrmlParser.h"
//#include "X3DSAX2Handlers.h"
//#include "GLUTWindow.h"
#include "Group.h"
#include "Transform.h"
#include "Scene.h"
#include "KeySensor.h"
#include "MouseSensor.h"
#ifndef MACOSX
#include "SpaceWareSensor.h"
#endif
//#include "DEFNodes.h"
#include "Viewpoint.h"
#include "X3DViewpointNode.h"
#include "DeviceInfo.h"
#include "INIFile.h"
#include "ResourceResolver.h"
//#include "PythonScript.h"
#include "Console.h"

#ifdef MACOSX
#include "FreeImage.h"
#include <xercesc/util/PlatformUtils.hpp>
#endif

#include "HapticsRenderers.h"
//#include "OpenHapticsRenderer.h"

using namespace std;
using namespace H3D;

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
static const wxChar *TITLE     = _T("H3D Player ");
static const wxChar *AUTHOR    = _T("\nRamkumar Shankar\n\nCopyright 2006-2007.  All Rights Reserved.");
static const wxChar *ABOUT     = _T("About");
static const wxChar *FILETYPES = _T( "x3d files|*.x3d|"
                                     "All files|*.*"
                                   );

/*******************Constructor*********************/
H3DWxFrame::H3DWxFrame( wxWindow *_parent, wxWindowID _id,
                        const wxString& _title, const wxPoint& _pos,
                        const wxSize& _size, long _style,
                        const wxString& _name ):
wxFrame(_parent, _id, _title, _pos, _size, _style, _name )
{
  theConsole.reset( new consoleDialog(this, wxID_ANY, "Console", wxDefaultPosition, 
                                     wxDefaultSize, wxDEFAULT_DIALOG_STYLE) );

  wxAcceleratorEntry entries[1];
  entries[0].Set(wxACCEL_NORMAL, (int) WXK_ESCAPE, FRAME_RESTORE);
  wxAcceleratorTable accel(1, entries);
  SetAcceleratorTable(accel);

  	scene.reset( new Scene );
	ks.reset ( new KeySensor );
	ms.reset ( new MouseSensor );
	#ifndef MACOSX
	ss.reset (new SpaceWareSensor);
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


  //H3DWxWidgetsWindow *glwindow = new H3DWxWidgetsWindow(theWxFrame);
  //Main Menu Bar
  menuBar  = (wxMenuBar *) NULL;

  //Top level menus
  fileMenu = (wxMenu *)	NULL;
  rendererMenu = (wxMenu *) NULL;
  deviceMenu = (wxMenu *) NULL;
  viewpointMenu = (wxMenu *) NULL;
  navigationMenu = (wxMenu *) NULL;
  advancedMenu = (wxMenu *) NULL;
  helpMenu = (wxMenu *)	NULL;

  //sub menus
  renderoptionsMenu = (wxMenu *) NULL;
//  devicecontrolMenu = (wxMenu *) NULL;

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

  //Render Options submenu
  renderoptionsMenu = new wxMenu;
  renderoptionsMenu->AppendCheckItem(BASIC_WIREFRAME, "Wireframe", "Wireframe");
  renderoptionsMenu->AppendCheckItem(BASIC_NOTEXTURES, "No Textures", "No Textures");
  renderoptionsMenu->AppendCheckItem(BASIC_SMOOTHSHADING, "Smooth Shading", "Smooth Shading");

  //Renderer Menu
  rendererMenu = new wxMenu;
  rendererMenu->AppendCheckItem(FRAME_FULLSCREEN, "Fullscreen Mode", "View in fullscreen");
  rendererMenu->AppendCheckItem(FRAME_MIRROR, "Mirror in Y", "Mirror Scene in Y");
  rendererMenu->AppendSeparator();
  rendererMenu->Append(FRAME_CHOOSERENDERER, "Choose Haptics Renderer", "Select a haptics renderer");
  rendererMenu->Append(FRAME_RENDERMODE, "Render Mode...", "Graphical Renderer Options");
  rendererMenu->AppendSeparator();
  rendererMenu->Append(BASIC_PREFRENDERER, "Rendering Options...", renderoptionsMenu, "Scenegraph rendering options");
  rendererMenu->AppendSeparator();
  rendererMenu->Append(FRAME_SETTINGS, "Settings...", "Scenegraph rendering options");

/*  //Device Control submenu
  devicecontrolMenu = new wxMenu;
  devicecontrolMenu->AppendRadioItem(BASIC_STARTDEVICE, "Start", "Start haptic device(s)");
  devicecontrolMenu->AppendRadioItem(BASIC_STOPDEVICE, "Stop", "Stop haptic device(s)"); */

  //Device Control Menu
  deviceMenu = new wxMenu;
  deviceMenu->AppendCheckItem(FRAME_DEVICECONTROL, "Toggle Haptics", "Start/Stop haptic devices");
  deviceMenu->AppendSeparator();

  //Viewpoint Menu
  viewpointMenu = new wxMenu;
//  viewpointMenu->Append(BASIC_SELECT, "Select", "Select a viewpoint");

  //Navigation Menu
  navigationMenu = new wxMenu;

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
  menuBar->Append(rendererMenu, "&Rendering");
  menuBar->Append(deviceMenu, "&Device Control");
  menuBar->Append(viewpointMenu, "&Viewpoints");
  menuBar->Append(navigationMenu, "&Navigation");
  menuBar->Append(advancedMenu, "&Advanced");
  menuBar->Append(helpMenu, "&Help");
  SetMenuBar(menuBar);

  //Disable some menus initially
  menuBar->EnableTop(2, false);
  menuBar->EnableTop(3, false);
  menuBar->EnableTop(4, false);

}

/*******************Event Table*********************/
BEGIN_EVENT_TABLE(H3DWxFrame, wxFrame)
	EVT_MENU (FRAME_EXIT, H3DWxFrame::OnExit)
	EVT_MENU (FRAME_OPEN, H3DWxFrame::OnOpenFile)
  EVT_MENU (FRAME_OPEN_URL, H3DWxFrame::OnOpenFileURL)
	EVT_MENU (FRAME_CLOSE, H3DWxFrame::OnCloseFile)
	EVT_MENU (FRAME_FULLSCREEN, H3DWxFrame::OnFullscreen)
	EVT_MENU (FRAME_SETTINGS, H3DWxFrame::OnSettings)
  EVT_MENU (FRAME_RESTORE, H3DWxFrame::RestoreWindow)
	EVT_MENU (FRAME_MIRROR, H3DWxFrame::MirrorScene)
	EVT_MENU (FRAME_RENDERMODE, H3DWxFrame::RenderMode)
	EVT_MENU (FRAME_CONSOLE, H3DWxFrame::ShowConsole)
	EVT_MENU_HIGHLIGHT (FRAME_SELECTION, H3DWxFrame::GetSelection)
	EVT_MENU (FRAME_VIEWPOINT, H3DWxFrame::ChangeViewpoint)
	EVT_MENU (FRAME_NAVIGATION, H3DWxFrame::ChangeNavigation)
  EVT_MENU (FRAME_CHOOSERENDERER, H3DWxFrame::ChangeRenderer)
  EVT_MENU (FRAME_DEVICECONTROL, H3DWxFrame::ToggleHaptics)
	//EVT_MENU (BASIC_SAVE, BasicFrame::OnSave)
	//EVT_MENU (BASIC_SAVE_AS, BasicFrame::OnSaveAs)
	//EVT_MENU (BASIC_FONT, BasicFrame::OnChooseFont)
	//EVT_MENU (BASIC_DIR, BasicFrame::OnChooseDir)
	EVT_MENU (FRAME_ABOUT, H3DWxFrame::OnAbout)
	EVT_MENU (FRAME_HELP, H3DWxFrame::OnHelp)
END_EVENT_TABLE()


/*******************Member Functions*********************/

bool H3DWxFrame::loadFile( const string &filename ) {
  string h3d_root = getenv( "H3D_ROOT" );
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
    if( use_space_mouse ) ss.reset( new SpaceWareSensor );
#endif
    X3D::DEFNodes dn;
//    KeyRotation *kr = new KeyRotation;
    QuitAPIField *quit_api = new QuitAPIField;
    
    if( deviceinfo_file.size() ){
      try {
        device_info = X3D::createX3DNodeFromURL( deviceinfo_file );
      } catch( const Exception::H3DException &e ) {
        Console(3) << "Warning: Could not create default DeviceInfo node "
                   << "from file \"" << deviceinfo_file << "\": "
                   << e << endl;
      }
    }

    DeviceInfo *di = DeviceInfo::getActive();
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

    Console(3) << "Loading " << filename << endl;
    if ( filename.size() > 4 && 
         filename.find( ".wrl", filename.size()-5 ) != string::npos )
      t->children->push_back( X3D::createVRMLFromURL( filename, 
                                                      &dn ) );
      else
        t->children->push_back( X3D::createX3DFromURL( filename, 
                                                       &dn ) );
  

	/****************************Intialize Viewpoints****************************/
	//Enable Viewpoints Menu
	menuBar->EnableTop(3, true);
	//Create Viewpoint list
	VPlist = X3DViewpointNode::getViewpointHierarchy();
	//Store number of viewpoints for processing later
	viewpointCount = VPlist.size();
	//Create Viewpoints menu using list
	//If no viewpoints are specified in the file
	if (viewpointCount == 0) {
		viewpointMenu->Append(FRAME_VIEWPOINT, "Default", "Default Viewpoint");
		viewpointMenu->Enable(FRAME_VIEWPOINT, false);
	}
	else {
		int i = 0;
		for (X3DViewpointNode::ViewpointList::iterator vp = VPlist.begin(); vp != VPlist.end(); vp++) {
			string vpDescription = (*vp)->description->getValue();
			viewpointMenu->AppendRadioItem(FRAME_VIEWPOINT + i, vpDescription.c_str(), "Select a viewpoint");
			if ((*vp)->isBound->getValue()) {
				viewpointMenu->Check(FRAME_VIEWPOINT+i, true);
			}
			Connect(FRAME_VIEWPOINT +i,wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler(H3DWxFrame::GetSelection));
			Connect(FRAME_VIEWPOINT +i,wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(H3DWxFrame::ChangeViewpoint));
      i++;
		}
  }


	/****************************Navigation Info****************************/
	//Enable Navigation Menu
	menuBar->EnableTop(4, true);
//	H3DWxFrame::buildNavMenu();
	buildNavMenu();

	/****************************Device Info****************************/
	//Enable Device Menu
	menuBar->EnableTop(2, true);
  if (DeviceInfo::getActive()) {
    mydevice = DeviceInfo::getActive();
    //myH3Ddevice = mydevice->device->getValue();
    allDevices = mydevice->device->getValue();
    NodeVector::const_iterator nv = allDevices.begin();
  }

  int i = 0;
  for (NodeVector::const_iterator nv = allDevices.begin(); nv != allDevices.end(); nv++) {
    wxString item = wxString ("Device ") << i;
		deviceMenu->AppendRadioItem(FRAME_DEVICE + i, item, "Select a viewpoint");
    i++;
  }

    // set up routes to rotate the model
/*    ks->keyPress->route( quit_api );
    ks->actionKeyPress->route( kr );
    ms->leftButton->route( kr );
    ms->motion->route( kr );
#ifndef MACOSX
    if( use_space_mouse )
      ss->instantRotation->route( kr );
#endif
	//kr->setValue( Rotation(1, 0, 0, 0 ) );
	kr->route( t->rotation ); */

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

    g->children->push_back( t.get() );

    
    // create a window to display
    
//    H3DWxFrame *theWxFrame = new H3DWxFrame(NULL, wxID_ANY, "H3D Player", wxDefaultPosition, wxSize(800, 600));
//	theWxFrame->glwindow = new H3DWxWidgetsWindow(theWxFrame);
	//This next line is used to set the icon file h3d.ico, when created.
	//theWxframe->SetIcon(wxIcon(_T("h3d_icn")));
    //H3DWxWidgetsWindow *glwindow = new H3DWxWidgetsWindow(theWxFrame);
    
    // Using this line instead of the two previous lines will make
    // H3DWxWidgetsWindow create an instance of a wxframe with no menus and use
    // this as parent to the canvas.
    // H3DWxWidgetsWindow *glwindow = new H3DWxWidgetsWindow();

	this->glwindow->fullscreen->setValue( fullscreen );
    this->glwindow->mirrored->setValue( mirrored );
    this->glwindow->renderMode->setValue( render_mode );

	
//    this->glwindow->width->setValue(width);
//    this->glwindow->height->setValue(height);

   /* H3DWxWidgetsWindow *glwindow2 = new H3DWxWidgetsWindow;
    glwindow2->fullscreen->setValue( fullscreen );
    glwindow2->mirrored->setValue( mirrored );
    glwindow2->renderMode->setValue( render_mode );
    glwindow2->width->setValue(width);
    glwindow2->height->setValue(height);*/
    scene->window->push_back( glwindow );
    //scene->window->push_back( glwindow2 );
    scene->sceneRoot->setValue( g.get() );
  }
  catch (const Exception::H3DException &e) {
    MessageBox(HWND_DESKTOP, e.message.c_str(), 
                             "Error", MB_OK | MB_ICONEXCLAMATION);
    return false;
  }
  return true;
}

//Open a file
void H3DWxFrame::OnOpenFileURL(wxCommandEvent & event) {
   auto_ptr< wxTextEntryDialog > text_dialog( new wxTextEntryDialog ( this,
													   "Open URL",
													   GetCurrentPath(),
													   "") );
   if( text_dialog->ShowModal() == wxID_OK ) {
     string s = text_dialog->GetValue();
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
    
    string filename =  currentPath += "\\" + currentFilename;
    loadFile( filename );
  }

}

//Close File
void H3DWxFrame::OnCloseFile(wxCommandEvent & event) {
	t->children->clear();
	SetStatusText("Open a file...", 0);
  SetStatusText("",1);
	//Viewpoint::set_bind(false);
	//viewpoint.get()->set_bind->setValue(false);
	viewpoint.reset (NULL);
	device_info.reset (NULL);
	//kr->setValue( Rotation(1, 0, 0, 0 ) );

	//Delete items from viewpoint menu & disconnect events
	for (int i = 0; i <= viewpointCount; i++) {
		viewpointMenu->Destroy(FRAME_VIEWPOINT + i);
		Disconnect(FRAME_VIEWPOINT + i,wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler(H3DWxFrame::GetSelection));
		Disconnect(FRAME_VIEWPOINT + i,wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(H3DWxFrame::ChangeViewpoint));
	}

	//Delete items from navigation menu & disconnect events
	for (int j = 0; j <= navTypeCount; j++) {
		navigationMenu->Destroy(FRAME_NAVIGATION + j);
		Disconnect(FRAME_NAVIGATION + j,wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler(H3DWxFrame::GetSelection));
		Disconnect(FRAME_NAVIGATION + j,wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(H3DWxFrame::ChangeNavigation));
	}

	//Disable menus again
  menuBar->EnableTop(2, false);
	menuBar->EnableTop(3, false);
	menuBar->EnableTop(4, false);
}


//About dialog
void H3DWxFrame::OnAbout(wxCommandEvent & event)
{ wxString t = TITLE;

  t.append( AUTHOR );
  
  wxMessageDialog aboutDialog ( this, t, ABOUT, wxOK);
  aboutDialog.ShowModal();
}

//Help event
void H3DWxFrame::OnHelp(wxCommandEvent & event)
{

}

//Fullscreen mode
void H3DWxFrame::OnFullscreen (wxCommandEvent & event)
{
	H3DWxFrame::ShowFullScreen(true, wxFULLSCREEN_NOMENUBAR | wxFULLSCREEN_NOTOOLBAR | wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION );
	glwindow->fullscreen->setValue( true );
	rendererMenu->Check(FRAME_FULLSCREEN, true);
	SetStatusText("Press ESC to exit fullscreen mode", 0);
  SetStatusText("Viewing in Fullscreen", 1);
}

//Restore from fullscreen
void H3DWxFrame::RestoreWindow(wxCommandEvent & event)
{
	H3DWxFrame::ShowFullScreen(false, wxFULLSCREEN_ALL);
	glwindow->fullscreen->setValue( false );
	rendererMenu->Check(FRAME_FULLSCREEN, false);
	SetStatusText(currentFilename, 0);
	SetStatusText(currentPath, 1);
}

void H3DWxFrame::MirrorScene(wxCommandEvent & event)
{
	lastmirror = glwindow->mirrored->getValue();
	glwindow->mirrored->setValue(!lastmirror);
	if ( glwindow->mirrored->getValue() ) {
		SetStatusText("Scene mirrored in Y", 0);
	}
	else {
		SetStatusText(currentFilename, 0);
	}
}

//Render Mode
void H3DWxFrame::RenderMode(wxCommandEvent & event)
{
	const wxString rendermodes[] = { _T("No Stereo"), 
									 _T("Quad Buffered Stereo"),
									 _T("Horizontal Split"),
									 _T("Vertical Split"),
									 _T("Horizontal Interlaced"),
									 _T("Vertical Interlaced"),
									 _T("Sharp Display"),
									 _T("Red-Blue Stereo"),
									 _T("Red-Cyan Stereo") } ;

	wxSingleChoiceDialog selectMode(this,
                                _T("Please select a graphical rendering mode"),
                                _T("Render Modes"),
                                WXSIZEOF(rendermodes), rendermodes);

	if (selectMode.ShowModal() == wxID_OK)
    {
		switch ( selectMode.GetSelection() ) {
			case 0:
				renderMode = "MONO";
				break;
			case 1:
				renderMode = "QUAD_BUFFERED_STEREO";
				break;
			case 2:
				renderMode = "HORIZONTAL_SPLIT";
				break;
			case 3:
				renderMode = "VERTICAL_SPLIT";
				break;
			case 4:
				renderMode = "HORIZONTAL_INTERLACED";
				break;
			case 5:
				renderMode = "VERTICAL_INTERLACED";
				break;
			case 6:
				renderMode = "VERTICAL_INTERLACED_GREEN_SHIFT";
				break;
			case 7:
				renderMode = "RED_BLUE_STEREO";
				break;
			case 8:
				renderMode = "RED_CYAN_STEREO";
				break;
		}

		glwindow->renderMode->setValue( renderMode.c_str());

        //wxMessageDialog dialog2(this, selectMode.GetStringSelection(), _T("Got string"));
        //dialog2.ShowModal();
    }
}

//Choose Haptics Renderer
void H3DWxFrame::ChangeRenderer(wxCommandEvent & event)
{
  const wxString hapticrenderers[] = { _T("OpenHaptics"), 
									 _T("CHAI3D"),
									 _T("God Object"),
									 _T("Ruspini") } ;

	wxSingleChoiceDialog selectHapticRenderer(this,
                                _T("Please select a haptic renderer"),
                                _T("Haptic Renderers"),
                                WXSIZEOF(hapticrenderers), hapticrenderers);

	if (selectHapticRenderer.ShowModal() == wxID_OK) {
		switch ( selectHapticRenderer.GetSelection() ) {
			case 0:
				renderMode = "OpenHaptics";
        myH3Ddevice = mydevice->device->getValueByIndex(0);
        myH3Ddevice->hapticsRenderer->setValue(new OpenHapticsRenderer());
//        for (NodeVector::const_iterator nv = allDevices.begin(); nv != allDevices.end(); nv++) {
//          static_cast < H3DHapticsDevice *> (*nv)->hapticsRenderer->setValue(new OpenHapticsRenderer);
//        }
				break;
			case 1:
//				renderMode = "CHAI3D";
#ifdef HAVE_CHAI3D
//        for (NodeVector::const_iterator nv = allDevices.begin(); nv != allDevices.end(); nv++) {
//          static_cast < H3DHapticsDevice *> (*nv)->hapticsRenderer->setValue(new CHAI3DRenderer);
//        }
#endif
				break;
			case 2:
				renderMode = "God Object";
        myH3Ddevice = mydevice->device->getValueByIndex(0);
        myH3Ddevice->hapticsRenderer->setValue(new GodObjectRenderer);
//        for (NodeVector::const_iterator nv = allDevices.begin(); nv != allDevices.end(); nv++) {
//          static_cast < H3DHapticsDevice *> (*nv)->hapticsRenderer->setValue(new GodObjectRenderer);
//        }
				break;
			case 3:
				renderMode = "Ruspini";
        myH3Ddevice = mydevice->device->getValueByIndex(0);
        myH3Ddevice->hapticsRenderer->setValue(new RuspiniRenderer);
/*        for (NodeVector::const_iterator nv = allDevices.begin(); nv != allDevices.end(); nv++) {
          static_cast < H3DHapticsDevice *> (*nv)->hapticsRenderer->setValue(new RuspiniRenderer);
        } */
				break;
    }
  }
}

//Toggle haptics
void H3DWxFrame::ToggleHaptics (wxCommandEvent & event) {
  	//device_info.reset (NULL);
}

//Show console event
void H3DWxFrame::ShowConsole(wxCommandEvent & event)
{
  theConsole->Show();
}

//Change Viewpoint
void H3DWxFrame::ChangeViewpoint (wxCommandEvent & event)
{	
	//Default viewpoint
	if (viewpointCount == 0) {
		//do nothing
	}
	else {
		//Find viewpoint index
		int index = selection - FRAME_VIEWPOINT;
		//Get to that index in the viewpoint list
		X3DViewpointNode::ViewpointList::iterator vp = VPlist.begin();
		for (int i = 0; i < index; i++) {
			vp++;
		}
		//Enable that viewpoint
		(*vp)->set_bind->setValue(true);
	}
}

//Change Navigation
void H3DWxFrame::ChangeNavigation (wxCommandEvent & event)
{
	mynav->setNavType ((navigationMenu->GetLabel(selection)).c_str());
}

//Gets Menu Selections
void H3DWxFrame::GetSelection (wxMenuEvent & event)
{
	selection = event.GetMenuId();
}

//Exit event
void H3DWxFrame::OnExit (wxCommandEvent & event)
{
	Close(TRUE); 
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

//Build Navigation Menu
void H3DWxFrame::buildNavMenu () {
	//Get active navigation info object
	if (NavigationInfo::getActive()) {
		mynav = NavigationInfo::getActive();
		//Store allowed navigation types and count
		vector<string> navTypes = mynav->type->getValue();

		if (mynav->getUsedNavType() == "NONE") {
			navigationMenu->Append(FRAME_NAVIGATION, "Unavailable", "Navigation Disabled");
			navigationMenu->Enable(FRAME_NAVIGATION, false);
		}
		else {
			vector<string> allowedTypes;
			vector<string>::iterator navList = navTypes.begin();
      bool hasAny = false;
			for (vector<string>::iterator navList = navTypes.begin(); navList != navTypes.end(); navList++) {
				if (validateNavType(*navList) && (*navList != "NONE")) {
					if (allowedTypes.empty()) {
            if ((*navList != "ANY")) {
              allowedTypes.push_back(*navList);
            }
            else {
              allowedTypes.push_back("EXAMINE");
              allowedTypes.push_back("FLY");
              allowedTypes.push_back("WALK");
              allowedTypes.push_back("LOOKAT");
              break;
            }
          }
					else {
						bool found = false;
						for (vector<string>::iterator allowedList = allowedTypes.begin(); allowedList != allowedTypes.end(); allowedList++) {
              if ((*navList == "ANY")) {
                hasAny = true;
                found = true;
              }
              else if ((*allowedList) == (*navList)) {
								found = true;
								break;
							}
						}
						if (!found) {
							allowedTypes.push_back(*navList);
						}
					}
				}
      }
      if (hasAny) {
        vector<string> allTypes;
        allTypes.push_back("EXAMINE");
        allTypes.push_back("FLY");
        allTypes.push_back("WALK");
        allTypes.push_back("LOOKAT");
   			vector<string>::iterator allList = allTypes.begin();
  			for (vector<string>::iterator allList = allTypes.begin(); allList != allTypes.end(); allList++) {
          bool found = false;
          for (vector<string>::iterator allowedList = allowedTypes.begin(); allowedList != allowedTypes.end(); allowedList++) {
            if ((*allowedList) == (*allList)) {
              found = true;
              break;
            }
          }
          if (!found) {
            allowedTypes.push_back(*allList);
          }
        }
      }
			navTypeCount = allowedTypes.size();
			int j = 0;
			for (vector<string>::iterator menuList = allowedTypes.begin(); menuList != allowedTypes.end(); menuList++) {
				string typeName = (*menuList);
				navigationMenu->AppendRadioItem(FRAME_NAVIGATION + j, typeName.c_str(), "Select a navigation mode");
				Connect(FRAME_NAVIGATION + j,wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler(H3DWxFrame::GetSelection));
				Connect(FRAME_NAVIGATION + j,wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(H3DWxFrame::ChangeNavigation));
				j++;
      }
/*      if ((mynav->getUsedNavType() == "ANY") && (navTypeCount > 1)) {
        mynav->setNavType ((navigationMenu->GetLabel(FRAME_NAVIGATION)).c_str());
        navigationMenu->Check(FRAME_NAVIGATION, true);
      }
      else {
        mynav->setNavType ("NULL");
      } */
    int index = 0;
 		  for (vector<string>::iterator menuList = allowedTypes.begin(); menuList != allowedTypes.end(); menuList++) {
        if (mynav->getUsedNavType() == (*menuList)) {
          navigationMenu->Check(FRAME_NAVIGATION+index, true);
          break;
        }
        index++;
      }
    }
  }
  else {
    mynav = new NavigationInfo;
    vector<string> allTypes;
    allTypes.push_back("EXAMINE");
    allTypes.push_back("FLY");
    allTypes.push_back("WALK");
    allTypes.push_back("LOOKAT");
    allTypes.push_back("NONE");
    int j = 0;
    for (vector<string>::iterator allList = allTypes.begin(); allList != allTypes.end(); allList++) {
      navigationMenu->AppendRadioItem(FRAME_NAVIGATION + j, (*allList), "Select a navigation mode");
		  Connect(FRAME_NAVIGATION + j,wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler(H3DWxFrame::GetSelection));
			Connect(FRAME_NAVIGATION + j,wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(H3DWxFrame::ChangeNavigation));
			j++;
    }
    mynav->setNavType ("EXAMINE");
  }
}


//Validate NavType
//Checks whether a navigation type is valid
bool H3DWxFrame::validateNavType (string a) {
	if ( a == "ANY" || a == "EXAMINE" || a == "FLY" || a == "WALK" || a == "LOOKAT" || a == "NONE" ) {
		return true;
	}
	return false;
}


void H3DWxFrame::OnSettings (wxCommandEvent & event) {
  SettingsDialog dialog(this, event.GetId());
  dialog.ShowModal();
}


// ----------------------------------------------------------------------------
// SettingsDialog
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(SettingsDialog, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(SettingsDialog, wxPropertySheetDialog)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* win, int dialogType)
{
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);

    int tabImage1 = -1;
    int tabImage2 = -1;
    int tabImage3 = -1;

    int resizeBorder = wxRESIZE_BORDER;

    m_imageList = NULL;

    Create(win, wxID_ANY, _("Settings"), wxDefaultPosition, wxDefaultSize,
        wxDEFAULT_DIALOG_STYLE| (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, resizeBorder)
    );

    // If using a toolbook, also follow Mac style and don't create buttons
    CreateButtons(wxOK | wxCANCEL |
                        (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxHELP)
    );

    wxBookCtrlBase* notebook = GetBookCtrl();
    notebook->SetImageList(m_imageList);

    wxPanel* generalSettings = CreateGeneralSettingsPage(notebook);
    wxPanel* openhaptics_settings = CreateOpenHapticsSettingsPage(notebook);
    wxPanel* debug_settings = CreateDebugSettingsPage(notebook);

    notebook->AddPage(generalSettings, _("General"), true, tabImage1);
    notebook->AddPage(openhaptics_settings, _("OpenHaptics"), false, tabImage2);
    notebook->AddPage(debug_settings, _("Debug"), false, tabImage3);

    LayoutDialog();
}

SettingsDialog::~SettingsDialog()
{
    
}

wxPanel* SettingsDialog::CreateDebugSettingsPage(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    //// adaptive viewpoirt

    wxBoxSizer* draw_bound_sizer = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* draw_bound = new wxCheckBox(panel, ID_DRAW_BOUNDS, _("&Draw geometry bounding boxes"), wxDefaultPosition, wxDefaultSize);
    draw_bound_sizer->Add(draw_bound, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add( draw_bound_sizer, 0, wxGROW|wxALL, 0);

    wxBoxSizer* draw_triangles_sizer = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* draw_triangles = new wxCheckBox(panel, ID_DRAW_TRIANGLES, _("&Draw haptic triangles"), wxDefaultPosition, wxDefaultSize);
    draw_triangles_sizer->Add(draw_triangles, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add( draw_triangles_sizer, 0, wxGROW|wxALL, 0);

    wxBoxSizer* draw_tree_sizer = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* draw_tree = new wxCheckBox(panel, ID_DRAW_BOUND_TREE, _("&Draw bound tree"), wxDefaultPosition, wxDefaultSize);
    draw_tree_sizer->Add(draw_tree, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add( draw_tree_sizer, 0, wxGROW|wxALL, 0);

    wxBoxSizer* depth_sizer = new wxBoxSizer( wxHORIZONTAL );
    depth_sizer->Add(new wxStaticText(panel, wxID_ANY, _("&Depth:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    
    wxSpinCtrl* depth_spin = new wxSpinCtrl(panel, ID_CACHING_DELAY, wxEmptyString, wxDefaultPosition,
        wxSize(80, wxDefaultCoord));
    depth_sizer->Add(depth_spin, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add( depth_sizer );
  
    topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}


wxPanel* SettingsDialog::CreateOpenHapticsSettingsPage(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    //// adaptive viewpoirt

    wxBoxSizer* adaptive_viewport_sizer = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* adaptive_viewport = new wxCheckBox(panel, ID_ADAPTIVE_VIEWPORT, _("&Use adaptive viewport"), wxDefaultPosition, wxDefaultSize);
    adaptive_viewport_sizer->Add(adaptive_viewport, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add( adaptive_viewport_sizer, 0, wxGROW|wxALL, 0);

    wxBoxSizer* haptic_camera_sizer = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* haptic_camera = new wxCheckBox(panel, ID_HAPTIC_CAMERA, _("&Use haptic camera view"), wxDefaultPosition, wxDefaultSize);
    haptic_camera_sizer->Add(haptic_camera, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add( haptic_camera_sizer, 0, wxGROW|wxALL, 0);

    wxBoxSizer* full_geom_render_sizer = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* full_geom_render = new wxCheckBox(panel, ID_FULL_GEOMETRY_RENDER, _("&Force full geometry render"), wxDefaultPosition, wxDefaultSize);
    full_geom_render_sizer->Add(full_geom_render, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add( full_geom_render_sizer, 0, wxGROW|wxALL, 0);

    wxArrayString shape_choices;
    shape_choices.Add(wxT("Feedback buffer"));
    shape_choices.Add(wxT("Depth buffer"));
    shape_choices.Add(wxT("Custom"));

    wxBoxSizer* shape_sizer = new wxBoxSizer( wxHORIZONTAL );
 
    shape_sizer->Add(new wxStaticText(panel, wxID_ANY, _("&Default shape type:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    wxChoice* shape_choice = new wxChoice(panel, ID_BOUND_TYPE, wxDefaultPosition, wxDefaultSize, shape_choices );
    shape_sizer->Add(shape_choice, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(shape_sizer, 0, wxGROW|wxALL, 5);

    topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

wxPanel* SettingsDialog::CreateGeneralSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    //// Graphics caching
    wxStaticBox* graphics_caching_box = new wxStaticBox(panel, wxID_ANY, _("Graphics caching:"));

    wxBoxSizer* graphics_caching_sizer = new wxStaticBoxSizer( graphics_caching_box, wxVERTICAL );
    item0->Add(graphics_caching_sizer, 0, wxGROW|wxALL, 5);

    wxBoxSizer* display_list_sizer = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* display_list_checkbox = new wxCheckBox(panel, ID_USE_DISPLAY_LISTS, _("&Use display lists"), wxDefaultPosition, wxDefaultSize);
    display_list_sizer->Add(display_list_checkbox, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    graphics_caching_sizer->Add(display_list_sizer, 0, wxGROW|wxALL, 0);

    wxCheckBox* only_geoms_checkbox = new wxCheckBox(panel, ID_CACHE_ONLY_GEOMS, _("&Cache only geometries"), wxDefaultPosition, wxDefaultSize);
    display_list_sizer->Add(only_geoms_checkbox, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  //  graphics_caching_sizer->Add(only_geoms_sizer, 0, wxGROW|wxALL, 0);

    wxBoxSizer* caching_delay_sizer = new wxBoxSizer( wxHORIZONTAL );
    caching_delay_sizer->Add(new wxStaticText(panel, wxID_ANY, _("&Caching delay:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    
    wxSpinCtrl* caching_delay_spin = new wxSpinCtrl(panel, ID_CACHING_DELAY, wxEmptyString, wxDefaultPosition,
        wxSize(80, wxDefaultCoord));
    caching_delay_sizer->Add(caching_delay_spin, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    graphics_caching_sizer->Add( caching_delay_sizer );

    // Geometry bound
    wxStaticBox* bound_tree_box = new wxStaticBox(panel, wxID_ANY, _("Bound tree:"));

    wxArrayString bound_choices;
    bound_choices.Add(wxT("Oriented bounding box"));
    bound_choices.Add(wxT("Axis-aligned bounding box"));
    bound_choices.Add(wxT("Sphere"));

    wxBoxSizer* bound_tree_sizer = new wxStaticBoxSizer( bound_tree_box, wxVERTICAL );
    item0->Add(bound_tree_sizer, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemSizer2 = new wxBoxSizer( wxHORIZONTAL );

    wxChoice* choice2 = new wxChoice(panel, ID_BOUND_TYPE, wxDefaultPosition, wxDefaultSize, bound_choices );

    itemSizer2->Add(new wxStaticText(panel, wxID_ANY, _("&Bound type:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
   // itemSizer2->Add(5, 5, 1, wxALL, 0);
    itemSizer2->Add(choice2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    bound_tree_sizer->Add(itemSizer2, 0, wxGROW|wxALL, 5);

    wxBoxSizer* max_triangles_sizer = new wxBoxSizer( wxHORIZONTAL );
    max_triangles_sizer->Add(new wxStaticText(panel, wxID_ANY, _("&Max triangles:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    
    wxSpinCtrl* max_triangles_spin = new wxSpinCtrl(panel, ID_MAX_TRIANGLES, wxEmptyString, wxDefaultPosition,
        wxSize(80, wxDefaultCoord));
    max_triangles_sizer->Add(max_triangles_spin, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bound_tree_sizer->Add( max_triangles_sizer );

    //// FONT SIZE SELECTION

    wxStaticBox* haptics_box = new wxStaticBox(panel, wxID_ANY, _("Haptics options:"));
    wxBoxSizer* haptics_box_sizer = new wxStaticBoxSizer( haptics_box, wxVERTICAL );

    wxArrayString face_choices;
    face_choices.Add(wxT("Front and back"));
    face_choices.Add(wxT("Front only"));
    face_choices.Add(wxT("Back only"));

    wxBoxSizer* face_choice_sizer = new wxBoxSizer( wxHORIZONTAL );
    face_choice_sizer->Add(new wxStaticText(panel, wxID_ANY, _("&Touchable face:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    wxChoice* face_choice = new wxChoice(panel, ID_TOUCHABLE_FACE, wxDefaultPosition, wxDefaultSize, face_choices );
    face_choice_sizer->Add(face_choice, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    haptics_box_sizer->Add(face_choice_sizer, 0, wxGROW|wxALL, 5);

    wxBoxSizer* max_distance_sizer = new wxBoxSizer( wxHORIZONTAL );
    max_distance_sizer->Add(new wxStaticText(panel, wxID_ANY, _("&Max distance:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* max_distance = new wxTextCtrl(panel, ID_MAX_DISTANCE, wxEmptyString, wxDefaultPosition,
        wxSize(40, wxDefaultCoord)); 
    max_distance_sizer->Add(max_distance, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    haptics_box_sizer->Add(max_distance_sizer, 0, wxGROW|wxALL, 5);

    wxBoxSizer* look_ahead_sizer = new wxBoxSizer( wxHORIZONTAL );
    look_ahead_sizer->Add(new wxStaticText(panel, wxID_ANY, _("&Look ahead factor:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* look_ahead = new wxTextCtrl(panel, ID_LOOK_AHEAD_FACTOR, wxEmptyString, wxDefaultPosition,
        wxSize(40, wxDefaultCoord) );
    look_ahead_sizer->Add(look_ahead, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    haptics_box_sizer->Add(look_ahead_sizer, 0, wxGROW|wxALL, 5);

    wxBoxSizer* use_bound_tree_sizer = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* use_bound_tree_checkbox = new wxCheckBox(panel, ID_USE_BOUND_TREE, _("&Use bound tree"), wxDefaultPosition, wxDefaultSize);
    use_bound_tree_sizer->Add(use_bound_tree_checkbox, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    //haptics_box_sizer->Add(use_bound_tree_checkbox, 0, wxGROW|wxALL, 0);
    haptics_box_sizer->Add(use_bound_tree_sizer, 0, wxGROW|wxALL, 0);

    item0->Add(haptics_box_sizer, 0, wxGROW|wxLEFT|wxRIGHT, 5);

    topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    topSizer->AddSpacer(5);

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

