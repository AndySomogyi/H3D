///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "H3DViewer.h"

///////////////////////////////////////////////////////////////////////////

TreeViewDialog::TreeViewDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_menubar1 = new wxMenuBar( 0 );
	m_menubar1->Enable( false );
	m_menubar1->Hide();
	
	RightClickMenu = new wxMenu();
	wxMenuItem* TreeViewCollapseAll;
	TreeViewCollapseAll = new wxMenuItem( RightClickMenu, wxID_ANY, wxString( wxT("Collapse all") ) , wxEmptyString, wxITEM_NORMAL );
	RightClickMenu->Append( TreeViewCollapseAll );
	
	wxMenuItem* TreeViewExpandAll;
	TreeViewExpandAll = new wxMenuItem( RightClickMenu, wxID_ANY, wxString( wxT("Expand all") ) , wxEmptyString, wxITEM_NORMAL );
	RightClickMenu->Append( TreeViewExpandAll );
	
	wxMenuItem* TreeViewCollapseChildren;
	TreeViewCollapseChildren = new wxMenuItem( RightClickMenu, wxID_ANY, wxString( wxT("Collapse children") ) , wxEmptyString, wxITEM_NORMAL );
	RightClickMenu->Append( TreeViewCollapseChildren );
	
	RightClickMenu->AppendSeparator();
	
	wxMenuItem* TreeViewNodeWatch;
	TreeViewNodeWatch = new wxMenuItem( RightClickMenu, wxID_ANY, wxString( wxT("Add node field watch") ) , wxEmptyString, wxITEM_NORMAL );
	RightClickMenu->Append( TreeViewNodeWatch );
	
	wxMenuItem* TreeViewSaveX3D;
	TreeViewSaveX3D = new wxMenuItem( RightClickMenu, wxID_ANY, wxString( wxT("Save node snapshot as X3D..") ) , wxEmptyString, wxITEM_NORMAL );
	RightClickMenu->Append( TreeViewSaveX3D );
	
	m_menubar1->Append( RightClickMenu, wxT("Action") );
	
	RightClickMenuGeometry = new wxMenu();
	wxMenuItem* TreeViewCollapseAll1;
	TreeViewCollapseAll1 = new wxMenuItem( RightClickMenuGeometry, wxID_ANY, wxString( wxT("Collapse all") ) , wxEmptyString, wxITEM_NORMAL );
	RightClickMenuGeometry->Append( TreeViewCollapseAll1 );
	
	wxMenuItem* TreeViewExpandAll1;
	TreeViewExpandAll1 = new wxMenuItem( RightClickMenuGeometry, wxID_ANY, wxString( wxT("Expand all") ) , wxEmptyString, wxITEM_NORMAL );
	RightClickMenuGeometry->Append( TreeViewExpandAll1 );
	
	wxMenuItem* TreeViewCollapseChildren1;
	TreeViewCollapseChildren1 = new wxMenuItem( RightClickMenuGeometry, wxID_ANY, wxString( wxT("Collapse children") ) , wxEmptyString, wxITEM_NORMAL );
	RightClickMenuGeometry->Append( TreeViewCollapseChildren1 );
	
	RightClickMenuGeometry->AppendSeparator();
	
	wxMenuItem* TreeViewNodeWatch1;
	TreeViewNodeWatch1 = new wxMenuItem( RightClickMenuGeometry, wxID_ANY, wxString( wxT("Add node field watch") ) , wxEmptyString, wxITEM_NORMAL );
	RightClickMenuGeometry->Append( TreeViewNodeWatch1 );
	
	wxMenuItem* TreeViewSaveX3D1;
	TreeViewSaveX3D1 = new wxMenuItem( RightClickMenuGeometry, wxID_ANY, wxString( wxT("Save node snapshot as X3D..") ) , wxEmptyString, wxITEM_NORMAL );
	RightClickMenuGeometry->Append( TreeViewSaveX3D1 );
	
	wxMenuItem* TreeViewSaveSTL;
	TreeViewSaveSTL = new wxMenuItem( RightClickMenuGeometry, wxID_ANY, wxString( wxT("Save geometry triangles as STL..") ) , wxEmptyString, wxITEM_NORMAL );
	RightClickMenuGeometry->Append( TreeViewSaveSTL );
	
	m_menubar1->Append( RightClickMenuGeometry, wxT("Action") );
	
	this->SetMenuBar( m_menubar1 );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter1->SetSashGravity( 1 );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( TreeViewDialog::m_splitter1OnIdle ), NULL, this );
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	TreeViewTree = new wxTreeCtrl( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	bSizer3->Add( TreeViewTree, 1, wxALL|wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	FieldValuesGrid = new wxGrid( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	FieldValuesGrid->CreateGrid( 2, 2 );
	FieldValuesGrid->EnableEditing( true );
	FieldValuesGrid->EnableGridLines( true );
	FieldValuesGrid->EnableDragGridSize( false );
	FieldValuesGrid->SetMargins( 0, 0 );
	
	// Columns
	FieldValuesGrid->SetColSize( 0, 114 );
	FieldValuesGrid->SetColSize( 1, 102 );
	FieldValuesGrid->EnableDragColMove( false );
	FieldValuesGrid->EnableDragColSize( true );
	FieldValuesGrid->SetColLabelSize( 15 );
	FieldValuesGrid->SetColLabelValue( 0, wxT("Field") );
	FieldValuesGrid->SetColLabelValue( 1, wxT("Value") );
	FieldValuesGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	FieldValuesGrid->EnableDragRowSize( true );
	FieldValuesGrid->SetRowLabelSize( 0 );
	FieldValuesGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	FieldValuesGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer4->Add( FieldValuesGrid, 1, wxALL|wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer4 );
	m_panel2->Layout();
	bSizer4->Fit( m_panel2 );
	m_splitter1->SplitVertically( m_panel1, m_panel2, 346 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( TreeViewDialog::OnClose ) );
	this->Connect( wxEVT_IDLE, wxIdleEventHandler( TreeViewDialog::OnIdle ) );
	this->Connect( TreeViewCollapseAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewCollapseAll ) );
	this->Connect( TreeViewExpandAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewExpandAll ) );
	this->Connect( TreeViewCollapseChildren->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewCollapseChildren ) );
	this->Connect( TreeViewNodeWatch->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewNodeWatch ) );
	this->Connect( TreeViewSaveX3D->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewSaveX3D ) );
	this->Connect( TreeViewCollapseAll1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewCollapseAll ) );
	this->Connect( TreeViewExpandAll1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewExpandAll ) );
	this->Connect( TreeViewCollapseChildren1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewCollapseChildren ) );
	this->Connect( TreeViewNodeWatch1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewNodeWatch ) );
	this->Connect( TreeViewSaveX3D1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewSaveX3D ) );
	this->Connect( TreeViewSaveSTL->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewSaveSTL ) );
	TreeViewTree->Connect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( TreeViewDialog::OnTreeRightClick ), NULL, this );
	TreeViewTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( TreeViewDialog::OnNodeSelected ), NULL, this );
	FieldValuesGrid->Connect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( TreeViewDialog::OnCellEdit ), NULL, this );
}

