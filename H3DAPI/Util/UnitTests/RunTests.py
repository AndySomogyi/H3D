"""
This script is supposed to run unit tests on every H3DAPI node. 

Requirements:
  You must install Imagemagick (and add to PATH) in order for image comparison to function, otherwise rendering tests will always fail.
"""

import os, sys
import time
import platform
from Queue import Queue, Empty
from threading  import Thread
import tempfile
import string
import argparse
import math
import signal
import glob
#import Image
import ConfigParser
import json
import datetime
from collections import namedtuple
import shlex

import MySQLdb

from TestResults import TestResults
from Variations import Option, Variation
from ProcessWrapper import *
import subprocess



parser = argparse.ArgumentParser(
  description='Runs python tests')
parser.add_argument('--workingdir', dest='workingdir', 
                    default=os.getcwd().replace('', ''),
                    help='The directory containing the unit tests.')
parser.add_argument('--processworkingdir', dest='processworkingdir', 
                    default=None,
                    help='The working directory for the test process. If omitted, will be set to base of X3D file to test')
parser.add_argument('--processpath', dest='processpath',
                    help='The path to the application used to run the tests. This is so you don''t need H3DLoad in your path',
                    default='')
parser.add_argument('--processname', dest='processname', 
                    default='H3DLoad',
                    help='The application used to run the tests.')
parser.add_argument('--processargs', dest='processargs',
                    default='',
                    help='Can be used to pass arguments to the process that you run. These will be put before the name of the .x3d file for the test that is being launched. This can be used to do things like running a python script instead of H3D by setting the processname to python.exe and the args to the path of the script you want to run.')
parser.add_argument('--output', dest='output',
                   default='output',
                   help='The directory to output the results to.')
parser.add_argument('--only_validate', dest='only_validate', action='store_true',
                    default=False,
                    help='Does not run the test cases, only goes through the output directory and compares the generated output. Will not report start/termination results. Can not be combined with only_generate.')
parser.add_argument('--only_generate', dest='only_generate', action='store_true',
                    default=False,
                    help='does not run validation on the generated output. Will not report rendering or performance comparison results. Can not be combined with only_validate.')
parser.add_argument('--timestamp', dest='timestamp', help='Format is YYYY-MM-DD HH:MM:SS. The time part of the datetime is optional. This datetime will be stored in the database for all the tests this script will run. If nothing is specified it will default to the current time.',
                    default=datetime.datetime.strftime(datetime.datetime.today(), "%Y-%m-%d %H:%M:%S"))                    
parser.add_argument('--dbhost', dest='dbhost', help='Address to database.')
parser.add_argument('--dbname', dest='dbname', help='Database name.')
parser.add_argument('--dbuser', dest='dbuser', help='Database user. Needs write-access to the database.')
parser.add_argument('--dbpass', dest='dbpass', help='Database password.')
parser.add_argument('--servername', dest='servername', help='The name of this server. All results from tests run by this script will be associated with this name.',
                    default='Unknown')
parser.add_argument('--RunTestsDir', dest='RunTestsDir', help='The location of UnitTestsUtil.py and UnitTestsBoilerplate.py. This is for the cases where RunTests.py is being run from a different directory, for example for targeting a specific build of H3D.',
                    default=os.path.dirname(os.path.realpath(__file__)).replace('\\', '/'))
parser.add_argument('--case', dest='case', help='The name of a specific case located somewhere in or below the workingdir. If this is specified then only this Case, out of all the cases in all the TestDefs, will be run', default='')
parser.add_argument('--testdefs', dest='testdefs', help='The name of one or more testdefs located somewhere in or below the workingdir. If this is specified then only cases in these TestDefs will be run. This can be combined with --case if you have multiple testdefs containing cases with the same name and only want to run one of them.', default='')
parser.add_argument('--resolution', dest='resolution', help='The resolution h3dload should be run at (only used for h3dload), in the format widthxheight, for example 800x600', default='640x480')
parser.add_argument('--inject_at_end_of_scene', dest='inject_at_end_of_scene', help='Specifies if the testing boilerplate nodes should be injected before </Scene> instead of the standard behaviour of injecting after <Scene>. For compatibility with projects that do search-and-replace inside the x3d file and might match information in one of the unittesting nodes if they come before nodes that are expected to be in Scene.', default=False)
parser.add_argument('--simulationbasedir', dest='simulationbasedir', help='Path to the directory where the project that is being tested has its SimulationBase  python directory, this is required for using the settings testdef property.', default=None)
args = parser.parse_known_args()[0]


def createFilename(orig_url, var_name):
  """ Returns a valid file name based on the provided path and variable name. """
  valid_chars = "-_.() %s%s" % (string.ascii_letters, string.digits)
  filename= orig_url + var_name
  return ''.join(c for c in filename if c in valid_chars).strip ( '.' )

def isError ( result ):
  """ Returns true if one of the possible errors occurred. """
  return (result.errors > 0 or not result.starts_ok or not result.terminates_ok) and not result.skipped

def getExitCode ( results ):
  """ Returns an exit code based on the test results. """
  for result in results:
    for variation in result:
      v= variation[0]
      r= variation[1]
      if isError ( r ):
        return 1
  return 0
  
   
