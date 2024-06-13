<?php
$servername = "localhost";
$username = "root";
$password = "";
$dname = "elderly";

// Create connection
$database = new mysqli($servername, $username, $password, $dname);

// Check connection
if ($database->connect_error) {
  die("Connection failed: " . $database->connect_error);
}
// echo "Connected successfully";
?>