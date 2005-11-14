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
//
//////////////////////////////////////////////////////////////////////////////

#include "H3DApi.h"
#ifdef HAVE_FREEIMAGE
#include <FreeImage.h>
#endif
#include <xercesc/util/PlatformUtils.hpp>
#include "Exception.h"
#ifdef LINUX
#include <fontconfig/fontconfig.h>
#endif

using namespace H3D;

#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                       ) {
  XERCES_CPP_NAMESPACE_USE
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
#ifdef HAVE_FREEIMAGE
      FreeImage_Initialise();
#endif
      XMLPlatformUtils::Initialize();
      break;
    }
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
#ifdef HAVE_FREEIMAGE
      FreeImage_DeInitialise();
#endif
      XMLPlatformUtils::Terminate();
      break;
    }
  return TRUE;
}
#else 
#ifdef __cplusplus
extern "C" {
#endif
  void __attribute__((constructor)) initAPI( void ) {
    XERCES_CPP_NAMESPACE_USE
#ifdef HAVE_FREEIMAGE
    FreeImage_Initialise();
#endif
#ifdef LINUX
    FcInit();
#endif 
    XMLPlatformUtils::Initialize();
  }
  void __attribute__((destructor)) finiAPI( void ) {
    XERCES_CPP_NAMESPACE_USE
#ifdef HAVE_FREEIMAGE
    FreeImage_DeInitialise();
#endif
    XMLPlatformUtils::Terminate();
  }
#ifdef __cplusplus
}
#endif

#endif