class TestCaseRunner ( object ):

  def __init__ ( self, filepath, startup_time= 10, shutdown_time= 5, resolution=None, testable_callback= None, fake_fullscreen_due_to_PIL_bug = True, error_reporter = None):
    self.filepath= filepath
    self.startup_time= startup_time
    self.shutdown_time= shutdown_time
    self.testable_callback= testable_callback
    self.early_shutdown_file = '%s/test_complete' % (args.RunTestsDir)
    self.startup_time_multiplier = 1
    self.db = None
    # Used to get intermediate failed results in order to be able to cancel test early and still get some results.
    self.error_reporter = error_reporter

  def getProcess( self ):
    platform_system = platform.system()
    if platform_system == 'Windows':
      return ProcessWin32()
    elif platform_system == 'Linux':
      return ProcessUnix()

  def _countWarnings ( self, test_results ):
    outhead, outpart, outtail = test_results.std_out.partition("console_start")
    errhead, errpart, errtail = test_results.std_err.partition("console_start")
    outtail = outtail.partition("console_end")[2]
    errtail = errtail.partition("console_end")[2]
    haystack = outhead + outtail[outtail.find('\n')+1:] + errhead + errtail[errtail.find('\n')+1:]

    haystack= haystack.lower()
    return ( haystack.count ( "warning" ), haystack.count ( "error" ) )

  def launchTest ( self, url, cwd, resolution=None ):
    if h3d_process_name == "H3DLoad":
      if resolution:
        self.load_flags = ["--no-fullscreen","--screen=" + resolution]
      else:
        self.load_flags = ["--no-fullscreen","--screen=" + args.resolution]        
    else:
      self.load_flags = []

    process = self.getProcess()
    proc_cmd = os.path.join(args.processpath, h3d_process_name)
    proc_url = url

    if ' ' in proc_cmd:
      proc_cmd = '"' + proc_cmd + '"'
    if ' ' in proc_url:
      proc_url = '"' + proc_url + '"'

    if self.processargs != []:
      process.launch ( [proc_cmd] + self.processargs + self.load_flags + [proc_url], cwd)
    else:
      process.launch ( [proc_cmd] + self.load_flags + [proc_url], cwd)
    return process

  def testStartUp ( self, url, cwd, variation, resolution=None):
    """ Returns true if the example can be started. """
    
    if h3d_process_name == "H3DLoad":
      if resolution:
        self.load_flags = ["--no-fullscreen","--screen=" + resolution]
      else:
        self.load_flags = ["--no-fullscreen","--screen=" + args.resolution]        
    else:
      self.load_flags = []    

    process = self.getProcess()

    if self.processargs != []:
      return process.testLaunch ( [os.path.join(args.processpath, h3d_process_name)] + self.processargs + self.load_flags + [url], cwd, self.startup_time, self.shutdown_time, 1 if variation and variation.global_insertion_string_failed else self.startup_time_multiplier, self.early_shutdown_file )
    else:
      return process.testLaunch ( [os.path.join(args.processpath, h3d_process_name)] + self.load_flags + [url], cwd, self.startup_time, self.shutdown_time, 1 if variation and variation.global_insertion_string_failed else self.startup_time_multiplier, self.early_shutdown_file )
  
  def getExePath(self,program):
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None
  
  def runTestCase (self, filename, test_case, url, orig_url= None, var_name= "", variation = None):
    if orig_url is None:
      orig_url= url
    
    test_results= TestResults( )
    test_results.filename= filename
    test_results.name= test_case.name
    test_results.url= orig_url

    self.processargs = [arg for arg in (args.processargs.split(' ') + test_case.processargs.split(' ')) if arg != ""]

    self.startup_time = test_case.starttime
    self.shutdown_time = test_case.runtime
    if args.processworkingdir is None:
      cwd= os.getcwd()
    else:
      cwd= args.processworkingdir
      print "working dir for process is: " + cwd
    filename= os.path.abspath ( url )
    
    if os.path.isfile( self.early_shutdown_file ):
      os.remove( self.early_shutdown_file )
    if os.path.isfile("childProcessIdList.txt"):
      os.remove( "childProcessIdList.txt" )

    process= self.launchTest ( url, cwd, test_case.resolution)

    time_slept = 0.0
    while time_slept < (self.startup_time ):
      time.sleep(0.5)
      time_slept += 0.5
      if not process.isRunning():
        break
    if not process.isRunning ():
      print "Test finished successfully after " + str(time_slept) + "s"
      test_results.std_out= process.getStdOut()
      test_results.std_err= process.getStdErr()
      if test_case.ignore_warnings:
        test_results.warnings, test_results.errors = [0,0]
      else:
        test_results.warnings, test_results.errors= self._countWarnings ( test_results )
      test_results.terminates_ok = os.path.isfile(self.early_shutdown_file)
      return test_results
   
    # We continue only if the process is still running after the startup time and sleep until the runtime runs out or until the process stops running
    time_slept = 0.0
    while time_slept < self.shutdown_time and process.isRunning():
      time.sleep(0.5)
      time_slept += 0.5

    if not process.isRunning ():
      print "Test finished successfully after " + str(self.startup_time + time_slept) + "s"
      test_results.terminates_ok= True
      test_results.std_out= process.getStdOut()
      test_results.std_err= process.getStdErr()
      if test_case.ignore_warnings:
        test_results.warnings, test_results.errors = [0,0]
      else:
        test_results.warnings, test_results.errors= self._countWarnings ( test_results )
      return test_results
    else:
      print "Shutdown timeout hit, test looks like it crashed or froze."
      # To ensure the launched process is killed, use pskill and 
      # id information stored in childProcessedIDList.txt to force kill those ids. 
      # try to locate childProcessIdList.txt file which is supposed to be generated
      # by test case to indicate the the id of the process
      pskill_path = self.getExePath("pskill.exe")
      if pskill_path is not None:
        if os.path.isfile("childProcessIdList.txt"):
          f = open("childProcessIdList.txt",'r')
          process_ids = f.read().split()
          f.close()
          for process_id in process_ids:
            print "force kiling id", process_id
            kill_process = str(pskill_path)+" /accepteula -t -nobanner "+str(process_id)
            p = subprocess.Popen( kill_process, shell=False )
        else:
          print "pskill located, but cannot locate the id for the process that started for current test case"
      else: 
        print "pskill does not exist, use internal process kill to try to locate and kill all related processes"

      try:
        process.kill ()
        time_slept = 0
        self.shutdown_timeout = 60
        while time_slept < self.shutdown_timeout and process.isRunning():
          time.sleep(0.5)
          time_slept += 0.5
      except:
        pass
      test_results.std_out= process.getStdOut()
      test_results.std_err= process.getStdErr()
      if test_case.ignore_warnings:
        test_results.warnings, test_results.errors = [0,0]
      else:
        test_results.warnings, test_results.errors= self._countWarnings ( test_results )
      return test_results
  

  def parseTestDefinitionFile(self, file_path):
    """
    Parses the specified test definition file and returns a list of namedtuples containing the following:      
      name: the name of this test case
      x3d: x3d file path
      baseline: baseline folder
      script: the test script
      All of these values default to None
    The list will contain one namedtuple for each Section in the specified definition file
    """
    confParser = ConfigParser.RawConfigParser(defaults={'x3d':None, 'baseline':None, 'script':None, 'runtime':1, 'starttime':1, 'fuzz': 2, 'threshold': 20, 'resolution': None, 'processargs': '', 'physics_engine': None, 'settings': None, 'ignore_warnings': None}, allow_no_value=True)
    try:
      confParser.read(file_path)
    except:
      print sys.exc_info()[0]
      return None
    result = []
    for sect in confParser.sections():
      if sect != 'Default':
        test_case = namedtuple('TestDefinition', ['name', 'filename', 'x3d', 'baseline', 'script', 'runtime', 'starttime', 'resolution', 'processargs', 'maxtrials', 'physics_engine','settings', 'ignore_warnings'])
        test_case.name = sect
        test_case.x3d = confParser.get(sect, 'x3d')
        test_case.baseline = confParser.get(sect, 'baseline folder')
        test_case.script = confParser.get(sect, 'script')
        try: 
          test_case.runtime = confParser.getfloat(sect, 'timeout')
        except:
          test_case.runtime = 1
        try:
          test_case.starttime = confParser.getfloat(sect, 'starttime')
        except:
          test_case.starttime = 1
        try:
          test_case.fuzz = confParser.getfloat(sect, 'fuzz')
        except:
          test_case.fuzz = 2
        try:
          test_case.threshold = confParser.getfloat(sect, 'threshold')
        except:
          test_case.threshold = 20
        try:
          test_case.resolution = confParser.get(sect, 'resolution')
        except:
          test_case.resolution = None

        try:
          test_case.processargs = confParser.get(sect, 'arguments')
        except:
          test_case.processargs = ''
          
        try:
          test_case.maxtrials = confParser.getint(sect, 'maxtrials')
        except:
          test_case.maxtrials = 1

        try:
          test_case.physics_engine = confParser.get(sect, 'physics_engine')
        except:
          test_case.physics_engine = None
        
        try:
          test_case.settings = confParser.get(sect, 'settings')
        except:
          test_case.settings = None
                  
        try:
          test_case.ignore_warnings = confParser.getboolean(sect, 'ignore_warnings')
        except:
          test_case.ignore_warnings = False

        if args.case == "" or args.case == test_case.name:
          result.append(test_case)

    return result

  def processTestDef(self, file, testCase, results, directory):
    """
        
    """
    all_tests_successful = True
    
    output_dir = os.path.abspath(os.path.join(directory, "output"))
    rendering_dir = os.path.join(directory, output_dir, 'renderings')
    text_dir = os.path.join(directory, output_dir, 'text')
    for dir in [output_dir, rendering_dir, text_dir]:
      if not os.path.exists(dir):
        os.mkdir(dir)


    if os.path.exists(os.path.join(rendering_dir)):
      for file in os.listdir(rendering_dir):
        if file.startswith("diff_"):
          os.remove(os.path.join(rendering_dir, file))
                
    script = """
    <MetadataString DEF='TestCaseName' value='%s'/>
    <MetadataString DEF='TestCaseScriptFolder' value='%s'/>
    <MetadataString DEF='TestCaseScriptFilename' value='%s'/>
    <MetadataString DEF='TestCaseDefFolder' value='%s'/>
    <MetadataString DEF='TestBaseFolder' value='%s'/>
    <MetadataFloat DEF='StartTime' value='%f'/>
    <PythonScript DEF='TestScript' url='%s'></PythonScript>""" % (testCase.name,
                                                                  os.path.split(os.path.abspath(os.path.join(directory, testCase.script)))[0].replace('\\', '/'),
                                                                  os.path.splitext(os.path.split(testCase.script)[1])[0],
                                                                  os.path.abspath(directory).replace('\\', '/'),
                                                                  args.RunTestsDir,
                                                                  testCase.starttime,
                                                                  os.path.join(args.RunTestsDir, 'UnitTestBoilerplate.py'))
    v = Variation (testCase.name, script)
    # Check if we should change settings
    if not testCase.settings is None:
      if args.simulationbasedir is None:
        print("Error! No --simulationbasedir flag! The Settings testdef property requires access to SimulationBase! Skipping TestCase!")
        all_tests_successful = False
        exitcode = 1
        return None
      else:
        try:
          print("Doing settings injection!")
          settings_new, settings_old = shlex.split(testCase.settings)
          if settings_new is None or settings_old is None:
            print("Error! Settings property needs two values, the settings file to use and the settings file that needs to be modified!")
            all_tests_successful = False
            exitcode = 1
            return None
          settings_new = directory + "\\" + settings_new
          settings_old = directory + "\\" + settings_old
          sys.path.append(args.simulationbasedir)
          import SimulationBase.Settings
          settings_file = SimulationBase.Settings.ConfigINI()
          if os.path.exists(settings_old + ".original.ini"):
            try:
              print settings_old+'_original.ini already exists! This probably means that RunTests was interrupted during a previous execution.'
              print "Restoring " + settings_old + " before continuing..."
              os.remove(settings_old)
              os.rename(settings_old + ".original.ini", settings_old)  
            except:
              print "Failed to restore! Please check the files manually."
              return
            
          settings_file.load(settings_old)
          if not settings_file.save(settings_old + ".original.ini"):
            print("Failed to backup original file.")
          settings_file_new = SimulationBase.Settings.ConfigINI()
          settings_file_new.load(settings_new)
          for section in settings_file_new.getSections():
            for option in settings_file_new.getOptions(section):
              settings_file.set(section, option, settings_file_new.get(section, option))
          if not settings_file.save(settings_old):
            print("Failed to save new file")
        except Exception as e:
          print(str(e))
        
        
    # use svn info to attempt to find the repo url of this test and its test script
    p = subprocess.Popen( 'svn info "' + os.path.join(directory, testCase.x3d) + '"', stdout=subprocess.PIPE, shell=False )
    svn_info_out, _ = p.communicate()
    if p.returncode != 0:
      print("Unable to obtain svn info for test x3d file, won't include it in results")
    else:
      #include it here
      for line in svn_info_out.split('\r\n'):
        if line.startswith("URL: "):
          svn_url_x3d = line[5:]
          break
    p = subprocess.Popen('svn info "' + os.path.join(directory, testCase.script) + '"', stdout=subprocess.PIPE, shell=False )
    svn_info_out, _ = p.communicate()
    if p.returncode != 0:
      print("Unable to obtain svn info for test x3d file, won't include it in results")
    else:
      #include it here
      for line in svn_info_out.split('\r\n'):
        if line.startswith("URL: "):
          svn_url_script = line[5:]
          break

    # Check if we should change the physics engine being used for this test and if so then use Variation's Option feature to ensure the test is run with that engine
    if not testCase.physics_engine is None:
      v.options.append ( Option ( ["RigidBodyCollection"], "physicsEngine", testCase.physics_engine ) )

    # Create a temporary x3d file containing our injected script

    original_path = os.path.join(directory, testCase.x3d)
    try:
      if os.path.exists(original_path+'_original.x3d'):
        print original_path+'_original.x3d already exists! This probably means that RunTests was interrupted during a previous execution.'
        print "Restoring " + original_path + " before continuing..."
        try:
          if os.path.exists(original_path):
            os.remove(original_path)
          os.rename(original_path+'_original.x3d', original_path)
          print "Restored! Continuing the testing..."
        except:
          print "Failed to restore! Please check the files manually."
          return
      if (os.path.exists(os.path.join(output_dir, "validation.txt"))):
        os.remove(os.path.join(output_dir, "validation.txt"))

      success, variation_path= self._createVariationFile ( v, os.path.join(directory, testCase.x3d))
      os.rename(original_path, original_path+'_original.x3d') # rename the original .x3d file
      os.rename(variation_path, original_path) # swap in our variation file for the original
    except:
      pass
    try:
      # Run the test
      for trial in range(testCase.maxtrials):
        print "NOTE: Running trial %d/%d..." % (trial+1, testCase.maxtrials)
        result = self.runTestCase (file, testCase, os.path.abspath(original_path), os.path.join(directory, testCase.x3d), v.name, v)
        if result.terminates_ok:
          print result.std_err
          print result.std_out
          break
        else:
          print "WARNING: Trial %d/%d failed!" % (trial+1, testCase.maxtrials)
        
  #    print os.path.abspath(output_dir + '\\validation.txt')
      result.parseValidationFile(testCase, os.path.abspath(output_dir + '\\validation.txt'), os.path.abspath(os.path.join(directory, testCase.baseline)), os.path.abspath(output_dir + '\\text\\'), testCase.fuzz, testCase.threshold)
      if result.success and all_tests_successful:
        exitcode = 0
      else:
        exitcode = 1
    except Exception as e:
      print str(e)
  

    try:
      os.remove(original_path)
    except:
      pass
    try:
      os.remove(variation_path)
    except:
      pass
    try:
      os.rename(original_path+'_original.x3d', original_path)
    except:
      pass
      
    try:
      if testCase.settings != None:
        settings_new, settings_old = shlex.split(testCase.settings)
        os.remove(directory + "\\" + settings_old)
        os.rename(directory + "\\" + settings_old+".original.ini", directory + "\\" + settings_old)
    except:
      pass

    if result:
      result.svn_url_x3d = svn_url_x3d
      result.svn_url_script = svn_url_script

    return result

  def processAllTestDefinitions ( self, directory= ".", output_dir= ".", fileExtensions= [".testdef"] ):
    """
        
    """
    try:
      os.makedirs(self.filepath)
    except:
      pass
     
    results = []
    
    self.server_name = args.servername;
    self.ConnectDB()
    
    curs = self.db.cursor()
    curs.execute("SELECT id FROM servers WHERE server_name='%s'" % self.server_name)
    res = curs.fetchone()
    if res == None:
      print("Failed to obtain server id from db")
      return
    self.server_id = res[0]

    all_tests_successful = True
    all_tests_run = True
    
    found_tests = False
    for root, dirs, files in os.walk(directory):
      for file in files:
        base, ext= os.path.splitext(file)
        if ext.lower() in fileExtensions:
          if args.testdefs == "" or base in args.testdefs.split():
            file_path= os.path.join(root,file)
            testCases = self.parseTestDefinitionFile(file_path)
            for testCase in testCases:
              if testCase != None and testCase.x3d != None and testCase.script != None:
                found_tests = True
                print "Testing: " + testCase.name
                case_results = self.processTestDef(file, testCase, results, root)
                results.append(case_results)
                all_tests_successful = case_results.success and all_tests_successful
                all_tests_run = case_results.terminates_ok and all_tests_run
                testCase.filename = (os.path.relpath(file_path, directory)).replace('\'', '/') # This is used to set up the tree structure for the results page. It will store this parameter in the database as a unique identifier of this specific file of tests.
                if case_results != None:  
                  self.UploadResultsToSQL(testCase, case_results, root)            
    if not found_tests:
      print "No valid tests found in: " + os.path.abspath(directory)
              
    return results, all_tests_successful, all_tests_run
  

  def _isTestable ( self, file_path ):
    
    if self.testable_callback:
      return self.testable_callback ( file_path )
    else:
      return True
  def _createVariationFile ( self, variation, file_path ):
    
    orig_file= open ( file_path, 'r' )
    
    
    variation_file= tempfile.NamedTemporaryFile(dir=os.path.dirname(file_path), suffix='.x3d', delete= False )
    original_contents= orig_file.read()
    variation_contents= variation.parse ( original_contents, inject_at_end_of_scene=args.inject_at_end_of_scene)
    variation_file.write ( variation_contents )
    variation_file.close()
    
    return (variation_contents!=original_contents, variation_file.name)
 


  def ConnectDB(self):
    if self.db == None:
      try:
        print "Attempting to connect to results database at " + args.dbhost
        self.db = MySQLdb.connect(host=args.dbhost, db=args.dbname, user=args.dbuser, passwd=args.dbpass)
        print "Connected!"
        self.db.autocommit = True
        curs = self.db.cursor()
        curs.execute("SELECT * FROM servers WHERE server_name='%s'" % self.server_name)
        if curs.fetchone() == None:
          curs.execute("INSERT INTO servers (server_name) VALUES ('%s')" % self.server_name)
        curs.close()
        self.db.commit()
      except Exception as e:
        print(str(e))        
        
        
  def UploadResultsToSQL(self, testCase, case_results, output_dir):
    self.ConnectDB()
    print "Uploading results."
    curs = self.db.cursor()
    
    # fetch the optional test_description.txt file from the directory
    testcat_description = ""
    if os.path.exists(os.path.join(args.workingdir, os.path.split(testCase.filename)[0], "description.txt")):
      description_file = open(os.path.join(args.workingdir, os.path.split(testCase.filename)[0], "description.txt"), "r")
      testcat_description = description_file.read()
    # check if there is a description for this directory already, if so replace it if it differs from this one
    curs.execute("SELECT id, description FROM test_categories WHERE path='%s'" % self.db.escape_string(os.path.split(testCase.filename)[0]))
    res = curs.fetchone()
    if res == None:
      #Doesn't exist, so upload it
      curs.execute("INSERT INTO test_categories (path, description) VALUES ('%s', '%s')" % (self.db.escape_string(os.path.split(testCase.filename)[0]), self.db.escape_string(testcat_description)))
    else:
      if curs[1] != testcat_description:
        curs.execute("UPDATE test_categories SET description='%s' WHERE id=%d" % (self.db.escape_string(testcat_description), curs[0]))

    # Then ensure the test file and test_case exists in the database
    curs.execute("SELECT id, description FROM test_files WHERE filename='%s'" % os.path.splitext(testCase.filename.replace("\\", "/"))[0])
    res = curs.fetchone()
    if res == None: # test_file doesn't exist, so add it
      curs.execute("INSERT INTO test_files (filename) VALUES ('%s')" % os.path.splitext(testCase.filename.replace("\\", "/"))[0]) 
      curs.execute("SELECT id FROM test_files WHERE filename='%s'" % os.path.splitext(testCase.filename.replace("\\", "/"))[0])
      res = curs.fetchone()
    else:
      if res[1] != testcat_description:
        curs.execute("UPDATE test_files SET description='%s' WHERE id=%d" % (self.db.escape_string(testcat_description), res[0]))
    testfile_id = res[0]
    
    
    # Also add this test run to the database
    if not hasattr(self, "test_run_id"):
      curs.execute("INSERT INTO test_runs (timestamp, server_id) VALUES (\"" + args.timestamp + "\", %d)" % (self.server_id))
      self.test_run_id = curs.lastrowid
      if res == None:
        print("Failed to insert test run in db")
        return


    # Now ensure the test_case exists in the database
    curs.execute("SELECT test_cases.id, test_cases.svn_url_x3d, test_cases.svn_url_script FROM test_cases JOIN test_files WHERE case_name='%s'" % testCase.name)
    res = curs.fetchone()
    if res == None:
      curs.execute("INSERT INTO test_cases (case_name, svn_url_x3d, svn_url_script) VALUES ('%s', '%s', '%s')" % (testCase.name, case_results.svn_url_x3d, case_results.svn_url_script))
      curs.execute("SELECT id FROM test_cases WHERE case_name='%s'" % testCase.name)
      res = curs.fetchone()
    else:
      if (res[1] != case_results.svn_url_x3d) or (res[2] != case_results.svn_url_script):
        curs.execute("UPDATE test_cases SET svn_url_x3d='%s', svn_url_script='%s' WHERE test_cases.id=%d" % (case_results.svn_url_x3d, case_results.svn_url_script, res[0]))
    testcase_id = res[0]


    #StepResultTuple = namedtuple('StepResult', ['step_name', 'success', 'results']) # results is an array of results
    #PerformanceResultTuple = namedtuple("PerformanceResult", ['fps_min', 'fps_max', 'fps_avg', 'fps_mean', 'fps_full'])
    #RenderingResultTuple = namedtuple("RenderingResult", ['success', 'baseline_path','output_path', 'diff_path'])
    #ConsoleResultTuple = namedtuple("ConsoleResult", ['success', 'baseline_path', 'output', 'diff'])
    #CustomResultTuple = namedtuple("CustomResult", ['success', 'baseline_path', 'output', 'diff'])
    for step in case_results.step_list:
      #First ensure the test step exists in the database
      curs.execute("SELECT test_steps.id FROM test_steps WHERE step_name='%s' AND test_case_id=%d" % (self.db.escape_string(step.step_name), testcase_id))
      res = curs.fetchone()
      if res == None:
        curs.execute("INSERT INTO test_steps (step_name, test_case_id) VALUES ('%s', %d)" % (self.db.escape_string(step.step_name), testcase_id))
        curs.execute("SELECT id FROM test_steps WHERE step_name='%s'" % self.db.escape_string(step.step_name))
        res = curs.fetchone()
      teststep_id = res[0]

      for result in step.results:
        if type(result).__name__ == 'ErrorResult':
          curs.execute("INSERT INTO error_results (test_run_id, file_id, case_id, step_id, stdout, stderr) VALUES (%d, %d, %d, %d, '%s', '%s')" % (self.test_run_id, testfile_id, testcase_id, teststep_id, self.db.escape_string(result.stdout), self.db.escape_string(result.stderr)))

        elif type(result).__name__ == 'ConsoleResult':
          output_string = ''
          for line in result.output:
            output_string += line
          if result.success:
            curs.execute("INSERT INTO console_results (test_run_id, file_id, case_id, step_id, success, output) VALUES (%d, %d, %d, %d, 'Y', '%s')" % (self.test_run_id, testfile_id, testcase_id, teststep_id, self.db.escape_string(output_string)))
          else:
            # Go read the baseline file so we can add that to the output
            if os.path.exists(result.baseline_path):
              f = open(result.baseline_path)
              baseline_string = f.read()
              f.close()
              curs.execute("INSERT INTO console_results (test_run_id, file_id, case_id, step_id, success, output, baseline, diff) VALUES (%d, %d, %d, %d, 'N', '%s', '%s', '%s')" % (self.test_run_id, testfile_id, testcase_id, teststep_id, self.db.escape_string(output_string), self.db.escape_string(baseline_string), self.db.escape_string(result.diff)))
            else:
              curs.execute("INSERT INTO console_results (test_run_id, file_id, case_id, step_id, success, output, baseline, diff) VALUES (%d, %d, %d, %d, 'N', '%s', NULL, NULL)" % (self.test_run_id, testfile_id, testcase_id, teststep_id, self.db.escape_string(output_string)))
            
        elif type(result).__name__ == 'CustomResult':
          output_string = ''
          for line in result.output:
            output_string += line
          if result.success:
            curs.execute("INSERT INTO custom_results (test_run_id, file_id, case_id, step_id, success, output) VALUES (%d, %d, %d, %d, 'Y', '%s')" % (self.test_run_id, testfile_id, testcase_id, teststep_id, self.db.escape_string(output_string)))
          else:
            # Go read the baseline file so we can add that to the output
            if os.path.exists(result.baseline_path):
              f = open(result.baseline_path)
              baseline_string = f.read()
              f.close()
            else:
              baseline_string = 'Baseline not found'
            curs.execute("INSERT INTO custom_results (test_run_id, file_id, case_id, step_id, success, output, baseline, diff) VALUES (%d, %d, %d, %d, 'N', '%s', '%s', '%s')" % (self.test_run_id, testfile_id, testcase_id, teststep_id, self.db.escape_string(output_string), self.db.escape_string(baseline_string), self.db.escape_string(result.diff)))

        elif type(result).__name__ == 'PerformanceResult':
          # Insert the performance results, but only if all the tests in this step succeeded!
          if step.success:
