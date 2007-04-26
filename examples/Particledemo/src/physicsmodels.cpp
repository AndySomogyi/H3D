#include "physicsmodels.h"

using namespace std;
using namespace H3D;
using namespace HAPI;

// ---------------------------------------------------------------------------
//  BoundedPhysicsModelDialog Definition
// ---------------------------------------------------------------------------

IMPLEMENT_CLASS(BoundedPhysicsModelDialog, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(BoundedPhysicsModelDialog, wxPropertySheetDialog)
  EVT_CHECKBOX(ID_ENABLED, BoundedPhysicsModelDialog::handleSettingsChange)
END_EVENT_TABLE()

BoundedPhysicsModelDialog::BoundedPhysicsModelDialog(wxWindow* win, ParticleSystem* PS)
{
  SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);

  int tabImage1 = -1;
  int tabImage2 = -1;
  int tabImage3 = -1;

  int resizeBorder = wxRESIZE_BORDER;

  m_imageList = NULL;

  Create(win, wxID_ANY, _("Bounded Physics Model"), wxDefaultPosition, wxDefaultSize,
      wxDEFAULT_DIALOG_STYLE| (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, resizeBorder)
  );

  // If using a toolbook, also follow Mac style and don't create buttons
  CreateButtons(wxOK | wxCANCEL |
                      (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxHELP)
  ); 

  wxBookCtrlBase* notebook = GetBookCtrl();
  notebook->SetImageList(m_imageList);

  wxPanel* BoundedPhysicsModelSettings = CreateBoundedPhysicsModelSettingsPage(notebook);

  notebook->AddPage(BoundedPhysicsModelSettings, _("Physics Properties"), true, tabImage1);

  LayoutDialog();

  //Set pointEmitter pointer to NULL initally
  boundedPhysicsModel = (BoundedPhysicsModel *) NULL;
  boundedPhysicsModel = new BoundedPhysicsModel;
  //Make ePS point to frame ParticleSystem
  //PS->emitter->setValue(pointEmitter);
  //setParticleSystem (PS);
  ePS = PS;
  //PS->emitter->setValue(pointEmitter);
}

BoundedPhysicsModelDialog::~BoundedPhysicsModelDialog()
{

}

wxPanel* BoundedPhysicsModelDialog::CreateBoundedPhysicsModelSettingsPage(wxWindow *parent)
{
  wxPanel* panel = new wxPanel(parent, wxID_ANY);
  
  wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
  wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

  //ENABLE
  wxBoxSizer* enableSizer = new wxBoxSizer( wxHORIZONTAL );
  wxCheckBox* boundedPhysicsModelEnable = new wxCheckBox(panel, ID_ENABLED, _("&Enable"), wxDefaultPosition, wxDefaultSize);
  enableSizer->Add(boundedPhysicsModelEnable, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  item0->Add(enableSizer, 0, wxGROW|wxALL, 0);

  topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );

  panel->SetSizer(topSizer);
  topSizer->Fit(panel);

  return panel;
}

void BoundedPhysicsModelDialog::handleSettingsChange (wxCommandEvent & event) {
  boundedPhysicsModel = dynamic_cast<BoundedPhysicsModel *> (ePS->physics->getValueByIndex(0));

  int id = event.GetId(); 

  if ( id == ID_ENABLED )
    boundedPhysicsModel->enabled->setValue( event.IsChecked() );
}

// ---------------------------------------------------------------------------
//  GravityPhysicsModelDialog Definition
// ---------------------------------------------------------------------------

