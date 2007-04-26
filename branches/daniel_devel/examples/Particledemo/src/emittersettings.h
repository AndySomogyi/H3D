#include <wx/wx.h>
#include <wx/propdlg.h> 
#include <wx/bookctrl.h>
#include <wx/docview.h>
#include <wx/spinctrl.h>
#include <wx/config.h>
#include <wx/confbase.h>
#include <wx/tokenzr.h>

//#include <SFVec3f.h>

#include "PointEmitter.h"
#include "ConeEmitter.h"
#include "ExplosionEmitter.h"
//#include "PolylineEmitter.h"
//#include "VolumeEmitter.h"
#include "ParticleSystem.h"
#include "SurfaceEmitter.h"

using namespace std;
using namespace H3D;

// ---------------------------------------------------------------------------
//  Base Emitter Dialog Declaration
//  All dialogs are derived from this base class
// ---------------------------------------------------------------------------

class EmitterDialog: public wxPropertySheetDialog
{
DECLARE_CLASS(EmitterDialog);
public:
  EmitterDialog ();
  ~EmitterDialog ();

  void setParticleSystem (ParticleSystem * PS);
  void setEmitter (X3DParticleEmitterNode * emitter);

protected:
    enum {
        ID_SHOW_TOOLTIPS = 100,

        ID_SPEED,
        ID_VARIATION,
        ID_MASS,
        ID_SURFACE_AREA,
    };

    wxImageList*    m_imageList;
    ParticleSystem * ePS;

DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------------------------
//  PointEmitter Dialog Declaration
// ---------------------------------------------------------------------------

class PointEmitterDialog: public EmitterDialog
{
DECLARE_CLASS(PointEmitterDialog);
public:
  PointEmitterDialog(wxWindow* parent, ParticleSystem* PS);
  ~PointEmitterDialog();

  void handleSettingsChange (wxCommandEvent & event);
  wxPanel* CreatePointEmitterSettingsPage(wxWindow* parent);  

protected:
  enum {
    ID_DIRECTION,
    ID_POSITION,
  };
  wxImageList*    m_imageList;
  PointEmitter * pointEmitter;
  double val;

DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------------------------
//  PolylineEmitter Dialog Declaration
// ---------------------------------------------------------------------------

class PolylineEmitterDialog: public EmitterDialog
{
DECLARE_CLASS(PolylineEmitterDialog)
public:
    PolylineEmitterDialog(wxWindow* parent, ParticleSystem* PS);
    ~PolylineEmitterDialog();

  /*void handleSettingsChange (wxCommandEvent & event);
  void handleSpinEvent (wxSpinEvent & event);
  void OnOk (wxCommandEvent & event);
  void OnCancel (wxCommandEvent & event);*/

  wxPanel* CreatePolylineEmitterSettingsPage(wxWindow* parent);  
  
protected:

    enum {
        ID_COORD_INDEX,
    };

    wxImageList*    m_imageList;
//    PolylineEmitter * polylineEmitter;
//    ParticleSystem * ePS;

DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------------------------
//  VolumeEmitter Dialog Declaration
// ---------------------------------------------------------------------------

class VolumeEmitterDialog: public EmitterDialog
{
DECLARE_CLASS(VolumeEmitterDialog)
public:
    VolumeEmitterDialog(wxWindow* parent, ParticleSystem* PS);
    ~VolumeEmitterDialog();

  /*void handleSettingsChange (wxCommandEvent & event);
  void handleSpinEvent (wxSpinEvent & event);
  void OnOk (wxCommandEvent & event);
  void OnCancel (wxCommandEvent & event);*/

  wxPanel* CreateVolumeEmitterSettingsPage(wxWindow* parent);  
  
protected:

    enum {
        ID_COORD_INDEX,
    };

    wxImageList*    m_imageList;
//    VolumeEmitter * volumeEmitter;
//    ParticleSystem * ePS;

DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------------------------
//  ConeEmitter Dialog Declaration
// ---------------------------------------------------------------------------

class ConeEmitterDialog: public EmitterDialog
{
DECLARE_CLASS(ConeEmitterDialog);
public:
  ConeEmitterDialog(wxWindow* parent, ParticleSystem* PS);
  ~ConeEmitterDialog();

  void handleSettingsChange (wxCommandEvent & event);
  wxPanel* CreateConeEmitterSettingsPage(wxWindow* parent);  

protected:
  enum {
    ID_DIRECTION,
    ID_POSITION,
  };
  wxImageList*    m_imageList;
  ConeEmitter * coneEmitter;
  double val;

DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------------------------
//  ExplosionEmitter Dialog Declaration
// ---------------------------------------------------------------------------

class ExplosionEmitterDialog: public EmitterDialog
{
DECLARE_CLASS(ExplosionEmitterDialog);
public:
  ExplosionEmitterDialog(wxWindow* parent, ParticleSystem* PS);
  ~ExplosionEmitterDialog();

  void handleSettingsChange (wxCommandEvent & event);
  wxPanel* CreateExplosionEmitterSettingsPage(wxWindow* parent);  

protected:
  enum {
    ID_POSITION,
  };
  wxImageList*    m_imageList;
  ExplosionEmitter * explosionEmitter;
  double val;

DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------------------------
//  SurfaceEmitter Dialog Declaration
// ---------------------------------------------------------------------------

class SurfaceEmitterDialog: public EmitterDialog
{
DECLARE_CLASS(SurfaceEmitterDialog);
public:
  SurfaceEmitterDialog(wxWindow* parent, ParticleSystem* PS);
  ~SurfaceEmitterDialog();

  void handleSettingsChange (wxCommandEvent & event);
  wxPanel* CreateSurfaceEmitterSettingsPage(wxWindow* parent);  

protected:
  enum {
    ID_COORD_INDEX,
    ID_SURFACE,
  };
  wxImageList*    m_imageList;
  SurfaceEmitter * surfaceEmitter;
  double val;

DECLARE_EVENT_TABLE()
};

//class SurfaceEmitterGeometryDialog: public wxPropertySheetDialog
//{
//DECLARE_CLASS(SurfaceEmitterGeometryDialog);
//public:
//  EmitterDialog ();
//  ~EmitterDialog ();
//
//  void setParticleSystem (ParticleSystem * PS);
//  void setEmitter (X3DParticleEmitterNode * emitter);
//
//protected:
//    enum {
//        ID_SHOW_TOOLTIPS = 100,
//
//        ID_SPEED,
//        ID_VARIATION,
//        ID_MASS,
//        ID_SURFACE_AREA,
//    };
//
//    wxImageList*    m_imageList;
//    ParticleSystem * ePS;
//
//DECLARE_EVENT_TABLE()
//};
