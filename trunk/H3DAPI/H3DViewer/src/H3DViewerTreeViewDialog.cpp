#include "H3DViewerTreeViewDialog.h"
#include "H3DViewerFieldValuesDialog.h"
#include <fstream>
#include <wx/wx.h>
#include <H3D/Scene.h>
#include <H3D/Viewpoint.h>
#include <H3D/IndexedTriangleSet.h>
#include <H3D/Coordinate.h>
#include <H3D/MatrixTransform.h>
#include <H3D/X3DShapeNode.h>
#include <H3D/X3DGeometryNode.h>
#include <H3D/X3D.h>

H3DViewerTreeViewDialog::H3DViewerTreeViewDialog( wxWindow* parent )
:
  TreeViewDialog( parent ),
  shown_last_loop( false ),
  displayed_node( NULL )
{
  TreeViewTree->AddRoot( wxT("World") );
  // add the bindable nodes in the tree view
  bindable_tree_id = TreeViewTree->AppendItem( TreeViewTree->GetRootItem(), 
                                               wxT("Active bindable nodes") );
  TreeViewTree->Expand( TreeViewTree->GetRootItem() );
  SetMenuBar( NULL );
}

H3DViewerTreeViewDialog::~H3DViewerTreeViewDialog() {
  // If the menu bar is set to NULL in the constructor it will not be
  // cleaned up. Done here explicitly.
  if( GetMenuBar() == NULL )
    delete m_menubar1;
}

void H3DViewerTreeViewDialog::OnNodeSelected( wxTreeEvent& event ) {

 TreeIdMap::iterator ni = node_map.find( event.GetItem().m_pItem );
  if( ni == node_map.end() ) {
    selected_node.reset( NULL );
  } else {
    selected_node.reset( (*ni).second.get() );
  }
}

void H3DViewerTreeViewDialog::showEntireSceneAsTree( bool expand_new ) {
  // show the scene in the tree view.
  list< pair< H3D::Node *, string > > l;
  Scene *s =  *Scene::scenes.begin();
  l.push_back(make_pair(s, s->defaultXMLContainerField() ) );

  updateNodeTree( TreeViewTree->GetRootItem(), l, expand_new );

  l.clear();  
  const X3DBindableNode::StackMapType &stacks = X3DBindableNode::getStackMap();
  for( X3DBindableNode::StackMapType::const_iterator i = stacks.begin(); 
       i != stacks.end();i++ ) {
    X3DBindableNode *b = X3DBindableNode::getActive( (*i).first );
    if( b ) l.push_back( make_pair(b, b->defaultXMLContainerField() ) );
  }
  updateNodeTree( bindable_tree_id, l, false );
}

void H3DViewerTreeViewDialog::addNodeToTree( wxTreeItemId tree_id, 
                                             H3D::Node *n,
                                             string container_field,
                                             bool expand ) {
  if( !n ) return;

  // the name in the tree is NodeType(DEFed name)
  string tree_string = n->getTypeName();
  if( n->hasName() ) {
    tree_string = tree_string + " (" + n->getName() + ")";
  }

  if( container_field != n->defaultXMLContainerField() ) {
    tree_string = tree_string + " (cf: " + container_field + ")";
  }
  // add an item for this node in the tree
  wxTreeItemId new_id = TreeViewTree->AppendItem( tree_id, wxString( tree_string.c_str(), wxConvUTF8 ) );
  unsigned int s1 = node_map.size();

  // add an entry for the tree_id-node pair 
  node_map[ new_id.m_pItem ].reset( n );


  // recursively add all the child nodes of the node to the tree
  H3DNodeDatabase *db = H3DNodeDatabase::lookupTypeId( typeid( *n ) );
  for( H3DNodeDatabase::FieldDBConstIterator i = db->fieldDBBegin();
       db->fieldDBEnd() != i; i++ ) {
    Field *f = i.getField( n ); //n->getField( *i );
    
    if( SFNode *sfnode = dynamic_cast< SFNode * >( f ) ) {
      if( sfnode->getAccessType() != Field::INPUT_ONLY ) {
        addNodeToTree( new_id, sfnode->getValue(), sfnode->getName(), expand );
      }
    } else if( MFNode *mfnode = dynamic_cast< MFNode * >( f ) ) {
      if( mfnode->getAccessType() != Field::INPUT_ONLY ) {
        for( MFNode::const_iterator i = mfnode->begin(); i != mfnode->end(); i++ ) {
          addNodeToTree( new_id, *i, mfnode->getName(), expand );
        }
      }
    }
  }

  // make the tree be open down to leaves or not.
  if( expand ) 
    TreeViewTree->Expand( new_id );
}


