

function LoadSQLModel(test_run_id, result_callback) {
  // The model is a tree implemented with nested arrays
  // Every node is a category and every leaf is a test file.
  //
  // The node members are:
  // name : string
  // children : an array of other nodes
  //
  // The leaf (ie. test file) members are:
  // name : the test filename
  // testcases : an array of testcases
  // 
  // The testcases array has one entry for every testcase in the test file
  // The testcase members are:
  // name : the testcase name
  // time : the timestamp of the test run
  // server_id : the id of the server that ran this test
  // server_name : the name of the server that ran this test
  // min_fps : float
  // avg_fps : float
  // max_fps : float
  // history : an array containing history data from other runs of the same test
  //
  // The history array contains one entry for every previous run of this testcase on every test server sorted by their timestamp
  // The history object members are:
  // time : the timestamp of the testrun
  // server_id : the id of the server that ran this test
  // server_name : the name of the server that ran this test
  // min_fps : float
  // avg_fps : float
  // max_fps : float
  
  // Connect database
  $.ajax({
      type: 'GET',
      url: 'get_results.php?test_run_id=' + test_run_id,
      dataType: 'json',
      success:result_callback,
      error: function() {
        LoadSQLModel(test_run_id, result_callback);
      },
      async: true
  });
}
   
var all_graphs = [];
var display_options =  {
  displayFramerate: false,
  properties: {
    available: ["mean", "percent"],
    selected: "mean",
    ignore: ["id", "level", "children", "data"],
  },
  servers:  {
    available: [],
    selected: [],
    current: "" // servers.current is the server that the test run we are looking at is from
  },
  testruns: {
    selected: -1
  }
};

        
var hash_server = decodeURI(location.hash.substr(location.hash.indexOf('server=')).split('&')[0].split('=')[1]).replace(/--/g, ' ');
var hash_run = location.hash.substr(location.hash.indexOf('testrun=')).split('&')[0].split('=')[1];
var hash_category = decodeURI(location.hash.substr(location.hash.indexOf('category=')).split('&')[0].split('=')[1]).replace(/--/g, ' ');
var hash_case = decodeURI(location.hash.substr(location.hash.indexOf('case=')).split('&')[0].split('=')[1]);


function getSelectedUnit() {
  if(display_options.properties.selected == "mean") {
    if(!display_options.displayFramerate)
      return "ms";
    else
      return "fps";
  } else {
    return "%";
  }
}

function getSelectedProperty(obj) {
  if(display_options.properties.selected == "mean" && display_options.displayFramerate) {
    if(obj && parseFloat(obj["mean"]) != 0)
      return (1000/parseFloat(obj["mean"])).toString();
    else
      return 0;
  } else
    return obj[display_options.properties.selected];
}


function generateDisplayOptionsList(model) {
  if(model) {
    for(var i = 0; i < model.length; i++) {
      if(model[i]) {
        if(model[i].hasOwnProperty('children'))
          generateDisplayOptionsList(model[i].children); 
        else {
          for(var j = 0; j < model[i].testcases.length; j++) {
            var testcase = model[i].testcases[j];
            if(testcase.result_type=='performance') {
              if($.inArray(testcase.server_name, display_options.servers.available) < 0) {
                display_options.servers.available.push(testcase.server_name);
              }
              if(display_options.servers.selected.length == 0) {
                display_options.servers.selected.push(testcase.server_name);
              }
              for(var propertyName in testcase.profiler_data) {
                if ($.inArray(propertyName, display_options.properties.ignore) < 0) {
                  if($.inArray(propertyName, display_options.properties.available) < 0) {
                    display_options.properties.available.push(propertyName);
                  }             
                }
              }
              for(var k = 0; k < testcase.profiler_data.data.length; ++k) {
                if($.inArray(testcase.profiler_data.data[k].server_name, display_options.servers.available) < 0) {
                  display_options.servers.available.push(testcase.profiler_data.data[k].server_name);
                }
              }
//              for(var k = 0; k < testcase.history.length; k++) {
//                if($.inArray(testcase.history[k].server_name, display_options.servers.available) < 0) {
//                  display_options.servers.available.push(testcase.history[k].server_name);
//                }        
//                for(var propertyName in testcase.history[k].profiler_data) {
//                  if ($.inArray(propertyName, display_options.properties.ignore) < 0) {
//                    if($.inArray(propertyName, display_options.properties.available) < 0) {
//                      display_options.properties.available.push(propertyName);
//                    }             
//                  }
//                }
//              }
            }
          }
        }               
      }
    }
  }
}

