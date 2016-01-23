<?php

  // send content
  $file_content = "";
  $telem_content = "";
  for($i=0; $i<30; $i++) {
    $file_content = file_get_contents("status_mjpeg.txt");
    $telem_content  = file_get_contents("status_telemetry.txt");
      
    if($file_content != $_GET["last"] || $telem_content == "telemetry") break;
    usleep(100000);
  }
    
  if ($telem_content == "telemetry")
  {
      file_put_contents("status_telemetry.txt", "no telemetry");
      echo $telem_content;
  }
  else
  {
    touch("status_mjpeg.txt");
    echo $file_content;
  }

?>