#            curs.execute("INSERT INTO performance_results (test_run_id, file_id, case_id, step_id, min_fps, max_fps, avg_fps, mean_fps, full_case_data) VALUES (%d, %d, %d, %d, %s, %s, %s, %s, '%s')" % (self.test_run_id, testfile_id, testcase_id, teststep_id, result.fps_min, result.fps_max, result.fps_avg, result.fps_mean, result.fps_full))
            curs.execute("INSERT INTO performance_results (test_run_id, file_id, case_id, step_id, full_profiling_data) VALUES (%d, %d, %d, %d, '%s')" % (self.test_run_id, testfile_id, testcase_id, teststep_id, result.profiling_data_full))
            perf_res_id = curs.lastrowid
            for data in result.profiling_data_lines:
              curs.execute("INSERT INTO performance_result_data (performance_result_id, level, identifier, mean, percent) VALUES (%d, %d, '%s', %f, %f)" % (perf_res_id, int(data.level), data.id, float(data.mean), float(data.percent)) )

        elif type(result).__name__ == 'RenderingResult':
            step_name = step.step_name
            # Fetch the current baseline so we can compare it to the one in the database
            if os.path.exists(result.baseline_path):
              baseline_file = open(result.baseline_path, 'rb')
              baseline_image = baseline_file.read()
              baseline_file.close()
              curs.execute("SELECT id, image FROM rendering_baselines WHERE file_id=%s AND case_id=%s AND step_id='%s' ORDER BY timestamp DESC LIMIT 1" % (testfile_id, testcase_id, teststep_id))
              res = curs.fetchone()
              if res == None or res[1] != baseline_image:
                curs.execute(("INSERT INTO rendering_baselines (file_id, case_id, step_id, timestamp, image) VALUES (%d, %d, %d, '%s'" % (testfile_id, testcase_id, teststep_id, args.timestamp)) + ", %s)", [baseline_image,])