function refreshDisplayOptions(model) {
  $('#Option_Properties').empty();
  $('#Option_Servers').empty();
  generateDisplayOptionsList(model);
  $('#Option_Properties').append('<h3 class="Options_Header">Properties:</h3>');
  $('#Option_Servers').append('<h3 class="Options_Header">Servers:</h3>');
  for(var i = 0; i < display_options.properties.available.length; i++) {
    var rb = $('<input>');
    rb.attr('type', 'radio');    
    rb.attr('name', 'Property_RadioButton');
    if(display_options.properties.available[i] == display_options.properties.selected)
      rb.prop('checked', true);
      
    rb.data('propName', display_options.properties.available[i]);
    
    $('#Option_Properties').append(rb);
    $('#Option_Properties').append(display_options.properties.available[i] + "<br/>")
    
    rb.change(function() {
      if($(this).prop('checked')) {
        if(display_options.properties.selected != $(this).data('propName')) {
          display_options.properties.selected = $(this).data('propName');
          $('.TestResult').each(function() {
        var model = $(this);
        if($(this).data('model').result_type=="performance") {
          setTimeout(function() {
              generateGraph(model);
          }, 0);
        }
      });
        }
      }
    });        
  }
  
  for(var i = 0; i < display_options.servers.available.length; i++) {
    var cb = $('<input>');
    cb.attr('type', 'checkbox');
    if($.inArray(display_options.servers.available[i], display_options.servers.selected) > -1)
      cb.prop('checked', true);
      
    cb.data('propName', display_options.servers.available[i]);
    
    $('#Option_Servers').append(cb);
    $('#Option_Servers').append(display_options.servers.available[i] + "<br/>");
    
    cb.change(function() {
      if(!$(this).prop('checked')) {
        if(display_options.servers.selected.length > 1) {
         var index = $.inArray($(this).data('propName'), display_options.servers.selected);
          if(index > -1) { // If it's in the selected list then remove it from the list
            display_options.servers.selected.splice(index, 1); // This just removmes this one element from the list.
          }
        } else {
          $(this).prop('checked', true);
          return;
        }
      } else {
       var index = $.inArray($(this).data('propName'), display_options.servers.selected);
        if(index < 0) { // If it isn't in the selected list then add it
          display_options.servers.selected.push($(this).data('propName'));
        }     
      }
      $('.TestResult').each(function() {
        var model = $(this);
        if($(this).data('model').result_type=="performance") {
          setTimeout(function() {
              generateGraph(model);
          }, 0);
        }
      });
    });
  }
  
  var cbFPS = $("<input>");
  cbFPS.attr('type', 'checkbox');
  cbFPS.click(function(){
    if($(this).prop('checked'))
      display_options.displayFramerate = true;
    else
      display_options.displayFramerate = false;
    $('.TestResult').each(function() {
        var model = $(this);
        if($(this).data('model').result_type=="performance") {
          setTimeout(function() {
              generateGraph(model);
          }, 0);
        }
      });
  });
  $('#Option_Properties').append(cbFPS);
  $('#Option_Properties').append("Display perf data as framerate");
  
}