void H3DViewerTreeViewDialog::updateNodeTree( wxTreeItemId tree_id, 
                                              list< pair< H3D::Node *, string > > nodes,
                                              bool expand_new ) {

  // find all children of tree_id
  list< wxTreeItemId > children_ids;
  wxTreeItemIdValue cookie;
  wxTreeItemId id = TreeViewTree->GetFirstChild( tree_id, cookie );
  while( id.IsOk() ) {
    children_ids.push_back( id );
    id = TreeViewTree->GetNextSibling( id );
  }
  
  // update each of the tree ids that already exists. Either by updating
  // them if they still refer to a node or deleting them otherwise.
  bool have_node_in_tree = false;
  for( list< wxTreeItemId >::iterator i = children_ids.begin();
       i != children_ids.end(); i++ ) {
    // find the node corresponding to the id in the current tree view.

    if( node_map.find( (*i).m_pItem ) == node_map.end() ) {
      continue;
    }

    Node *id_node = node_map[ (*i).m_pItem ].get(); 

    // check if this node still exists in the new node structure
    list< pair< H3D::Node *, string > >::iterator ni;
    for( ni = nodes.begin(); ni != nodes.end(); ni++ ) {
      if( (*ni).first == id_node ) break;
    }

    if( ni != nodes.end() ) {
      // the node the tree id refers to still exists on this level
      // so recurse down.

      // find all child nodes of the node
      list< pair< H3D::Node *, string > > child_nodes;
      H3DNodeDatabase *db = H3DNodeDatabase::lookupTypeId( typeid( *id_node ) );
      for( H3DNodeDatabase::FieldDBConstIterator j = db->fieldDBBegin();
           db->fieldDBEnd() != j; j++ ) {
         Field *f = j.getField( id_node ); //Field *f = id_node->getField( *j );

        if( SFNode *sfnode = dynamic_cast< SFNode * >( f ) ) {
          if( sfnode->getAccessType() != Field::INPUT_ONLY ) {
            Node *n = sfnode->getValue();
            if( n ) child_nodes.push_back( make_pair( sfnode->getValue(), sfnode->getName() ) );
          }
        } else if( MFNode *mfnode = dynamic_cast< MFNode * >( f ) ) {
          if( mfnode->getAccessType() != Field::INPUT_ONLY ) {
            for( MFNode::const_iterator mf = mfnode->begin(); 
                 mf != mfnode->end(); mf++ ) {
              if( *mf ) child_nodes.push_back( make_pair( *mf, mfnode->getName() ) );
            }
          }
        }        
      }
      // update
      updateNodeTree( *i, child_nodes, expand_new );
      nodes.erase( ni );
    } else {
      // the node does not exist, so remove the tree id and all its children.
      deleteTree( *i );
    }
  }

  // add all new nodes to the tree. 
  for( list< pair< H3D::Node *, string > >::iterator i = nodes.begin();
       i != nodes.end(); i++ ) {
    addNodeToTree( tree_id, (*i).first, (*i).second, expand_new );
  }

  // make the tree be open down to leaves by default.
  //TreeViewTree->Expand( tree_id );
}


void H3DViewerTreeViewDialog::deleteTree( const wxTreeItemId &id ) { 
  list< wxTreeItemId > children_ids;
  wxTreeItemIdValue cookie;
  wxTreeItemId child_id = TreeViewTree->GetFirstChild( id, cookie );
  while( child_id.IsOk() ) {
    children_ids.push_back( child_id );
    child_id = TreeViewTree->GetNextSibling( child_id );
  }

  for( list< wxTreeItemId >::iterator i = children_ids.begin();
       i != children_ids.end(); i++ ) {
    deleteTree( *i );
  }

  node_map.erase( id.m_pItem ); 
  TreeViewTree->Delete( id );
}