#              elif res[1] != baseline_image:
#                curs.execute("INSERT into rendering_baselines  image=%s WHERE id=%s", [baseline_image, res[0]])
            
            if not result.success: #validation failed, if possible we should upload both the rendering and the diff
              if result.diff_path != '':
                diff_file = open(result.diff_path, 'rb')
                diff_image = diff_file.read()
                diff_file.close()
              else:
                diff_image = 'NULL'
              if result.output_path != '':
                output_file = open(result.output_path, 'rb')
                output_image = output_file.read()
                output_file.close()
              else:
                output_image = 'NULL'
              curs.execute("INSERT INTO rendering_results (test_run_id, file_id, case_id, step_id, success, output_image, diff_image) VALUES (%d, %d, %d, %d" % (self.test_run_id, testfile_id, testcase_id, teststep_id) + ", 'N', %s, %s)", [output_image, diff_image]);
            else:
              curs.execute("INSERT INTO rendering_results (test_run_id, file_id, case_id, step_id, success) VALUES (%d, %d, %d, %d, 'Y')" % (self.test_run_id, testfile_id, testcase_id, teststep_id));

    curs.close()
    self.db.commit()

  


   
class TestReport ( object ):
  
  def reportResults ( self, results ):
    output= ""
    if args.only_generate:
      return output
    all_renderings_succeeded = True

    for result in results:
      for variation in result:
        v= variation[0]
        r= variation[1]
        output+= "Test results for: " + str(r.name) + " (" + v.name + ")\n"
        output+= "\n"
        if not args.only_validate:
          output+= "Started OK    : %s\n" % str(r.starts_ok)
          output+= "Exited OK     : %s\n" % str(r.terminates_ok)
        if r.result_type == 'rendering':
          output+= "renderings:\n"
          for i in range(0, len(r.renderings)):
            output+= "  %s [%s]\n" % (r.step_names[i], ("OK" if r.renderings_ok[i] else "Failed"))
          if False in r.renderings_ok:
            all_renderings_succeeded = False

    if r.result_type == 'rendering':
      output+= "\n%s\n" % ("All rendering comparisons OK!" if all_renderings_succeeded else "One or more rendering comparisons Failed!")
    return output
    
