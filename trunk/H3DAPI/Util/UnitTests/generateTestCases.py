﻿''' This script parses the specified file looking for a list of H3D nodes and field names, each containing information about the valid values for that field, and generates test x3d files and scripts that test those values or value ranges for every field and ensures no error is thrown for any of them.

The format of the file with the node and field names should be a series of entries like the following, one for every node, with one set of field/type/values properties for every field on that node:

node=NameOfTheNode

field=fieldname
type=float/string/int
values=(for string): comma, separated, list, of, valid, values (for numerical value): lowest_allowed_value, highest_allowed_value

field=otherfield
type=again, float/string/int
values=same as before


Please note that you should not specify both the values and range properties. You should only specify one of the two.
Also note that you need to ensure you always put field, type and values in order.
'''

test_script_header = '''
from UnitTestUtil import *
from H3DInterface import *
from H3DUtils import *

\"""
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
\"""


'''


x3d_template = '''<?xml version="1.0" encoding="utf-8"?>
<X3D profile='H3DAPI' version='1.4'>
  <head>
    <meta name='title' content='%s'/>
    <meta name='description' content='Generated x3d file for unit testing'/>
    <meta name='author' content='SenseGraphics AB, 2006-2016'/>
  </head>

  <Scene>
    <Viewpoint position='0 0 0' />
    <%s DEF='N'/>
  </Scene>
</X3D>

''' 

color_list = [['red', '255 0 0'],
              ['green', '0 255 0'],
              ['blue', '0 0 255'],
              ['yellow', '255 255 0'],
              ['cyan', '0 255 255'],
              ['purple', '255 0 255']]

import argparse, os, traceback, sys
from math import *

parser = argparse.ArgumentParser(description='Generates Test Cases for UnitTest')
parser.add_argument('--workingDir', dest='workingDir', 
                    help='The directory that any relative paths start from and where you want the output files to end up in',
                    default=os.getcwd())
parser.add_argument('--caseDir', dest='caseDir',
                    help='The directory that all the .CaseDef files are located in',
                    default='.')
args = parser.parse_known_args()[0]



inf = sys.float_info.max
neginf = sys.float_info.min

def writeFloatField(py, field_name, name_suffix, field_value, should_write_color=False, color=None):
#  print "Writing function " + "def test" + field_name + name_suffix + "():\n"
  py.write("@screenshot(start_time=0.1)\n")
  py.write("def test" + field_name + name_suffix + "():\n")
  
  py.write("  #Code that sets " + field_name + " to value " + str(eval(field_value)) + "\n")
  if color and should_write_color:
    py.write("  #Also initiates the color " + color[0] + " for this series of tests \n")
  py.write("  node = getNamedNode('N')\n")
  if color and should_write_color:
    py.write("  node.getField('color').setValueFromString('" + color[1] + "')\n\n")
  py.write("  node.getField('" + field_name + "').setValue(" + str(eval(field_value)) + ")\n\n")


def parseFloatField(py, node, field, should_write_color, color=None):
  value = field['values']
  if value[0] != field['default']:
    if color:
      writeFloatField(py, field['name'], "_" + color[0] + "_lowest", value[0], should_write_color, color)
      should_write_color = False
    else:
      writeFloatField(py, field['name'], "_lowest", value[0])
  
  if value[1] != field['default']:
    if color:
      writeFloatField(py, field['name'], "_" + color[0] + "_highest", value[1], should_write_color, color)
      should_write_color = False
    else:
      writeFloatField(py, field['name'], "_highest", value[1])

 
  if color:
    writeFloatField(py, field['name'], "_" + color[0] + "_default", field['default'], should_write_color, color)
    should_write_color = False
  else:
    writeFloatField(py, field['name'], "_default", field['default'])
          
  py.write("\n\n\n")


