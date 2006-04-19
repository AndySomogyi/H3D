from H3DInterface import *

depth = SFInt32()
depth.setValue(0)

keysensor = createX3DNodeFromString( "<KeySensor />" )[0]

class NormalKeyHandler( AutoUpdate( SFString ) ):
  def update( self, event ):
    key = event.getValue()
    if( key =="a" ):
      depth.setValue( depth.getValue() + 1 )
    elif( key == "s" ):
      depth.setValue( depth.getValue() - 1 )
    return key

normalKeyHandler = NormalKeyHandler()
keysensor.keyPress.route( normalKeyHandler )