void H3DViewerTreeViewDialog::displayFieldsFromNode( Node *n ) {
  bool new_node = n != displayed_node.get();
  displayed_node.reset( n );
  if( !n ) {
#ifdef DEFAULT_VALUES
    default_values_node.reset( NULL );
#endif
    //SetTitle(wxT(""));
    if( FieldValuesGrid->GetNumberRows() > 0 )
      FieldValuesGrid->DeleteRows( 0, FieldValuesGrid->GetNumberRows() );
    return;
  }

  if( new_node ) {
    FieldValuesGrid->SaveEditControlValue();
  }

  //SetTitle( wxString(n->getTypeName().c_str(),wxConvUTF8) );
  H3DNodeDatabase *db = H3DNodeDatabase::lookupTypeId( typeid( *n ) );
#ifdef DEFAULT_VALUES
  if( new_node ) {
    default_values_node.reset( db->createNode() );
  }
#endif

  list< FieldDBElement * > init_only_fields;
  list< FieldDBElement * > input_only_fields;
  list< FieldDBElement * > output_only_fields;
  list< FieldDBElement * > input_output_fields;

  for( H3DNodeDatabase::FieldDBConstIterator i = db->fieldDBBegin();
       db->fieldDBEnd() != i; i++ ) {
    Field *f = i.getField( n ); 
    if( dynamic_cast< SFNode * >( f ) ||
        dynamic_cast< MFNode * >( f ) ) {
      continue;    
    }

    if( f->getAccessType() == Field::INITIALIZE_ONLY ) {
      init_only_fields.push_back( i.getFieldDBElement() );
    } else if( f->getAccessType() == Field::INPUT_ONLY ) {
      input_only_fields.push_back( i.getFieldDBElement() );
    } else if( f->getAccessType() == Field::OUTPUT_ONLY ) {
      output_only_fields.push_back( i.getFieldDBElement() );
    } else if( f->getAccessType() == Field::INPUT_OUTPUT ) {
      input_output_fields.push_back( i.getFieldDBElement() );
    }
  }

  int rows = 0;

  // initialize only fields.
  if( init_only_fields.size() > 0 ) {
    if( rows >= FieldValuesGrid->GetNumberRows() )
      FieldValuesGrid->AppendRows(1);
    FieldValuesGrid->SetCellSize( rows, 0, 1, 2 );
    FieldValuesGrid->SetCellAlignment(rows, 0, wxALIGN_CENTRE, wxALIGN_CENTRE);
    FieldValuesGrid->SetCellValue(rows, 0, _T("Initialize only"));
    FieldValuesGrid->SetReadOnly( rows, 0 );
    FieldValuesGrid->SetCellBackgroundColour( rows, 0, wxColour(159, 222,222) );
    FieldValuesGrid->SetCellTextColour( rows, 0, wxColour(100, 100,100) );
    rows++;
  }

  for( list< FieldDBElement * >::iterator i = init_only_fields.begin();
       i != init_only_fields.end(); i++ ) {
    Field *f = (*i)->getField( n ); 
    updateRowFromFieldDB( rows, n, *i, new_node );
    rows++;
  }

  // input/output  fields.
  if( input_output_fields.size() > 0 ) {
    if( rows >= FieldValuesGrid->GetNumberRows() )
      FieldValuesGrid->AppendRows(1);
    FieldValuesGrid->SetCellSize( rows, 0, 1, 2 );
    FieldValuesGrid->SetCellAlignment( rows, 0, wxALIGN_CENTRE, wxALIGN_CENTRE);
    FieldValuesGrid->SetCellValue( rows, 0, _T("Input/Output"));
    FieldValuesGrid->SetReadOnly( rows, 0 );
    FieldValuesGrid->SetCellBackgroundColour( rows, 0, wxColour(159, 222,222) );
    FieldValuesGrid->SetCellTextColour( rows, 0, wxColour(100, 100,100) );
    rows++;
  }

  for( list< FieldDBElement * >::iterator i = input_output_fields.begin();
       i != input_output_fields.end(); i++ ) {
    Field *f = (*i)->getField( n ); 
    updateRowFromFieldDB( rows, n, *i, new_node );
    rows++;
  }
  
  // output only fields.
  if( output_only_fields.size() > 0 ) {
    if( rows >= FieldValuesGrid->GetNumberRows() )
      FieldValuesGrid->AppendRows(1);
    FieldValuesGrid->SetCellSize( rows, 0, 1, 2 );
    FieldValuesGrid->SetCellAlignment( rows, 0, wxALIGN_CENTRE, wxALIGN_CENTRE);
    FieldValuesGrid->SetCellValue( rows, 0, _T("Output only"));
    FieldValuesGrid->SetReadOnly( rows, 0 );
    FieldValuesGrid->SetCellBackgroundColour( rows, 0, wxColour(159, 222,222) );
    FieldValuesGrid->SetCellTextColour( rows, 0, wxColour(100, 100,100) );
    rows++;
  }

  for( list< FieldDBElement * >::iterator i = output_only_fields.begin();
       i != output_only_fields.end(); i++ ) {
    Field *f = (*i)->getField( n ); 
    updateRowFromFieldDB( rows, n, *i, new_node );
    rows++;
  }

  // input only fields.
  if( input_only_fields.size() > 0 ) {
    if( rows >= FieldValuesGrid->GetNumberRows() )
      FieldValuesGrid->AppendRows(1);
    FieldValuesGrid->SetCellSize( rows, 0, 1, 2 );
    FieldValuesGrid->SetCellAlignment( rows, 0, wxALIGN_CENTRE, wxALIGN_CENTRE);
    FieldValuesGrid->SetCellValue( rows, 0, _T("Input only"));
    FieldValuesGrid->SetReadOnly( rows, 0 );
    FieldValuesGrid->SetCellBackgroundColour( rows, 0, wxColour(159, 222,222) );
    FieldValuesGrid->SetCellTextColour( rows, 0, wxColour(100, 100,100) );
    rows++;
  }

  for( list< FieldDBElement * >::iterator i = input_only_fields.begin();
       i != input_only_fields.end(); i++ ) {
    Field *f = (*i)->getField( n ); 
    updateRowFromFieldDB( rows, n, *i, new_node );
    rows++;
  }

  if( rows < FieldValuesGrid->GetNumberRows() ) {
    FieldValuesGrid->DeleteRows( rows, FieldValuesGrid->GetNumberRows() - rows );
  }

}

void H3DViewerTreeViewDialog::clearTreeView() {
  list< pair< Node *, string> > l;
  updateNodeTree( TreeViewTree->GetRootItem(), l );
  updateNodeTree( bindable_tree_id, l, false );
  displayFieldsFromNode( NULL );

}

void H3DViewerTreeViewDialog::OnIdle( wxIdleEvent& event ) {
  try {
  if( IsShown() ) {
    TimeStamp now;
    if( now - last_fields_update > 0.1 ) {
      displayFieldsFromNode( selected_node.get() );
      last_fields_update = now;
    }

    if( now - last_tree_update > 1 ) {
      showEntireSceneAsTree( !shown_last_loop );
      last_tree_update = now;
    }
  } else if( shown_last_loop ) {
    // make sure we do not hold any references to any nodes by clearing
    // it.
    clearTreeView();
  }

  shown_last_loop = IsShown();
  } catch( ... ) {
    // ignore any errors
  }
}

void H3DViewerTreeViewDialog::OnCellEdit( wxGridEvent& event ) {
  if( displayed_node.get() ) {
    int col = event.GetCol();
    int row = event.GetRow();
    string s( FieldValuesGrid->GetCellValue( row, col ).mb_str() );
    if( col == 1 ) {
      string field_name( FieldValuesGrid->GetCellValue( row, 0 ).mb_str());
      Field *f = displayed_node->getField( field_name );
      if( SFBool *sfbool = dynamic_cast< SFBool * >( f ) ) {
         sfbool->setValue( s == "1" );
      } else if( ParsableField *pf = dynamic_cast< ParsableField * >( f ) ) {
        try {
          pf->setValueFromString( s );
        } catch(...) {

        }
      }
    }
  }
}

