//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2013, SenseGraphics AB
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

#include <stack>
#include <list>
#include <H3D/Field.h>
#include <H3D/Node.h>
#include <H3D/DEFNodes.h>
#include <H3D/ProtoDeclaration.h>
#include <H3D/X3D.h>
#include <H3D/H3DScriptNode.h>

#include <H3DUtil/Exception.h>
#include <H3DUtil/AutoRef.h>
#include <H3DUtil/AutoPtrVector.h>

#ifdef HAVE_XERCES

#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax/Locator.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>

XERCES_CPP_NAMESPACE_USE

#endif


namespace H3D {
  namespace X3D {
    /// 
    /// \class XMLParseError
    /// An exception for errors occuring while parsing an X3D XML file.  
    /// 
    H3D_API_EXCEPTION( XMLParseError );


#ifdef HAVE_XERCES
    /// This class implements the SAX2 ContentHandler and ErrorHandler 
    /// interface in order to build an H3D scenegraph from a X3D XML file.
    ///
    class X3DSAX2Handlers : public DefaultHandler {
    public:
      ///  Constructor.
      ///  \param dn A structure to store the mapping between DEF names
      ///  and the nodes the reference
      /// \param _exported_nodes A structure to store the exported nodes.
      /// \param _proto_declarations Storage of all proto declarations.
			/// \param _called_from_proto_declaration True if instance is created
			/// by ProtoDeclaration class.
      X3DSAX2Handlers( DEFNodes *dn = NULL,
                       DEFNodes *_exported_nodes = NULL,
                       PrototypeVector *_proto_declarations = NULL,
											 bool _called_from_proto_declaration = false ): 
        proto_instance( NULL ),
        proto_declaration( NULL ),
        proto_body_count( 0 ),
        defining_proto_body( 0 ),
        defining_proto_interface( false ),
        defining_proto_connections( false ),
        defining_extern_proto( false ),
        delete_DEF_map( dn == NULL ),
        delete_exported_map( _exported_nodes == NULL ),
        delete_proto_declarations( _proto_declarations == NULL ),
        DEF_map( dn ),
        exported_nodes( _exported_nodes ),
        proto_declarations( _proto_declarations ),
				called_from_proto_declaration( _called_from_proto_declaration ),
        locator( NULL ),
        profile_set( false ),
        meta_set( false ),
        inside_head(false),
        inside_cdata( false ) {
        
        if( !DEF_map ) {
          DEF_map = new DEFNodes();
        }
        if( !_exported_nodes ) {
          exported_nodes = new DEFNodes();
        }
        if( !_proto_declarations ) {
          proto_declarations = new PrototypeVector;
        }
      };

