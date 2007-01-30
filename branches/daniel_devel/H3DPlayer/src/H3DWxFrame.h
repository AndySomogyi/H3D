#include <wx/wx.h>
#include "H3DWxWidgetsWindow.h"

#include "Group.h"
#include "Transform.h"
#include "Scene.h"
#include "KeySensor.h"
#include "MouseSensor.h"
#ifndef MACOSX
#include "SpaceWareSensor.h"
#endif
//#include "DEFNodes.h"
//#include "Viewpoint.h"
#include "DeviceInfo.h"
#include "INIFile.h"
#include "ResourceResolver.h"
//#include "PythonScript.h"
#include "Console.h"

#include "NavigationInfo.h"

#ifdef MACOSX
#include "FreeImage.h"
#include <xercesc/util/PlatformUtils.hpp>
#endif


#include "consoleDialog.h"
using namespace std;
using namespace H3D;

// ---------------------------------------------------------------------------
//  Required class
// ---------------------------------------------------------------------------

class KeyRotation : public TypedField< SFRotation, 
                                       Types< SFInt32,
                                              SFBool,
                                              SFVec2f,
                                              SFRotation> > {
  virtual void update() {
    bool button_pressed = static_cast< SFBool * >(routes_in[1])->getValue();
    Vec2f motion = static_cast< SFVec2f * >(routes_in[2])->getValue();
    motion.y = -motion.y;

    if( button_pressed && motion * motion > Constants::f_epsilon ) {
      Vec2f perp = Vec2f( -motion.y, motion.x );
      perp.normalize();
      value = Rotation( perp.x, perp.y, 0, motion.length() * 0.01f ) * value;
    }
    
    else if( event.ptr == routes_in[0] ) {
      int key = static_cast< SFInt32 * >(routes_in[0])->getValue();
      if( key == KeySensor::UP ) {

		  value = Rotation( 1,0,0,-0.1f ) * value;
      }
      if( key == KeySensor::DOWN ) {
        value = Rotation( 1,0,0,0.1f ) * value;
      }
      if( key == KeySensor::LEFT ) {
        value = Rotation( 0,1,0,-0.1f ) * value;
      }
      if( key == KeySensor::RIGHT ) {
        value = Rotation( 0,1,0,0.1f ) * value;
      }
    }
    else if( routes_in.size() > 3 ) {
      if( event.ptr == routes_in[3] ) {
        Rotation r = static_cast< SFRotation * >(routes_in[3])->getValue();
        value = r * value;
      }
    }
  }
};




/* string GET_ENV_INI_DEFAULT_FILE( INIFile &ini_file,
                            const string &ENV,
                            const string &DISPLAY_PATH,
                            const string &COMMON_PATH,
                            const string &GROUP,
                            const string &VAR ) {
  char *env = getenv(ENV.c_str());
  if( env ) return env;
  
  if( ini_file.hasOption(GROUP,VAR) ) { 
    string option = ini_file.get( GROUP, VAR );
    
    ifstream inp( (DISPLAY_PATH + option).c_str() );
    inp.close();
    if(!inp.fail()) return DISPLAY_PATH + option;

    inp.clear();
    inp.open( (COMMON_PATH + option).c_str() );
    inp.close();
    if(!inp.fail()) return COMMON_PATH + option;
  }
  return "";
} */

// ---------------------------------------------------------------------------
//  H3DWxFrame Definition
// ---------------------------------------------------------------------------

class H3DWxFrame: public wxFrame
{
public:
  H3DWxFrame( wxWindow *_parent, wxWindowID _id, const wxString& _title,
              const wxPoint& _pos, const wxSize& _size,
              long _style = wxDEFAULT_FRAME_STYLE,
              const wxString& name = "H3D Player" );

  //Pointer to H3DWxWidgetsWindow
  H3DWxWidgetsWindow *glwindow;

  /***************Interface Features*****************/
  //Main Menu Bar
  wxMenuBar  *menuBar;
  //Menu Bar Items
  wxMenu     *fileMenu, *rendererMenu, *deviceMenu, *viewpointMenu, *navigationMenu, *advancedMenu, *helpMenu;
  //Submenus
  wxMenu *renderoptionsMenu;
  wxMenu *devicecontrolMenu;

  wxObject menuItem;

  /***************Member Functions*****************/
  void OnOpenFile	(wxCommandEvent & event);
  void OnCloseFile	(wxCommandEvent & event);
  void OnExit     	(wxCommandEvent & event);
  void OnAbout		(wxCommandEvent & event);
  void OnHelp		(wxCommandEvent & event);
  void OnFullscreen	(wxCommandEvent & event);
  void RestoreWindow(wxCommandEvent & event);
  void MirrorScene	(wxCommandEvent & event);
  void RenderMode	(wxCommandEvent & event);
  void ShowConsole	(wxCommandEvent & event);
  void GetSelection (wxMenuEvent & event);
  void ChangeViewpoint (wxCommandEvent & event);
  void ChangeNavigation (wxCommandEvent & event);

  /***************Standard trivial functions***************/
  wxString GetCurrentFilename();
  void SetCurrentFilename(wxString);
  void SetCurrentPath(wxString);
  wxString GetCurrentPath();
  bool validateNavType(string);
  void buildNavMenu();

private:
	wxString currentFilename;
	wxString currentPath;
	bool lastmirror;
	wxString renderMode;
	int selection;
	int viewpointCount;
	int navTypeCount;
	X3DViewpointNode::ViewpointList VPlist;
	NavigationInfo *mynav;

	//One time intialization variables
	string settings_path;
	string common_path;
	string deviceinfo_file;
	string stylus_file;
	string viewpoint_file;
	string render_mode;

	//KeyRotation *kr;

	//Autoref Variables
	AutoRef< Scene > scene;
	AutoRef< KeySensor > ks;
	AutoRef< MouseSensor > ms;
	AutoRef< SpaceWareSensor > ss;
	AutoRef< Transform > t;
	AutoRef< Node > device_info;
	AutoRef< Node > viewpoint;
	AutoRef< Node > default_stylus;
	AutoRef< Group > g;

  auto_ptr< consoleDialog > theConsole;

protected:
  DECLARE_EVENT_TABLE()
};

enum
{ FRAME_EXIT    =   wxID_HIGHEST + 1,
  FRAME_OPEN,
  FRAME_CLOSE,
  FRAME_FULLSCREEN,
  FRAME_RESTORE,
  FRAME_MIRROR,
  FRAME_CONSOLE,
  FRAME_SELECTION,
  FRAME_VIEWPOINTSELECTION,
  FRAME_VIEWPOINT,
//  FRAME_VIEWPOINT,
//  FRAME_MIRRORON,
//  FRAME_MIRROROFF,
//  BASIC_SAVE,
//  BASIC_SAVE_AS,
//  BASIC_FONT,
  FRAME_NAVIGATION = 6500,
  BASIC_CHOOSERENDERER = 6550,
  FRAME_RENDERMODE,
  BASIC_PREFRENDERER,
  BASIC_WIREFRAME,
  BASIC_NOTEXTURES,
  BASIC_SMOOTHSHADING,
  BASIC_SELECT,
  BASIC_DEVICECONTROL,
  BASIC_STARTDEVICE,
  BASIC_STOPDEVICE,
  BASIC_NAVWALK,
  BASIC_NAVEXAMINE,
  BASIC_NAVFLY,
  BASIC_DIR,
  FRAME_ABOUT,
  FRAME_HELP
};