TreeViewDialog::~TreeViewDialog()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( TreeViewDialog::OnClose ) );
	this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( TreeViewDialog::OnIdle ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewCollapseAll ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewExpandAll ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewCollapseChildren ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewNodeWatch ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewSaveX3D ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewCollapseAll ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewExpandAll ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewCollapseChildren ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewNodeWatch ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewSaveX3D ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TreeViewDialog::OnTreeViewSaveSTL ) );
	TreeViewTree->Disconnect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( TreeViewDialog::OnTreeRightClick ), NULL, this );
	TreeViewTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( TreeViewDialog::OnNodeSelected ), NULL, this );
	FieldValuesGrid->Disconnect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( TreeViewDialog::OnCellEdit ), NULL, this );
}

FieldValuesDialog::FieldValuesDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	FieldValuesGrid = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	FieldValuesGrid->CreateGrid( 2, 2 );
	FieldValuesGrid->EnableEditing( true );
	FieldValuesGrid->EnableGridLines( true );
	FieldValuesGrid->EnableDragGridSize( false );
	FieldValuesGrid->SetMargins( 0, 0 );
	
	// Columns
	FieldValuesGrid->SetColSize( 0, 95 );
	FieldValuesGrid->SetColSize( 1, 102 );
	FieldValuesGrid->EnableDragColMove( false );
	FieldValuesGrid->EnableDragColSize( true );
	FieldValuesGrid->SetColLabelSize( 15 );
	FieldValuesGrid->SetColLabelValue( 0, wxT("Field") );
	FieldValuesGrid->SetColLabelValue( 1, wxT("Value") );
	FieldValuesGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	FieldValuesGrid->EnableDragRowSize( true );
	FieldValuesGrid->SetRowLabelSize( 0 );
	FieldValuesGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	FieldValuesGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer2->Add( FieldValuesGrid, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer2 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_IDLE, wxIdleEventHandler( FieldValuesDialog::OnIdle ) );
	FieldValuesGrid->Connect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( FieldValuesDialog::OnCellEdit ), NULL, this );
}

