#include "INIFile.h"
#include <fstream>

using namespace std;
using namespace H3D;

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


//  AutoRef needs to be declared global
/*AutoRef< Scene > scene( new Scene );
AutoRef< KeySensor > ks( new KeySensor );
AutoRef< MouseSensor > ms( new MouseSensor );
#ifndef MACOSX
  AutoRef< SpaceWareSensor > ss;
#endif
AutoRef< Transform > t( new Transform );
AutoRef< Node > device_info;
AutoRef< Node > viewpoint;
#ifdef USE_HAPTICS
AutoRef< Node > default_stylus;
#endif
AutoRef< Group > g( new Group );*/