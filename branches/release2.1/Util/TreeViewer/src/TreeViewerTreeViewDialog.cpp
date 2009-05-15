#include "TreeViewerTreeViewDialog.h"
#include "TreeViewerApp.h"

TreeViewerTreeViewDialog::TreeViewerTreeViewDialog( wxWindow* parent )
:
  TreeViewDialog( parent ),
  displayed_node( NULL )
{
  TreeViewTree->AddRoot( wxT("World") );
  // add the bindable nodes in the tree view
  bindable_tree_id = TreeViewTree->AppendItem( TreeViewTree->GetRootItem(), 
                                               wxT("Active bindable nodes") );
  TreeViewTree->Expand( TreeViewTree->GetRootItem() ); 
}


void TreeViewerTreeViewDialog::OnNodeSelected( wxTreeEvent& event ) {

  TreeViewerFieldValuesDialog *field_values = 
    static_cast< TreeViewerApp * >(wxTheApp )->field_values_dialog;
 TreeIdMap::iterator ni = node_map.find( event.GetItem().m_pItem );
  if( ni == node_map.end() ) {
    field_values->displayFieldsFromNode( NULL );
    displayFieldsFromNode( NULL );
  } else {
    field_values->displayFieldsFromNode( (*ni).second.get() );
    displayFieldsFromNode( (*ni).second.get() );
  }
}

void TreeViewerTreeViewDialog::showEntireSceneAsTree( wxTreeItemId tree_id,
                                                      bool expand_new ) {
  //node_map.clear();

  // show the scene in the tree view.
  //addNodeToTree( tree_id, *Scene::scenes.begin() );
  list< H3D::Node * > l;
  l.push_back( *Scene::scenes.begin() );

  updateNodeTree( tree_id, l, expand_new );


  l.clear();  
  const X3DBindableNode::StackMapType &stacks = X3DBindableNode::getStackMap();
  for( X3DBindableNode::StackMapType::const_iterator i = stacks.begin(); 
       i != stacks.end();i++ ) {
    l.push_back( X3DBindableNode::getActive( (*i).first ) );
  }
  updateNodeTree( bindable_tree_id, l, false );
}

void TreeViewerTreeViewDialog::addNodeToTree( wxTreeItemId tree_id, 
                                              H3D::Node *n,
                                              bool expand ) {
  if( !n ) return;

  // the name in the tree is NodeType(DEFed name)
  string tree_string = n->getTypeName();
  if( n->hasName() ) {
    tree_string = tree_string + " (" + n->getName() + ")";
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
        addNodeToTree( new_id, sfnode->getValue(), expand );
      }
    } else if( MFNode *mfnode = dynamic_cast< MFNode * >( f ) ) {
      if( mfnode->getAccessType() != Field::INPUT_ONLY ) {
        for( MFNode::const_iterator i = mfnode->begin(); i != mfnode->end(); i++ ) {
          addNodeToTree( new_id, *i, expand );
        }
      }
    }
  }

  // make the tree be open down to leaves or not.
  if( expand ) 
    TreeViewTree->Expand( new_id );
}


void TreeViewerTreeViewDialog::updateNodeTree( wxTreeItemId tree_id, 
                                               list< H3D::Node *> nodes,
                                               bool expand_new ) {

  // find all children of tree_id
  list< wxTreeItemId > children_ids;
  wxTreeItemIdValue cookie;
  wxTreeItemId id = TreeViewTree->GetFirstChild( tree_id, cookie );
  while( id.IsOk() ) {
    children_ids.push_back( id );
    id = TreeViewTree->GetNextChild( id, cookie );
  }
  
  // update each of the tree ids that already exists. Either by updating
  // them if they still refer to a node or deleting them otherwise.
  bool have_node_in_tree = false;
  for( list< wxTreeItemId >::iterator i = children_ids.begin();
       i != children_ids.end(); i++ ) {
    // find the node corresponding to the id in the current tree view.

    if( node_map.find( (*i).m_pItem ) == node_map.end() ) continue;

    Node *id_node = node_map[ (*i).m_pItem ].get(); 

    // check if this node still exists in the new node structure
    list< H3D::Node * >::iterator ni = std::find( nodes.begin(), nodes.end(), id_node );
    if( ni != nodes.end() ) {
      // the node the tree id refers to still exists on this level
      // so recurse down.

      // find all child nodes of the node
      list< H3D::Node * > child_nodes;
      H3DNodeDatabase *db = H3DNodeDatabase::lookupTypeId( typeid( *id_node ) );
      for( H3DNodeDatabase::FieldDBConstIterator j = db->fieldDBBegin();
           db->fieldDBEnd() != j; j++ ) {
         Field *f = j.getField( id_node ); //Field *f = id_node->getField( *j );

        if( SFNode *sfnode = dynamic_cast< SFNode * >( f ) ) {
          if( sfnode->getAccessType() != Field::INPUT_ONLY ) {
            Node *n = sfnode->getValue();
            if( n ) child_nodes.push_back( sfnode->getValue() );
          }
        } else if( MFNode *mfnode = dynamic_cast< MFNode * >( f ) ) {
          if( mfnode->getAccessType() != Field::INPUT_ONLY ) {
            for( MFNode::const_iterator mf = mfnode->begin(); 
                 mf != mfnode->end(); mf++ ) {
              if( *mf ) child_nodes.push_back( *mf );
            }
          }
        }        
      }
      // update
      updateNodeTree( *i, child_nodes, expand_new );
      nodes.erase( ni );
    } else {
      // the node does not exist, so remove the tree id.
      TreeViewTree->Delete( *i );
      node_map.erase( (*i).m_pItem ); 
    }
  }

  // add all new nodes to the tree. 
  for( list< H3D::Node *>::iterator i = nodes.begin();
       i != nodes.end(); i++ ) {
    addNodeToTree( tree_id, *i, expand_new );
  }

  // make the tree be open down to leaves by default.
  //TreeViewTree->Expand( tree_id );
}


