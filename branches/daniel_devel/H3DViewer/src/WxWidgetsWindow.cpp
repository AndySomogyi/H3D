//////////////////////////////////////////////////////////////////////////////
//    Copyright 2006-2007, SenseGraphics AB
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
/// \file H3DWxWidgetsWindow.cpp
/// \brief CPP file for H3DWxWidgetsWindow.
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include <H3DWxWidgetsWindow.h>
#ifndef WIN32
#include <X3DKeyDeviceSensorNode.h>
#include <MouseSensor.h>
#endif


using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase H3DWxWidgetsWindow::database( "H3DWxWidgetsWindow", 
                                      &(newInstance<H3DWxWidgetsWindow>), 
                                      typeid( H3DWxWidgetsWindow ),
                                      &(H3DWindowNode::database) );

H3DWxWidgetsWindow::H3DWxWidgetsWindow( wxWindow *_theParent,
                                       Inst< SFInt32     > _width,
                        Inst< SFInt32     > _height,
                        Inst< SFBool      > _fullscreen,
                        Inst< SFBool      > _mirrored,
                        Inst< RenderMode  > _renderMode, 
                        Inst< SFViewpoint > _viewpoint,
                        Inst< SFTime      > _time     ) :
  H3DWindowNode( _width, _height, _fullscreen, _mirrored, _renderMode,
                 _viewpoint, _time ),
  theWindow( _theParent ){
  type_name = "H3DWxWidgetsWindow";
  database.initFields( this );
  
  int attribList[8];
  attribList[0] = WX_GL_RGBA;
  attribList[1] = WX_GL_DOUBLEBUFFER;
  attribList[2] = WX_GL_DEPTH_SIZE;
  attribList[3] = 24;
  attribList[4] = WX_GL_STENCIL_SIZE;
  attribList[5] = 8;
#ifdef MACOSX
  // TODO: stereo mode does not work with mac
  attribList[6] = 0;
#else
  attribList[6] = WX_GL_STEREO;
  attribList[7] = 0;
#endif
 
  if( !theWindow ) {
     theWindow = new wxFrame( NULL, wxID_ANY, "H3DwxFrame", wxDefaultPosition,
                              wxSize( width->getValue(), height->getValue() ));
  }

  theWxGLCanvas = new H3DwxGLCanvas( this, theWindow, -1, wxDefaultPosition,
              wxSize( width->getValue(), height->getValue() ), attribList );
#ifdef WIN32
  hWnd = (HWND)(theWxGLCanvas->GetHandle());
  if( hWnd )
    wpOrigProc = (WNDPROC) SetWindowLong(hWnd, 
                          GWL_WNDPROC, (LONG) WindowProc);
#endif
}

void H3DWxWidgetsWindow::initWindow() {
  last_fullscreen = !fullscreen->getValue();
  theWindow->Show();
  theWxGLCanvas->Show();
  theWxGLCanvas->SetCurrent();
  glClearColor( 0, 0, 0, 1 );
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glClear( GL_COLOR_BUFFER_BIT );
}

void H3DWxWidgetsWindow::setFullscreen( bool fullscreen ) {
  if( last_fullscreen != fullscreen ) {
    wxTopLevelWindow * tlw = dynamic_cast< wxTopLevelWindow * >(theWindow);
#ifndef MACOSX
    //TODO: fullscreen does not work well on macosx
    if( tlw )
      tlw->ShowFullScreen(fullscreen);
#endif
    last_fullscreen = fullscreen;
  }
}

void H3DWxWidgetsWindow::swapBuffers() {
   theWxGLCanvas->SwapBuffers();
}

void H3DWxWidgetsWindow::makeWindowActive() {
  theWxGLCanvas->SetCurrent();
}