var highest_x = 60;
var lowest_time = 0;
var highest_time = 0;
var highest_y = 0;
function generateDatasets(testcase) {
  var stack_count = 0;
  lowest_time = 0;
  highest_time = 0;
  var datasets = [];
  for(var s = 0; s < display_options.servers.selected.length; s++) {
    var server = display_options.servers.selected[s];
    var recur = function(level, arr) {
    if(level >= 1 && display_options.displayFramerate) {
      return;
    }
//      arr.children.sort(function(a, b) { return a.data[a.data.length-1].mean > b.data[b.data.length-1].mean; });
      if(arr.hasOwnProperty("children"))
        for(var i = 0; i < arr.children.length; ++i) {
          var dataset = [];
          if(level < 3) {
            for(var j = 0; j < arr.children[i].data.length; ++j) {
              if(arr.children[i].data[j].server_name == server) {           
                if(parseFloat(getSelectedProperty(arr.children[i].data[j])) <= 0) {
                  dataset.push([new Date(arr.children[i].data[j].timestamp.replace(' ', 'T')).getTime(), null, arr.children[i].data[j]]);
                } else {
                  dataset.push([new Date(arr.children[i].data[j].timestamp.replace(' ', 'T')).getTime(), parseFloat(getSelectedProperty(arr.children[i].data[j])), arr.children[i].data[j]]);
                  if(lowest_time == 0 || new Date(arr.children[i].data[j].timestamp.replace(' ', 'T')) < new Date(lowest_time)) {
                    lowest_time = arr.children[i].data[j].timestamp.replace(' ', 'T');
                  }
                  if(highest_time == 0 || arr.children[i].data[j].timestamp.replace(' ', 'T') > highest_time) {
                    highest_time = arr.children[i].data[j].timestamp.replace(' ', 'T');
                  }                   
                }
              }
            }
            if(dataset.length > 0)    
              datasets.push({label: arr.children[i].id, stack: level + (s*4), data: dataset, server_name: server, prof_data: arr.children[i]});
          }
          if(arr.children[i].hasOwnProperty("children"))
            recur(level+1, arr.children[i]);
        }

    };
    if(testcase.hasOwnProperty("profiler_data")) {
      var root = {'children':[testcase.profiler_data]};
      recur(0, root);
    }
      
  }
  
  if(lowest_time == 0 || new Date(testcase.time) < new Date(lowest_time)) {
    lowest_time = testcase.time;
  }
  if(highest_time == 0 || new Date(testcase.time) > new Date(highest_time)) {
    highest_time = testcase.time;
  }                
  return datasets;
}


