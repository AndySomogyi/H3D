<?php

error_reporting(E_ALL);

require('mysql_conf.php');

ini_set('memory_limit', '512M');


// Helper function for executing an sql query and running the results
function fetch_result($db, $query, $single_row = False) {
  if(!$fetch_result = mysqli_query($db, $query)) {
		die("ERROR: " . mysqli_error($db));
	}	
  $fetched_data = array();

  if($single_row) {
    if($row = mysqli_fetch_assoc($fetch_result)) {
      return $row;
    } else {
      return null;
    }
  } else {
    while($row = mysqli_fetch_assoc($fetch_result)) {
      array_push($fetched_data, $row);
    }
  return $fetched_data;
  }
}

function do_svn_command($command) {
  global $svnuser, $svnpass;
  $svn_command = 'svn --non-interactive --no-auth-cache --username ' . $svnuser . ' --password ' . $svnpass . ' ' . $command . " 2>&1";
  $svn_output = shell_exec($svn_command);
  return $svn_output;
}

$has_valid_svn_path = false;

for($i = 0; $i < count($svnpaths); ++$i) {
  if(is_bool(strpos(do_svn_command("info " . $svnpaths[$i]), '(Not a valid URL)'))) {
    $has_valid_svn_path = true;
  }
}
if(!$has_valid_svn_path) {
    echo "No valid svn paths in \$svn_paths, check server configuration";
    exit;
}

// since svn commands are case-sensitive, we have a function that tests the four most likely capitalizations of .TestDef, just to be extra sure we won't get problems with it later.
// returns an empty string if no matching case was found, which also could mean that the provided svnpath is invalid.
function get_testdef_path($svnpath, $testdef_filename) {
  $testdef_path = $testdef_filename . ".TestDef";
  if(is_bool(strpos(do_svn_command("info " . $svnpath . $testdef_path), '(Not a valid URL)'))) {
    return $testdef_path;
  }
  $testdef_path = $testdef_filename . ".testdef";
  if(is_bool(strpos(do_svn_command("info " . $svnpath . $testdef_path), '(Not a valid URL)'))) {
    return $testdef_path;
  }  
  $testdef_path = $testdef_filename . ".Testdef";
  if(is_bool(strpos(do_svn_command("info " . $svnpath . $testdef_path), '(Not a valid URL)'))) {
    return $testdef_path;
  }
  $testdef_path = $testdef_filename . ".testDef";
  if(is_bool(strpos(do_svn_command("info " . $svnpath . $testdef_path), '(Not a valid URL)'))) {
    return $testdef_path;
  }
  return "";
}




