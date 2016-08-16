<?php
require('mysql_conf.php');




$image_query = "SELECT %s FROM %s WHERE id=?";

$whatever = "SELECT rendering_results.output_image,rendering_results.diff_image,image AS baseline_image
 FROM   rendering_results
        left join rendering_baselines AS baseline
               ON rendering_results.file_id = baseline.file_id
                 AND rendering_results.case_id = baseline.case_id
                    AND rendering_results.step_id = baseline.step_id
 WHERE  test_runs.id = %d
 GROUP  BY case_id,file_id,step_name";
 
 
if($_GET['type']=="result") {
  echo fetch_image($db, $image_query, "rendering_results", "output_image");
} else if ($_GET['type']=="diff") {
  echo fetch_image($db, $image_query, "rendering_results", "diff_image");
} else if ($_GET['type']=="baseline") {
  echo fetch_image($db, $image_query, "rendering_baselines", "image");
}

function fetch_image($db, $query, $table_name, $column_name) {
  if($image_statement = $db->prepare(sprintf($query, $column_name, $table_name))) {
    $image_statement->bind_param("i", $_GET['id']);
    $image_statement->execute();
    $image_statement->bind_result($image);
    if($image_statement->fetch()) {
    header('Content-Type: image/png');
      echo $image;
    } else {
      die("ERROR, unable to fetch: " . mysqli_error($db));
    }     
  } else {
   die("ERROR with query: " . mysqli_error($db));
 }
}

?>