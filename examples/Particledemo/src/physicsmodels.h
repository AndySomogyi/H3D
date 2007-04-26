#include <wx/wx.h>
#include <wx/propdlg.h> 
#include <wx/bookctrl.h>
#include <wx/docview.h>
#include <wx/spinctrl.h>
#include <wx/config.h>
#include <wx/confbase.h>
#include <wx/tokenzr.h>

//#include <SFVec3f.h>

//#include "PointEmitter.h"
//#include "ConeEmitter.h"
//#include "ExplosionEmitter.h"
//#include "PolylineEmitter.h"
//#include "VolumeEmitter.h"
#include "ParticleSystem.h"
#include "WindPhysicsModel.h"
#include "BoundedPhysicsModel.h"
#include "GravityPhysicsModel.h"
//#include "SurfaceEmitter.h"

using namespace std;
using namespace H3D;

// ---------------------------------------------------------------------------
//  Bounded Physics Model Dialog Declaration
// ---------------------------------------------------------------------------

class BoundedPhysicsModelDialog: public wxPropertySheetDialog
{
DECLARE_CLASS(BoundedPhysicsModelDialog);
public:
  BoundedPhysicsModelDialog (wxWindow* parent, ParticleSystem* PS);
  ~BoundedPhysicsModelDialog ();

  void handleSettingsChange (wxCommandEvent & event);
  wxPanel* CreateBoundedPhysicsModelSettingsPage(wxWindow* parent);

protected:
  enum {
    ID_ENABLED,
    ID_GEOMETRY,
  };

  wxImageList* m_imageList;
  ParticleSystem* ePS;
  BoundedPhysicsModel* boundedPhysicsModel;

DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------------------------
//  Gravity Physics Model Dialog Declaration
// ---------------------------------------------------------------------------

class GravityPhysicsModelDialog: public wxPropertySheetDialog
{
DECLARE_CLASS(GravityPhysicsModelDialog);
public:
  GravityPhysicsModelDialog (wxWindow* parent, ParticleSystem* PS);
  ~GravityPhysicsModelDialog ();

  void handleSettingsChange (wxCommandEvent & event);
  wxPanel* CreateGravityPhysicsModelSettingsPage(wxWindow* parent);

protected:
  enum {
    ID_ENABLED,
    ID_GRAVITY,
  };

  wxImageList* m_imageList;
  ParticleSystem* ePS;
  GravityPhysicsModel* gravityPhysicsModel;
  double val;

DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------------------------
//  Wind Physics Model Dialog Declaration
// ---------------------------------------------------------------------------

class WindPhysicsModelDialog: public wxPropertySheetDialog
{
DECLARE_CLASS(WindPhysicsModelDialog);
public:
  WindPhysicsModelDialog (wxWindow* parent, ParticleSystem* PS);
  ~WindPhysicsModelDialog ();

  void handleSettingsChange (wxCommandEvent & event);
  wxPanel* CreateWindPhysicsModelSettingsPage(wxWindow* parent);

  //void setParticleSystem (ParticleSystem * PS);
  //void setEmitter (X3DParticleEmitterNode * emitter);

protected:
  enum {
    ID_SHOW_TOOLTIPS = 100,
    ID_ENABLED,
    ID_DIRECTION,
    ID_GUSTINESS,
    ID_SPEED,
    ID_TURBULENCE,
  };

  wxImageList*    m_imageList;
  ParticleSystem * ePS;
  WindPhysicsModel * windPhysicsModel;
  double val;

DECLARE_EVENT_TABLE()
};