IMPLEMENT_CLASS(GravityPhysicsModelDialog, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(GravityPhysicsModelDialog, wxPropertySheetDialog)
  EVT_CHECKBOX(ID_ENABLED, GravityPhysicsModelDialog::handleSettingsChange)
  EVT_TEXT(ID_GRAVITY, GravityPhysicsModelDialog::handleSettingsChange)
END_EVENT_TABLE()

GravityPhysicsModelDialog::GravityPhysicsModelDialog(wxWindow* win, ParticleSystem* PS)
{
  SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);

  int tabImage1 = -1;
  int tabImage2 = -1;
  int tabImage3 = -1;

  int resizeBorder = wxRESIZE_BORDER;

  m_imageList = NULL;

  Create(win, wxID_ANY, _("Gravity Physics Model"), wxDefaultPosition, wxDefaultSize,
      wxDEFAULT_DIALOG_STYLE| (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, resizeBorder)
  );

  // If using a toolbook, also follow Mac style and don't create buttons
  CreateButtons(wxOK | wxCANCEL |
                      (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxHELP)
  ); 

  wxBookCtrlBase* notebook = GetBookCtrl();
  notebook->SetImageList(m_imageList);

  wxPanel* GravityPhysicsModelSettings = CreateGravityPhysicsModelSettingsPage(notebook);

  notebook->AddPage(GravityPhysicsModelSettings, _("Physics Properties"), true, tabImage1);

  LayoutDialog();

  //Set pointEmitter pointer to NULL initally
  gravityPhysicsModel = (GravityPhysicsModel *) NULL;
  gravityPhysicsModel = new GravityPhysicsModel;
  //Make ePS point to frame ParticleSystem
  //PS->emitter->setValue(pointEmitter);
  //setParticleSystem (PS);
  ePS = PS;
  //PS->emitter->setValue(pointEmitter);
}

GravityPhysicsModelDialog::~GravityPhysicsModelDialog()
{

}

