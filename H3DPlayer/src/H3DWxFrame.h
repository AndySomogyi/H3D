#include <wx/wx.h>
#include <wx/propdlg.h> 
#include <wx/bookctrl.h>
#include <wx/docview.h>
#include <wx/spinctrl.h>
#include <wx/config.h>
#include <wx/confbase.h>
#include "H3DWxWidgetsWindow.h"
#include "GlobalSettings.h"

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

/* class KeyRotation : public TypedField< SFRotation, 
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
}; */




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

// Property sheet dialog
class SettingsDialog: public wxPropertySheetDialog
{
DECLARE_CLASS(SettingsDialog)
public:
    SettingsDialog(wxWindow* parent, GlobalSettings *gs );
    ~SettingsDialog();

  void handleSettingsChange (wxCommandEvent & event);
  void handleSpinEvent (wxSpinEvent & event);

    wxPanel* CreateGeneralSettingsPage(wxWindow* parent, GlobalSettings *gs );
    wxPanel* CreateOpenHapticsSettingsPage(wxWindow* parent, GlobalSettings *gs);
  wxPanel* CreateDebugSettingsPage(wxWindow* parent, GlobalSettings *gs);
  
  
protected:

    enum {
        ID_SHOW_TOOLTIPS = 100,

        ID_AUTO_SAVE,
        ID_AUTO_SAVE_MINS,
        ID_LOAD_LAST_PROJECT,

        ID_APPLY_SETTINGS_TO,
        ID_BACKGROUND_STYLE,
        ID_FONT_SIZE,

        ID_MAX_TRIANGLES,
        ID_USE_DISPLAY_LISTS,
        ID_CACHE_ONLY_GEOMS,
        ID_CACHING_DELAY,
        ID_BOUND_TYPE,
        ID_MAX_DISTANCE,
        ID_TOUCHABLE_FACE,
        ID_LOOK_AHEAD_FACTOR,
        ID_USE_BOUND_TREE,
        ID_OH_SHAPE_TYPE,
        ID_ADAPTIVE_VIEWPORT,
        ID_HAPTIC_CAMERA,
        ID_FULL_GEOMETRY_RENDER,
        ID_DRAW_BOUNDS,
        ID_DRAW_TRIANGLES,
        ID_DRAW_BOUND_TREE,
        ID_DRAW_TREE_DEPTH
        
    };

    wxImageList*    m_imageList;

DECLARE_EVENT_TABLE()
};

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
  //File History Menu
  wxFileHistory *recentFiles;
  //Submenus
  wxMenu *renderoptionsMenu;


  //Config object to save information (settings, file history etc...)
  wxConfigBase *recentList;

  bool loadFile( const string &file );
  void clearData();

  /***************Member Functions*****************/
  void OnOpenFile	(wxCommandEvent & event);
  void OnMRUFile  (wxCommandEvent & event);
  void OnOpenFileURL	(wxCommandEvent & event);
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
  void ChangeRenderer (wxCommandEvent & event);
  void ToggleHaptics  (wxCommandEvent & event);
  void OnSettings( wxCommandEvent & event);

  /***************Standard trivial functions***************/
  wxString GetCurrentFilename();
  void SetCurrentFilename(wxString);
  void SetCurrentPath(wxString);
  wxString GetCurrentPath();
  bool validateNavType(string);
  void SaveMRU ();
  void buildNavMenu();
  void readSettingsFromINIFile( const string &filename,GlobalSettings *gs );

private:
	wxString currentFilename;
	wxString currentPath;
	bool lastmirror;
	wxString renderMode;
	int selection;
	int viewpointCount;
	int navTypeCount;
  int deviceCount;
	X3DViewpointNode::ViewpointList VPlist;
	NavigationInfo *mynav;
	DeviceInfo *mydevice;
	H3DHapticsDevice *myH3Ddevice;
	NodeVector allDevices;

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
  auto_ptr< SettingsDialog > settings;

protected:
  DECLARE_EVENT_TABLE()
};




enum
{ FRAME_EXIT    =   wxID_HIGHEST + 1,
  FRAME_OPEN,
  FRAME_OPEN_URL,
  FRAME_CLOSE,
  FRAME_FULLSCREEN,
  FRAME_RESTORE,
  FRAME_MIRROR,
  FRAME_CONSOLE,
  FRAME_SELECTION,
  FRAME_VIEWPOINT,
  FRAME_NAVIGATION = 6500,
  FRAME_DEVICE = 6550,
  FRAME_CHOOSERENDERER = 6600,
  FRAME_RENDERMODE,
  FRAME_DEVICECONTROL,
  BASIC_PREFRENDERER,
  BASIC_WIREFRAME,
  BASIC_NOTEXTURES,
  BASIC_SMOOTHSHADING,
  BASIC_SELECT,
  BASIC_NAVWALK,
  BASIC_NAVEXAMINE,
  BASIC_NAVFLY,
  BASIC_DIR,
  FRAME_ABOUT,
  FRAME_HELP,
  FRAME_SETTINGS
};


