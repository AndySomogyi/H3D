//////////////////////////////////////////////////////////////////////////////
//    Copyright 2009-2014, SenseGraphics AB
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
/// \file H3DViewerTreeViewDialog.h
/// \brief Header file for H3DViewerTreeViewDialog.
/// Subclass of TreeViewDialog, which is generated by wxFormBuilder.
///
//
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __H3DViewerTreeViewDialog__
#define __H3DViewerTreeViewDialog__

#include <map>
#include <list>

// Node.h must be included after H3DViewer.h in order to work in wxWidgets >=2.9 MSW
// but before for older versions.
#include <wx/version.h>

#if( wxMAJOR_VERSION > 2 || wxMINOR_VERSION >= 9 ) 
#include "H3DViewer.h"
#include <H3D/Node.h>
#include <H3D/X3DTextureNode.h>
#else
#include <H3D/Node.h>
#include <H3D/X3DTextureNode.h>
#include "H3DViewer.h"
#endif

#include "H3DViewerFieldValuesDialog.h"
#include "H3DViewerFieldValuesDialogPropGrid.h"

#define DEFAULT_VALUES

#ifdef HAVE_WXPROPGRID
#define USE_PROPGRID
#endif

using namespace H3D;

// A panel which renders a bitmap image
class wxImagePanel : public wxPanel {
public:
  wxImagePanel(wxWindow* parent);
 
  void paintEvent(wxPaintEvent & evt);
  void setImage ( const wxImage& _image );

  DECLARE_EVENT_TABLE()

protected:
  wxBitmap image;
};

/// An image viewer window which renders an X3DTextureNode
class H3DViewImage : public ViewImage {
public:
  H3DViewImage ( wxWindow* parent, X3DTextureNode& _texture );

protected:
  virtual void OnSave( wxCommandEvent& event );
  virtual void OnRefresh( wxCommandEvent& event );
  virtual void OnAutoRefresh( wxCommandEvent& event );
  virtual void OnTimer ( wxTimerEvent& event );

  void updateImage ();

  wxImagePanel* draw_pane;
  AutoRef < X3DTextureNode > texture;
  unsigned char* image_data;
};

/** Implementing TreeViewDialog */
class H3DViewerTreeViewDialog : public TreeViewDialog
{
public:
  typedef enum {
    /// Expand tree for all nodes
    EXPAND_ALL,
    /// Expand tree for all nodes that inherit from X3DGroupingNode
    EXPAND_GROUP,
    /// Don't expand anything
    EXPAND_NONE
  } ExpandMode;

  /** Constructor */
  H3DViewerTreeViewDialog( wxWindow* parent );

  /// Destructor
  ~H3DViewerTreeViewDialog();

  /// Open an image viewer window to render the specified texture
  void showImage ( X3DTextureNode& _image );

  void collectAllTriangles( Node *n, 
                            const Matrix4f &transform,
                            vector< Vec3f > &triangles );

  virtual void btnCloseClick(wxCommandEvent& event);

  /// Callback for when the window is closed. Hides the window
  /// instead of destroying it.
  virtual void OnClose( wxCloseEvent& event );

  // Callback for when a node is selected in the tree view.
  // It updates the node field view.
  virtual void OnNodeSelected( wxTreeEvent& event );

  // Callback for when Enter is typed in text control.
  virtual void onSearchTextCtrl( wxCommandEvent& event );

  /// Callback for when user right clicked on a tree node.
  virtual void OnTreeRightClick( wxTreeEvent& event );
        
  // Callback for idle. Does dynamic updates of field values and tree view.
  void OnIdle( wxIdleEvent& event );

  // Callback for profiling checkbox
  void OnProfileCheckbox( wxCommandEvent& event );

  // Handle key-press events for whole window
  virtual void onCharHook ( wxKeyEvent& event );

  // Give focus to the search box
  void highlightSearchBox ();

  // Update the node field view with the given node.
  void displayFieldsFromNode( Node * );

  // Update the tree view of the entire scene. The expand_new argument
  // determines if hew nodes should be added with the tree expanded
  // or not.
  void showEntireSceneAsTree( ExpandMode expand = EXPAND_ALL );

  // Clear all data from the tree view. Releases all references
  // to any node in the dialog.
  void clearTreeView();

  // Expand the item and all children below it.
  void expandTree( const wxTreeItemId &id ); 

  // Collapse the item and all children below it.
  void collapseTree( const wxTreeItemId &id ); 

  // Delete a part of a wx tree, both from the wx widget and from
  // the map of nodes.
  void deleteTree( const wxTreeItemId &id );

  // true if this dialog was shown last loop.
  bool shown_last_loop;

  // Add a view of the node as a child to the tree. expand determines
  // if the tree should be expanded from the start or node.
  void addNodeToTree( wxTreeItemId tree_id, 
                      H3D::Node *n, 
                      string container_field,
                      ExpandMode expand = EXPAND_ALL );

  // Returns a string to use as the label for a Node in the tree.
  string getNodeLabel( H3D::Node *n, const string &container_field = "" );

  // Update a current tree. The list of nodes is a list of all the nodes
  // that are supposed to be children to tree_id. All nodes that are already
  // there are updated, new nodes are added and parts that are nod in the list
  // are deleted.
  void updateNodeTree( wxTreeItemId tree_id, 
                       std::list< pair< H3D::Node *, string > >,
                       ExpandMode expand = EXPAND_ALL,
                       bool check_if_expanded = true );

  // The time of the last update to the tree view.
  TimeStamp last_tree_update;

  typedef  std::map< wxTreeItemIdValue, H3D::AutoRef< H3D::Node > > TreeIdMap;
  // A map from tree id to node to be able to lookup what Node a part of the
  // tree view refers to.
  TreeIdMap node_map;

  // The id of the tree node that holds bindable node info.
  wxTreeItemId bindable_tree_id;


  // The node that was last selected. This will be used to
  // set the displayed node in isIdle. Cannot use only displayed_node
  // since we have to divide it into two parts, selecting and displaying
  // in order to work around a crash that happens in a choice editor.
  AutoRef< Node > selected_node;

  /// Returns the number of triangles in X3DGeometryNode. -1 of unknown.
  int getNrTriangles( X3DGeometryNode *geom );

#ifdef USE_PROPGRID
  H3DViewerFieldValuesPanelPropGrid *field_values_panel;
#else
  H3DViewerFieldValuesPanel *field_values_panel;
#endif
  
  MenuContainer *menu_container;
  // if set to true the labels of all Nodes in the node tree will be updated next
  // loop.
  bool force_update_labels;

  // Help function to onSearchTextCtrl, recursive function to find the string in any wx label for any tree item.
  bool onSearchTextCtrlHelp( const wxTreeItemId &item, const wxString &to_find, wxTreeItemId &found_item, bool (*compare_func)( const wxString &s1, const wxString &s2 ), const wxTreeItemId &check_parent = wxTreeItemId() );
};
#endif // __H3DViewerTreeViewDialog__