void H3DViewerTreeViewDialog::expandTree( const wxTreeItemId &id ) {
  if( id.IsOk() ) {
    wxTreeItemIdValue cookie;
    wxTreeItemId child_id = TreeViewTree->GetFirstChild( id, cookie );
    while( child_id.IsOk() ) {
      expandTree( child_id );
      child_id = TreeViewTree->GetNextSibling( child_id );
    }
    TreeViewTree->Expand( id );
  }
}

void H3DViewerTreeViewDialog::collapseTree( const wxTreeItemId &id ) {
  if( id.IsOk() ) {
    wxTreeItemIdValue cookie;
    wxTreeItemId child_id = TreeViewTree->GetFirstChild( id, cookie );
    while( child_id.IsOk() ) {
      collapseTree( child_id );
      child_id = TreeViewTree->GetNextSibling( child_id );
    }
    TreeViewTree->Collapse( id );
  }
}

void H3DViewerTreeViewDialog::OnTreeRightClick( wxTreeEvent& event ) {
  TreeViewTree->SelectItem( event.GetItem() );
  TreeIdMap::iterator ni = node_map.find( event.GetItem().m_pItem );
  X3DGeometryNode *geom = NULL;
  
  if( ni != node_map.end() ) {
   geom = dynamic_cast< X3DGeometryNode * >( (*ni).second.get() );
  }

  if(geom) PopupMenu( RightClickMenuGeometry );
  else PopupMenu( RightClickMenu );
}

/// Callback for collapse all menu choice.
void H3DViewerTreeViewDialog::OnTreeViewCollapseAll( wxCommandEvent& event ) {
  wxTreeItemId id = TreeViewTree->GetSelection();
  if( id.IsOk() ) {
    collapseTree( id );
  }
}

/// Callback for expand all menu choice.
void H3DViewerTreeViewDialog::OnTreeViewExpandAll( wxCommandEvent& event ) {
  wxTreeItemId id = TreeViewTree->GetSelection();
  if( id.IsOk() ) {
    expandTree( id );
  }
}

/// Callback for collapse children menu choice.
void H3DViewerTreeViewDialog::OnTreeViewCollapseChildren( wxCommandEvent& event ) {
  wxTreeItemId id = TreeViewTree->GetSelection();
  wxTreeItemIdValue cookie;
  wxTreeItemId child_id = TreeViewTree->GetFirstChild( id, cookie );
  while( child_id.IsOk() ) {
    collapseTree( child_id );
    child_id = TreeViewTree->GetNextSibling( child_id );
  }
}

/// Callback for node watch menu choice.
void H3DViewerTreeViewDialog::OnTreeViewNodeWatch( wxCommandEvent& event ) {
  wxTreeItemId id = TreeViewTree->GetSelection();
  
  TreeIdMap::iterator ni = node_map.find( id.m_pItem );
  if( ni == node_map.end() ) {
    wxMessageBox( wxT("Selected tree item is not a node"),
                  wxT("Error"),
                  wxOK | wxICON_EXCLAMATION);
  } else {
    H3DViewerFieldValuesDialog *fv = new H3DViewerFieldValuesDialog( this );
    fv->displayFieldsFromNode( (*ni).second.get() );
    fv->Show();
  }
}

/// Callback for node save x3d menu choice.
void H3DViewerTreeViewDialog::OnTreeViewSaveX3D( wxCommandEvent& event ) {
  wxTreeItemId id = TreeViewTree->GetSelection();
  
  TreeIdMap::iterator ni = node_map.find( id.m_pItem );
  if( ni == node_map.end() ) {
    wxMessageBox( wxT("Selected tree item is not a node"),
                  wxT("Error"),
                  wxOK | wxICON_EXCLAMATION);
  } else {
    auto_ptr< wxFileDialog > file_dialog ( new wxFileDialog ( this,
                                                   wxT("File to save as.."),
                                                   wxT(""),
                                                   wxT(""),
                                                   wxT("*.*"),
                                                   wxSAVE,
                                                   wxDefaultPosition) );

    if (file_dialog->ShowModal() == wxID_OK) {
      std::string filename(file_dialog->GetPath().mb_str());
      std::ofstream os( filename.c_str() );
      if( os.fail() ) {
        wxMessageBox( wxT("Unable to open selected file"), 
                      wxT("Error"),
                      wxOK | wxICON_EXCLAMATION);
      }
      
      try {
        X3D::writeNodeAsX3D( os,
                             (*ni).second.get() );
      } catch (const Exception::H3DException &e) {
        stringstream s;
        s << e;
        wxMessageBox( wxString(s.str().c_str(),wxConvUTF8), wxT("Error"),
                      wxOK | wxICON_EXCLAMATION);
      }
      os.close();
    }
  }
}

