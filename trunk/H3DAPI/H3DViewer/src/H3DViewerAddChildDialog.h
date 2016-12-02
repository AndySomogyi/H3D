#ifndef __H3DViewerAddChildDialog__
#define __H3DViewerAddChildDialog__

/**
@file
Subclass of AddChildDialog, which is generated by wxFormBuilder.
*/

#include "H3DViewer.h"

//// end generated include

#include <H3D/Node.h>
#include <H3D/H3DNodeDatabase.h>


using namespace H3D;

/** Implementing AddChildDialog */
class H3DViewerAddChildDialog : public AddChildDialog
{
	protected:
		// Handlers for AddChildDialog events.
		void cbNewNodeNameOnKeyDown( wxKeyEvent& event );
		void cbNewNodeNameOnText( wxCommandEvent& event );
		void cbNewNodeNameOnTextEnter( wxCommandEvent& event );
		void btSizerAddNodeOnCancelButtonClick( wxCommandEvent& event );
		void btSizerAddNodeOnOKButtonClick( wxCommandEvent& event );
	public:
		/** Constructor */
		H3DViewerAddChildDialog( wxWindow* parent );
	//// end generated class members

  private:
    bool block_OnText;
    wxArrayString available_nodes;
  public:
   wxString GetNodeName();

};

#endif // __H3DViewerAddChildDialog__
