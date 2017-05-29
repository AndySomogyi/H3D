<?php

$dbhost = 'localhost';
$dbname = 'testserver_new';
$dbuser = 'ResultReader';
$dbpass = 'results';
$svnuser = ''; // Needs to have read and write access to $svnpath.
$svnpass = ''; 
$svnpath = ''; // Needs to be a full svn url for checking out the root directory of the tests that are shown on this page. Necessary for baseline uploads.

$db = mysqli_connect($dbhost, $dbuser, $dbpass, $dbname);
if(mysqli_connect_errno($db)) {
  echo "Failed to connect to MySQL: " . mysqli_connect_error();
}


?>