function generateGraph(div) {
  $('.TestResult_graph', div).remove();
  $('.TestResult_data', div).remove();
  var model = div.data('model');
  var graph_data = generateDatasets(model);

  var graph_div = $('<div>');
  graph_div.addClass('TestResult_graph');
  
  
  if(lowest_time == highest_time) {
    highest_time = (new Date(lowest_time).getTime()+1)
  }
  highest_time = (new Date(highest_time).getTime())
  lowest_time = (new Date(lowest_time).getTime())
  
   
  var data_div = $('<div>');
  data_div.addClass('TestResult_data');
  var data_list = $('<ul>');
  data_list.addClass('TestResult_data_list');
  data_list.append('Latest:');
  data_div.append(data_list);
  div.append(data_div);
  
   
  var graph_options = {
    series: {
      lines: {
        show: true,
        fill: true,
        steps: false,
      },
      points: {
        show: true
      }
    },
    colors: ["#7AD65C", "#5CD0D6", "#765CD6", "#D25CD6", "#D65C5C", "#D68F5C", "#D65C7E"],
    grid:  { hoverable: true }, //important! flot.tooltip requires this
    tooltip: {
      show: true,
      content: function(label, xval, yval, flotItem) {
        var total = 0;//parseFloat(yval);
        var res = flotItem.series.server_name + "<br/>" + label + ": " + getSelectedProperty(flotItem.series.data[flotItem.dataIndex][2]) + getSelectedUnit() + "<br/>";
        if(flotItem.series.prof_data.hasOwnProperty("children")) {
          for(var i = 0; i < flotItem.series.prof_data.children.length; ++i) {
            for(var d = 0; d < flotItem.series.prof_data.children[i].data.length; ++d) {
              if(flotItem.series.prof_data.children[i].data[d].timestamp.replace(' ', 'T') == flotItem.series.data[flotItem.dataIndex][2].timestamp.replace(' ', 'T')) {              
                var val = parseFloat(getSelectedProperty(flotItem.series.prof_data.children[i].data[d]));
                total += val;
                res += flotItem.series.prof_data.children[i].id + ": " + val + getSelectedUnit() +"<br/>";
                break;
              }
            }
          }
        }
        res += "stack: " + flotItem.series.stack;
        return res;
      }
    },
    xaxis: {
      mode: "time",
      min: lowest_time,
      max: highest_time,
      zoomRange: [(highest_time-lowest_time)/2, highest_time-lowest_time],
      panRange: [lowest_time, highest_time],
      tickFormatter: function (val, axis) {
          if(val) {
          var d = new Date(val);
          return d.toISOString().split("T")[0];
          }
          else return "NaN"
      }
    },
    yaxis: {
      min: 0,
      panRange: [0, (display_options.properties.selected == 'percent') ? 100 : null],
      zoomRange: [0, (display_options.properties.selected == 'percent') ? 100 : null],
    },
    zoom: {
      interactive: true
    },
    pan: {
      interactive: true
    },    
    legend: {
      show: true,
      container: data_list,
      sorted: false,
      labelFormatter: function(label, item) {
        if(item.server_name == display_options.servers.current)
          return label + " - " + getSelectedProperty(item.data[item.data.length-1][2]) + getSelectedUnit();
        else
          return null;
      }
    }
  }
		
  $('body').append(graph_div);


  // Options can be set globally. 
 
  $.plot(graph_div, graph_data, graph_options);
  
  data_div.detach();
  div.append(graph_div); 
  div.append(data_div);

           
}


function getImageBlobURL(blob, download_name) {
  var byteCharacters = atob(blob);
  var byteNumbers = new Array(byteCharacters.length);
  for (var i = 0; i < byteCharacters.length; i++) {
      byteNumbers[i] = byteCharacters.charCodeAt(i);
  }
  var arrayBufferView = new Uint8Array(byteNumbers);
  var blob = new Blob( [ arrayBufferView ], { type: "image/png" } );
  var urlCreator = window.URL || window.webkitURL;
  var imageUrl = urlCreator.createObjectURL( blob );
  var container = $('<div>');
  var link = $("<a>");
  link.addClass("image_download_link");
  link.attr("href", imageUrl);
  link.attr("target", imageUrl);
  link.attr("download", download_name);
  link.append("(Download)");
  var img = $("<img>");
  img.attr("src", imageUrl);
  img.addClass("TestResult_image");
  container.append(img);
  container.append(link);
  
  return container;
}

