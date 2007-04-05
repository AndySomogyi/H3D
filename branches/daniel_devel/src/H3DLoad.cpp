// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <fstream>
#include "X3D.h"
#include <string.h>

#include <iostream>

#include "VrmlParser.h"
#include "X3DSAX2Handlers.h"
#include "GLUTWindow.h"
#include "Group.h"
#include "Transform.h"
#include "Scene.h"
#include "KeySensor.h"
#include "MouseSensor.h"
#ifndef MACOSX
#include "SpaceWareSensor.h"
#endif
#include "DEFNodes.h"
#include "Viewpoint.h"
#include "DeviceInfo.h"
#include "INIFile.h"
#include "ResourceResolver.h"
#include "PythonScript.h"

using namespace std;
using namespace H3D;

H3D_API_EXCEPTION( QuitAPIException );

class QuitAPIField: public AutoUpdate< SFString > {
  virtual void update() {
    string s = static_cast< SFString * >(routes_in[0])->getValue();
    if( s[0] == 27 ) {
      throw QuitAPIException();
    }
  }
};


#define GET4(ENV,GROUP,VAR,DEFAULT) \
( getenv(ENV) ?                     \
  string(getenv(ENV)) :             \
  ( ini_file.hasOption(GROUP,VAR) ? \
    ini_file.get( GROUP, VAR ) :    \
    DEFAULT ) )

#define GET_ENV_INI_DEFAULT(ENV,PATH,GROUP,VAR,DEFAULT) \
( getenv(ENV) ?                                         \
  string(getenv(ENV)) :                                 \
  ( ini_file.hasOption(GROUP,VAR) ?                     \
    PATH + ini_file.get( GROUP, VAR ) :                 \
    DEFAULT ) )

string GET_ENV_INI_DEFAULT_FILE( INIFile &ini_file,
                            const string &ENV,
                            const string &DISPLAY_PATH,
                            const string &COMMON_PATH,
                            const string &GROUP,
                            const string &VAR ) {
  char *env = getenv(ENV.c_str());
  if( env ) return env;
  
  if( ini_file.hasOption(GROUP,VAR) ) { 
    string option = ini_file.get( GROUP, VAR );
    
    ifstream inp( (DISPLAY_PATH + option).c_str() );
    inp.close();
    if(!inp.fail()) return DISPLAY_PATH + option;

    inp.clear();
    inp.open( (COMMON_PATH + option).c_str() );
    inp.close();
    if(!inp.fail()) return COMMON_PATH + option;
  }
  return "";
}

#define GET_INT(GROUP,VAR,DEFAULT)  \
( ini_file.hasOption(GROUP,VAR) ? \
  atoi(ini_file.get( GROUP, VAR ).c_str()) :    \
  DEFAULT )

#define GET_BOOL(GROUP,VAR,DEFAULT)   \
( ini_file.hasOption(GROUP,VAR) ?     \
  ini_file.getBoolean( GROUP, VAR ) : \
  DEFAULT )


