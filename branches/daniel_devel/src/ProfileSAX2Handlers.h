//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2007, SenseGraphics AB
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
/// \file ProfileSAX2Handlers.h
/// This file implements the SAX2 ContentHandler and ErrorHandler interfaces
/// for parsing of XML X3D files and building a H3D scene graph from it.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __PROFILEX3DSAX2HANDLERS_H__
#define __PROFILEX3DSAX2HANDLERS_H__


#include <stack>
#include <list>
#include <map>
#include <vector>
#include <string>

#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax/Locator.hpp>

#if defined(_MSC_VER) || defined(__BORLANDC__)
#pragma comment( lib, "xerces-c_2.lib" )
#endif

XERCES_CPP_NAMESPACE_USE
using namespace std;

    class ProfileSAX2Handlers : public DefaultHandler {
    public:
      ///  Constructor.
      ProfileSAX2Handlers(): 
        locator( NULL ) {
        current_level = 0;
        in_node = false;
        in_prerequisites = false;
        in_profile = false;
      };

      /// Destructor.
      ~ProfileSAX2Handlers() {
      };

        
      void setDocumentLocator( const Locator *const _locator ) {
        locator = (Locator *)_locator;
      }
      

      /// Handler for the SAX ContentHandler interface.
      /// Resets the node member to NULL
      virtual void startDocument () {
      }

      /// Handler for the SAX ContentHandler interface.
      /// Builds the H3D scene graph.
      /// \param uri the URI of the associated namespace for this element
      /// \param localname the local part of the element name
      /// \param qname the QName of this element
      /// \param attrs the specified or defaulted attributes.
      /// \exception X3D::XMLParseError
      /// 
      void startElement(const XMLCh* const uri, 
                        const XMLCh* const localname, 
                        const XMLCh* const qname, 
                        const Attributes& attrs);
        
      /// Handler for the SAX ContentHandler interface.
      /// Builds the H3D scene graph.
      /// \param uri the URI of the associated namespace for this element
      /// \param localname the local part of the element name
      /// \param qname the QName of this element
      /// \exception X3D::XMLParseError
      /// 
      void endElement (const XMLCh *const uri,
                       const XMLCh *const localname,
                       const XMLCh *const qname);
      
      /// Handler for the SAX ErrorHandler interface.
      /// Prints a warning message to the Console.
      ///
      void warning(const SAXParseException& exc);

      /// Handler for the SAX ErrorHandler interface.
      /// Throws an X3D::XMLParseError exception with the error information.
      /// \exception X3D::XMLParseError
      void error(const SAXParseException& exc);

      /// Handler for the SAX ErrorHandler interface.
      /// Throws an XMLParseError exception with the error information.
      /// \exception X3D::XMLParseError
      void fatalError(const SAXParseException& exc);

      struct myX3DNode {
        map< int, vector< string > > fields_per_level;
        string name;
      };

      typedef vector< myX3DNode > myX3DNodeVector;

      struct myX3DComponent {
        myX3DComponent() {
          conformance_level = 0;
        }
        map< int, myX3DNodeVector > nodes_per_level;
        string name;
        map< int, vector< pair< string, int > > > prerequisites;
        int conformance_level;
        int used_level;
        int max_level;
      };

      typedef vector< myX3DComponent > myX3DComponentVector;

      struct myX3DProfile {
        string name;
        map< string, int > component_names;
      };

      typedef vector< myX3DProfile > myX3DProfileVector;

      myX3DComponentVector &getComponentVector() {
        return component_vector;
      }

      myX3DProfileVector &getProfileVector() {
        return profile_vector;
      }

    protected:

      vector< myX3DComponent > component_vector;
      vector< myX3DProfile > profile_vector;

      int current_level;
      bool in_node;
      bool in_prerequisites;
      bool in_profile;

      /// Returns a string with the current location as given
      /// by the current Locator object. system id and line number
      /// is included. Only valid during event callbacks.
      string getLocationString();

      /// The locator object of the parser. Can be used to retrieve 
      /// the location of any document handler event in the XML source 
      /// document.
      Locator *locator;
        
    };
#endif