class TestReportHTML ( object ):
  
  def __init__ ( self, filepath, only_failed= False ):
    self.filepath= filepath
    self.only_failed= only_failed
  
  def reportResults ( self, results ):
  
    try:
      os.makedirs(self.filepath)
    except:
      pass
    
    f= open ( os.path.join(self.filepath,'default.css'), 'w' )
    f.write ( self._generateCSS() )
    f.close()
    
    f= open ( os.path.join(self.filepath,'index_failed.html' if self.only_failed else 'index.html'), 'w' )
  
    output= """
<html>
  <head>
    <title>Test results</title>
    <link rel="stylesheet" href="default.css" type="text/css" />
  </head>
  <body>
    <h1>Test results</h1>
    %s
    <table>
""" % ("<h2>Failed only</h2>" if self.only_failed else "")
  
    skipped_list= []
  
    for result in results:
      if result[0][1].skipped:
        skipped_list.append ( result[0][1] )
    
      if self._includeResult ( result ):
      
        output+= "<tr>"
        r= result[0][1]
        output+= "<td><h3>" + r.name + "</h3>\n"
        
        for variation in result:
          v= variation[0]
          r= variation[1]
      
          report_path= os.path.join (
            self.filepath,
            v.name + "_report.html" )
      
          output+= "<td>"
          output+= "<h3>" + v.name + ( "" if r.created_variation else " (Unchanged file)" ) + "</h3>"
          if r.test_type == 'rendering':
            rendering = self._getFirstErrorrendering( r )
            if rendering == "":
              rendering = self._getrendering( r, count=1 )
          else:
            rendering = self._getrendering( r, count=1 )

          output+= rendering
          if not args.only_validate:
            output+= "<p>Start: " + self._getStatus ( r.started_ok ) + "; "
            output+= "Exit:" + self._getStatus ( r.terminated_ok ) + "; "
          if not args.only_generate:
            output+= "renderings: " + self._getrenderingStatus( r.renderings_ok ) + "</p>\n"
          output+= "<p>Errors: " + self._getErrorCount ( r.errors, 'fail' ) + "; "
          output+= "Warnings:" + self._getErrorCount ( r.warnings, 'warn' ) + "</p>\n"
          if not args.only_generate and r.has_fps:
            output+= "<p>Graphics frame rate: " + self._getFPS( r.fps_mean ) + "</p>\n"
          output+= "<p>[<a href='" + os.path.split(report_path)[1] + "'>details</a>]</p>"
          output+= "</td>"
          
          f1= open ( report_path, 'w' )
          f1.write ( self._reportTestcase ( r ) )
          f1.close()
          
        output+= "</tr>"
      
    output+= """
    </table>
    <h2>Skipped</h2>
    <ul>
    """
    if skipped_list:
      for r in skipped_list:
        output+= "<li>%s [%s]</li>" % (r.name, r.url)
    else:
      output+= "<li>None</li>"
      
    output+= """
    </ul>
  </body>
</html>"""
      
    f.write ( output )
    f.close()
      
    return output
    
  def _includeResult ( self, result ):
    for variation in result:
      if self._includeVariation ( variation[1] ):
        return True
    return False
    
  def _includeVariation ( self, variation ):
    return (variation.errors > 0 or variation.warnings > 0 or not variation.started_ok or not variation.terminated_ok or not self.only_failed) and not variation.skipped

  def _getThumbnail(self, rendering_path, thumb_dir):