int main(int argc, char* argv[]) {
#ifdef H3DAPI_LIB
  initializeH3D();
#endif

#ifdef HAVE_PYTHON
  PythonScript::setargv( argc, argv );
#endif
  // Settings and command line arguments ---
  
  // Help message
  
  string command = argv[0];
  string help_message = "\n";
  help_message += "Usage: " + command + " [Options] <X3D file>\n";
  help_message += "\n";
  help_message += "Options:\n";
  help_message += "\n";
  help_message += " -m --mirror            Flip screen upside down\n";
  help_message += " -M --no-mirror         Surpress flipping screen\n";
  help_message += " -f --fullscreen        Open in fullscreen mode\n";
  help_message += " -F --no-fullscreen     Surpress fullscreen mode\n";
  help_message += "    --screen=<W>x<H>    Set width and height for window\n";
  help_message += "\n";
  help_message += "    --deviceinfo=<file> Use <file> as device info\n";
  help_message += "    --stylus=<file>     Use <file> as stylus model\n";
  help_message += "    --viewpoint=<file>  Use <file> as viewpoint\n";
  help_message += "    --rendermode=<mode> Use render mode <mode>\n";
  help_message += " -s --spacemouse        Use 3DConnexion space mouse to\n";
  help_message += "                        navigate scene.\n";
  help_message += "\n";
  help_message += " -h --help              This help message\n";
  help_message += "\n";
  
  // H3D root, XML file and setup file
  

  vector<string> xml_files;

  char *r = getenv( "H3D_ROOT" );

  string h3d_root = r ? r : ""; 

  INIFile ini_file( h3d_root + "/settings/h3dload.ini" );
  
  // Console settings
  
  bool console_show_time = GET_BOOL("console", "show_time", false);
  bool console_show_level = GET_BOOL("console", "show_level", false);
  int console_output_level = GET_INT("console", "output_level", 3 );

  ostream *console_ostream = 0;
  string ostream_str = GET4("H3D_CONSOLE_OSTREAM",
                            "console","ostream",(string)"cerr");
  if ( ostream_str == "cerr" )
    console_ostream = &cerr;
  else if( ostream_str == "cout" )
    console_ostream = &cout;
  else
    Console(4) << "Invalid valid value \"" <<  ostream_str
               << "\" on ostream variable. Must be cerr or cout."<< endl;
  
  Console.setShowTime(console_show_time);
  Console.setShowLevel(console_show_level);
  Console.setOutputLevel(console_output_level);
  Console.setOutputStream(*console_ostream);
  
  // Graphics, devices, models and such
  
  string settings_path = 
    GET_ENV_INI_DEFAULT( "H3D_DISPLAY",
                         h3d_root + "/settings/display/",
                         "display","type",
                         h3d_root + "/settings/common/" );
  
  string common_path =  h3d_root + "/settings/common/";

  string deviceinfo_file =
    GET_ENV_INI_DEFAULT_FILE( ini_file, "H3D_DEFAULT_DEVICEINFO",
                              settings_path + "/device/",
                              common_path + "/device/",
                              "haptics device","device" );
  
  string stylus_file =
    GET_ENV_INI_DEFAULT_FILE( ini_file, "H3D_STYLUS",
                              common_path + "/stylus/",
                              common_path + "/stylus/",
                              "haptics device","stylus" );
  
  string viewpoint_file =
    GET_ENV_INI_DEFAULT_FILE( ini_file, "H3D_DEFAULT_VIEWPOINT",
                              settings_path + "/viewpoint/",
                              common_path + "/viewpoint/",
                              "graphical", "viewpoint" );

  string render_mode = GET4( "H3D_RENDERMODE",
                             "graphical", "rendermode",
                             (string)"MONO" );
  
  int width = GET_INT("graphical", "width", 640 );
  int height = GET_INT("graphical", "height", 480 );
  
  bool fullscreen    = GET_BOOL("graphical", "fullscreen", false);
  if( char *buffer = getenv("H3D_FULLSCREEN") ) {
    if (strcmp( buffer, "TRUE" ) == 0 ){
      fullscreen = true; }
    else if (strcmp( buffer, "FALSE" ) == 0 ){
      fullscreen = false; }
    else
      Console(4) << "Invalid valid value \"" << buffer 
                 << "\" on environment "
                 << "variable H3D_FULLSCREEN. Must be TRUE or FALSE. "
                 << endl;
  }
  
  bool mirrored      = GET_BOOL("graphical", "mirrored", false);
  if( char *buffer = getenv("H3D_MIRRORED") ) {
    if (strcmp( buffer, "TRUE" ) == 0 ){
      mirrored = true; }
    else if (strcmp( buffer, "FALSE" ) == 0 ){
      mirrored = false; }
    else
      Console(4) << "Invalid valid value \"" << buffer 
                 << "\" on environment "
                 << "variable H3D_MIRRORED. Must be TRUE or FALSE. "<< endl;
  }
  
  bool use_space_mouse = false;

  // Command line arguments ---
  
  for( int i = 1 ; i < argc ; i++ ){
    
    if( argv[i][0] != '-' ){
      xml_files.push_back( string(argv[i]) );
      continue;
    }
    
    switch(argv[i][1]){
      
      // Long arguments ---
      
    case '-':
      
      if( !strcmp(argv[i]+2,"help") ){
        std::cout << help_message << endl;
        return 0;
      }
      
      else if( !strcmp(argv[i]+2,"mirror") ){
        mirrored = true; }
      
      else if( !strcmp(argv[i]+2,"no-mirror") ){
        mirrored = false; }
      
      else if( !strcmp(argv[i]+2,"fullscreen") ){
        fullscreen = true; }
      
      else if( !strcmp(argv[i]+2,"no-fullscreen") ){
        fullscreen = false; }
      
      else if( !strncmp(argv[i]+2,"screen=",
                        strlen("screen=")) ){
        height = atoi( strstr(argv[i],"x")+1 );
        *strstr(argv[i],"x") = '\0';
        width = atoi( strstr(argv[i],"=")+1 );
      }
      
      else if( !strncmp(argv[i]+2,"deviceinfo=",
                        strlen("deviceinfo=")) ){
        deviceinfo_file = strstr(argv[i],"=")+1; }
      
      else if( !strncmp(argv[i]+2,"stylus=",
                        strlen("stylus=")) ){
        stylus_file = strstr(argv[i],"=")+1; }
      
      else if( !strncmp(argv[i]+2,"viewpoint=",
                        strlen("viewpoint=")) ){
        viewpoint_file = strstr(argv[i],"=")+1; }
      
      else if( !strncmp(argv[i]+2,"rendermode=",
                        strlen("rendermode=")) ){
        render_mode = strstr(argv[i],"=")+1; }

       else if( !strcmp(argv[i]+2,"spacemouse") ){
        use_space_mouse = true; }
      else {
        Console(4) << "Unknown argument "
                   << "'" << argv[i] << "'" << endl; }
      break;
      
      // Short arguments ---
      
    case 'm':
      mirrored = true;
      break;
      
    case 'M':
      mirrored = false;
      break;
      
    case 'f':
      fullscreen = true;
      break;
      
    case 'F':
      fullscreen = false;
      break;
      
    case 's':
      use_space_mouse = true;
      break;

    default:
      Console(4) << "Unknown argument "
                 << "'" << argv[i] << "'" << endl;
    }
  }
  
  if (!xml_files.size()){
    Console(4) << help_message << endl;
    return 1;
  }
  
  
  // Loading X3D file and setting up VR environment ---
  
  try {
    AutoRef< KeySensor > ks( new KeySensor );
    AutoRef< MouseSensor > ms( new MouseSensor );
#ifndef MACOSX
    AutoRef< SpaceWareSensor > ss;
    if( use_space_mouse ) ss.reset( new SpaceWareSensor );
#endif
    X3D::DEFNodes dn;
    QuitAPIField *quit_api = new QuitAPIField;
    AutoRef< Transform > t( new Transform );
    AutoRef< Node > device_info;
    AutoRef< Node > viewpoint;
    AutoRef< Scene > scene( new Scene );
    
    if( deviceinfo_file.size() ){
      try {
        device_info = X3D::createX3DNodeFromURL( deviceinfo_file );
      } catch( const Exception::H3DException &e ) {
        Console(3) << "Warning: Could not create default DeviceInfo node "
                   << "from file \"" << deviceinfo_file << "\": "
                   << e << endl;
      }
    }

    DeviceInfo *di = DeviceInfo::getActive();
    if( di && stylus_file.size() ) {
      AutoRef< Node > default_stylus;
      try {
        default_stylus = X3D::createX3DNodeFromURL( stylus_file );
      } catch( const Exception::H3DException &e ) {
        Console( 4 ) << "Warning: Could not create default stylus "
                     << "from file \"" << stylus_file << "\": "
                     << e << endl;
      }
      
      for( DeviceInfo::MFDevice::const_iterator i = di->device->begin();
           i != di->device->end(); i++ ) {
        H3DHapticsDevice *d = static_cast< H3DHapticsDevice * >(*i);
        if( !d->stylus->getValue() )
          d->stylus->setValue( default_stylus );
      }
    }
    
    for( vector<string>::iterator file = xml_files.begin() ;
         file != xml_files.end() ; file++ ){
      Console(3) << "Loading " << *file << endl;
      if ( file->size() > 4 && 
           file->find( ".wrl", file->size()-5 ) != string::npos )
        t->children->push_back( X3D::createVRMLFromURL( *file, 
                                                        &dn ) );
      else
        t->children->push_back( X3D::createX3DFromURL( *file, 
                                                       &dn ) );
    }
    
    ks->keyPress->route( quit_api );

    AutoRef< Group > g( new Group );
#ifndef MACOSX
    if( use_space_mouse )
      g->children->push_back(ss.get());
#endif    
    // create a Viewpoint if it does not exist.
    if( !X3DViewpointNode::getActive() && viewpoint_file.size() ) {
      try {
        viewpoint = X3D::createX3DNodeFromURL( viewpoint_file );
      } catch( const Exception::H3DException &e ) {
        Console(3) << "Warning: Could not create default Viewpoint node "
                   << "from file \"" << viewpoint_file << "\": "
                   << e << endl;
      }
    }
    
    if( X3DBindableNode::getStack( "DeviceInfo" ).size() > 1 ) {
      device_info.reset( NULL );
    }

    g->children->push_back( t.get() );

    
    // create a window to display
    GLUTWindow *glwindow = new GLUTWindow;

    glwindow->fullscreen->setValue( fullscreen );
    glwindow->mirrored->setValue( mirrored );
    glwindow->renderMode->setValue( render_mode );
    glwindow->width->setValue(width);
    glwindow->height->setValue(height);
    scene->window->push_back( glwindow );
    scene->sceneRoot->setValue( g.get() );
    Scene::mainLoop();
  }
  catch (const QuitAPIException &) {
  }

  catch (const Exception::H3DException &e) {
    Console(4) << e << endl;
#ifdef H3DAPI_LIB
    deinitializeH3D();
#endif
    return 1;
  }

#ifdef H3DAPI_LIB
  deinitializeH3D();
#endif
  return 0;
}

