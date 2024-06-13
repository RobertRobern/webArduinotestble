<?php
// Database connection parameters
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "elderly";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Retrieve heart rate value from the form
$tempRate = $_POST['tempRateValue'];

// Sanitize and validate the data (you may need to customize this based on your requirements)
$tempRate = mysqli_real_escape_string($conn, $tempRate);

// Insert heart rate data into database table
$sql = "INSERT INTO temperature (temperature) VALUES ('$tempRate')";

echo $tempRate;
if ($conn->query($sql) === TRUE) {
    echo "Heart rate data saved successfully";
} else {
    echo "Error: " . $sql . "<br>" . $conn->error;
}

// Close connection
$conn->close();
?>
