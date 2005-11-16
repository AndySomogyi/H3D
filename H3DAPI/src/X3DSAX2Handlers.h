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
/// \file X3DSAX2Handlers.h
/// This file implements the SAX2 ContentHandler and ErrorHandler interfaces
/// for parsing of XML X3D files and building a H3D scene graph from it.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __X3DSAX2HANDLERS_H__
#define __X3DSAX2HANDLERS_H__

#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax/Locator.hpp>
#include <stack>
#include "Field.h"
#include "Node.h"
#include "Exception.h"
#include "DEFNodes.h"
#include "AutoRef.h"


XERCES_CPP_NAMESPACE_USE
namespace H3D {
  namespace X3D {
    /// 
    /// \class XMLParseError
    /// An exception for errors occuring while parsing an X3D XML file.  
    /// 
    H3D_API_EXCEPTION( XMLParseError );

    /// This class implements the SAX2 ContentHandler and ErrorHandler 
    /// interface in order to build an H3D scenegraph from a X3D XML file.
    ///
    class X3DSAX2Handlers : public DefaultHandler {
    public:
      ///  Constructor.
      ///  \param dn A structure to store the mapping between DEF names
      ///  and the nodes the reference
      X3DSAX2Handlers( DEFNodes *dn = NULL ): 
        error_on_elements( false ),
        delete_DEF_map( dn == NULL ),
        DEF_map( dn ),
        locator( NULL ){
        if( !DEF_map ) {
          DEF_map = new DEFNodes();
        } 
      };

      /// Destructor.
      ~X3DSAX2Handlers() {
        if( delete_DEF_map ) 
          delete DEF_map;
      };

        
      void setDocumentLocator( const Locator *const _locator ) {
        locator = (Locator *)_locator;
      }
      
      /// Get the node that has been generated by the last parsing. Only
      /// valid after a successful call to the parse() function of a
      /// SAX2 Parser with class as a ContentHandler.
      inline const AutoRef<Node> &getResultingNode() { 
        return resulting_node;
      }

      /// Handler for the SAX ContentHandler interface.
      /// Resets the node member to NULL
      virtual void startDocument () { 
        resulting_node.reset( NULL );
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
      /// Prints a warning message to cerr.
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
      
    protected:
      /// Encapsulate a node with its XML containerField attribute.
      /// References the node that it encapsulates.
      class NodeElement {
      public:
        /// Constructor.
        NodeElement( Node *_node,
                     const string &_container_field = "" ):
          container_field( _container_field ),
          node( _node ) {
          if( node ) node->ref();
        }

        /// Copy constructor.
        NodeElement( const NodeElement& ne ) {
          node = ne.node;
          container_field = ne.container_field;
          if( node ) node->ref();
        }

        /// Destructor.
        ~NodeElement() {
          if( node ) node->unref();
        }
            
        /// Set the Node value.
        void setNode( Node *n ) {
          if( n )
            n->ref();
          if( node ) 
            node->unref();
          node = n;
        }
            
        /// Get the Node value.
        Node *getNode() { 
          return node;
        }

        /// Set the container field value.
        void setContainerField( const string &s ) {
          container_field = s;
        }
            
        /// Get the container field value. 
        const string &getContainerField() { 
          return container_field;
        }
      private:
        string container_field;
        Node *node;
            
      };
        
      /// Returns a string with the current location as given
      /// by the current Locator object. system id and line number
      /// is included. Only valid during event callbacks.
      string getLocationString();

      /// A stack of NodeElements. Used during parsing to keep track
      /// of the created Nodes.
      typedef std::stack< NodeElement > NodeElementStack;

      /// A stack of nodes that have been created while traversing the XML 
      /// structure
      NodeElementStack node_stack;
        
      /// If TRUE a call to startElement() will raise an XMLParseError 
      bool error_on_elements;
        
      /// If true the DEF_map member will be deleted when the X3DSAX2handlers
      /// object is destructed
      bool delete_DEF_map;
        
      /// A structure containing a map between DEF names and nodes that
      /// are found during parsing.
      DEFNodes *DEF_map;

      /// The Node that is created during parsing
      AutoRef<Node> resulting_node;

      /// The locator object of the parser. Can be used to retrieve 
      /// the location of any document handler event in the XML source 
      /// document.
      Locator *locator;
        
    };
  } 
}
#endif