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
/// \file ConsoleDialog.h
/// \brief Header file for ConsoleDialog.
///
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __CONSOLEDIALOG__
#define __CONSOLEDIALOG__

// ---------------------------------------------------------------------------
//  Console Dialog Class
// ---------------------------------------------------------------------------
#include <wx/wx.h>
#include <sstream>
#include <memory>
class consoleDialog: public wxDialog
{
public:
	consoleDialog ( wxWindow *parent,
					wxWindowID id,
					const wxString &title,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = wxDEFAULT_DIALOG_STYLE
					);
	wxTextCtrl *logText;
	wxString GetText();

  friend void wxLockGUI( void * );
  friend void wxUnlockGUI( void * );

protected:
  std::stringstream other_thread_output;
  std::auto_ptr< std::ostream >console_stream;
	void OnConsoleClose (wxCommandEvent & event);
  void OnIdle( wxIdleEvent &event );
	DECLARE_EVENT_TABLE();
 
};

#endif