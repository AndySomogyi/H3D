from H3DInterface import *
from H3DUtils import *

import sys
import os
from Queue import Queue, Empty
from inspect import getmembers, isfunction
import operator
from importlib import import_module
import traceback



timer_callbacks = {}

def scheduleCallback ( timeSource, time, func, data ):
  """ Schedule a delayed callback, using a custom field as the timer.
  """
  
  if not id(timeSource) in timer_callbacks:
    timer = TimerCallback ()
    # Replace time source
    for f in timer.getRoutesIn():
      f.unroute ( timer )
    timeSource.routeNoEvent ( timer )
    timer_callbacks[id(timeSource)] = timer
  else:
    timer = timer_callbacks[id(timeSource)]
  
  # Add out callback
  timer.addCallback ( time, func, data )

class UnitTestHelper :
  def __init__( self, early_shutdown_file, output_file_prefix, output_filename_prefix):
    self.test_funcs = Queue()
    self.screenshot_queue = Queue()
    self.early_shutdown_file = early_shutdown_file
    self.output_file_prefix = output_file_prefix
    self.screenshot_counter = 0    
    self.output_filename_prefix = output_filename_prefix
    self.validation_file = output_file_prefix + "/validation.txt"
    self.last_func = None
    self.customPrintHelper = None
    temp = open(self.validation_file, 'w')
    temp.flush()
    temp.close()


  def addTests(self, funclist):
    for func in funclist:
      self.test_funcs.put(func)

  def doTesting(self):
    if not self.last_func is None:
      try:
        for validation in self.last_func.validation:
          validation['post'](self, validation, self.validation_file)
      except Exception as e:
        print str(e)
   
    try: # If there're more test functions queued up they'll be called, otherwise we shut down.
      func_data = self.test_funcs.get(False)
      self.last_step_name = func_data[0]
      try:
        f = open(self.validation_file, 'a')
        f.write(self.last_step_name + '\n')
        f.flush()
        f.close()
      except Exception as E:
        print str(e)
      func = func_data[1]
      self.last_func = func_data[1]
      start_time = None
      run_time = None
      time_source = time
      absolute_time = False
      for validation in func.validation:
        if 'start_time' in validation:
          if start_time is None or start_time < validation['start_time']:
            start_time = validation['start_time']
        if 'run_time' in validation:
          if run_time is None or run_time < validation['run_time']:
            run_time = validation['run_time']
        if 'time_source' in validation and not validation['time_source'] is None:
          time_source = validation['time_source']
        if 'absolute_time' in validation and not validation['absolute_time'] is None:
          absolute_time = validation['absolute_time']

      if not(start_time is None):
        scheduleCallback ( time_source, (0 if absolute_time else time_source.getValue())+start_time, UnitTestHelper.startFuncDelayed, (self,func, run_time, time_source) )
      else:
        try:
          for validation in func.validation:
            validation['init'](self, validation, self.validation_file)
        except Exception as e:
          print(str(e))
        try:
          func()
        except Exception as e:
          print "Exception in step " + func.__name__ + ": " + str(e)
          traceback.print_exc(3)
        if(not (run_time is None)):
          scheduleCallback ( time_source, time_source.getValue()+run_time, UnitTestHelper.doTesting, (self,))
        else:
          scheduleCallback ( time_source, time_source.getValue()+1, UnitTestHelper.doTesting, (self,))          
      return
    except Exception as e:
      shutdown_file = open( self.early_shutdown_file, 'w' )
      shutdown_file.write( "OK" )
      shutdown_file.flush()
      shutdown_file.close()
      throwQuitAPIException()

  def startFuncDelayed(self, func, run_time, time_source):
      try:
        for validation in func.validation:
          validation['init'](self, validation, self.validation_file)
      except Exception as e:
        print(str(e))
      try:
        func()
      except Exception as e:
          print "Exception in step " + func.__name__ + ": " + str(e)
          traceback.print_exc(3)

      if(not (run_time is None)):
        scheduleCallback ( time_source, time_source.getValue()+run_time, UnitTestHelper.doTesting, (self,))
      else:
        scheduleCallback ( time_source, time_source.getValue()+0.5, UnitTestHelper.doTesting, (self,))

  def printCustom(self, value):
    if self.customPrintHelper is None:
      print "Error: Test script called printCustom from a function that does not have the @custom decorator"
    else:
      self.customPrintHelper(self, value)


TestCaseScriptFolder = getNamedNode('TestCaseScriptFolder').getField('value').getValueAsString().replace('"', '')
TestCaseDefFolder = getNamedNode('TestCaseDefFolder').getField('value').getValueAsString().replace('"', '')
TestBaseFolder = getNamedNode('TestBaseFolder').getField('value').getValueAsString().replace('"', '')
sys.path.append(TestBaseFolder) # This is so we can properly import from UnitTestUtil.py
TestCaseScriptFilename = getNamedNode('TestCaseScriptFilename').getField('value').getValueAsString().replace('"', '')
sys.path.append(TestCaseScriptFolder)
TestCaseName = getNamedNode('TestCaseName').getField('value').getValueAsString().replace('"', '')
StartTime = getNamedNode('StartTime').getField('value').getValue()[0]
res = import_module(TestCaseScriptFilename)
res.__scriptnode__ = globals()['__scriptnode__']

def linenumber_of_member(m):
    try:
        return m[1].func_code.co_firstlineno
    except AttributeError:
        return -1

# import all the functions that have our validator decorators attached. We identify them by the presence of a validation array.
# the result is a list of tuples containing (function name, function address), so we sort by the latter to ensure the test functions will be executed in the same order as they appear in the file
testfunctions_list = [o for o in getmembers(res) if isfunction(o[1]) and hasattr(o[1], "validation")]
testfunctions_list.sort(key=linenumber_of_member)

testHelper = UnitTestHelper(TestBaseFolder+"/test_complete", os.path.abspath(os.path.join(TestCaseDefFolder, "output").replace("\\", '/')), TestCaseName + '_')
testHelper.addTests(testfunctions_list)
res.printCustom = testHelper.printCustom
res.TestCaseDefFolder = TestCaseDefFolder

scheduleCallback ( time, time.getValue()+StartTime, UnitTestHelper.doTesting, (testHelper,))