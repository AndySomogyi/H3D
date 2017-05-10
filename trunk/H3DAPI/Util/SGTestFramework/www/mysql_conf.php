<?php

$dbhost = 'localhost';
$dbname = 'testserver_new';
$dbuser = 'ResultReader';
$dbpass = 'results';

$db = mysqli_connect($dbhost, $dbuser, $dbpass, $dbname);
if(mysqli_connect_errno($db)) {
  echo "Failed to connect to MySQL: " . mysqli_connect_error();
}


?>