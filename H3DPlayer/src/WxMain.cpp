// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
//#pragma once
//#include "header.h"
#include "H3DWxFrame.h"
#include "H3DWxWidgetsWindow.h"
//#include <fstream>
//#include "X3D.h"
//#include <string.h>

//#include <iostream>

//#include "X3DSAX2Handlers.h"
//#include "GLUTWindow.h"
#include "VrmlParser.h"
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
#include "PythonScript.h"

#ifdef MACOSX
#include "FreeImage.h"
#include <xercesc/util/PlatformUtils.hpp>
#endif

// ---------------------------------------------------------------------------
//  Required classes and definitions
// ---------------------------------------------------------------------------
using namespace std;
using namespace H3D;


H3D_API_EXCEPTION( QuitAPIException );

class QuitAPIField: public AutoUpdate< SFString > {
  virtual void update() {
    string s = static_cast< SFString * >(routes_in[0])->getValue();
    if( s[0] == 27 ) { 
      throw QuitAPIException();
    }
  }
};


// Define a new application type
class MyApp: public wxApp
{
public:
    bool OnInit();

    void OnIdle(wxIdleEvent& event);
    bool OnExceptionInMainLoop();
protected:
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyApp, wxApp)
  EVT_IDLE(MyApp::OnIdle)
END_EVENT_TABLE()

void MyApp::OnIdle(wxIdleEvent& event) {
  for( set< Scene * >::iterator i = Scene::scenes.begin();
    i != Scene::scenes.end();
    i++ ) {
    if( (*i)->isActive() )
      (*i)->idle();
  }
  wxApp::OnIdle(event);
}

IMPLEMENT_APP(MyApp)

bool MyApp::OnExceptionInMainLoop() {
  try {
    throw;
  }
  catch (const QuitAPIException &) {
    return false;
  }
  catch (const Exception::H3DException &e) {
    MessageBox(HWND_DESKTOP, e.message.c_str(), 
                             "Error", MB_OK | MB_ICONEXCLAMATION);
    return false;
  }
  wxApp::OnExceptionInMainLoop();  
}

bool MyApp::OnInit()
{
  SetVendorName(_T("SenseGraphics AB"));
  SetAppName(_T("H3D Player"));
  // create a window to display
  H3DWxFrame *theWxFrame = new H3DWxFrame(NULL, wxID_ANY, "H3D Player", wxDefaultPosition, wxSize(800, 600));
	//glwindow constructed in the frame constructor.  Next line redundant.
	//theWxFrame->glwindow = new H3DWxWidgetsWindow(theWxFrame);
	theWxFrame->Show(true);
  //This next line is used to set the icon file h3d.ico, when created.
	//theWxframe->SetIcon(wxIcon(_T("h3d_icn")));

    
    // Using this line instead of the two previous lines will make
    // H3DWxWidgetsWindow create an instance of a wxframe with no menus and use
    // this as parent to the canvas.
    // H3DWxWidgetsWindow *glwindow = new H3DWxWidgetsWindow();

  return true;
}