def writeVec3fField(py, name, name_suffix, x, y, z):
  py.write("@screenshot(start_time=0.1)\n")
  py.write("def test" + name + name_suffix + "():\n")
  
  py.write("  #Code that sets " + name + " to value " + str(x) + " " + str(y) + " " + str(z) + "\n")
  py.write("  node = getNamedNode('N')\n")
  py.write("  node.getField('" + name + "').setValueFromString('" + str(x) + " " + str(y) + " " + str(z) + "')\n\n")  


def parseVec3fField(py, node, field):
  value = [eval(val) for val in field['values']]
  default = [eval(val) for val in field['default'].split(" ")]
  if value[0] <= 1 and value[1] >= 1:
    writeVec3fField(py, field['name'], "_normal_x",      1,  0,  0)
    writeVec3fField(py, field['name'], "_normal_y",      0,  1,  0)
    writeVec3fField(py, field['name'], "_normal_z",      0 , 0,  1)
    writeVec3fField(py, field['name'], "_normal_xy",     1,  1,  0)
    writeVec3fField(py, field['name'], "_normal_xz",     1,  0,  1)
    writeVec3fField(py, field['name'], "_normal_yz",     0,  1,  1)
    writeVec3fField(py, field['name'], "_normal_xyz",    1,  1,  1)  
  
  if value[0] <= -1 and value[1] >= -1:
    writeVec3fField(py, field['name'], "_normal_Invx",    -1,  0,  0)
    writeVec3fField(py, field['name'], "_normal_Invxy",   -1,  1,  0)
    writeVec3fField(py, field['name'], "_normal_xInvy",    1, -1,  0)
    writeVec3fField(py, field['name'], "_normal_InvxInvy",  -1, -1,  0)
    writeVec3fField(py, field['name'], "_normal_Invxz",   -1,  0,  1)
    writeVec3fField(py, field['name'], "_normal_xInvz",    1,  0, -1)
    writeVec3fField(py, field['name'], "_normal_InvxInvz",  -1,  0, -1)
    writeVec3fField(py, field['name'], "_normal_Invxyz",  -1,  1,  1)
    writeVec3fField(py, field['name'], "_normal_xInvyz",   1,  -1, 1)
    writeVec3fField(py, field['name'], "_normal_xyInvz",   1,  1, -1)
    writeVec3fField(py, field['name'], "_normal_InvxInvyz", -1, -1,  1)
    writeVec3fField(py, field['name'], "_normal_InvxyInvz", -1,  1, -1)
    writeVec3fField(py, field['name'], "_normal_xInvyInvz",  1, -1, -1)
    writeVec3fField(py, field['name'], "_normal_InvxInvyInvz",-1, -1, -1)
    writeVec3fField(py, field['name'], "_normal_Invy",     0, -1,  0)
    writeVec3fField(py, field['name'], "_normal_Invyz",    0, -1,  1)
    writeVec3fField(py, field['name'], "_normal_yInvz",    0,  1, -1)
    writeVec3fField(py, field['name'], "_normal_InvyInvz",   0, -1, -1)
    writeVec3fField(py, field['name'], "_normal_Invz",      0 , 0,  -1)
  
  
  writeVec3fField(py, field['name'], "_lowest_x", value[0], default[1],   default[2])
  writeVec3fField(py, field['name'], "_lowest_xy", value[0], value[0],   default[2])
  writeVec3fField(py, field['name'], "_lowest_xz", value[0], default[1],  value[0])
  writeVec3fField(py, field['name'], "_lowest_xyz", value[0], value[0],   value[0])
  writeVec3fField(py, field['name'], "_lowest_y", default[0] , value[0],  default[2])
  writeVec3fField(py, field['name'], "_lowest_yz", default[0] , value[0],  value[0])
  writeVec3fField(py, field['name'], "_lowest_y", default[0] , default[1],  value[0])
  
  
  writeVec3fField(py, field['name'], "_highest_x", value[1], default[1],   default[2])
  writeVec3fField(py, field['name'], "_highest_xy", value[1], value[1],   default[2])
  writeVec3fField(py, field['name'], "_highest_xz", value[1], default[1],  value[1])
  writeVec3fField(py, field['name'], "_highest_xyz", value[1], value[1],   value[1])
  writeVec3fField(py, field['name'], "_highest_y", default[0] , value[1],  default[2])
  writeVec3fField(py, field['name'], "_highest_yz", default[0] , value[1],  value[1])
  writeVec3fField(py, field['name'], "_highest_y", default[0] , default[1],  value[1])
  
  writeVec3fField(py, field['name'], "_default", default[0], default[1], default[2])
    