/// Callback for node save VRML menu choice.
void H3DViewerTreeViewDialog::OnTreeViewSaveVRML( wxCommandEvent& event ) {
  wxTreeItemId id = TreeViewTree->GetSelection();
  
  TreeIdMap::iterator ni = node_map.find( id.m_pItem );
  if( ni == node_map.end() ) {
    wxMessageBox( wxT("Selected tree item is not a node"),
                  wxT("Error"),
                  wxOK | wxICON_EXCLAMATION);
  } else {
    auto_ptr< wxFileDialog > file_dialog ( new wxFileDialog ( this,
                                                   wxT("File to save as.."),
                                                   wxT(""),
                                                   wxT(""),
                                                   wxT("*.*"),
                                                   wxSAVE,
                                                   wxDefaultPosition) );

    if (file_dialog->ShowModal() == wxID_OK) {
      std::string filename(file_dialog->GetPath().mb_str());
      std::ofstream os( filename.c_str() );
      if( os.fail() ) {
        wxMessageBox( wxT("Unable to open selected file"), 
                      wxT("Error"),
                      wxOK | wxICON_EXCLAMATION);
      }
      
      try {
        X3D::writeNodeAsVRML( os,
                              (*ni).second.get() );
      } catch (const Exception::H3DException &e) {
        stringstream s;
        s << e;
        wxMessageBox( wxString(s.str().c_str(),wxConvUTF8), wxT("Error"),
                      wxOK | wxICON_EXCLAMATION);
      }
      os.close();
    }
  }
}


/// Callback for node save x3d menu choice.
void H3DViewerTreeViewDialog::OnTreeViewSaveSTL( wxCommandEvent& event ) {
  wxTreeItemId id = TreeViewTree->GetSelection();
  
  TreeIdMap::iterator ni = node_map.find( id.m_pItem );
  if( ni == node_map.end() ) {
    wxMessageBox( wxT("Selected tree item is not a node"),
                  wxT("Error"),
                  wxOK | wxICON_EXCLAMATION);
    return;
  } else {
    X3DGeometryNode *geom = 
      dynamic_cast< X3DGeometryNode * >( (*ni).second.get() );

    if( !geom ) {
      wxMessageBox( wxT("Selected tree item is not a geometry node"),
                    wxT("Error"),
                    wxOK | wxICON_EXCLAMATION);
      return;
    }

    auto_ptr< wxFileDialog > file_dialog( new wxFileDialog ( this,
                                                   wxT("File to save as.."),
                                                   wxT(""),
                                                   wxT(""),
                                                   wxT("*.*"),
                                                   wxSAVE,
                                                   wxDefaultPosition) );

    if (file_dialog->ShowModal() == wxID_OK) {
      std::string filename(file_dialog->GetPath().mb_str());
      std::ofstream os( filename.c_str() );
      if( os.fail() ) {
        wxMessageBox( wxT("Unable to open selected file"), 
                      wxT("Error"),
                      wxOK | wxICON_EXCLAMATION);
      }
      
      try {
        X3D::writeGeometryAsSTL( os, geom );
      } catch (const Exception::H3DException &e) {
        stringstream s;
        s << e;
        wxMessageBox( wxString(s.str().c_str(),wxConvUTF8), wxT("Error"),
                      wxOK | wxICON_EXCLAMATION);
      }
      os.close();
    }
  }
}

void H3DViewerTreeViewDialog::OnClose( wxCloseEvent& event ) {
  Hide();
}

void H3DViewerTreeViewDialog::btnCloseClick(wxCommandEvent& event) {
  Hide();
}