#    if not os.path.exists(rendering_path):
#      return ""
#    else:
#      im = Image.open(rendering_path)
#      im.thumbnail((256,256), Image.ANTIALIAS)
#      rendering_filename = os.path.split(rendering_path)[1]
#      thumb_path = os.path.join(thumb_dir, os.path.splitext(rendering_filename)[0] + "_thumb.png")
#      im.save(thumb_path, "PNG")
#      return thumb_path
    pass
    
  def _reportTestcase ( self, result ):   

    output= """
<html>
  <head>
    <title>%s</title>
    <link rel="stylesheet" href="default.css" type="text/css" />
  </head>
  <body>
    <h1>%s</h1>
    <p>%s</p>
    %s
    <h2>Startup</h2>
    %s
    <h2>Shutdown</h2>
    %s
    <h2>Standard output</h2>
    <pre>%s</pre>
    <h2>Standard error</h2>
    <pre>%s</pre>""" % (
      result.name,
      result.name, result.url,
      self._getrendering ( result, count = (1 if result.test_type == 'performance' else 0)),
      self._getStatus ( result.started_ok ), 
      self._getStatus ( result.terminated_ok ),
      result.std_out,
      result.std_err)
    if not args.only_generate and result.has_fps:
      """<h2>Graphics frame rate</h2>
      <pre>Min: %s</pre>
      <pre>Max: %s</pre>
      <pre>Avg: %s</pre>
      <pre>Mean: %s</pre>""" % (self._getFPS( result.fps_min ),
      self._getFPS( result.fps_max ),
      self._getFPS( result.fps_avg ),
      self._getFPS( result.fps_mean ))

    if result.variation and result.variation.name != "Default":
      output = output + \
               """  <h2>Variation settings:</h2>"""
      for i, option in enumerate( result.variation.options ):
        if option.field_name != "":
          output = output + \
                   "    <h3>Option %s:</h2>\n" % str(i) + \
                   "    <pre>Node: " + self._getFPS( ", ".join( option.node_type ) ) + "</pre>\n" + \
                   "    <pre>Field name: " + self._getFPS( option.field_name ) + "</pre>\n" + \
                   "    <pre>Field value: " + self._getFPS( option.field_value ) + "</pre>\n"

    output = output + \
