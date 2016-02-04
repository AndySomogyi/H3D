from UnitTestUtil import *
from H3DInterface import *
from H3DUtils import *

"""
An example test script
Any number of functions can be defined here. The test functions shouldn't take any parameters.
If the test is a screenshot test then a screenshot will be taken immediately after the x3d file is done loading.
H3D will by default run for a second between every call to one your specified test functions but you can increase that time by passing a parameter to the decorators
"""

@screenshot(10)
def after10():
  pass

#@screenshot(10)
#def after20():
#  pass