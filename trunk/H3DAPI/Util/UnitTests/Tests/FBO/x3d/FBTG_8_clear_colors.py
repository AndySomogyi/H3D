
from UnitTestUtil import *
from H3DInterface import *
from H3DUtils import *

"""
  Test script file
  Any number of functions can be defined here. The test functions shouldn't take any parameters.
  
  Decorators are used to specify that a function is a test step and to specify what forms of validation should be used on that step.
  Test steps are executed in the order they are listed in this script file.
  
  The available decorators, and their parameters, are shown below.
  Parameters inside brackets [] are optional.
  In the case of start_time and run_time the highest start_time and the highest run_time specified by any decorator for one test step will be the value that is used.
  so if a function has one decorator that specifies start_time as 2 and another decorator that specifies it as 5 then the start_time that is used is 5. The same applies for run_time.
  
  Decorators:
    @console([start_time])
      Will wait the specified start_time and then execute the function. Any console output when the function is executed will be stored and validated
      If no start_time is specified then it will not wait before calling it and will instead call it immediately after the previous function is finished (or immediately after the test has started)
      Console output will also be stored as <testcase folder>/output/text/<case name>_<step_name>_console.txt
    
    @custom([start_time])
      Behaves the same as @console except instead of storing console output it will store all the strings passed to the function printCustom(string)
      printCustom(string) will only work in a test step function that has the @custom decorator and is only intended to be called by you inside your test steps
      The optional start_time parameter is the same as for @console
      Custom output will also be stored as <testcase folder>/output/text/<case name>_<step_name>_custom.txt
    
    @screenshot([start_time], [run_time])
      Will wait the specified start_time, execute the function, and then wait the specified run_time. After waiting it will take a screenshot that will be validated.
      Screenshot output will also be stored as <testcase folder>/output/renderings/<case name>_<step_name>.png
      
    @performance([start_time], run_time)
      Will wait the specified start_time, execute the function, and then record performance data during the specified run_time.
      Performance data is not in itself validated and as such is not output anywhere except the results database.
      The recorded performance data WILL BE IGNORED unless there is also another decorator that is validated successfully.
      On other words we will not store and display performance data for a test that has not been verified to give the expected output. @screenshot is recommended for this since we typically will only want to record performance data for a test that also rendered correctly.
"""

@screenshot()
def firstStep():
  pass