"""  </body>
</html>"""
    return output
    
  def _generateCSS ( self ):
    return """
body {
  font-family: sans-serif;
}

.success {
  color: green;
}

.warn {
  color: orange;
}

.fail {
  color: red;
  font-weight: bold;
}

td, th {
  width: 280px;
}

img {
  float: left;
}

"""
    
  def _getStatus ( self, status ):
    return "<span class='success'>OK</span>" if status else "<span class='fail'>FAIL</span>"
  
  def _getrenderingStatus ( self, status_list):
    return "<span class='success'>OK</span>" if not False in status_list else "<span class='fail'>FAIL</span>"
    
  def _getrendering ( self, result, count = 0 ):
    """
    Iterates through the renderings from a testcase result and returns HTML code for them.
    If count is not > 0 then it will only get that many renderings. Otherwise it will get all available renderings.
    """
    res = "<p>"
    for i in range(0, len(result.renderings)):
      if result.test_type == 'rendering' and result.rendering_diffs[i] != "":
        img_path = result.rendering_diffs[i]
      else:
        img_path = result.renderings[i]
      thumb_path = self._getThumbnail(img_path, self.filepath)
      res += "<a href='" + os.path.relpath(img_path, self.filepath) + "'><image src='" + os.path.relpath(thumb_path, self.filepath) + "' /></a>"
      count -= 1
      if count == 0:
        break
    res += "</p>\n"
    return res

  def _getFirstErrorrendering( self, result ):
    """ Iterates through all the test steps of a rendering test and returns HTML code for the first one that failed and has a diff image. """
    for i in range(0, len(result.renderings_ok)):
      if not result.renderings_ok[i] and result.rendering_diffs[i] != '':
        img_path = result.rendering_diffs[i]
        thumb_path = self._getThumbnail(img_path, self.filepath)
        return "<p><a href='" + os.path.relpath(img_path, self.filepath) + "'><image src='" + os.path.relpath(thumb_path, self.filepath) + "' /></a></p>\n"
    return ""
      
  def _getErrorCount ( self, nrErrors, errorType ):
    return ("<span class='success'>%s</span>" if nrErrors==0 else "<span class='"+errorType+"'>%s</span>") % nrErrors
  
  def _getFPS ( self, fps ):
    return ( "<span class='success'>%s</span>" % fps ) if fps != "" else "<span class='fail'>-</span>"
        

 

