// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------

#include <wx/wx.h>
#include <iostream>
#include "consoleDialog.h"
#include <Console.h>

using namespace std;
//using namespace h3d;

// ---------------------------------------------------------------------------
//  Implementation
// ---------------------------------------------------------------------------

/*******************Constructor*********************/

consoleDialog::consoleDialog ( wxWindow *parent,
							   wxWindowID id,
							   const wxString &title,
							   const wxPoint& pos,
							   const wxSize& size,
							   long style
							   ):
wxDialog (parent, id, title, pos, size, style)
{
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	// create text ctrl with minimal size 400x200
	logText = (wxTextCtrl *) NULL;
	logText = new wxTextCtrl ( this, -1, "", wxDefaultPosition, wxSize(400, 200), wxTE_MULTILINE | wxTE_READONLY );

	topsizer->Add(logText, 
				  1,            // make vertically stretchable
				  wxEXPAND |    // make horizontally stretchable
				  wxALL,        //   and make border all around
				  10 );         // set border width to 10 */

    wxButton *closeButton = new wxButton( this, wxID_CLOSE, "Close" );

    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );
    button_sizer->Add(closeButton, 
	  				  0,           // make horizontally unstretchable
					  wxALL,       // make border all around (implicit top alignment)
					  10 );        // set border width to 10

	topsizer->Add(button_sizer,
				  0,                // make vertically unstretchable
				  wxALIGN_CENTER ); // no border and centre horizontally

	SetSizer( topsizer );      // use the sizer for layout

	topsizer->SetSizeHints( this );   // set size hints to honour minimum size

  std::streambuf *sbOld = std::cerr.rdbuf();
	std::cerr.rdbuf(logText);
	ostream *t = new ostream(logText);
  H3DUtil::Console.setOutputStream( *t );
	
  std::cerr.rdbuf(sbOld); 
  std::cerr << "TESTS" << endl;
	//*logText << 123.456 << " some text\n";
}

/*******************Event Table*********************/
BEGIN_EVENT_TABLE(consoleDialog, wxDialog)
	EVT_BUTTON (wxID_CLOSE, consoleDialog::OnConsoleClose)
END_EVENT_TABLE()

/*******************Member Functions*********************/
void consoleDialog::OnConsoleClose(wxCommandEvent &event)
{
	Close(TRUE);
}
