#ifndef __CONSOLEDIALOG__
#define __CONSOLEDIALOG__

// ---------------------------------------------------------------------------
//  Console Dialog Class
// ---------------------------------------------------------------------------
#include <wx/wx.h>

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

private:
	void OnConsoleClose (wxCommandEvent & event);
	DECLARE_EVENT_TABLE();
};

#endif