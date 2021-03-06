

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
  var url = 'get_results.php?test_run_id=' + test_run_id;
  if(display_options.fetchPerformanceData) {
    url += '&get_perf=true';
  }
  $.ajax({
      type: 'GET',
      url: url,
      dataType: 'json',
      success:result_callback,
      error: function() {
      },
      async: true
  });
}
   
var all_graphs = [];
var display_options =  {
  fetchPerformanceData: false,
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
var open_cases = decodeURI(location.hash.substr(location.hash.indexOf('opencases=')).split('&')[0].split('=')[1]).split(',');
if(open_cases[0] == "undefined") {
  open_cases = [];
}

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
    
    
    $('#baseline_result_span').html('');
    $('#baseline_loading_spinner_container').hide();
    
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
  
  var cbGetPerf = $("<input>");
  cbGetPerf.attr('type', 'checkbox');
  cbGetPerf.prop('checked', display_options.fetchPerformanceData);
  cbGetPerf.click(function(){
    if($(this).prop('checked'))
      display_options.fetchPerformanceData = true;
    else
      display_options.fetchPerformanceData = false;
  });
  $('#Option_Properties').append(cbGetPerf);
  $('#Option_Properties').append("Include performance data</br>");
  
  
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


function getImageBlobURL(id, image_type, download_name) {
  var container = $('<div>');
  var link = $("<a>");
  link.addClass("image_download_link");
  link.attr("href",  "get_image.php?type="+image_type+"&id="+id+"&name="+download_name);
  link.attr("target",  "get_image.php?type="+image_type+"&id="+id+"&name="+download_name);
  link.attr("download", download_name);
  link.append("(Download)");
  var img = $("<img>");
  img.attr("src", "get_image.php?type="+image_type+"&id="+id+"&name="+download_name);
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
    if(testcase.baseline_id != "") {
      var image_container = $('<div>');
      image_container.addClass('TestResult_image_div'); 
      image_container.append("Baseline:</br>");
      image_container.append(getImageBlobURL(testcase.baseline_id, "baseline", download_name));
      container.append(image_container);
    }
  } else { // Didn't succeed
    var succeeded = $("<span>");
    succeeded.addClass('test_failed');
    if(testcase.new_failure == 'Y') {
      succeeded.addClass('test_failed_new');
    }
    div.append(succeeded);
    
/*    if (testcase.diff_image == "") {
      if(testcase.output_image == "")
         succeeded.append("Step failed - No image output!");
      else
         succeeded.append("Step failed - No diff available!");
    }    
    else */if (testcase.baseline_id == "")
      succeeded.append("Step failed - No baseline found!");        
    else
      succeeded.append("Step failed - Invalid output!");
        
    if(testcase.baseline_id != "") {
      var image_container = $('<div>');
      image_container.addClass('TestResult_image_div'); 
      image_container.append("Baseline:</br>");
      image_container.append(getImageBlobURL(testcase.baseline_id, "baseline",download_name));
      container.append(image_container);
    }
    
      var image_container = $('<div>');
      image_container.addClass('TestResult_image_div'); 
      image_container.append("Output:</br>");
      image_container.append(getImageBlobURL(testcase.id, "result", download_name));
      container.append(image_container);
    
    if((testcase.output_image != "") && (testcase.baseline_image != "")) {
      var image_container = $('<div>');
      image_container.addClass('TestResult_image_div'); 
      image_container.append("Diff:</br>");
      image_container.append(getImageBlobURL(testcase.id, "diff", diff_download_name));
      container.append(image_container);

      // Add baseline update checkbox here
      var cb = $('<input>Include in baseline update</input>');
      cb.attr('type', 'checkbox');
      cb.addClass('baselineUpdateCheckbox');
      cb.data('model', testcase);
      container.append("<br />");
      container.append(cb);
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
    if(testcase.new_failure == 'Y') {
      succeeded.addClass('test_failed_new');
    }
    if(!testcase.hasOwnProperty("text_baseline"))
      succeeded.append("Step failed - No baseline!");
    else if(!testcase.hasOwnProperty("text_output"))
      succeeded.append("Step failed - No output!");
    else {
      succeeded.append("Step failed - Invalid output!");
    }
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
    if(testcase.new_failure == 'Y') {
      succeeded.addClass('test_failed_new');
    } 
    if(testcase.hasOwnProperty("text_baseline")) {
      var baseline = $('<div>');
      baseline.addClass('stdout_div');
      baseline.append("<b>Baseline:</b></br></br>");
      baseline.append(testcase.text_baseline.split('\n').join('</br>'));
      container.append(baseline);
    }
    if(testcase.hasOwnProperty("text_diff")) {
    var diff = $('<div>');
      diff.addClass('stdout_div');
      diff.append("<b>Diff:</b></br></br>");
      diff.append(testcase.text_diff.split('\n').join('</br>'));
      container.append(diff);
    }
    
    if(testcase.hasOwnProperty("text_baseline") && testcase.hasOwnProperty("text_diff")) {
      // Add baseline update checkbox here
      var cb = $('<input>Include in baseline update</input>');
      cb.attr('type', 'checkbox');
      cb.addClass('baselineUpdateCheckbox');
      cb.data('model', testcase);
      container.append("<br />");
      container.append(cb);    
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
  
  if(testcase.new_failure == 'Y') {
    container.addClass('test_failed_new');
  } 
  
  div.append(container);
}


function ConstructTestCases(model, target, path) {
  var container = $('<div>');
  container.addClass('Test_Container');
  
  if (model.description) {
    var description = $("<div>");
    description.addClass('TestCase');
    description.addClass('description');
    description.append(model.description);
    container.append(description);
  }

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
          $("input", $(target).parent()).prop("checked", true);
          if(open_cases.indexOf(model.testcases[i].id) < 0) {
            open_cases.push(model.testcases[i].id);
          }
        } else {
          if(open_cases.indexOf(model.testcases[i].id) > -1) {
            $("input", $(target).parent()).prop("checked", true);
          } else {
            case_name.addClass("minimized");
          }
        }
          
        $(case_name).data("testcase", model.testcases[i]);
        case_name.click(function(){ // onclick function for toggling the presence of a minimized-class
          $(this).toggleClass("minimized");
          var testcase = $(this).data("testcase");
          if($(this).hasClass("minimized")) {
            if(open_cases.indexOf(testcase.id) > -1) {
              open_cases.splice(open_cases.indexOf(testcase.id), 1);
            }
          } else if(open_cases.indexOf(testcase.id) < 0){
            open_cases.push(testcase.id);
          }
          window.location.hash = window.location.hash.split('#')[0] + encodeURI("#server=" + display_options.servers.current.replace(/ /g, '--') + "&testrun=" + display_options.testruns.selected + "&category=" + path.replace(/ /g, '--') + "&case=" + testcase.name)          
          URLUpdateOpenCases();
        });
        
        var case_name_link = $("<a>");
        case_name_link.append("Case: " + model.testcases[i].name);
        //case_name_link.attr('href', );
        case_name.append(case_name_link);
        if(model.testcases[i].success == 'Y') {
          case_name.addClass("test_successful");
        } else {
          case_name.addClass("test_failed");
          if(model.testcases[i].new_failure == 'Y') {
            case_name.addClass('test_failed_new');
          }
        }
        
        case_div.append(case_name);

        if (model.testcases[i].svn_url_x3d && model.testcases[i].svn_url_x3d != "") {
          var case_svn_x3d_link = $("<a>");
          case_svn_x3d_link.append("x3d");
          case_svn_x3d_link.attr('href', encodeURI(model.testcases[i].svn_url_x3d));
          case_div.append(case_svn_x3d_link);
          case_div.append(" ");
        }
        if (model.testcases[i].svn_url_script && model.testcases[i].svn_url_script != "") {
          var case_svn_script_link = $("<a>");
          case_svn_script_link.append("script");
          case_svn_script_link.attr('href', encodeURI(model.testcases[i].svn_url_script));
          case_div.append(case_svn_script_link);
        }
        
        container.append(case_div);
        current_case_name = model.testcases[i].name;
      }
      
      if(model.testcases[i].success != 'Y') {
        case_name.removeClass("test_successful");
        case_name.addClass("test_failed");
        if(model.testcases[i].new_failure == 'Y') {
          case_name.addClass('test_failed_new');
        }
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
        if(model.testcases[i].step_name == '') {
          name_div.append("Testcase failed");
        } else {
          name_div.append(model.testcases[i].step_name);
        }
        name_div.addClass("test_failed");
        if(model.testcases[i].new_failure == 'Y') {
          name_div.addClass('test_failed_new');
        }
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
      
      if (model[i].hasOwnProperty('children')) {
        if (model[i].description) {
            var description_container = $('<ul>');
			description_container.addClass("Category_Item");
		    var description = $("<div>");
		    description.addClass('TestCase');
		    description.append(model[i].description);
            description_container.append(description);
		    ul.append(description_container);
        }
        ConstructList(model[i].children, ul, path + model[i].name + "/");
        if($('.test_failed', ul).length > 0) {
          name.addClass('test_failed');
          if($('.test_failed_new', ul).length > 0) {
            name.addClass("test_failed_new");
          }
        }         
        else
          name.addClass('test_successful');
      }
      else if (model[i].hasOwnProperty('testcases')) {
        if(model[i].success == undefined || (model[i].success))
          name.addClass('test_successful');
        else
          name.addClass('test_failed');
        ConstructTestCases(model[i], ul, path + model[i].name + "/");
        if($('.test_failed', ul).length > 0) {
          name.addClass('test_failed');
          if($('.test_failed_new', ul).length > 0) {
            name.addClass("test_failed_new");
          }
        }            
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
  $('#Operations').hide();
  $('#baseline_result_span').html('');
  $('#baseline_loading_spinner_container').hide();
  
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
            if(res[i].new_failure) {
              div.addClass('test_failed_new');
            }
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
              open_cases = [];
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
  $('tr', $('#Summary_Table')).not('#Summary_Table_Header').remove();  
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
  open_cases = [];
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
            if(res[i].new_failure) {
              div.addClass('test_failed_new');
            }
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
  $('#Operations').show();
  $('tr', $('#Summary_Table')).not('#Summary_Table_Header').remove();
  
  $.ajax({
    type: 'GET',
    url: 'get_summary.php?test_run_id=' + test_run_id,
    dataType: 'json',
    success: function(data) {
        $('tr', $('#Summary_Table')).not('#Summary_Table_Header').remove();
        var all_succeeded = true;
        var total_success = 0;
        var total_fail = 0;
        var total_step_fail = 0;
        var total_error = 0;
        var total_cases = 0;
        for(var i = 0; i < Object.keys(data).length; ++i) {
          var key = Object.keys(data)[i];
          var row = $('<tr>');
          
          var cell = $('<td>');
          cell.html(key);
          row.append(cell);
          
          var cell = $('<td>');
          cell.html(data[key].successful.toString());
          row.append(cell);
          all_succeeded = all_succeeded && data[key].successful;
          
          var cell = $('<td>');
          cell.html(data[key].success_count);
          row.append(cell);
          total_success += data[key].success_count;
          
          var cell = $('<td>');
          cell.html(data[key].fail_count);
          row.append(cell);
          total_fail += data[key].fail_count;
          
          var cell = $('<td>');
          cell.html(data[key].step_fail_count);
          row.append(cell);
          total_step_fail += data[key].step_fail_count;
          
          var cell = $('<td>');
          cell.html(data[key].error_count);
          row.append(cell);
          total_error += data[key].error_count;
          
          var cell = $('<td>');
          cell.html(data[key].success_count + data[key].fail_count);
          row.append(cell);
          total_cases += data[key].success_count + data[key].fail_count;
          
          var cell = $('<td>');
          cell.html(((data[key].success_count/(data[key].success_count + data[key].fail_count))*100).toFixed(2) + "%");
          row.append(cell);
          
          if($('#Summary_Table > tbody'))
            $('#Summary_Table > tbody').append(row);
          else
            $('#Summary_Table').append(row);
        }
      if(Object.keys(data).length > 0) {
        var row = $('<tr>');
        
        var cell = $('<td>');
        cell.html("Total");
        row.append(cell);
        
        var cell = $('<td>');
        cell.html(all_succeeded.toString());
        row.append(cell);
        
        var cell = $('<td>');
        cell.html(total_success);
        row.append(cell);
        
        var cell = $('<td>');
        cell.html(total_fail);
        row.append(cell);
        
        var cell = $('<td>');
        cell.html(total_step_fail);
        row.append(cell);
        
        var cell = $('<td>');
        cell.html(total_error);
        row.append(cell);
        
        var cell = $('<td>');
        cell.html(total_cases);
        row.append(cell);
        
        var cell = $('<td>');
        cell.html(((total_success/total_cases)*100).toFixed(2) + "%");
        
        row.append(cell);
        
        if($('#Summary_Table > tbody'))
          $('#Summary_Table > tbody').append(row);
        else
          $('#Summary_Table').append(row);      
      }
      }
    });
  
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
    
    $('.baselineUpdateCheckbox').prop('checked', false);
    
    $('#baseline_result_span').html('');
    $('#baseline_loading_spinner_container').hide();
          
    $('#Operations_Update_Baselines').unbind().click(function() {
      var cases = [];
      var commit_message = "";
      while (commit_message == "") {
        commit_message = prompt("Please enter a commit message describing the update.");
        if(commit_message == "") {
          alert("Please do not enter an empty commit message!");
        }
      }
      if(commit_message == null) {
        alert("No baseline update was done.");
      } else {
        $('#baseline_result_span').html('Updating, please wait...');
        $('#baseline_loading_spinner_container').show();
        var checkboxes = $('.baselineUpdateCheckbox:checked');
        for(var i = 0; i < checkboxes.length; ++i) {
          var data = $(checkboxes[i]).data('model');
          cases.push({'file_id': data.file_id,
                     'case_id': data.case_id,
                     'step_id': data.step_id,
                     'validation_type' : data.result_type});
        }
        $.ajax({
            type: 'POST',
            url: 'update_baselines.php?test_run_id=' + test_run_id,
            data: {
              'data' : JSON.stringify({cases}),
              'commit_message' : commit_message  
            },
            success: function(data) {            
              $('#baseline_loading_spinner_container').hide();
              $('#baseline_result_span').html(data);
            },
            error: function(data) {
              $('#baseline_loading_spinner_container').hide();
              $('#baseline_result_span').html('Error when submitting command: ' + data.responseText);
            },
            async: true
        });      
      }
    });
    
    // Set up the toggle buttons.
    $('#Options_Toggle_Only_Show_Failed').data('only_showing_failed', false);
    $('#Options_Toggle_Only_Show_Failed').prop('value', 'Only Show Failed Cases');
    $('#Options_Toggle_Only_Show_Failed').unbind().click(function(){
      if($(this).data('only_showing_failed')) {
        $(".TestCase:not(:has(.test_failed)):not(.description)").show(); // Showing the test cases
        $('ul:not(:has(.test_failed))').show() // Showing the test categories
        $(this).data('only_showing_failed', false);
        $('#Options_Toggle_Only_Show_Failed').prop('value', 'Only Show Failed Cases');
      } else {
        $(".TestCase:not(:has(.test_failed)):not(.description)").hide(); // Hiding the test cases
        $('ul:not(:has(.test_failed))').hide() // Hiding the test categories
        $(this).data('only_showing_failed', true);
        $('#Options_Toggle_Only_Show_Failed').prop('value', 'Show All Cases');
      }
    });
    
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

    $('#Options_Collapse_Steps').prop('value', 'Collapse Visible Steps');
    $('#Options_Collapse_Steps').click(function(){
      $('.TestStep_name:visible').addClass('minimized');
    });
    $('#Options_Expand_Steps').prop('value', 'Expand Visible Steps');
    $('#Options_Expand_Steps').click(function(){
      $('.TestStep_name:visible').removeClass('minimized');
    });

    $(".TestRun").unbind("click");
    $(".TestRun").click(OnTestRunClick);      
  });
  
}



var model = null;

$(document).ready(function(){
  GetServerList();
});



function URLUpdateOpenCases() {
  window.location.hash = window.location.hash.replace(/[&]opencases[=][0-9,]+/g, "");
  if(open_cases.length > 0) {
    window.location.hash = window.location.hash + "&opencases=" + open_cases.join();
  }
}