function LoadSQLModel() {
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
  // fps_min : float
  // fps_avg : float
  // fps_max : float
  // history : an array containing history data from other runs of the same test
  //
  // The history array contains one entry for every previous run of this testcase on every test server sorted by their timestamp
  // The history object members are:
  // time : the timestamp of the testrun
  // server_id : the id of the server that ran this test
  // server_name : the name of the server that ran this test
  // fps_min : float
  // fps_avg : float
  // fps_max : float
  
  var res = []
  
  // Connect database
  $.ajax({
      type: 'GET',
      url: 'get_results.php',
      dataType: 'json',
      success: function(data) { res = data; },
      async: false
  });
  
  return res;
}
   
var all_graphs = [];
var display_options =  {
  properties: {
    available: ["fps_min", "fps_avg", "fps_max"],
    selected: ["fps_min", "fps_avg", "fps_max"],
    ignore: ["name", "time", "history", "server_id", "server_name"],
  },
  servers:  {
    available: [],
    selected: [],
  }
};

function generateDisplayOptionsList(model) {
  for(var i = 0; i < model.length; i++) {
    if(model[i].hasOwnProperty('children'))
      generateDisplayOptionsList(model[i].children); 
    else {
      for(var j = 0; j < model[i].testcases.length; j++) {
      var testcase = model[i].testcases[j];
        if($.inArray(testcase.server_name, display_options.servers.available) < 0) {
          display_options.servers.available.push(testcase.server_name);
        }
        if(display_options.servers.selected.length == 0) {
          display_options.servers.selected.push(testcase.server_name);
        }
        for(var propertyName in testcase) {
          if ($.inArray(propertyName, display_options.properties.ignore) < 0) {
            if($.inArray(propertyName, display_options.properties.available) < 0) {
              display_options.properties.available.push(propertyName);
            }             
          }
        }
        for(var k = 0; k < testcase.history.length; k++) {
        if($.inArray(testcase.history[k].server_name, display_options.servers.available) < 0) {
          display_options.servers.available.push(testcase.history[k].server_name);
        }        
          for(var propertyName in testcase.history[k]) {
            if ($.inArray(propertyName, display_options.properties.ignore) < 0) {
              if($.inArray(propertyName, display_options.properties.available) < 0) {
                display_options.properties.available.push(propertyName);
              }             
            }
          }
        }        
      }
    }               
  }
}

function refreshDisplayOptions(model) {
  $('#Properties').empty();
  $('#Servers').empty();
  generateDisplayOptionsList(model);
  $('#Properties').append('<h3>Properties:</h3>');
  $('#Servers').append('<h3>Servers:</h3>');
  for(var i = 0; i < display_options.properties.available.length; i++) {
    var cb = $('<input>');
    cb.attr('type', 'checkbox');    
    if($.inArray(display_options.properties.available[i], display_options.properties.selected) > -1)
      cb.prop('checked', true);
      
    cb.data('propName', display_options.properties.available[i]);
    
    $('#Properties').append(cb);
    $('#Properties').append(display_options.properties.available[i] + "<br/>")
    
    cb.change(function() {
      if(!$(this).prop('checked')) {
        if(display_options.properties.selected.length > 1) {
         var index = $.inArray($(this).data('propName'), display_options.properties.selected);
          if(index > -1) { // If it's in the selected list then remove it from the list
            display_options.properties.selected.splice(index, 1); // This just removes this one element from the list.
          }
        } else {
          $(this).prop('checked', true);
          return;
        }
      } else {
       var index = $.inArray($(this).data('propName'), display_options.properties.selected);
        if(index < 0) { // If it isn't in the selected list then add it
          display_options.properties.selected.push($(this).data('propName'));
        }     
      }
      $('.TestCase').each(function() { generateGraph($(this));}); // regenerates all the graphs
    });    
    
  }
  for(var i = 0; i < display_options.servers.available.length; i++) {
    var cb = $('<input>');
    cb.attr('type', 'checkbox');
    if($.inArray(display_options.servers.available[i], display_options.servers.selected) > -1)
      cb.prop('checked', true);
      
    cb.data('propName', display_options.servers.available[i]);
    
    $('#Servers').append(cb);
    $('#Servers').append(display_options.servers.available[i] + "<br/>");
    
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
      $('.TestCase').each(function() { generateGraph($(this));}); // regenerates all the graphs
    });
  }
}