void H3DViewerTreeViewDialog::updateRowFromFieldDB( int row, 
                                                     Node *n,
                                                     FieldDBElement *db,
                                                     bool new_node ) {
  Field *f = db->getField( n );
  if( SFNode *sfnode = dynamic_cast< SFNode * >( f ) ) {
    
  } else if( MFNode *mfnode = dynamic_cast< MFNode * >( f ) ) {
    
  } else {
    wxGridCellEditor *current_editor = FieldValuesGrid->GetCellEditor( row, 1 );
    wxGridCellRenderer *current_renderer = FieldValuesGrid->GetCellRenderer( row, 1 );
    wxGridCellEditor *editor = current_editor;
    wxGridCellRenderer *renderer = current_renderer;
    string value;
    string default_value;
    Field *default_field = db->getField( default_values_node.get() );
    bool allow_cell_update = true;

    FieldValuesGrid->SetCellSize( row, 0, 1, 1 );
    FieldValuesGrid->SetCellAlignment(row, 0, wxALIGN_LEFT, wxALIGN_TOP);
    FieldValuesGrid->SetReadOnly( row, 0 );
    FieldValuesGrid->SetCellBackgroundColour( row, 0, *wxWHITE );
    FieldValuesGrid->SetCellTextColour( row, 0, *wxBLACK );

    FieldValuesGrid->SetCellSize( row, 1, 1, 1 );
    FieldValuesGrid->SetCellAlignment(row, 1, wxALIGN_LEFT, wxALIGN_TOP);
    FieldValuesGrid->SetCellBackgroundColour( row, 1, *wxWHITE );
    FieldValuesGrid->SetCellTextColour( row, 1, *wxBLACK );
    if( f->getAccessType() == Field::INITIALIZE_ONLY ||
        f->getAccessType() == Field::OUTPUT_ONLY ) {
      FieldValuesGrid->SetReadOnly( row, 1, true );
    } else {
      FieldValuesGrid->SetReadOnly( row, 1, false );
    }


    if( SFString *sfstring = dynamic_cast< SFString * >( f ) ) {
      // set renderer to string renderer
      // GetCellRenderer increases reference count.
      // Decrease reference count to clean up memory properly before
      // replacing pointer.
      renderer->DecRef();
      renderer = FieldValuesGrid->GetDefaultRenderer();
      // use a choice editor if a set of valid values have been
      // specified for the field.
      if( sfstring->hasValidValues() ) {    
        // set editor
        if( !dynamic_cast< wxGridCellChoiceEditor * >( current_editor ) ||
            new_node ) {
          const set<string > &valid_values = sfstring->getValidValues();
          wxArrayString choices;
          choices.Alloc( valid_values.size() );
          for( set<string>::const_iterator i = valid_values.begin();
               i != valid_values.end(); i++ ) {
            choices.Add( wxString( (*i).c_str(), wxConvUTF8 ));
          }
          // GetCellEditor increases reference count.
          // Decrease reference count to clean up memory properly before
          // replacing pointer.
          editor->DecRef();
          editor = new wxGridCellChoiceEditor( choices, false );
        }
      } else {
        // no valid values specified, use string editor.
        // GetCellEditor increases reference count.
        // Decrease reference count to clean up memory properly before
        // replacing pointer.
        editor->DecRef();
        editor = FieldValuesGrid->GetDefaultEditor();
      }
      
      // set value
      value = "";
      if( sfstring->getAccessType() != Field::INPUT_ONLY ) {
        value = sfstring->getValue();
        if( default_field ) {
          default_value = 
            static_cast< SFString * >( default_field )->getValue();
        }
      }
    } else if( SFBool *sfbool = dynamic_cast< SFBool * >( f ) ) {
      // set renderer
      if( !dynamic_cast< wxGridCellBoolRenderer * >( current_renderer ) ) {
        // GetCellRenderer increases reference count.
        // Decrease reference count to clean up memory properly before
        // replacing pointer.
        renderer->DecRef();
        renderer = new wxGridCellBoolRenderer;
      }
      
      // set editor
      if( !dynamic_cast< wxGridCellBoolEditor * >( current_editor ) ) {
        // GetCellEditor increases reference count.
        // Decrease reference count to clean up memory properly before
        // replacing pointer.
        editor->DecRef();
        editor = new wxGridCellBoolEditor;
      }
      
      if( FieldValuesGrid->GetGridCursorRow() ==  row ) {
        if( f->getAccessType() != Field::INPUT_ONLY )
          FieldValuesGrid->SaveEditControlValue();
        allow_cell_update = FieldValuesGrid->GetGridCursorCol() !=  1;
      }
      
      // set value
      value = "0";
      if( sfbool->getAccessType() != Field::INPUT_ONLY ) {
        bool checked = sfbool->getValue();
        value = checked ? "1" : "0";
      }
      
      // set default value
      default_value = "0";
      if( sfbool->getAccessType() != Field::INPUT_ONLY  ) {
        if( default_field ) {
          SFBool *sf = static_cast< SFBool *>( default_field );
          bool checked = sf->getValue();
          default_value = checked ? "1" : "0";
        }
      }
    }  else if( ParsableField *pfield = dynamic_cast< ParsableField * >( f ) ) {
      // set renderer and editor
      // GetCellEditor and GetCellRenderer increases reference count.
      // Decrease reference count to clean up memory properly before
      // replacing pointer.
      editor->DecRef();
      renderer->DecRef();
      editor = FieldValuesGrid->GetDefaultEditor();
      renderer = FieldValuesGrid->GetDefaultRenderer();

      // set value
      if( pfield->getAccessType() != Field::INPUT_ONLY ) {
        value = pfield->getValueAsString();
      }
      
      // set default value
      default_value = "0";
      if( pfield->getAccessType() != Field::INPUT_ONLY  ) {
        if( default_field ) {
          ParsableField *df = static_cast< ParsableField * >( default_field );
          default_value = df->getValueAsString();
        }
      }
    }
    
    // if not enough row, append a new one
    if( row >= FieldValuesGrid->GetNumberRows() )
      FieldValuesGrid->AppendRows(1);

    // set the editor and renderer of the cell
    FieldValuesGrid->SetCellRenderer( row, 1, renderer );
    FieldValuesGrid->SetCellEditor( row, 1, editor );

    bool changed_color = false;
#ifdef DEFAULT_VALUES
    wxColour current_color = FieldValuesGrid->GetCellTextColour( row, 0 );
    // set the color of the text.
    if( value == default_value ) {
      FieldValuesGrid->SetCellTextColour( row, 0, *wxBLACK );
      FieldValuesGrid->SetCellTextColour( row, 1, *wxBLACK );
    } else {
      FieldValuesGrid->SetCellTextColour( row, 0, *wxRED );
      FieldValuesGrid->SetCellTextColour( row, 1, *wxRED );
    }
    changed_color = FieldValuesGrid->GetCellTextColour( row, 0 ) != current_color;
#endif
    
    //  set the field name value if it has changed.
    if( string( FieldValuesGrid->GetCellValue( row, 0 ).mb_str() ) != f->getName() ||
        changed_color )
      FieldValuesGrid->SetCellValue( row, 0, wxString( f->getName().c_str() , wxConvUTF8) );
    
    // set field value if changed and not currently being edited.
    if( allow_cell_update &&
        string( FieldValuesGrid->GetCellValue( row, 1 ).mb_str() ) != value &&
        !FieldValuesGrid->IsCellEditControlEnabled() ){
      FieldValuesGrid->SetCellValue( row, 1, wxString( value.c_str(), wxConvUTF8) );
    }
  }
}