function generateImages(div) {
  var testcase = $(div).data('model');
  var container = $('<div>');
  container.addClass('TestResult_rendering');
  
  var download_name = testcase.name + "_" + testcase.step_name + ".png";
  var diff_download_name =  "diff_" + download_name;
  // If it succeded then show the baseline image
  // If it failed and there's no baseline then show the output and complain about the lack of a baseline
  // If it failed and there is a baseline then show the baseline, the diff and the output
  if(testcase.success == 'Y') {
    var succeeded = $("<span>");
    succeeded.addClass('test_successful');
    succeeded.append("Step successful!");
    div.append(succeeded);
    var image_container = $('<div>');
    if(testcase.baseline_image != "") {
      var image_container = $('<div>');
      image_container.addClass('TestResult_image_div'); 
      image_container.append("Baseline:</br>");
      image_container.append(getImageBlobURL(testcase.baseline_image, download_name));
      container.append(image_container);
    }
  } else { // Didn't succeed
    var succeeded = $("<span>");
    succeeded.addClass('test_failed');
    div.append(succeeded);
    
    if (testcase.diff_image == "") {
      if(testcase.output_image == "")
         succeeded.append("Step failed - No image output!");
      else
         succeeded.append("Step failed - No diff available!");
    }    
    else if (testcase.baseline_image == "")
      succeeded.append("Step failed - No baseline found!");        
    else
      succeeded.append("Step failed - Invalid output!");
        
    if(testcase.baseline_image != "") {
      var image_container = $('<div>');
      image_container.addClass('TestResult_image_div'); 
      image_container.append("Baseline:</br>");
      image_container.append(getImageBlobURL(testcase.baseline_image, download_name));
      container.append(image_container);
    }
    if(testcase.output_image != "") {
      var image_container = $('<div>');
      image_container.addClass('TestResult_image_div'); 
      image_container.append("Output:</br>");
      image_container.append(getImageBlobURL(testcase.output_image, download_name));
      container.append(image_container);
    }
    if((testcase.output_image != "") && (testcase.baseline_image != "")) {
      var image_container = $('<div>');
      image_container.addClass('TestResult_image_div'); 
      image_container.append("Diff:</br>");
      image_container.append(getImageBlobURL(testcase.diff_image, diff_download_name));
      container.append(image_container);
    } 
  }
  div.append(container);
}

function generateConsole(div) {
  var testcase = $(div).data('model');
  var container = $('<div>');
  var succeeded = $("<span>");
  div.append(succeeded);
  
  if(testcase.success == "N"){
    succeeded.addClass('test_failed');
    if(!testcase.text_baseline)
      succeeded.append("Step failed - No baseline!");
    else if(!testcase.text_output)
      succeeded.append("Step failed - No output!");
    else 
      succeeded.append("Step failed - Invalid output!");
  } else {
    succeeded.addClass('test_successful');
    succeeded.append("Step successful!");
  } 
  
  container.addClass('TestResult_console');
  var output = $('<div>');
  output.addClass('stdout_div');
  output.append("<b style='text-transform:capitalize;'>" + testcase.result_type + ":</b></br></br>");
  output.append(testcase.text_output.split('\n').join('</br>'));
  container.append(output);
  if(testcase.success == "N") {  
    if(baseline) {
      var baseline = $('<div>');
      baseline.addClass('stdout_div');
      baseline.append("<b>Baseline:</b></br></br>");
      baseline.append(testcase.text_baseline.split('\n').join('</br>'));
      container.append(baseline);
    }
    if(diff) {
    var diff = $('<div>');
      diff.addClass('stdout_div');
      diff.append("<b>Diff:</b></br></br>");
      diff.append(testcase.text_diff.split('\n').join('</br>'));
      container.append(diff);
    }
  } else {
    $('.TestStep_name', div).addClass('minimized');
  }
  div.append(container);
}

function generateError(div) {
  var testcase = $(div).data('model');
  var container = $('<div>');
  container.addClass('TestResult_error');
  
  var std = $('<div>');
  std.addClass('std_div');
  std.append(testcase.stdout.split('\n').join('</br>'));
  std.append("</br></br><b>stderr:</b></br></br>");
  std.append(testcase.stderr.split('\n').join('</br>'));
  container.append(std);
  
  div.append(container);
}


