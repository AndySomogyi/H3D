#include <wx/wx.h>
#include <wx/propdlg.h> 
#include <wx/bookctrl.h>
#include <wx/docview.h>
#include <wx/spinctrl.h>
#include <wx/config.h>
#include <wx/confbase.h>
#include "H3DWxWidgetsWindow.h"
#include "GlobalSettings.h"

#include "Box.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Sphere.h"

#include "Group.h"
#include "Transform.h"
#include "Scene.h"
#include "KeySensor.h"
#include "MouseSensor.h"

#include "SpaceWareSensor.h"

//#include "DEFNodes.h"
//#include "Viewpoint.h"
#include "DeviceInfo.h"
#include "INIFile.h"
#include "ResourceResolver.h"
//#include "PythonScript.h"
#include "Console.h"
#include "ParticleSystem.h"

#include "NavigationInfo.h"


#include "consoleDialog.h"
#include "emittersettings.h"
#include "physicsmodels.h"
//Particle System Specific


using namespace std;
using namespace H3D;

// ---------------------------------------------------------------------------
//  H3DWxFrame Definition
// ---------------------------------------------------------------------------

class H3DWxFrame: public wxFrame
{
public:
  H3DWxFrame( wxWindow *_parent, wxWindowID _id, const wxString& _title,
              const wxPoint& _pos, const wxSize& _size,
              long _style = wxDEFAULT_FRAME_STYLE,
              const wxString& name = "MedX3D Demo" );

  //Pointer to H3DWxWidgetsWindow
  H3DWxWidgetsWindow *glwindow;

  /***************Interface Features*****************/
  //Main Menu Bar
  wxMenuBar  *menuBar;
  //Menu Bar Items
  wxMenu     *fileMenu, *particleMenu, *physicsMenu, *emitterMenu, *advancedMenu, *helpMenu;
  //Sub menus
  wxMenu     *geometryMenu;
  
  bool loadFile( const string &file );
  void clearData();

  /***************Member Functions*****************/
  void OnOpenFile	(wxCommandEvent & event);
  void OnOpenFileURL	(wxCommandEvent & event);
  void OnCloseFile	(wxCommandEvent & event);
  void OnExit     	(wxCommandEvent & event);
  void OnWindowExit (wxCloseEvent & event);
  void OnAbout		(wxCommandEvent & event);
  void OnHelp		(wxCommandEvent & event);
  void ShowConsole	(wxCommandEvent & event);
  void GetSelection (wxMenuEvent & event);
  void OnParticleGeometry (wxCommandEvent & event);
  void ToggleParticles (wxCommandEvent & event);
  void ParticleGeneration (wxCommandEvent & event);
  void PointEmitterSettingsDialog (wxCommandEvent & event);
  void PolylineEmitterSettingsDialog (wxCommandEvent & event);
  void VolumeEmitterSettingsDialog (wxCommandEvent & event);
  void ConeEmitterSettingsDialog (wxCommandEvent & event);
  void ExplosionEmitterSettingsDialog (wxCommandEvent & event);
  void BoundedPhysicsModelSettingsDialog (wxCommandEvent & event);
  void WindPhysicsModelSettingsDialog (wxCommandEvent & event);
  void GravityPhysicsModelSettingsDialog (wxCommandEvent & event);

  /***************Standard trivial functions***************/
  wxString GetCurrentFilename();
  void SetCurrentFilename(wxString);
  void SetCurrentPath(wxString);
  wxString GetCurrentPath();
  void CreateGeometries();


  bool ParticleStatus;
  //void readSettingsFromINIFile( const string &filename,GlobalSettings *gs );

private:
	wxString currentFilename;
	wxString currentPath;
  //ConeEmitter * coneEmitter;
  //PointEmitter * pointEmitter;
  ParticleSystem *PS;

	int selection;

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

	consoleDialog *  theConsole;

  //Emitters
  ConeEmitterDialog * coneEmitterSettings;
  PointEmitterDialog * pointEmitterSettings;
  PolylineEmitterDialog * polylineEmitterSettings;
  VolumeEmitterDialog * volumeEmitterSettings;
  ExplosionEmitterDialog * explosionEmitterSettings;

  //Physics models
  BoundedPhysicsModelDialog * boundedPhysicsModelSettings;
  GravityPhysicsModelDialog * gravityPhysicsModelSettings;
  WindPhysicsModelDialog * windPhysicsModelSettings;

  //Geometries
  Box * box;
  Cone * cone;
  Cylinder * cylinder;
  Sphere * sphere;

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
  FRAME_OPENHAPTICS,
  FRAME_CHAI3D,
  FRAME_GODOBJECT,
  FRAME_RUSPINI,
  FRAME_RENDERMODE,
  FRAME_MONO,
  FRAME_QUADBUFFERED,
  FRAME_HORZSPLIT,
  FRAME_VERTSPLIT,
  FRAME_HORZINTERLACED,
  FRAME_VERTINTERLACED,
  FRAME_SHARPDISPLAY,
  FRAME_REDBLUE,
  FRAME_REDCYAN,
  FRAME_PARTICLECONTROL,
  FRAME_PARTICLECREATION,
  FRAME_DEVICECONTROL,
  FRAME_HAPTICSCONTROL,
  FRAME_GEOMETRY,
  FRAME_QUAD,
  FRAME_TRIANGLE,
  FRAME_POINT,
  FRAME_SPRITE,
  FRAME_LINE,
  FRAME_CONEEMITTER,
  FRAME_POINTEMITTER,
  FRAME_POLYLINEEMITTER,
  FRAME_VOLUMEEMITTER,
  FRAME_EXPLOSIONEMITTER,
  FRAME_BOUNDEDPHYSICSMODEL,
  FRAME_GRAVITYPHYSICSMODEL,
  FRAME_WINDPHYSICSMODEL,
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


