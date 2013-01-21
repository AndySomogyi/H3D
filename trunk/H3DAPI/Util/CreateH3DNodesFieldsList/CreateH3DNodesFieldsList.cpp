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
//
/// \file NodeRoutesToDotFile.cpp
/// \brief CPP file that goes through the node database and generates
/// dot files that can be used with doxygen documentation.
///
//
//
//////////////////////////////////////////////////////////////////////////////
#include <H3D/H3DNodeDatabase.h>
#include <H3D/Scene.h>
#include <H3DUtil/Exception.h>
#include <H3D/Node.h>
#include <set>
#include <queue>
#include <fstream>

using namespace std;
using namespace H3D;

// The nodes and field names in this list are ignored when generating the list of nodes and fields.
vector< pair< string, string > > deprecated_names_list;

// The nodes and field names in this list are for those where the name of the field is not the name of the variable containing the field.
// for example Text::stringF which contains the field string.
vector< pair< string, pair< string, string > > > field_name_not_variable_name;


struct less_than_field_name {
  bool operator()(const pair< string, string > &s1, const pair< string, string > &s2 ) const {
        return s1.first < s2.first;
      }	
};

void writeNode( ostream &os, Node *n ) {
  if( !n ) {
    return;
  }

  H3DNodeDatabase *db = H3DNodeDatabase::lookupTypeId( typeid(*n) );

  string node_name = n->getTypeName();
  os << "/// <ul>" << endl
     << "///   <li>\\link H3D::" << node_name << " "
     << node_name << " \\endlink" << endl;

  if( db->fieldDBSize() > 0 ) {
    vector< pair< string, string > > ordered_field_names;
    for( H3DNodeDatabase::FieldDBConstIterator i = db->fieldDBBegin();
         i != db->fieldDBEnd(); i++ ) {
			string tmp_field_name = i.getFieldDBElement()->getName();
			bool valid_name = true;
			for( unsigned int j = 0; j < deprecated_names_list.size(); j++ )
			  if( deprecated_names_list[j].first == node_name &&
				    deprecated_names_list[j].second == tmp_field_name )
						valid_name = false;
			if( valid_name ) {
			  bool field_name_is_variable_name = true;
				for( unsigned int j = 0; j < field_name_not_variable_name.size(); j++ )
				  if( field_name_not_variable_name[j].first == node_name &&
					    field_name_not_variable_name[j].second.first == tmp_field_name ) {
						ordered_field_names.push_back( pair< string, string >( tmp_field_name, field_name_not_variable_name[j].second.second ) );
						field_name_is_variable_name = false;
						break;
					}
				if( field_name_is_variable_name )
          ordered_field_names.push_back( pair< string, string >( tmp_field_name, i.getFieldDBElement()->getField(n)->getName() ) );
			}
    }
    sort( ordered_field_names.begin(), ordered_field_names.end() );
    os << "///     <ul>" << endl;

    for( vector< pair< string, string > >::iterator i = ordered_field_names.begin();
         i != ordered_field_names.end(); i++ ) {
      os << "///       <li>\\link H3D::" << node_name << "::"
         << (*i).second << " " << (*i).first << " \\endlink" << "</li>" << endl;
    }
    os << "///     </ul>" << endl;
  }
  os << "///   </li>" << endl
     << "/// </ul>" << endl;
  delete n;
}

#ifdef HAVE_UI
#include <H3D/UI/Frame.h>
#endif

#ifdef HAVE_RigidBodyPhysics
#include <H3D/RigidBodyPhysics/SliderJoint.h>
#endif

inline void resetSceneTimeField() {
  Scene::time.reset( new SFTime( TimeStamp() ) );
  Scene::time->setName( "Scene::time" );
}


int main(int argc, char* argv[]) {

  if (argc < 2){
    cerr << "Usage: " << argv[0] << " <Output file>" << endl;
    return 1;
  }
#ifdef HAVE_UI
  {
    // To make sure that UI is linked in if used.
    auto_ptr<Frame> kurt( new Frame );
  }
#endif

#ifdef HAVE_RigidBodyPhysics
  {
    // To make sure that UI is linked in if used.
    auto_ptr<SliderJoint> kurt( new SliderJoint );
  }
#endif

	deprecated_names_list.push_back( pair< string, string >( "ImportLibrary", "library" ) );

	field_name_not_variable_name.push_back( pair< string, pair< string, string > >( "Text", pair< string, string >( "string", "stringF" ) ) );

  string out_file = argv[1];

  ofstream os( out_file.c_str() );
  if( !os.is_open() )
    return 0;

  os << "/// \\file " << out_file << endl
     << "/// \\brief Extra page listing all nodes and fields." << endl
     << "/// \\page NodeFieldList List of nodes and their fields." << endl;


  try {
    vector< string > ordered_node_names;
    for( H3DNodeDatabase::NodeDatabaseConstIterator i = 
           H3DNodeDatabase::begin();
         i != H3DNodeDatabase::end();
         i++ ) {
      ordered_node_names.push_back( (*i).second->getName() );
    }

    sort( ordered_node_names.begin(), ordered_node_names.end() );

    for( vector< string >::iterator i = ordered_node_names.begin();
         i != ordered_node_names.end(); i++ ) {
      Node *n = H3DNodeDatabase::createNode( (*i) );
      if( !n ) {
        cerr << (*i) << ": No such node exists in the node database"
             << endl;
      } else {
        writeNode( os, n );
        resetSceneTimeField();
      }
    }
    cerr << "DONE!" << endl;
    os.close();
  } catch (const Exception::H3DException &e) {
      os.close();
      cerr << e << endl;
  }
  return 0;
}