BEGIN_EVENT_TABLE(H3DWxWidgetsWindow::H3DwxGLCanvas, wxGLCanvas)
  EVT_IDLE(H3DWxWidgetsWindow::H3DwxGLCanvas::OnIdle)
  EVT_SIZE(H3DWxWidgetsWindow::H3DwxGLCanvas::OnSize)
  EVT_PAINT(H3DWxWidgetsWindow::H3DwxGLCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(H3DWxWidgetsWindow::H3DwxGLCanvas::OnEraseBackground)
#ifndef WIN32
  EVT_KEY_DOWN( H3DWxWidgetsWindow::H3DwxGLCanvas::OnKeyDown )
  EVT_KEY_UP( H3DWxWidgetsWindow::H3DwxGLCanvas::OnKeyUp )
  EVT_CHAR( H3DWxWidgetsWindow::H3DwxGLCanvas::OnCharDown )
  EVT_LEFT_DOWN( H3DWxWidgetsWindow::H3DwxGLCanvas::onLeftMouseButtonDown )
  EVT_LEFT_UP( H3DWxWidgetsWindow::H3DwxGLCanvas::onLeftMouseButtonUp )
  EVT_LEFT_DCLICK( H3DWxWidgetsWindow::H3DwxGLCanvas::onLeftMouseButtonDown )
  EVT_MIDDLE_DOWN( H3DWxWidgetsWindow::H3DwxGLCanvas::onMiddleMouseButtonDown )
  EVT_MIDDLE_UP( H3DWxWidgetsWindow::H3DwxGLCanvas::onMiddleMouseButtonUp )
EVT_MIDDLE_DCLICK( H3DWxWidgetsWindow::H3DwxGLCanvas::onMiddleMouseButtonDown )
  EVT_RIGHT_DOWN( H3DWxWidgetsWindow::H3DwxGLCanvas::onRightMouseButtonDown )
  EVT_RIGHT_UP( H3DWxWidgetsWindow::H3DwxGLCanvas::onRightMouseButtonUp )
  EVT_RIGHT_DCLICK( H3DWxWidgetsWindow::H3DwxGLCanvas::onRightMouseButtonDown )
  EVT_MOTION( H3DWxWidgetsWindow::H3DwxGLCanvas::onMouseMotion )
  EVT_MOUSEWHEEL( H3DWxWidgetsWindow::H3DwxGLCanvas::onMouseWheelRotation )
#endif
END_EVENT_TABLE()

H3DWxWidgetsWindow::H3DwxGLCanvas::H3DwxGLCanvas( H3DWxWidgetsWindow *_myOwner,
                                                  wxWindow* _parent,
                                                  wxWindowID _id,
                                                  const wxPoint& _pos,
                                                  const wxSize& _size,
                                                  int* _attribList,
                                                  long _style, 
                                                  const wxString& _name,
                                                  const wxPalette& _palette ):
wxGLCanvas(_parent, _id, _pos, _size, _style | wxFULL_REPAINT_ON_RESIZE, _name,
           _attribList, _palette),
myOwner( _myOwner )
{
}

void H3DWxWidgetsWindow::H3DwxGLCanvas::OnIdle(wxIdleEvent& event) {
  event.RequestMore();
}

void H3DWxWidgetsWindow::H3DwxGLCanvas::OnSize( wxSizeEvent& event ) {
  wxGLCanvas::OnSize(event);
  // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
  int w, h;
  GetClientSize(&w, &h);
  if ( GetContext() )
    myOwner->reshape( w, h );
}

void H3DWxWidgetsWindow::H3DwxGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);  
  myOwner->display();
}

void H3DWxWidgetsWindow::H3DwxGLCanvas::OnEraseBackground(
  wxEraseEvent& WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
}