function generateDatasets(testcase) {
  var datasets = [];
  var hue = 80;
  
  for(var s = 0; s < display_options.servers.selected.length; s++) {
    var server = display_options.servers.selected[s];
    for(var propertyName in testcase) {
      if(($.inArray(propertyName, display_options.properties.selected) > -1) || (propertyName == "server_name") || (propertyName == "time")) {
        var color = "hsla("+Math.round(hue)+", 60%, 60%";
        var config = {
        label: propertyName, 
        fill: false,
        backgroundColor: color+", 0.2)",
        borderColor: color+", 1)",
        pointBorderColor: color+", 1)",
        pointBackgroundColor: "#fff",
        pointBorderWidth: 1,
        pointHoverRadius: 5,
        pointHoverBackgroundColor: color+", 1)",
        pointHoverBorderColor: color+", 1)",
        pointHoverBorderWidth: 2,
        data: [],
        labels : [],
        server_name : server,
        highest_y_value : 60
        };
        hue = (hue + 20 + Math.random()*40) % 360;
        
        var highest = 60;
        var new_dataset = [];
        if(!isNaN(testcase[propertyName]))
          highest = Math.max(testcase[propertyName], highest);
        if(testcase.hasOwnProperty("history")) {
          for(var j = 0; j < testcase.history.length; j++) {
            if((testcase.history[j].server_name == server)) {
              config.labels.push(testcase.history[j].time);
              if(!isNaN(testcase.history[j][propertyName]))
                highest = Math.max(testcase.history[j][propertyName], highest);
              new_dataset.push({x: testcase.history[j].time, y: testcase.history[j][propertyName]});
            }
          }
        }
        if(testcase.server_name == server) {
          new_dataset.push({x: testcase.time, y: testcase[propertyName]});
          config.labels.push(testcase.time);
          if(!isNaN(highest))
            highest = Math.max(highest, config.highest_y_value);
        }
        config.highest_y_value = highest;
        config.data = new_dataset;
        if(config.labels.length > 0) { // Only add this to the dataset list if it actually contains something.
          // But first make sure every previous dataset has matching points. And that this has matching points for every previous dataset.
          // Algorithm: For every previously existing dataset:
          //   Store an index into their dataset label array and my dataset label array.
          //   If one of the two indexes has reached the end of its list then add all remaining timestamps in the other list to the first list
          //   If the two timestamps at these indexes are equal then increment both indexes.
          //   If the two timestamps these indexes point at aren't equal then increment the earliest index until it no longer is lower or until it goes beyond the end of the list. Then add the other timestamp before that index.
          //   Increment the index of the earlier timestamp.
          for(var prev = 0; prev < datasets.length; prev++) {
            var theirs = 0;
            var len_t = datasets[prev].labels.length;
            var mine = 0;
            var len_m = config.labels.length;
            while((theirs < len_t) || (mine < len_m)) {
              if(theirs < len_t) {
                var their_timestamp = Date.parse(datasets[prev].labels[theirs]);
              } else {
                var their_timestamp = Date.now();
              }
              if(mine < len_m) {
                var my_timestamp = Date.parse(config.labels[mine]);
              } else {
                var my_timestamp = Date.now();
              }
                            
              if(their_timestamp == my_timestamp) {
                theirs++;
                mine++;
              } else if(their_timestamp < my_timestamp) {
                config.labels.splice(mine, 0, datasets[prev].labels[theirs]);
                if(config.label == "server_name")
                  config.data.splice(mine, 0, {x: datasets[prev].labels[theirs], y: config.server_name});
                else
                  config.data.splice(mine, 0, {x: datasets[prev].labels[theirs]});
                theirs++;
                mine++;
              } else if(their_timestamp > my_timestamp) {
                datasets[prev].labels.splice(theirs, 0, config.labels[mine]);
                if(datasets[prev].label == "server_name")
                  datasets[prev].data.splice(theirs, 0, {x: config.labels[mine], y: datasets[prev].server_name});
                else
                  datasets[prev].data.splice(theirs, 0, {x: config.labels[mine]});
                theirs++;
                mine++;
              } else {
                //Something went wrong if we end up here.
                break;                
              }
              
            }
          }
          datasets.push(config);
        }
     }
    }
  }
  return datasets;
}

function ConstructTestCases(model, target) {
  var container = $('<div>');
  container.addClass('Test_Container');
  for(var i = 0; i < model.length; i++) {
    var div = $('<div>');
    div.addClass('TestCase');
    var name_div = $('<div>');
    name_div.addClass('TestCase_name');
    name_div.append(model[i].name);
    div.append(name_div);
    div.data('model', model[i]); // Store the associated testCase with the div
    container.append(div);
  }
  target.append(container);
}


