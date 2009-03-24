#ifndef __TreeViewerTreeViewDialog__
#define __TreeViewerTreeViewDialog__

#include <H3D/Node.h>
#include <map>
#include <list>
/**
@file
Subclass of TreeViewDialog, which is generated by wxFormBuilder.
*/

#include "TreeViewer.h"


using namespace H3D;
/** Implementing TreeViewDialog */
class TreeViewerTreeViewDialog : public TreeViewDialog
{
public:
	/** Constructor */
	TreeViewerTreeViewDialog( wxWindow* parent );

  // Callback for when a node is selected in the tree view.
  // It updates the node field view.
  virtual void OnNodeSelected( wxTreeEvent& event );

  // Callback for when a cell in the node field view is changed.
  virtual void OnCellEdit( wxGridEvent& event );

  // Callback for idle. Does dynamic updates of field values and tree view.
  void OnIdle( wxIdleEvent& event );

  // Update the node field view with the given node.
  void displayFieldsFromNode( Node * );

  // Update the tree view of the entire scene. The expand_new argument
  // determines if hew nodes should be added with the tree expanded
  // or not.
  void showEntireSceneAsTree( bool expand_new = true );

  // Clear all data from the tree view. Releases all references
  // to any node in the dialog.
  void clearTreeView();

protected:
  // Delete a part of a wx tree, both from the wx widget and from
  // the map of nodes.
  void deleteTree( const wxTreeItemId &id );

  // true if this dialog was shown last loop.
  bool shown_last_loop;

  // Add a view of the node as a child to the tree. expand determines
  // if the tree should be expanded from the start or node.
  void addNodeToTree( wxTreeItemId tree_id, H3D::Node *n, bool expand = true );

  // Update a current tree. The list of nodes is a list of all the nodes
  // that are supposed to be children to tree_id. All nodes that are already
  // there are updated, new nodes are added and parts that are nod in the list
  // are deleted.
  void updateNodeTree( wxTreeItemId tree_id, 
                       std::list< H3D::Node *>, 
                       bool expand_new = true );

  // The time of the last update to the tree view.
  TimeStamp last_tree_update;

  typedef  std::map< wxTreeItemIdValue, H3D::AutoRef< H3D::Node > > TreeIdMap;
  // A map from tree id to node to be able to lookup what Node a part of the
  // tree view refers to.
  TreeIdMap node_map;

  // The id of the tree node that holds bindable node info.
  wxTreeItemId bindable_tree_id;

  // The time of the last update of the selected node field values.
  TimeStamp last_fields_update;
#ifdef DEFAULT_VALUES
  AutoRef< Node > default_values_node;
#endif
  // The node which values are currently being displayed.
  AutoRef< Node > displayed_node;
};

#endif // __TreeViewerTreeViewDialog__