/// Callback for node save x3d menu choice.
void H3DViewerTreeViewDialog::OnTreeViewSaveTrianglesX3D( wxCommandEvent& event ) {
  wxTreeItemId id = TreeViewTree->GetSelection();
  
  TreeIdMap::iterator ni = node_map.find( id.m_pItem );
  if( ni == node_map.end() ) {
    wxMessageBox( wxT("Selected tree item is not a node"),
                  wxT("Error"),
                  wxOK | wxICON_EXCLAMATION);
  } else {
    wxFileDialog *file_dialog = new wxFileDialog ( this,
                                                   wxT("File to save as.."),
                                                   wxT(""),
                                                   wxT(""),
                                                   wxT("*.*"),
                                                   wxSAVE,
                                                   wxDefaultPosition) ;

    if (file_dialog->ShowModal() == wxID_OK) {
      std::string filename(file_dialog->GetPath().mb_str());
      std::ofstream os( filename.c_str() );
      if( os.fail() ) {
        wxMessageBox( wxT("Unable to open selected file"), 
                      wxT("Error"),
                      wxOK | wxICON_EXCLAMATION);
      }
      
      try {
        Node *n = (*ni).second.get();

        AutoRef< IndexedTriangleSet > its( new IndexedTriangleSet );

        vector< Vec3f > triangles;
        triangles.reserve( 200 );
        collectAllTriangles( n, Matrix4f(), triangles ); 

        Coordinate *c = new Coordinate;
        c->point->setValue( triangles );
        vector< int > indices;
        indices.reserve( triangles.size() );
        for( unsigned int i = 0; i < triangles.size(); i++ ) {
          indices.push_back( i );
        }

        its->coord->setValue( c );
        its->index->setValue( indices );
       
        X3D::writeNodeAsX3D( os, its.get() );
      } catch (const Exception::H3DException &e) {
        stringstream s;
        s << e;
        wxMessageBox( wxString(s.str().c_str(),wxConvUTF8), wxT("Error"),
                      wxOK | wxICON_EXCLAMATION);
      }
      os.close();
    }
  }
}

void H3DViewerTreeViewDialog::collectAllTriangles( Node *n, 
                                                   const Matrix4f &transform,
                                                   vector< Vec3f > &triangles ) {

  if( !n ) return;

  
  if( X3DShapeNode *shape = dynamic_cast< X3DShapeNode * >( n ) ) {
    X3DGeometryNode *geom = shape->geometry->getValue();
    if( geom ) {
      vector< HAPI::Collision::Triangle > tris;
      geom->boundTree->getValue()->getAllTriangles( tris );
      for( unsigned int i = 0; i < tris.size(); i++ ) {
        triangles.push_back( transform * (Vec3f) tris[i].a );
        triangles.push_back( transform * (Vec3f) tris[i].b );
        triangles.push_back( transform * (Vec3f) tris[i].c );
      }
    }
  } 

  MatrixTransform *t = dynamic_cast< MatrixTransform * >( n );
  Matrix4f new_transform = t ? t->matrix->getValue() * transform :transform;

  H3DNodeDatabase *db = H3DNodeDatabase::lookupTypeId( typeid( *n ) );
  for( H3DNodeDatabase::FieldDBConstIterator i = db->fieldDBBegin();
       db->fieldDBEnd() != i; i++ ) {
    Field *f = i.getField( n ); 
    if( SFNode *sfnode = dynamic_cast< SFNode * >( f ) ) {
      collectAllTriangles( sfnode->getValue(), new_transform, triangles );
    } else if( MFNode *mfnode = dynamic_cast< MFNode * >( f ) ) {
      for( unsigned int j = 0; j < mfnode->size(); j++ ) {
        Node *n = mfnode->getValueByIndex( j ); 
        collectAllTriangles( n, new_transform, triangles );
      }
    }
  }
}

