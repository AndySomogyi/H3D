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
/// \file H3DViewerPluginsDialog.h
/// \brief Header file for H3DViewerPluginsDialog.
/// Subclass of PluginsDialog, which is generated by wxFormBuilder.
///
//
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __H3DViewerPluginsDialog__
#define __H3DViewerPluginsDialog__

#include "H3DViewer.h"

/** Implementing PluginsDialog */
class H3DViewerPluginsDialog : public PluginsDialog
{
protected:
  // Handlers for PluginsDialog events.
  void OnInstalledPluginSelected( wxCommandEvent& event );
  void OnAddPluginButton( wxCommandEvent& event );
  void OnRemovePluginButton( wxCommandEvent& event );
  void OnDisablePluginCheckbox( wxCommandEvent& event );
  void OnURLEvent( wxTextUrlEvent& event );
public:
  /** Constructor */
  H3DViewerPluginsDialog( wxWindow* parent );

  bool addPlugin( const wxString &path, bool force_overwrite = false );
  bool removePlugin( const wxString &plugin_name );
};

#endif // __H3DViewerPluginsDialog__
