#include "H3DViewerAddChildDialog.h"

H3DViewerAddChildDialog::H3DViewerAddChildDialog(wxWindow* parent)
  :
  AddChildDialog(parent)
{
  block_OnText = false; // Putting this in the initializer list makes wxFormBuilder generate a conflicting constructor
  cbNewNodeName->Clear(); // build the initial combobox list
  cbNewNodeName->Freeze(); // We freeze the list so it doesn't do a redraw for every Append.
  for (H3DNodeDatabase::NodeDatabaseConstIterator nodedb_it = H3DNodeDatabase::begin();
    H3DNodeDatabase::end() != nodedb_it; ++nodedb_it) {
    available_nodes.Add(wxString(nodedb_it->second->getName()));
    cbNewNodeName->Append(nodedb_it->second->getName());
  }
  cbNewNodeName->Thaw();
}

void H3DViewerAddChildDialog::cbNewNodeNameOnKeyDown(wxKeyEvent& event)
{
  if (event.GetKeyCode() == WXK_BACK) {
    // We need to make sure backspace actually deletes the selection because otherwise the last character
    // in the autocompleted part is erased and autocomplete just refills it all, so you can't erase anything.
    block_OnText = true;
    cbNewNodeName->RemoveSelection();
    block_OnText = false;
  }
  event.Skip();
}

void H3DViewerAddChildDialog::cbNewNodeNameOnText(wxCommandEvent& event)
{
  // There's no built-in mechanism for blocking recursive calls to OnText when setting the value
  // of the combobox from inside this event-handler so we suppress it ourselves with block_OnText.
  if (!block_OnText) {
    block_OnText = true;
    wxString new_value = event.GetString();
    bool exact_match = false;
    if (new_value.length() > 0) {
      wxArrayString matching_nodes;
      for (wxArrayString::const_iterator it = available_nodes.begin();
        it != available_nodes.end(); ++it) {
        if ((it->Find(new_value) == 0)) { // Has to start with what we've written to match
          matching_nodes.Add(*it);
          if (*it == new_value) {
            exact_match = true;
          }
        }
      }
      // If we stop finding matches then we don't clear the list, that way the
      // user gets to see previously matched valid ones with similar names.
      if (matching_nodes.Count() > 0) {                                         
        if (!exact_match) { // And we also only clear the list if there's no exact match for the same reason as above
          cbNewNodeName->Clear();
          for (wxArrayString::const_iterator match_it = matching_nodes.begin(); match_it != matching_nodes.end(); ++match_it) {
            cbNewNodeName->Append(*match_it);
          }
          cbNewNodeName->SetValue(*matching_nodes.begin());
          cbNewNodeName->SetInsertionPoint(new_value.length());
          cbNewNodeName->SetSelection(new_value.length(), matching_nodes.begin()->length());
          new_value = cbNewNodeName->GetValue();
        }
      }

    } else { // If the user didn't enter anything then we can just show the full list
      cbNewNodeName->Clear(); 
      cbNewNodeName->Freeze(); // We freeze the list so it doesn't do a redraw for every Append.
      for (wxArrayString::const_iterator it = available_nodes.begin(); it != available_nodes.end(); ++it) {
        cbNewNodeName->Append(*it);
      }
      cbNewNodeName->Thaw();
    }
    block_OnText = false;
  }
}

void H3DViewerAddChildDialog::cbNewNodeNameOnTextEnter(wxCommandEvent& event)
{
  EndModal(wxID_OK);
}

void H3DViewerAddChildDialog::btSizerAddNodeOnCancelButtonClick(wxCommandEvent& event)
{
  EndModal(wxID_CANCEL);
}

void H3DViewerAddChildDialog::btSizerAddNodeOnOKButtonClick(wxCommandEvent& event)
{
  EndModal(wxID_OK);
}


wxString H3DViewerAddChildDialog::GetNodeName()
{
  return cbNewNodeName->GetValue();
}