def next_line(file):
  line = file.readline()
  print "reading line: " + line
  while line and (line.strip() == '' or line[0] == '#'):
    line = file.readline()
    print "reading line: " + line
  if line == '':
    return None
  else:
    return line.strip()




for file in os.listdir(os.path.join(os.path.abspath(args.workingDir), args.caseDir)):
  base, ext= os.path.splitext(file)
  if ext.lower() == '.casedef':
    print base
    excepted = False
    try:
      file = open(os.path.join(os.path.abspath(args.workingDir), args.caseDir, file), 'r')
      line = ''
      nodes_to_generate = []
      line = next_line(file)
      while line != None:
        node_name = line.split('=')
        if node_name[0] != 'node':
          print "Error! Expected node, got " + '='.join(node_name)
          exit()
          break
        
        generate_colors = False
        
        line = next_line(file)
        if line.split('=')[0] == 'color':
          generate_colors = line.split('=')[1].lower() == 'true'
          line = next_line(file)
        
        fields_to_generate = []
        while line != None and line.split('=')[0] != 'node':
          field_name = line.split('=')
          if field_name[0] != 'field':
            print "Error! Expected field, got " + '='.join(field_name)
            exit()
            break
          
          line = next_line(file)
          field_type = line.split('=')
          if field_type[0] != 'type':
            print "Error! Expected type, got " + '='.join(field_type)
            exit()
            break
            
          line = next_line(file)
          field_default = line.split('=')
          if field_default[0] != 'default':
            print "Error! Expected default, got " + '='.join(field_default)
            exit()
            break
                        
          line = next_line(file)
          field_values_line = line.split('=')
          field_values = field_values_line[1].split(',')
          if field_values_line[0] != 'values':
            print "Error! Expected values, got " + '='.join(field_values_line)
            exit()
            break
          
          field = {}
          field['name'] = field_name[1]
          field['type'] = field_type[1]
          field['default'] = field_default[1]
          field['values'] = field_values
          fields_to_generate.append( field )
          line = next_line(file)
          
        node = {}
        node['name'] = node_name[1]
        node['fields'] = fields_to_generate
        node['generate_colors'] = generate_colors
        nodes_to_generate.append( node )      
      
      file.close()
#      print base + " contained: \r\n"
#      print nodes_to_generate
      for node in nodes_to_generate:
        if not os.path.exists(os.path.join(os.path.abspath(args.workingDir), node['name'] + '.x3d')):
          x3d = open(os.path.join(os.path.abspath(args.workingDir), node['name'] + '.x3d'), 'w')
          x3d.write(x3d_template % (node['name'] + ".x3d", node['name']))
          
        py = open(os.path.join(os.path.abspath(args.workingDir), node['name'] + '.py'), 'w')
        py.write(test_script_header)
     
        if node['generate_colors']:
          for color in color_list:
            first = True
            for field in node['fields']:
#              print "writing " + ", ".join(color)
              value = field['values']
              type = field['type'].lower()
              if type == 'float':
                parseFloatField(py, node, field, first, color)
                first = False
              elif type == 'vec3f':
                parseVec3fField(py, node, field)
        else:
          for field in node['fields']:
            type = field['type'].lower()
            if type == 'float':
              parseFloatField(py, node, field, False)
            elif type == 'vec3f':
              parseVec3fField(py, node, field)
          
        py.close()
        
    except Exception as e:
      print "Ignoring " + base + " because: " + str(e) + " with trace: "
      excepted = True
    if excepted:
      traceback.print_exc()
        