function ConstructTestCases(model, target, path) {
  var container = $('<div>');
  container.addClass('Test_Container');
  target.append(container);
  

  if(model.testcases.length > 0) {
    model.testcases.sort(function(a, b) {
    /* First check that it is the same case */
    var i = a.name.localeCompare(b.name);
    if (i == 0) { /* Same case, so return comparison of step name instead */
      return a.step_name.localeCompare(b.step_name);
    }
    else
      return a.name.localeCompare(b.name);
    }
    );
    var current_case_name = 'placeholder that shouldn\'t ever match';
    var current_step_name = 'another placeholder';
    for(var i = 0; i < model.testcases.length; i++) {
      if(model.testcases[i].name != current_case_name) {
        var case_div = $('<div>');
        case_div.addClass('TestCase');
        case_div.addClass('Category_Item');
        var case_name = $('<div>');
        case_name.addClass("TestResult_name");
        
        if(hash_server == display_options.servers.current && hash_category.startsWith(path) && hash_case == model.testcases[i].name) {
          $("input", $(target).parent()).prop("checked", true)
        } else {
          case_name.addClass("minimized");
        }
          
        case_name.click(function(){ // onclick function for toggling the presence of a minimized-class
          $(this).toggleClass("minimized");
        });
        
        var case_name_link = $("<a>");
        case_name_link.append("Case: " + model.testcases[i].name);
        case_name_link.attr('href', encodeURI("#server=" + display_options.servers.current.replace(/ /g, '--') + "&testrun=" + display_options.testruns.selected + "&category=" + path.replace(/ /g, '--') + "&case=" + model.testcases[i].name));
        case_name.append(case_name_link);
        if(model.testcases[i].success == 'Y') {
          case_name.addClass("test_successful");
        } else {
          case_name.addClass("test_failed");
        }
        
        case_div.append(case_name);        
        
        container.append(case_div);
        current_case_name = model.testcases[i].name;
      }
      
      if(model.testcases[i].success != 'Y') {
        case_name.removeClass("test_successful");
        case_name.addClass("test_failed");
      }
      
      // This is specifically for suppressing the green label
//      if(model.testcases[i].result_type == 'error' && $(".TestResult", case_div).length == 0 && (i < model.testcases.length-1 && (model.testcases[i+1].name != current_case_name)) ) {
//        case_name.removeClass("test_successful");
//        case_name.addClass("test_failed");
 //     }
            
      var step_div = $('<div>');
      step_div.addClass('TestResult');
      if(current_step_name != model.testcases[i].step_name) {
        step_div.addClass('TestResult_first_in_step');
        current_step_name = model.testcases[i].step_name;
      }
      var name_div = $('<div>');
      name_div.addClass('TestStep_name');
      name_div.click(function(){ // onclick function for toggling the presence of a minimized-class
        $(this).toggleClass("minimized");
      });
        
      if(model.testcases[i].result_type == 'error') {
        name_div.append("Testcase failed");
        name_div.addClass("test_failed");
      } else {
        name_div.append("Step: " + model.testcases[i].step_name);
      }
      step_div.append(name_div);
      step_div.data('model', model.testcases[i]); // Store the associated testCase with the div
      if(!(model.testcases[i].result_type == "performance" && !model.testcases[i].hasOwnProperty("profiler_data")))
        case_div.append(step_div);
      else {
        case_name.removeClass("test_successful");
        case_name.addClass("test_failed");
        container.removeClass("test_successful");
        container.addClass("test_failed");
        case_name.append(" - Missing performance data");
      }
    }
  }  
}


var CategoryCount = 0;

