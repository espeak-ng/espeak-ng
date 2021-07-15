<?php 
if (isset($_POST["espeakng"])) {
    header('Vary: Origin');
    header("Access-Control-Allow-Origin: chrome-extension://<id>");
    header("Access-Control-Allow-Methods: POST");
    header("Content-Type: application/octet-stream");
    header("X-Powered-By:");
    echo passthru($_POST["espeakng"]);
    exit();
  }