FieldValuesDialog::~FieldValuesDialog()
{
	// Disconnect Events
	this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( FieldValuesDialog::OnIdle ) );
	FieldValuesGrid->Disconnect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( FieldValuesDialog::OnCellEdit ), NULL, this );
}

PluginsDialog::PluginsDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Installed plugins:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer6->Add( m_staticText1, 0, wxALL, 5 );
	
	InstalledPluginsList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	InstalledPluginsList->SetMinSize( wxSize( 180,200 ) );
	
	bSizer6->Add( InstalledPluginsList, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button2 = new wxButton( this, wxID_ANY, wxT("Add.."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button2, 0, wxALL, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Remove.."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button1, 0, wxALL, 5 );
	
	bSizer6->Add( bSizer8, 1, wxEXPAND, 5 );
	
	
	bSizer6->Add( 0, 15, 0, wxEXPAND, 15 );
	
	DisablePluginsCheckBox = new wxCheckBox( this, wxID_ANY, wxT("Disable plugins"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer6->Add( DisablePluginsCheckBox, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	PluginInfoText = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_AUTO_URL|wxTE_READONLY|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
	PluginInfoText->Hide();
	
	bSizer7->Add( PluginInfoText, 1, wxEXPAND | wxALL, 5 );
	
	bSizer5->Add( bSizer7, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	
	// Connect Events
	InstalledPluginsList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( PluginsDialog::OnInstalledPluginSelected ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PluginsDialog::OnAddPluginButton ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PluginsDialog::OnRemovePluginButton ), NULL, this );
	DisablePluginsCheckBox->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PluginsDialog::OnDisablePluginCheckbox ), NULL, this );
	PluginInfoText->Connect( wxEVT_COMMAND_TEXT_URL, wxTextUrlEventHandler( PluginsDialog::OnURLEvent ), NULL, this );
}

PluginsDialog::~PluginsDialog()
{
	// Disconnect Events
	InstalledPluginsList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( PluginsDialog::OnInstalledPluginSelected ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PluginsDialog::OnAddPluginButton ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PluginsDialog::OnRemovePluginButton ), NULL, this );
	DisablePluginsCheckBox->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PluginsDialog::OnDisablePluginCheckbox ), NULL, this );
	PluginInfoText->Disconnect( wxEVT_COMMAND_TEXT_URL, wxTextUrlEventHandler( PluginsDialog::OnURLEvent ), NULL, this );
}