#ifndef __H3DViewerFieldValuesDialog__
#define __H3DViewerFieldValuesDialog__

#include <H3D/Node.h>

using namespace H3D;

/**
@file
Subclass of FieldValuesDialog, which is generated by wxFormBuilder.
*/

#define DEFAULT_VALUES
#include "H3DViewer.h"

/** Implementing FieldValuesDialog */
class H3DViewerFieldValuesDialog : public FieldValuesDialog
{
public:
	/** Constructor */
	H3DViewerFieldValuesDialog( wxWindow* parent );

  void OnIdle( wxIdleEvent& event );
  virtual void OnCellEdit( wxGridEvent& event );

  void displayFieldsFromNode( Node * );
protected:
  
  // Update the specified row in the field values grid with
  // values from the node and filed into given.
  void updateRowFromFieldDB( int row, 
                             Node *n,
                             FieldDBElement *db,
                             bool new_node );

  TimeStamp last_fields_update;
#ifdef DEFAULT_VALUES
  auto_ptr< Node > default_values_node;
#endif
  AutoRef< Node > displayed_node;

};

#endif // __H3DViewerFieldValuesDialog__