#ifndef WIN32
void H3DWxWidgetsWindow::H3DwxGLCanvas::OnKeyDown(wxKeyEvent& event)
{
  switch( event.GetKeyCode() ) {
    case WXK_F1: myOwner->onKeyDown( X3DKeyDeviceSensorNode::F1, true ); break;
    case WXK_F2: myOwner->onKeyDown( X3DKeyDeviceSensorNode::F2, true ); break;
    case WXK_F3: myOwner->onKeyDown( X3DKeyDeviceSensorNode::F3, true ); break;
    case WXK_F4: myOwner->onKeyDown( X3DKeyDeviceSensorNode::F4, true ); break;
    case WXK_F5: myOwner->onKeyDown( X3DKeyDeviceSensorNode::F5, true ); break;
    case WXK_F6: myOwner->onKeyDown( X3DKeyDeviceSensorNode::F6, true ); break;
    case WXK_F7: myOwner->onKeyDown( X3DKeyDeviceSensorNode::F7, true ); break;
    case WXK_F8: myOwner->onKeyDown( X3DKeyDeviceSensorNode::F8, true ); break;
    case WXK_F9: myOwner->onKeyDown( X3DKeyDeviceSensorNode::F9, true ); break;
    case WXK_F10: myOwner->onKeyDown( X3DKeyDeviceSensorNode::F10, true );
      break;
    case WXK_F11: myOwner->onKeyDown( X3DKeyDeviceSensorNode::F11, true );
      break;
    case WXK_F12: myOwner->onKeyDown( X3DKeyDeviceSensorNode::F12, true );
      break;
    case WXK_HOME: myOwner->onKeyDown( X3DKeyDeviceSensorNode::HOME, true );
      break;
    case WXK_END: myOwner->onKeyDown( X3DKeyDeviceSensorNode::END, true );
      break;
    case WXK_PRIOR: myOwner->onKeyDown( X3DKeyDeviceSensorNode::PGUP, true );
      break;
    case WXK_NEXT: myOwner->onKeyDown( X3DKeyDeviceSensorNode::PGDN, true );
      break;
    case WXK_UP: myOwner->onKeyDown( X3DKeyDeviceSensorNode::UP, true ); break;
    case WXK_DOWN: myOwner->onKeyDown( X3DKeyDeviceSensorNode::DOWN, true );
      break;
    case WXK_LEFT: myOwner->onKeyDown( X3DKeyDeviceSensorNode::LEFT, true );
      break;
    case WXK_RIGHT: myOwner->onKeyDown( X3DKeyDeviceSensorNode::RIGHT, true );
      break;
    case WXK_MENU: myOwner->onKeyDown( X3DKeyDeviceSensorNode::ALT, true );
      break;
    case WXK_CONTROL: myOwner->onKeyDown( X3DKeyDeviceSensorNode::CONTROL,
                        true ); break;
    case WXK_SHIFT: myOwner->onKeyDown( X3DKeyDeviceSensorNode::SHIFT, true );
      break;
    default: { event.Skip(); }
  }
}