void H3DViewerTreeViewDialog::OnTreeViewDeleteNode( wxCommandEvent& event ) {
  wxTreeItemId id = TreeViewTree->GetSelection();
  
  TreeIdMap::iterator ni = node_map.find( id.m_pItem );
  if( ni == node_map.end() ) {
    wxMessageBox( wxT("Selected tree item is not a node"),
                  wxT("Error"),
                  wxOK | wxICON_EXCLAMATION);
    return;
  }

  Node *selected_node = (*ni).second.get();

  if( dynamic_cast< Scene * >( selected_node ) ) {
    wxMessageBox( wxT("Deletion of Scene node not allowed."),
                  wxT("Error"),
                  wxOK | wxICON_EXCLAMATION);
    return;
  }

  if( dynamic_cast< H3DWindowNode * >( selected_node ) ) {
    wxMessageBox( wxT("Deletion of H3DWindowNode node not allowed."),
                  wxT("Error"),
                  wxOK | wxICON_EXCLAMATION);
    return;
  }

  wxTreeItemId parent_id = TreeViewTree->GetItemParent( id );
  TreeIdMap::iterator ni_parent = node_map.find( parent_id.m_pItem );

  if( ni_parent == node_map.end() ) {
    wxMessageBox( wxT("Selected tree item does not have a parent"),
                  wxT("Error"),
                  wxOK | wxICON_EXCLAMATION);
    return;
  }

  Node *n = (*ni_parent).second.get();

  H3DNodeDatabase *db = H3DNodeDatabase::lookupTypeId( typeid( *n ) );
  for( H3DNodeDatabase::FieldDBConstIterator i = db->fieldDBBegin();
       db->fieldDBEnd() != i; i++ ) {
    Field *f = i.getField( n ); 
    if( SFNode *sfnode = dynamic_cast< SFNode * >( f ) ) {
      if( sfnode->getValue() == selected_node ) {
        if( sfnode->getAccessType() == Field::OUTPUT_ONLY ||
            sfnode->getAccessType() == Field::INITIALIZE_ONLY ) {
           wxMessageBox( wxT("Deletion of node in INITIALIZE_ONLY or OUTPUT_ONLY field not allowed."),
                         wxT("Error"),
                         wxOK | wxICON_EXCLAMATION);
        } else {
          sfnode->setValue( NULL );
        }
        return;
      }
    } else if( MFNode *mfnode = dynamic_cast< MFNode * >( f ) ) {
      for( unsigned int j = 0; j < mfnode->size(); j++ ) {
        if( mfnode->getValueByIndex( j ) == selected_node ) { 
          if( mfnode->getAccessType() == Field::OUTPUT_ONLY ||
              mfnode->getAccessType() == Field::INITIALIZE_ONLY ) {
            wxMessageBox( wxT("Deletion of node in INITIALIZE_ONLY or OUTPUT_ONLY field not allowed."),
                          wxT("Error"),
                          wxOK | wxICON_EXCLAMATION);
          } else {
            mfnode->erase( selected_node );
          }
        }
      }
    }
  }
}

void H3DViewerTreeViewDialog::OnTreeViewAddChildNode( wxCommandEvent& event ) {
  wxTreeItemId id = TreeViewTree->GetSelection();
  
  TreeIdMap::iterator ni = node_map.find( id.m_pItem );
  if( ni == node_map.end() ) {
    wxMessageBox( wxT("Selected tree item is not a node"),
                  wxT("Error"),
                  wxOK | wxICON_EXCLAMATION);
  }

  vector< wxString > node_fields;

  Node *selected_node = (*ni).second.get();
  H3DNodeDatabase *db = H3DNodeDatabase::lookupTypeId( typeid( *selected_node ) );
  for( H3DNodeDatabase::FieldDBConstIterator i = db->fieldDBBegin();
       db->fieldDBEnd() != i; i++ ) {
    Field *f = i.getField( selected_node ); 
    if( SFNode *sfnode = dynamic_cast< SFNode * >( f ) ) {
      if( sfnode->getAccessType() == Field::INPUT_ONLY ||
            sfnode->getAccessType() == Field::INPUT_OUTPUT ) {
          node_fields.push_back( sfnode->getName() );
      } 
    } else if( MFNode *mfnode = dynamic_cast< MFNode * >( f ) ) {
      if( mfnode->getAccessType() == Field::INPUT_ONLY ||
          mfnode->getAccessType() == Field::INPUT_OUTPUT ) {
         node_fields.push_back( mfnode->getName() );
      }
    }
  }
  
  if( node_fields.empty() ) {
     wxMessageBox( wxT("Selected node does not have a SFNode or MFNode field."),
                   wxT("Error"),
                   wxOK | wxICON_EXCLAMATION);
    return;
  }

  string field_to_change;

  if( node_fields.size() > 1 ) {
    wxString *choices = new wxString[ node_fields.size() ];
    for( unsigned int i = 0; i < node_fields.size(); i++ ) {
      choices[i] = node_fields[i];
    }
    wxSingleChoiceDialog *choice_dialog = new wxSingleChoiceDialog ( this,
                                                                     wxT("Add/replace node in which field.."),
                                                                     wxT(""),
                                                                     node_fields.size(),
                                                                     choices );
    delete [] choices;

    if (choice_dialog->ShowModal() == wxID_OK) {
      field_to_change = node_fields[ choice_dialog->GetSelection() ];
    } else {
      return;
    }
  }

  wxTextEntryDialog *node_name_dialog = 
            new wxTextEntryDialog(this, 
                                  wxT("Enter the name of the node type you want to use" ),
                                  wxT("Add/replace node" ) );
  if (node_name_dialog->ShowModal() == wxID_OK) {
     Node *new_node = H3DNodeDatabase::createNode( node_name_dialog->GetValue().mb_str() );
     if( !new_node ) {
       wxMessageBox( wxT("No such node type exists: " + node_name_dialog->GetValue()),
                     wxT("Error"),
                     wxOK | wxICON_EXCLAMATION);
     } else {
       Field *f = selected_node->getField( field_to_change );
       SFNode *sfnode = dynamic_cast< SFNode * >( f );
       MFNode *mfnode = dynamic_cast< MFNode * >( f );
       
       if( sfnode ) {
         AutoRef< Node > old_node( sfnode->getValue() );
         try { 
           sfnode->setValue( new_node );
         } catch (...) {
           sfnode->setValue( old_node.get() );
           wxMessageBox( wxT("Invalid node type for field"),
                         wxT("Error"),
                         wxOK | wxICON_EXCLAMATION);
         }
       } else if( mfnode ) {
         try { 
           mfnode->push_back( new_node );
         } catch (...) {
            wxMessageBox( wxT("Invalid node type for field"),
                          wxT("Error"),
                          wxOK | wxICON_EXCLAMATION);
         }
       }
     }
  } 
}