function generateGraph(div) {
  $('.TestCase_graph', div).remove();
  $('.TestCase_data', div).remove();
  var model = div.data('model');
  var graph_data = generateDatasets(model);
  var highest = 60;
  for(dataset in graph_data) {
    highest = Math.max(highest, dataset.highest_y_value);
  }
    var graph_div = $('<div>');
    graph_div.addClass('TestCase_graph');        
    
    var graph_canvas = $('<canvas>');
    graph_div.append(graph_canvas);

  var graph_config = 
    {
      type : 'line',
      data: {
        labels: graph_data[0].labels,
        datasets: graph_data
        },
      options:{
        maintainAspectRatio: true,
        responsive: true,
        scaleShowLabels: true,
        legendCallback : function(controller){
        var data = controller.data;
          var res = "<p>Legend:</br>";
          var server = data.datasets[0].server_name;
            res += "<span>[" + server + "]</span></br>";
          for (var k=0; k<data.datasets.length; k++){
            if(data.datasets[k].server_name != server) {
              server = data.datasets[k].server_name;
              res += "<span>[" + server + "]</span></br>";
            }
            if($.inArray(data.datasets[k].label, display_options.properties.ignore) < 0) {
              res += "<span style=\"color:" +data.datasets[k].pointBorderColor+"\">";
              if(data.datasets[k].label){
                res += data.datasets[k].label;
              }
              res += "</span></br>";                  
            }
          }
          res += "</p>";							
          return res;
        },
          tooltips: {
              mode: 'label',
              callbacks : {
                label: function(tooltipItem, data) {
                  return data.datasets[tooltipItem.datasetIndex].label + ': ' + data.datasets[tooltipItem.datasetIndex].data[tooltipItem.index].y;
                }
              }
          },        
          scales: {
              xAxes: [{
                  display: true,
                  ticks : {
                    userCallback: function(value) {
                      if(value){
                        return value;//.substring(0, value.indexOf(' '));
                      } else {
                        return '';
                      }
                    }
                  }
              }],
              yAxes: [{
                  display: true,
                  scaleLabel: {
                      show: true,
                      labelString: 'Value'
                  },
                  ticks : {
                    beginAtZero: true,
                    suggestedMin: 0,
                    suggestedMax: highest,
                    userCallback: function(value) {
                    return value;
                    }
                  }
              }]
          },
      }
    }       
  
  // Chart.js will fail if the canvas isn't visible in the document when Chart() is called. For that reason we append graph_div to the body, call Chart() and then detach it so we can put it in div afterwards.
  $('body').append(graph_div);

  model.chart = Chart.Line(graph_canvas[0].getContext('2d'), graph_config);

  graph_div.detach();
 
  div.append(graph_div); 
  var data_div = $('<div>');
  data_div.addClass('TestCase_data');
  var data_list = $('<ul>');
  data_list.addClass('TestCase_data_list');
  data_list.append('Latest:');
  for(var i = 0; i < display_options.properties.selected.length; i++) {
    var prop = display_options.properties.selected[i];
    var line = '<li>' + prop + ': ';
    if(model.hasOwnProperty(prop))
      line += model[prop];
    else
      line += 'N/A';
    line += '</li>';
    data_list.append(line);
  }

           
  var getprops = function getAllProperties(obj){
    var allProps = []
      , curr = obj
    do{
        var props = Object.getOwnPropertyNames(curr)
        props.forEach(function(prop){
            if (allProps.indexOf(prop) === -1)
                allProps.push(prop)
        })
    }  while(curr = Object.getPrototypeOf(curr))
    return allProps
  }
  
  data_list.append(model.chart.generateLegend());
  data_div.append(data_list);
  div.append(data_div);
}




var CategoryCount = 0;

  var first = true;
function ConstructList(model, target) {
  for (var i = 0; i < model.length; i++) {
    var ul = $('<ul>');
    ul.attr('class', 'Category_Item');
    
    var label = $('<label>');
    label.attr('class', 'noselect');       
    var glyph = $('<div>');
    glyph.attr('class', 'glyph');
    glyph.html('▶');
      
    label.append(glyph);
    label.append('<h3>'+model[i].name+'</h3>');
    label.attr('for', 'category'+CategoryCount);
    
    var input = $('<input>');
    input.attr('type', 'checkbox');
    input.attr('id', 'category'+CategoryCount);
    input.addClass('category_list_checkbox');
    if(first)
      input.prop('checked', true);
    ul.append(input);
    ul.append(label);
    
    CategoryCount++;
    
    if(model[i].hasOwnProperty('children'))
      ConstructList(model[i].children, ul);
    else if (model[i].hasOwnProperty('testcases')) {
      ConstructTestCases(model[i].testcases, ul);
      first = false;
    }
           
    target.append(ul);
  }      
}



var model = LoadSQLModel();


$(document).ready(function(){
  $('#Options').draggable();
  model.push(model[0]);
  refreshDisplayOptions(model);
  ConstructList(model, $('#Categories'));
  $('.TestCase').each(function() { generateGraph($(this));}); // We make sure generateGraph gets called
  
});    

          
