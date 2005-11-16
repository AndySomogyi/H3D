from H3DInterface import *


refs = references.getValue()
cubemap = refs[0]
glass = refs[1]
polka = refs[2]

cubemap.transparency.setValue( 1 )
glass.transparency.setValue( 1 )
polka.transparency.setValue( 1 )

tick=0

# The SumTorques class just sums all SFVec3f fields that are routed to it.
class Timer( AutoUpdate( SFTime ) ):
  def update( self, event ):
    global tick, cubemap, glass, polka
    t = event.getValue()

    tick = tick + 0.01
    if tick > 20:
      tick = 3.0

    if tick >= 4 and tick <= 5:
      ofs = tick-4.0
      polka.transparency.setValue( 1-ofs )
      glass.transparency.setValue( 1 )
      cubemap.transparency.setValue( 1 )
    if tick >= 8 and tick <= 9:
      ofs = tick-8.0
      polka.transparency.setValue( ofs )
      glass.transparency.setValue( 1-ofs )
      cubemap.transparency.setValue( 1 )
    if tick >= 13 and tick <= 14:
      ofs = tick-13.0
      polka.transparency.setValue( 1 )
      glass.transparency.setValue( ofs )
      cubemap.transparency.setValue( 1-ofs )
    if tick >= 18 and tick <= 19:
      ofs = tick-18.0
      polka.transparency.setValue( 1 )
      glass.transparency.setValue( 1 )
      cubemap.transparency.setValue( ofs )
      
    return t

mytimer=Timer()
time.route( mytimer )