void H3DWxWidgetsWindow::H3DwxGLCanvas::OnKeyUp(wxKeyEvent& event)
{
  switch( event.GetKeyCode() ) {
    case WXK_F1: myOwner->onKeyUp( X3DKeyDeviceSensorNode::F1, true ); break;
    case WXK_F2: myOwner->onKeyUp( X3DKeyDeviceSensorNode::F2, true ); break;
    case WXK_F3: myOwner->onKeyUp( X3DKeyDeviceSensorNode::F3, true ); break;
    case WXK_F4: myOwner->onKeyUp( X3DKeyDeviceSensorNode::F4, true ); break;
    case WXK_F5: myOwner->onKeyUp( X3DKeyDeviceSensorNode::F5, true ); break;
    case WXK_F6: myOwner->onKeyUp( X3DKeyDeviceSensorNode::F6, true ); break;
    case WXK_F7: myOwner->onKeyUp( X3DKeyDeviceSensorNode::F7, true ); break;
    case WXK_F8: myOwner->onKeyUp( X3DKeyDeviceSensorNode::F8, true ); break;
    case WXK_F9: myOwner->onKeyUp( X3DKeyDeviceSensorNode::F9, true ); break;
    case WXK_F10: myOwner->onKeyUp( X3DKeyDeviceSensorNode::F10, true ); break;
    case WXK_F11: myOwner->onKeyUp( X3DKeyDeviceSensorNode::F11, true ); break;
    case WXK_F12: myOwner->onKeyUp( X3DKeyDeviceSensorNode::F12, true ); break;
    case WXK_HOME: myOwner->onKeyUp( X3DKeyDeviceSensorNode::HOME, true );
      break;
    case WXK_END: myOwner->onKeyUp( X3DKeyDeviceSensorNode::END, true );
      break;
    case WXK_PRIOR: myOwner->onKeyUp( X3DKeyDeviceSensorNode::PGUP, true );
      break;
    case WXK_NEXT: myOwner->onKeyUp( X3DKeyDeviceSensorNode::PGDN, true );
      break;
    case WXK_UP: myOwner->onKeyUp( X3DKeyDeviceSensorNode::UP, true ); break;
    case WXK_DOWN: myOwner->onKeyUp( X3DKeyDeviceSensorNode::DOWN, true );
      break;
    case WXK_LEFT: myOwner->onKeyUp( X3DKeyDeviceSensorNode::LEFT, true );
      break;
    case WXK_RIGHT: myOwner->onKeyUp( X3DKeyDeviceSensorNode::RIGHT, true );
      break;
    case WXK_MENU: myOwner->onKeyUp( X3DKeyDeviceSensorNode::ALT, true );
      break;
    case WXK_CONTROL: myOwner->onKeyUp( X3DKeyDeviceSensorNode::CONTROL,
                        true ); break;
    case WXK_SHIFT: myOwner->onKeyUp( X3DKeyDeviceSensorNode::SHIFT, true );
      break;
    default: { myOwner->onKeyUp(event.GetKeyCode(), false ); }
  }
}

void H3DWxWidgetsWindow::H3DwxGLCanvas::OnCharDown(wxKeyEvent& event)
{
  myOwner->onKeyDown( event.GetKeyCode(), false );
  event.Skip();
}

void H3DWxWidgetsWindow::H3DwxGLCanvas::onLeftMouseButtonDown(
  wxMouseEvent & event ) {
  myOwner->onMouseButtonAction( MouseSensor::LEFT_BUTTON, MouseSensor::DOWN );
  event.Skip();
}

void H3DWxWidgetsWindow::H3DwxGLCanvas::onLeftMouseButtonUp(
  wxMouseEvent & event ) {
  myOwner->onMouseButtonAction( MouseSensor::LEFT_BUTTON, MouseSensor::UP );
}

void H3DWxWidgetsWindow::H3DwxGLCanvas::onMiddleMouseButtonDown(
  wxMouseEvent & event ) {
  myOwner->onMouseButtonAction( MouseSensor::MIDDLE_BUTTON, MouseSensor::DOWN );
}

void H3DWxWidgetsWindow::H3DwxGLCanvas::onMiddleMouseButtonUp(
  wxMouseEvent & event ) {
  myOwner->onMouseButtonAction( MouseSensor::MIDDLE_BUTTON, MouseSensor::UP );
}

void H3DWxWidgetsWindow::H3DwxGLCanvas::onRightMouseButtonDown(
  wxMouseEvent & event ) {
  myOwner->onMouseButtonAction( MouseSensor::RIGHT_BUTTON, MouseSensor::DOWN );
}

void H3DWxWidgetsWindow::H3DwxGLCanvas::onRightMouseButtonUp(
  wxMouseEvent & event ) {
  myOwner->onMouseButtonAction( MouseSensor::RIGHT_BUTTON, MouseSensor::UP );
}
void H3DWxWidgetsWindow::H3DwxGLCanvas::onMouseMotion( wxMouseEvent & event ) {
  myOwner->onMouseMotionAction( event.GetX(), event.GetY() );
}
void H3DWxWidgetsWindow::H3DwxGLCanvas::onMouseWheelRotation(
  wxMouseEvent & event ) {
  myOwner->onMouseWheelAction(
    event.GetWheelRotation() > 0 ? MouseSensor::FROM : MouseSensor::TOWARDS );
}
#endif