void TreeViewerTreeViewDialog::displayFieldsFromNode( Node *n ) {
  bool new_node = n != displayed_node.get();
  displayed_node.reset( n );
  if( !n ) {
#ifdef DEFAULT_VALUES
    default_values_node.reset( NULL );
#endif
    SetTitle(wxT(""));
    if( FieldValuesGrid->GetNumberRows() > 0 )
      FieldValuesGrid->DeleteRows( 0, FieldValuesGrid->GetNumberRows() );
    return;
  }

  SetTitle( wxString(n->getTypeName().c_str(),wxConvUTF8) );
  H3DNodeDatabase *db = H3DNodeDatabase::lookupTypeId( typeid( *n ) );
#ifdef DEFAULT_VALUES
  if( new_node ) {
    default_values_node.reset( db->createNode() );
  }
#endif
  unsigned int rows = 0;
  for( H3DNodeDatabase::FieldDBConstIterator i = db->fieldDBBegin();
       db->fieldDBEnd() != i; i++ ) {
    Field *f = i.getField( n ); //n->getField( *i );
      
    if( SFNode *sfnode = dynamic_cast< SFNode * >( f ) ) {
        
    } else if( MFNode *mfnode = dynamic_cast< MFNode * >( f ) ) {

    } /*else if( SFBool *sfbool = dynamic_cast< SFBool * >( f ) ) {
      if( rows >= FieldValuesGrid->GetNumberRows() ) {
         FieldValuesGrid->AppendRows(1);
         FieldValuesGrid->SetCellRenderer( rows, 1, new wxGridCellBoolRenderer );
         FieldValuesGrid->SetCellEditor( rows, 1, new wxGridCellBoolEditor );
      }

      if( string( FieldValuesGrid->GetCellValue( rows, 0 ).mb_str() ) != *i )
        FieldValuesGrid->SetCellValue( rows, 0, wxString( (*i).c_str() , wxConvUTF8) );
      rows++;
      } */ else if( ParsableField *pfield = dynamic_cast< ParsableField * >( f ) ) {
      if( pfield->getAccessType() != Field::INPUT_ONLY ) {
        if( rows >= FieldValuesGrid->GetNumberRows() )
          FieldValuesGrid->AppendRows(1);
        if( string( FieldValuesGrid->GetCellValue( rows, 0 ).mb_str() ) != *i )
          FieldValuesGrid->SetCellValue( rows, 0, wxString( (*i).c_str() , wxConvUTF8) );

        string value = pfield->getValueAsString();
        if( string( FieldValuesGrid->GetCellValue( rows, 1 ).mb_str() ) != value )
          FieldValuesGrid->SetCellValue( rows, 1, wxString( value.c_str(), wxConvUTF8) );

#ifdef DEFAULT_VALUES
         ParsableField *df = 
           static_cast< ParsableField * >( i.getField( default_values_node.get() ) );
         if( df->getValueAsString() == pfield->getValueAsString() ) {
           FieldValuesGrid->SetCellTextColour( rows, 0, *wxBLACK );
           FieldValuesGrid->SetCellTextColour( rows, 1, *wxBLACK );
         } else {
           FieldValuesGrid->SetCellTextColour( rows, 0, *wxRED );
           FieldValuesGrid->SetCellTextColour( rows, 1, *wxRED );
         }
#endif

        rows++;


      }
    }
  
  }

  if( rows < FieldValuesGrid->GetNumberRows() ) {
    FieldValuesGrid->DeleteRows( rows, FieldValuesGrid->GetNumberRows() - rows );
  }

}


void TreeViewerTreeViewDialog::OnIdle( wxIdleEvent& event ) {
  TimeStamp now;
  if( now - last_fields_update > 0.1 ) {
    displayFieldsFromNode( displayed_node.get() );
    last_fields_update = now;
  }
}

void TreeViewerTreeViewDialog::OnCellEdit( wxGridEvent& event ) {
  if( displayed_node.get() ) {
    int col = event.GetCol();
    int row = event.GetRow();
    string s( FieldValuesGrid->GetCellValue( row, col ).mb_str() );
    if( col == 1 ) {
      string field_name( FieldValuesGrid->GetCellValue( row, 0 ).mb_str());
      Field *f = displayed_node->getField( field_name );
      if( ParsableField *pf = dynamic_cast< ParsableField * >( f ) ) {
        try {
          pf->setValueFromString( s );
        } catch(...) {

        }
      }
    }
  }
}

