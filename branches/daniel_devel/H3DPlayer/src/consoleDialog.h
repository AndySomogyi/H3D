// ---------------------------------------------------------------------------
//  Console Dialog Class
// ---------------------------------------------------------------------------

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