function UpdateBaseline($test_run_id, $test_case_id, $test_step_id, $test_file_id, $test_validation_type) {
  // Process should be as follows:
  // Identify the path of the baseline for the test.
  //   Get the TestDef path from the database using the file id
  //   Do an SVN checkout of the TestDef file
  //   Parse the TestDef file to find the matching test case and get the baseline folder location
  //   Generate the correct file path from the baseline folder path the names of the test case and step.
  // Do an svn checkout of the directory containing the file.
  // Replace its contents with the output of the test, as identified by its validation type and its case and step IDs.
  // Store the path to the file's directory in a global array so we can batch our commits.

  global $db, $svnpaths, $folders_to_commit;
  // Get the TestDef filename
  $testdef_filename = fetch_result($db, "SELECT filename FROM test_files WHERE id = " . mysqli_real_escape_string($db, $test_file_id) . ";", true);
  if($testdef_filename == null) {
    echo "Invalid testfile id.";
    return;
  } else {
   $testdef_filename = $testdef_filename['filename'];
  }

  // Also get the test case name
  $test_case_name = fetch_result($db, "SELECT case_name FROM test_cases WHERE id = " .  mysqli_real_escape_string($db, $test_case_id) . ";", true);
  if($test_case_name == null) {
    echo "Invalid testcase id.";
    return;
  } else {
    $test_case_name = $test_case_name['case_name'];
  }

  // Also get the test step name
  $test_step_name = fetch_result($db, "SELECT step_name FROM test_steps WHERE id = " .  mysqli_real_escape_string($db, $test_step_id) . ";", true);
  if($test_step_name == null) {
    echo "Invalid teststep id.";
    return;
  } else {
  $test_step_name = $test_step_name['step_name'];
  }

  
  $active_svn_path = "";
  for($i = 0; $i < count($svnpaths); ++$i) {
    $testdef_filename_temp = get_testdef_path($svnpaths[$i], $testdef_filename);
    if($testdef_filename_temp != "") {
      $testdef_filename = $testdef_filename_temp;
      $testdef_path = $svnpaths[$i] . $testdef_filename;
      $active_svn_path = $svnpaths[$i];
      break;
    }
  }
  if($active_svn_path == "") {
    echo "Unable to find matching testdef in any of the paths in \$svn_paths!";
    return;
  } else {
    echo "active svn path is " . $active_svn_path;
  }
  

  // Make sure we have a checkout of the testdef directory
  do_svn_command("checkout " . dirname($testdef_path) . " temp/" . dirname($testdef_filename));

  // Open the testdef file (which is a form of ini file)
  $previous_error_flags = error_reporting();
  error_reporting($previous_error_flags & ~E_DEPRECATED);
  $testdef = parse_ini_file("temp/" . $testdef_filename, true);
  error_reporting($previous_error_flags);
  $baseline_folder = str_replace("\\", "/", $testdef[$test_case_name]['baseline folder']);
  $baseline_path = dirname($testdef_filename) . "/" . $baseline_folder . "/";

  if($test_validation_type == "rendering") {
    $baseline_file = $baseline_path . $test_case_name . "_" . $test_step_name . ".png";
  } else if ($test_validation_type == "console") {
    $baseline_file = $baseline_path . $test_case_name . "_" . $test_step_name . "_console.txt";
  } else if ($test_validation_type == "custom") {
    $baseline_file = $baseline_path . $test_case_name . "_" . $test_step_name . "_custom.txt";
  }

  // Do an svn_checkout of the baseline folder, just in case it was not included in the checkout of the testdef folder
  //do_svn_command("checkout " . $active_svn_path . $baseline_path . " temp/" . $baseline_path);


  // Replace its contents with the output of the test, as identified by its validation type and its case and step IDs.

  if($test_validation_type == "rendering") {
    // Get the output file from the specified test run so we can write it to $baseline_file
    $output_image = fetch_result($db, "SELECT output_image FROM rendering_results WHERE test_run_id = " . mysqli_real_escape_string($db, $test_run_id) . " AND case_id = " . mysqli_real_escape_string($db, $test_case_id) . " AND step_id = " . mysqli_real_escape_string($db, $test_step_id) . ";", true);
    if($output_image == null) {
      echo "Unable to find output image for the specified test run/case/step.";
      return;
    } else {
      $output_image = $output_image['output_image'];
    }
    file_put_contents("temp/" . $baseline_file, $output_image);
    // Now queue up the commit
    if(!array_key_exists($baseline_path, $folders_to_commit)) {
      $folders_to_commit[$baseline_path] = 1;
    } else {
      $folders_to_commit[$baseline_path] = $folders_to_commit[$baseline_path] + 1;
    }
    
  } else if($test_validation_type == "console") {
    // Get the console output from the specified test run so we can write it to $baseline_file
    $output_console = fetch_result($db, "SELECT output FROM console_results WHERE test_run_id = " . mysqli_real_escape_string($db, $test_run_id) . " AND case_id = " . mysqli_real_escape_string($db, $test_case_id) . " AND step_id = " . mysqli_real_escape_string($db, $test_step_id) . ";", true);
    if($output_console == null) {
      echo "Unable to find console output for the specified test run/case/step.";
      return;
    } else {
      $output_console = $output_console['output'];
    }
    file_put_contents("temp/" . $baseline_file, $output_console);
    // Now queue up the commit
    if(!array_key_exists($baseline_path, $folders_to_commit)) {
      $folders_to_commit[$baseline_path] = 1;
    } else {
      $folders_to_commit[$baseline_path] = $folders_to_commit[$baseline_path] + 1;
    }
    
  } else if($test_validation_type == "custom") {
    // Get the custom output from the specified test run so we can write it to $baseline_file
    $output_custom = fetch_result($db, "SELECT output FROM custom_results WHERE test_run_id = " . mysqli_real_escape_string($db, $test_run_id) . " AND case_id = " . mysqli_real_escape_string($db, $test_case_id) . " AND step_id = " . mysqli_real_escape_string($db, $test_step_id) . ";", true);
    if($output_custom == null) {
      echo "Unable to find custom output for the specified test run/case/step.";
      return;
    } else {
      $output_custom = $output_custom['output'];
    }
    file_put_contents("temp/" . $baseline_file, $output_custom);
    // Now queue up the commit
    if(!array_key_exists($baseline_path, $folders_to_commit)) {
      $folders_to_commit[$baseline_path] = 1;
    } else {
      $folders_to_commit[$baseline_path] = $folders_to_commit[$baseline_path] + 1;
    }
  }

}



// Now to actually do everything!

// Doing an svn revert jut to be extra sure that our working copy is clean
echo do_svn_command('revert -R temp/*');

// Read the submitted cases.
$cases_to_update = json_decode($_POST['data'], true)['cases'];

$folders_to_commit = array();
$test_run_id = $_GET['test_run_id'];

for($i = 0; $i < count($cases_to_update); ++$i) {
  $case = $cases_to_update[$i];
  UpdateBaseline($test_run_id, $case['case_id'], $case['step_id'], $case['file_id'], $case['validation_type']);
}

$folder_keys = array_keys($folders_to_commit);
foreach($folder_keys as $folder) {
  do_svn_command("commit temp/" . $folder . ' -m "Updating ' . $folders_to_commit[$folder] . ' baseline(s) from web UI"');
  echo "Committed " . $folders_to_commit[$folder] . " file(s) to " . $folder . "</br>";
}
?>