print ""
print "WARNING: Do not change the window focus, or perform other input until the test is complete!"
print ""
#time.sleep(3)


h3d_process_name = args.processname

exitCode= 0

def isTestable ( file_name , files_in_dir):
  return True

#html_reporter_errors= TestReportHTML( os.path.join(args.output, "reports"), only_failed= True )
print "Running these tests using: " + subprocess.check_output('where.exe "' + (args.processpath + '":' if (args.processpath != "") else "") + h3d_process_name) # Run our test script and wait for it to finish executing


tester= TestCaseRunner( os.path.join(args.workingdir, ""), startup_time= 5, shutdown_time= 5, testable_callback= isTestable, error_reporter=None)
results, all_tests_successful, all_tests_run = tester.processAllTestDefinitions(directory=os.path.abspath(args.workingdir), output_dir=args.output)

if not all_tests_run:
  print "Error: Some tests were not run!"
  exitCode = 2
elif not all_tests_successful:
  print "Warning: One or more tests failed!"
  exitCode = 1


exit(exitCode)

#reporter= TestReport()
#print reporter.reportResults ( results )

#html_reporter= TestReportHTML( os.path.join(args.output, "reports") )
#html_reporter.reportResults ( results )
  
#html_reporter_errors.reportResults ( results )

#c = getExitCode ( results )

#if c != 0:
#  exitCode= c
  