var first = false;
function ConstructList(model, target, path) {

  for (var i = 0; i < model.length; i++) {
    if(model[i]) {
      var ul = $('<ul>');
      target.append(ul);
      ul.attr('class', 'Category_Item');
      
      var label = $('<label>');
      label.attr('class', 'noselect');       
      var glyph = $('<div>');
      glyph.attr('class', 'glyph');
      glyph.html('▶');
        
      label.append(glyph);
      var name = $('<a><h3>'+model[i].name+'</h3></a>');
      name.data('category_name', path + model[i].name + "/");
      name.click(function(){
        window.location.hash = encodeURI("#server=" + display_options.servers.current.replace(/ /g, '--') + "&testrun=" + display_options.testruns.selected + "&category=" + $(this).data('category_name').replace(/ /g, '--'));
      });
      label.append(name);

      label.attr('for', 'category'+CategoryCount);
      
      var input = $('<input>');
      input.attr('type', 'checkbox');
      input.attr('id', 'category'+CategoryCount);
      input.addClass('category_list_checkbox');
      if(first || (hash_server == display_options.servers.current && hash_category.startsWith(path + model[i].name))) {
        input.prop('checked', true);
        first = false;
      }
      
      
      ul.append(input);
      ul.append(label);
      
      CategoryCount++;
      
      if(model[i].hasOwnProperty('children')) {
        ConstructList(model[i].children, ul, path + model[i].name + "/");
        if($('.test_failed', ul).length > 0)
          name.addClass('test_failed');                
        else
          name.addClass('test_successful');
      }
      else if (model[i].hasOwnProperty('testcases')) {
        if(model[i].success == undefined || (model[i].success))
          name.addClass('test_successful');
        else
          name.addClass('test_failed');
        ConstructTestCases(model[i], ul, path + model[i].name + "/");
        if($('.test_failed', ul).length > 0)
          name.addClass('test_failed');                
        else
          name.addClass('test_successful');        
        
        first = false;
      }
             
    }
  }      
}

function GetServerList() {
  var res = [];
  display_options.servers.available = [];
  display_options.servers.selected = [];
  $('#Options_Toggle').hide();
  
  // Connect database
  $.ajax({
      type: 'GET',
      url: 'get_servers.php',
      dataType: 'json',
      success: function(data) { 
        var res = data; 
        var target = $('#Servers_List');
        target.empty();
        for(var i = 0; i < res.length; ++i) {
          var div = $('<div>');
          div.addClass('TestServer');
          div.addClass("noselect");
          if(!res[i].success) {
            div.addClass('test_failed');
          }
          div.append(res[i].name);
          div.data("server_id", res[i].id);
          div.data("server_name", res[i].name);
          div.click(function(){
              hash_run = null;
            SetServer($(this).data("server_id"), $(this).data("server_name"));
                $(".Selected_Server").removeClass('Selected_Server');
                $(this).addClass('Selected_Server');
              window.location.hash = encodeURI("#server=" + display_options.servers.current.replace(/ /g, '--'));
          });
          if(hash_server == res[i].name) {
            SetServer($(div).data("server_id"), $(div).data("server_name"));
                $(".Selected_Server").removeClass('Selected_Server');
                $(div).addClass('Selected_Server');
            }
          target.append(div);
        }
      }
  });
}

function SetServer(server_id, server_name) {
  GetTestRunList(server_id);
  $(".Selected_Server").removeClass('Selected_Server');
  $(".Selected_TestRun").removeClass('Selected_TestRun');
  $(this).addClass('Selected_Server');
  $('#Categories_List').empty();
  $('#TestRuns_List').empty();
  display_options.servers.available = [server_name];
  display_options.servers.selected = [server_name];
  display_options.servers.current = server_name;
  refreshDisplayOptions();
}

function OnTestRunClick(){
  $(".TestRun").unbind("click");
  if(display_options.testruns.selected != $(this).data("test_run_id")) {
    window.location.hash = encodeURI("#server=" + display_options.servers.current.replace(/ /g, '--') + "&testrun=" + $(this).data("test_run_id"));
    hash_run = $(this).data("test_run_id");
  }
  SetTestRun($(this).data("test_run_id"));
  $(".Selected_TestRun").removeClass('Selected_TestRun');
  $(this).addClass('Selected_TestRun');
  display_options.testruns.selected = $(this).data("test_run_id");
}