      /// Destructor.
      ~X3DSAX2Handlers() {
        if( delete_DEF_map ) 
          delete DEF_map;
        if( delete_exported_map )
          delete exported_nodes;
        if( delete_proto_declarations )
          delete proto_declarations;
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
      /// Is called at end of parsing and updates all script nodes
      /// named nodes.
      virtual void endDocument();

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
      
      /// Handler for the SAX LexicalHandler interface.
      //void comment( const XMLCh *const chars, 
      //const XMLSize_t length );

      /// Handler for the SAX LexicalHandler interface.
      /// Called on start of CDATA.
      void startCDATA();

      /// Handler for the SAX LexicalHandler interface.
      /// Called on start of CDATA.
      void endCDATA();
  
      /// Handler for the SAX DocumentHandler interface.
      /// Receive notification of character data inside an element.
      void characters( const XMLCh *const chars, 
                       const XMLSize_t length );

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
      
      /// If this is set to non-NULL then the parser is used to create an 
      /// X3DPrototypeInstance. This allows the IS and connect elements to be used.
      X3DPrototypeInstance *proto_instance;

    protected:
      
      /// This function will be called by startElement if the element is a
      /// ProtoDeclare element or a child of it. It replaces the normal 
      /// functionallity of startElement and creates a ProtoDeclaration instead.
      void protoStartElement( const XMLCh* const uri,
                              const XMLCh* const localname, 
                              const XMLCh* const qname,
                              const Attributes& attrs );
      
      /// This function will be called by endElement if the element is a
      /// ProtoDeclare element or a child of it. It replaces the normal 
      /// functionallity of startElement 
      void protoEndElement( const XMLCh* const uri,
                            const XMLCh* const localname, 
                            const XMLCh* const qname );

      /// This function is called when a field element has been found
      /// in a ProtoInterface element. The attrs argument are the attributes
      /// of the field element.
      void handleProtoInterfaceFieldElement( const Attributes &attrs );
      
      /// This function will be called if a connect element has been found
      /// in an IS element. The attrs argument are the attributes of the 
      /// ProtoInterface element and the parent argument is the node of the element
      /// that it lies within.
      void handleConnectElement( const Attributes &attrs, Node *parent );

      /// This function will be called if a ROUTE, ROUTE_NO_EVENT or ROUTE_WITH_EVENT element
      /// has been found.
      void handleRouteElement( const Attributes &attrs, 
                               bool route_no_event = false );

      /// This function will be called if a PROGRAM_SETTING element
      /// has been found.
      void handleProgramSettingElement( const Attributes &attrs );

      /// This function will be called if a "field" element has been found
      /// that is not part of a ProtoInterface element. If successful it returns
      /// a new Field with the specified attributes, if not NULL is returned.
      Field * handleFieldElement( const Attributes &attrs, Node *parent );

      /// This function will be called if a IMPORT element has been found.
      void handleImportElement( const Attributes &attrs  );
      
      /// This function will be called if a EXPORT element has been found.
      void handleExportElement( const Attributes &attrs  );

      /// This function will be called if a ProtoInstance element has been found. It
      /// returns an X3DPrototypeInstance of the specified prototype if it exists,
      /// otherwise NULL.
      X3DPrototypeInstance* handleProtoInstanceElement( const Attributes &attrs );

      /// This function will be called if a fieldValue element has been found in
      /// a ProtoInstance element.
      void handleFieldValueElement( const Attributes &attrs, Node *parent );

      /// This function will be called if a ExternProtoDeclare element has been found.
      void handleExternProtoDeclareElement( const Attributes &attrs );

      /// Encapsulate a node with its XML containerField attribute.
      /// References the node that it encapsulates.
      class NodeElement {
      public:
        /// Constructor.
        NodeElement( Node *_node,
                     const string &_container_field = "" ):
          container_field( _container_field ),
          node( _node ),
					have_connect_element( false ) {
          if( node ) node->ref();
        }

        /// Copy constructor.
        NodeElement( const NodeElement& ne ) {
          node = ne.node;
          container_field = ne.container_field;
          cdata = ne.cdata;
          if( node ) node->ref();
					have_connect_element = ne.have_connect_element;
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
            
        /// Set CDATA associated with the node.
        void setCDATA( const string &s ) {
          cdata = s;
        }

        /// Returns true if there is any CDATA associated with the node.
        bool haveCDATA() {
          return !cdata.empty();
        }

        /// Get the CDATA associated with the node.
        const string &getCDATA() {
          return cdata;
        }

        /// Get the container field value. 
        const string &getContainerField() { 
          return container_field;
        }

				/// Returns true if the node have a connect element.
				bool haveConnectElement() {
					return have_connect_element;
				}

				/// Set the have_connect_element variable.
				void setConnectElement( const bool &b ) {
					have_connect_element = b;
				}
      private:
        string container_field;
        Node *node;
        string cdata;
				bool have_connect_element;
      };

      /// Returns a string with the current location as given
      /// by the current Locator object. system id and line number
      /// is included. Only valid during event callbacks.
      string getLocationString();

      /// A stack of NodeElements. Used during parsing to keep track
      /// of the created Nodes.
      typedef std::stack< NodeElement > NodeElementStack;

      /// Will be set when a ProtoDeclare element is found and it is will be used in 
      /// protoStartElement and protoEndElement to define a prototype.
      /// On end of ProtoDeclare it will be added to proto_declarations and
      /// proto_declaration will be set to NULL again;
      AutoRef< ProtoDeclaration > proto_declaration;

			/// If true then this handler is created in proto_declaration so
			/// a ProtoDeclare element is allowed at top level.
			bool called_from_proto_declaration;

      /// The string_body string is used when defining ProtoBody. All elements will then
      /// just be translated to a string to be used by the ProtoDeclaration.
      string proto_body;

      /// Counter used know when a proto body part has finished. Will
      /// be incremented on each start of element end decremented on 
      /// end. This means each time it reaches 0 a body element is 
      /// finished.
      unsigned int proto_body_count;

      /// true if we are inside a ProtoBody element.
      H3DInt32 defining_proto_body;

      /// true if we are inside a ProtoInterface element.
      bool defining_proto_interface;

      /// true if we are inside an IS element.
      bool defining_proto_connections;

      /// true if we are inside an ExternProtoDeclare element.
      bool defining_extern_proto;

      /// A stack of nodes that have been created while traversing the XML 
      /// structure
      NodeElementStack node_stack;
        
      /// If true the DEF_map member will be deleted when the X3DSAX2handlers
      /// object is destructed
      bool delete_DEF_map;

      /// If true the exported_nodes member will be deleted when the X3DSAX2handlers
      /// object is destructed
      bool delete_exported_map;

      /// If true the proto_declarations member will be deleted when the X3DSAX2handlers
      /// object is destructed
      bool delete_proto_declarations;
        
      /// A structure containing a map between DEF names and nodes that
      /// are found during parsing.
      DEFNodes *DEF_map;

      /// A list of DEFNodes that contain the nodes that have been exported
      /// using the EXPORT statement.
      DEFNodes *exported_nodes;

      /// The ProtoDeclarations that have been defined with ProtoDeclare elements.
      PrototypeVector *proto_declarations;

      /// All instances of H3DScriptNode that has been instanced so far during
      /// parsing.
      AutoRefVector< H3DScriptNode > script_nodes;

      /// The Node that is created during parsing
      AutoRef<Node> resulting_node;

      /// The locator object of the parser. Can be used to retrieve 
      /// the location of any document handler event in the XML source 
      /// document.
      Locator *locator;

      /// Set after a profile is found
      /// (either in the X3D tag or in a PROFILE statement)
      bool profile_set;

      /// Set after a META is set
      bool meta_set;
      bool inside_head;

      /// Set to true while parser is parsing characters inside
      /// a CDATA construct.
      bool inside_cdata;

      /// The characters collected inside a CDATA construct. Set
      /// in the characters function when inside_cdata is true.
      /// Reset in endCDATA fucntion.
      string cdata;
    };

    H3DAPI_API SAX2XMLReader* getNewXMLParser();

#endif
  } 
}
#endif

