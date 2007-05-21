//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004, SenseGraphics AB
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
/// \file ProfilesAndComponents.h
/// \brief Header file for ProfilesAndComponents, classes for navigation
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __PROFILESANDCOMPONENTS_H__
#define __PROFILESANDCOMPONENTS_H__

#include "H3DApi.h"
#include "ProfileSAX2Handlers.h"
#include <string>
#include <vector>
#include <map>

namespace H3D {
  
  /// ProfilesAndComponents is a Singleton class used for taking care of
  /// profile and component conformance in H3DAPI
  class H3DAPI_API ProfilesAndComponents {
  public:

    /// destroy the instance when not needed anymore
    static void destroy();

    //static const ProfilesAndComponents * getInstance();

    static bool setProfile( std::string profile, std::string &err_msg,
                            std::string _version );
    static bool addComponent( std::string component,
                              int level, std::string &err_msg );

    static std::string getProfile();

    static void setMainProfileDone( bool _main_profile_set );

    // set to false before starting an application ( or loading a file )
    // to not do any profile and component checking.
    static bool check_profiles_components;

  private:
    bool setProfileInternal( std::string profile, std::string &err_msg,
                             std::string _version );
    bool addComponentInternal( std::string component,
                               int level, std::string &err_msg );

    bool findComponent( ProfileSAX2Handlers::myX3DComponentVector &component_vector,
                    string name, int &place );

    /// Temporary solution to know which profiles H3DAPI supports.
    std::vector< std::string > profiles_supported;
    //std::string used_profile;
    ProfileSAX2Handlers::myX3DProfile used_profile;
    std::string version;
    bool main_profile_set;
    map< std::string, std::map< std::string, int > > components_supported;
    //std::map< std::string, int > components_used;
    ProfileSAX2Handlers::myX3DComponentVector components_used;

    static ProfilesAndComponents * instance;

    /// Private declarations of constructors for singleton class.
    ProfilesAndComponents();
    ProfilesAndComponents(const ProfilesAndComponents&) {}
    ProfilesAndComponents& operator =
      (const ProfilesAndComponents&) { return *this; }
    ~ProfilesAndComponents() {}
  };

}

#endif