function GetTestRunList(server_id) {
  // Connect database
  $.ajax({
      type: 'GET',
      url: 'get_test_runs.php?server_id=' + server_id,
      dataType: 'json',
      success: function(data) { 
        var res = data;
        $("#TestRuns").show();
        var target = $("#TestRuns_List");
        target.empty();
        for(var i = 0; i < res.length; ++i) {
          var div = $('<div>');
          div.addClass('TestRun');
          div.addClass("noselect");
          if(!res[i].success) {
            div.addClass('test_failed');
          }
          div.append(res[i].timestamp);
          div.data("test_run_id", res[i].id);
          if(res[i].has_results) {
            div.click(OnTestRunClick);
            if(hash_server == display_options.servers.current && hash_run == res[i].id) {
              SetTestRun($(div).data("test_run_id"));
              $(".Selected_TestRun").removeClass('Selected_TestRun');
              $(div).addClass('Selected_TestRun');
              display_options.testruns.selected = $(div).data("test_run_id");
            }
          } else {
            div.addClass('TestRun_NoResults');
          }
          target.append(div);
        }
      }
  });
}


function SetTestRun(test_run_id) {
  $('#Categories_List').empty();
  $('#loading_spinner_container').show();

  LoadSQLModel(test_run_id, function(data) {
    $('#Categories_List').empty();
    $('#loading_spinner_container').hide();
    model = data;
    refreshDisplayOptions(model);
    $('#Options_Toggle').show();
    first = true;
    ConstructList(model, $('#Categories_List'), "");
    $('.TestResult').each(function() {
    var testResult = $(this).data('model');
      if(testResult.result_type=="performance") {
        generateGraph($(this));
      } else if (testResult.result_type=="rendering") {
        generateImages($(this));
      } else if (testResult.result_type=="console") {
        generateConsole($(this));
      } else if (testResult.result_type=="custom") {
        generateConsole($(this));
      } else if (testResult.result_type=='error') {
        generateError($(this));
      }
    });  

    // Set up the toggle buttons.
    $('#Options_Toggle_Categories').data('collapsed', true);
    $('#Options_Toggle_Categories').prop('value', 'Expand All Categories');
    $('#Options_Toggle_Categories').unbind().click(function(){
      if($(this).data('collapsed')) {
        $('.category_list_checkbox').prop('checked', true);
        $(this).data('collapsed', false);
        $('#Options_Toggle_Categories').prop('value', 'Collapse All Categories');
      } else {
        $('.category_list_checkbox').prop('checked', false);
        $(this).data('collapsed', true);
        $('#Options_Toggle_Categories').prop('value', 'Expand All Categories');
      }
    });
    
    
    $('#Options_Toggle_Cases').data('collapsed', true);
    $('#Options_Toggle_Cases').prop('value', 'Expand Visible Cases');
    $('#Options_Toggle_Cases').unbind().click(function(){
      if($(this).data('collapsed')) {
        $('.TestResult_name:visible').removeClass('minimized');
        $(this).data('collapsed', false);
        $('#Options_Toggle_Cases').prop('value', 'Collapse Visible Cases');
      } else {
        $('.TestResult_name:visible').addClass('minimized');
        $(this).data('collapsed', true);
        $('#Options_Toggle_Cases').prop('value', 'Expand Visible Cases');
      }
    });

    $('#Options_Toggle_Steps').data('collapsed', true);
    $('#Options_Toggle_Steps').prop('value', 'Expand Visible Steps');
    $('#Options_Toggle_Steps').unbind().click(function(){
      if($(this).data('collapsed')) {
        $('.TestStep_name:visible').removeClass('minimized');
        $(this).data('collapsed', false);
        $('#Options_Toggle_Steps').prop('value', 'Collapse Visible Steps');
      } else {
        $('.TestStep_name:visible').addClass('minimized');
        $(this).data('collapsed', true);
        $('#Options_Toggle_Steps').prop('value', 'Expand Visible Steps');
      }
    });  
    $(".TestRun").unbind("click");
    $(".TestRun").click(OnTestRunClick);      
  });
  
}



var model = null;

$(document).ready(function(){
  GetServerList();
});