wxPanel* GravityPhysicsModelDialog::CreateGravityPhysicsModelSettingsPage(wxWindow *parent)
{
  wxPanel* panel = new wxPanel(parent, wxID_ANY);
  
  wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
  wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

  //ENABLE
  wxBoxSizer* enableSizer = new wxBoxSizer( wxHORIZONTAL );
  wxCheckBox* gravityPhysicsModelEnable = new wxCheckBox(panel, ID_ENABLED, _("&Enable"), wxDefaultPosition, wxDefaultSize);
  enableSizer->Add(gravityPhysicsModelEnable, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  item0->Add(enableSizer, 0, wxGROW|wxALL, 0);

  //// GRAVITY
  wxBoxSizer* gravitySizer = new wxBoxSizer( wxHORIZONTAL );
  gravitySizer->Add(new wxStaticText(panel, wxID_ANY, _("&Gravity :")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  wxTextCtrl* gravityPhysicsModelGravity = new wxTextCtrl(panel, ID_GRAVITY, wxEmptyString, wxDefaultPosition, wxSize(40, wxDefaultCoord));
  gravitySizer->Add(gravityPhysicsModelGravity, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  item0->Add(gravitySizer, 0, wxGROW|wxALL, 0);

  topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );

  panel->SetSizer(topSizer);
  topSizer->Fit(panel);

  return panel;
}

void GravityPhysicsModelDialog::handleSettingsChange (wxCommandEvent & event) {
  gravityPhysicsModel = dynamic_cast<GravityPhysicsModel *> (ePS->physics->getValueByIndex(0));

  if (gravityPhysicsModel) {

  int id = event.GetId(); 

  if ( id == ID_ENABLED )
    gravityPhysicsModel->enabled->setValue( event.IsChecked() );
  else if ( id == ID_GRAVITY ) {
    wxString temp_string = event.GetString();
    wxStringTokenizer vec3_tkz( temp_string, wxT(" "), wxTOKEN_DEFAULT );
    vector< double > gravity_values;
    while ( vec3_tkz.HasMoreTokens() ) {
        wxString value = vec3_tkz.GetNextToken();
        if ( value.ToDouble(&val) ) {
          gravity_values.push_back( val );
        }
    }
    vector< double >::iterator iter = gravity_values.begin();
    int size = gravity_values.size();
    // Default values
    double x = 0, y = -9.8, z = 0;
    switch (size) {
      case 0:
        break;
      case 1:
        x = *iter;
        y = 0;
        z = 0;
        break;
      case 2:
        x = *iter;
        iter++;
        y = *iter;
        z = 0;
        break;
      case 3:
        x = *iter;
        iter++;
        y = *iter;
        iter++;
        z = *iter;
        break;
    }
    gravityPhysicsModel->gravity->setValue(Vec3f(x, y, z));
  }
  }
}



// ---------------------------------------------------------------------------
//  WindPhysicsModelDialog Definition
// ---------------------------------------------------------------------------

IMPLEMENT_CLASS(WindPhysicsModelDialog, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(WindPhysicsModelDialog, wxPropertySheetDialog)
  EVT_CHECKBOX(ID_ENABLED, WindPhysicsModelDialog::handleSettingsChange)
  EVT_TEXT(ID_SPEED, WindPhysicsModelDialog::handleSettingsChange)
  EVT_TEXT(ID_GUSTINESS, WindPhysicsModelDialog::handleSettingsChange)
  EVT_TEXT(ID_TURBULENCE, WindPhysicsModelDialog::handleSettingsChange)
  EVT_TEXT(ID_DIRECTION, WindPhysicsModelDialog::handleSettingsChange)
END_EVENT_TABLE()

WindPhysicsModelDialog::WindPhysicsModelDialog(wxWindow* win, ParticleSystem* PS)
{
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);

    int tabImage1 = -1;
    int tabImage2 = -1;
    int tabImage3 = -1;

    int resizeBorder = wxRESIZE_BORDER;

    m_imageList = NULL;

    Create(win, wxID_ANY, _("Wind Physics Model"), wxDefaultPosition, wxDefaultSize,
        wxDEFAULT_DIALOG_STYLE| (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, resizeBorder)
    );

    // If using a toolbook, also follow Mac style and don't create buttons
    CreateButtons(wxOK | wxCANCEL |
                        (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxHELP)
    ); 

    wxBookCtrlBase* notebook = GetBookCtrl();
    notebook->SetImageList(m_imageList);

    wxPanel* WindPhysicsModelSettings = CreateWindPhysicsModelSettingsPage(notebook);

    notebook->AddPage(WindPhysicsModelSettings, _("Physics Properties"), true, tabImage1);

    LayoutDialog();

    //Set pointEmitter pointer to NULL initally
    windPhysicsModel = (WindPhysicsModel *) NULL;
    windPhysicsModel = new WindPhysicsModel;
    //Make ePS point to frame ParticleSystem
    //PS->emitter->setValue(pointEmitter);
    //setParticleSystem (PS);
    ePS = PS;
    //PS->emitter->setValue(pointEmitter);

}

WindPhysicsModelDialog::~WindPhysicsModelDialog()
{

}

wxPanel* WindPhysicsModelDialog::CreateWindPhysicsModelSettingsPage(wxWindow *parent)
{
  wxPanel* panel = new wxPanel(parent, wxID_ANY);
  
  wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
  wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

  //ENABLE
  wxBoxSizer* enableSizer = new wxBoxSizer( wxHORIZONTAL );
  wxCheckBox* windPhysicsModelEnable = new wxCheckBox(panel, ID_ENABLED, _("&Enable"), wxDefaultPosition, wxDefaultSize);
  enableSizer->Add(windPhysicsModelEnable, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  item0->Add(enableSizer, 0, wxGROW|wxALL, 0);

  //// SPEED
  wxBoxSizer* speedSizer = new wxBoxSizer( wxHORIZONTAL );
  speedSizer->Add(new wxStaticText(panel, wxID_ANY, _("&Speed            :")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  wxTextCtrl* windPhysicsModelSpeed = new wxTextCtrl (panel, ID_SPEED, wxEmptyString, wxDefaultPosition, wxSize(40, wxDefaultCoord));
  speedSizer->Add(windPhysicsModelSpeed, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  item0->Add(speedSizer, 0, wxGROW|wxALL, 0);

  ////GUSTINESS
  wxBoxSizer* gustinessSizer = new wxBoxSizer( wxHORIZONTAL );
  gustinessSizer->Add(new wxStaticText(panel, wxID_ANY, _("&Gustiness      :")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  wxTextCtrl* windPhysicsModelGustiness = new wxTextCtrl (panel, ID_GUSTINESS, wxEmptyString, wxDefaultPosition, wxSize(40, wxDefaultCoord));
  gustinessSizer->Add(windPhysicsModelGustiness, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  item0->Add(gustinessSizer, 0, wxGROW|wxALL, 0);

  //// TURBULENCE
  wxBoxSizer* turbulenceSizer = new wxBoxSizer( wxHORIZONTAL );
  turbulenceSizer->Add(new wxStaticText(panel, wxID_ANY, _("&Turbulence    :")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  wxTextCtrl* windPhysicsModelTurbulence = new wxTextCtrl(panel, ID_TURBULENCE, wxEmptyString, wxDefaultPosition, wxSize(40, wxDefaultCoord));
  turbulenceSizer->Add(windPhysicsModelTurbulence, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  item0->Add(turbulenceSizer, 0, wxGROW|wxALL, 0);

  //// DIRECTION
  wxBoxSizer* directionSizer = new wxBoxSizer( wxHORIZONTAL );
  directionSizer->Add(new wxStaticText(panel, wxID_ANY, _("&Direction        :")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  wxTextCtrl* windPhysicsModelDirection = new wxTextCtrl(panel, ID_DIRECTION, wxEmptyString, wxDefaultPosition, wxSize(40, wxDefaultCoord));
  directionSizer->Add(windPhysicsModelDirection, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  item0->Add(directionSizer, 0, wxGROW|wxALL, 0);

  topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );

  panel->SetSizer(topSizer);
  topSizer->Fit(panel);

  return panel;
}

void WindPhysicsModelDialog::handleSettingsChange (wxCommandEvent & event) {
  windPhysicsModel = dynamic_cast<WindPhysicsModel *> (ePS->physics->getValueByIndex(0));

  int id = event.GetId();

  if ( id == ID_ENABLED )
    windPhysicsModel->enabled->setValue( event.IsChecked() );
  else if ( id == ID_SPEED )
    windPhysicsModel->speed->setValue( atof( event.GetString() ) );
  else if (id == ID_GUSTINESS)
    windPhysicsModel->gustiness->setValue ( atof( event.GetString() ) );
  else if (id == ID_TURBULENCE )
    windPhysicsModel->turbulence->setValue( atof( event.GetString() ) );
  else if (id == ID_DIRECTION) {
    wxString temp_string = event.GetString();
    wxStringTokenizer vec3_tkz( temp_string, wxT(" "), wxTOKEN_DEFAULT );
    vector< double > direction_values;
    while ( vec3_tkz.HasMoreTokens() ) {
        wxString value = vec3_tkz.GetNextToken();
        if ( value.ToDouble(&val) ) {
          direction_values.push_back( val );
        }
    }
    vector< double >::iterator iter = direction_values.begin();
    int size = direction_values.size();
    // Default values
    double x = 0, y = 1, z = 0;
    switch (size) {
      case 0:
        break;
      case 1:
        x = *iter;
        y = 0;
        z = 0;
        break;
      case 2:
        x = *iter;
        iter++;
        y = *iter;
        z = 0;
        break;
      case 3:
        x = *iter;
        iter++;
        y = *iter;
        iter++;
        z = *iter;
        break;
    }
    windPhysicsModel->direction->setValue(Vec3f(x, y, z));
  }
  Console (3) << "WIND PHYSICS MODEL STATS" << endl;
  Console (3) << "Enabled: " << windPhysicsModel->enabled->getValue() << endl;
  Console (3) << "Speed: " << windPhysicsModel->speed->getValue() << endl;
  Console (3) << "Gustiness: " << windPhysicsModel->gustiness->getValue() << endl;
  Console (3) << "Turbulence: " << windPhysicsModel->turbulence->getValue() << endl;
  Console (3) << "Direction: " << windPhysicsModel->direction->getValue() << endl;
}