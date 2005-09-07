//////////////////////////////////////////////////////////////////////////////
//    H3D API.   Copyright 2004, Danel Evestedt, Mark Dixon
//    All Rights Reserved
//
//
//
//////////////////////////////////////////////////////////////////////////////
#include "X3D.h"
#include "X3DSAX2Handlers.h"
#include "IStreamInputSource.h"
#include <sstream>

using namespace H3D;

SAX2XMLReader* X3D::getNewXMLParser() {
  SAX2XMLReader::ValSchemes    valScheme    = SAX2XMLReader::Val_Auto;
  bool                         doNamespaces = true;
  bool                         doSchema = true;
  bool                         schemaFullChecking = false;
  //bool                         doList = false;
  //bool                         errorOccurred = false;
  bool                         namespacePrefixes = false;
  //bool                         recognizeNEL = false;
  char                         localeStr[64];
  memset(localeStr, 0, sizeof localeStr);
  SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
  parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, doNamespaces);
  parser->setFeature(XMLUni::fgXercesSchema, doSchema);
  parser->setFeature(XMLUni::fgXercesSchemaFullChecking, 
                     schemaFullChecking);
  parser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, 
                     namespacePrefixes);
  if (valScheme == SAX2XMLReader::Val_Auto) {
    parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
    parser->setFeature(XMLUni::fgXercesDynamic, true);
  }
  if (valScheme == SAX2XMLReader::Val_Never) {
    parser->setFeature(XMLUni::fgSAX2CoreValidation, false);
  }
  if (valScheme == SAX2XMLReader::Val_Always) {
    parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
    parser->setFeature(XMLUni::fgXercesDynamic, false);
  }
  return parser;
}

Group* X3D::createX3DFromString( const string &str,
                                 DEFNodes *dn ) {
  Group *g = new Group;
  AutoRef< Node > n = createX3DNodeFromString( str, dn );
  if( n.get() )
    g->children->push_back( n.get() );
  return g;
}

Group* X3D::createX3DFromURL( const string &url,
                              DEFNodes *dn ) {
  Group *g = new Group;
  AutoRef< Node > n = createX3DNodeFromURL( url, dn );
  if( n.get() )
    g->children->push_back( n.get() );
  return g;
}

Group* X3D::createX3DFromStream( istream &is, 
                                 DEFNodes *dn,
                                 const XMLCh *const system_id ) {
  Group *g = new Group;
  AutoRef< Node > n = createX3DNodeFromStream( is, dn, system_id );
  if( n.get() )
    g->children->push_back( n.get() );
  return g;
}


AutoRef< Node > X3D::createX3DNodeFromString( const string &str,
                                              DEFNodes *dn ) {
  auto_ptr< SAX2XMLReader > parser( getNewXMLParser() );
  X3DSAX2Handlers handler( dn );
  stringstream s;
  s << str << ends;
  parser->setContentHandler(&handler);
  parser->setErrorHandler(&handler); 
  parser->parse( IStreamInputSource( s, (const XMLCh*)L"<string input>" ) );
  return handler.getResultingNode();
}

AutoRef< Node > X3D::createX3DNodeFromURL( const string &url,
                                           DEFNodes *dn ) {
  auto_ptr< SAX2XMLReader > parser( getNewXMLParser() );
  X3DSAX2Handlers handler( dn );
  parser->setContentHandler(&handler);
  parser->setErrorHandler(&handler); 
  parser->parse(url.c_str());
  return handler.getResultingNode();
}

AutoRef< Node > X3D::createX3DNodeFromStream( istream &is, 
                                              DEFNodes *dn,
                                              const XMLCh *const system_id ) {
  auto_ptr< SAX2XMLReader > parser( getNewXMLParser() );
  X3DSAX2Handlers handler( dn );
  parser->setContentHandler(&handler);
  parser->setErrorHandler(&handler); 
  parser->parse( IStreamInputSource( is, system_id ) );
  return handler.getResultingNode();
}

