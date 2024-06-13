<?php
// Database connection parameters
// $servername = "localhost";
// $username = "root";
// $password = "";
// $dname = "elderly";

// // Create connection
// $conn = new mysqli($servername, $username, $password, $dbname);

// // Check connection
// if ($conn->connect_error) {
//     die("Connection failed: " . $conn->connect_error);
// }

include('config.php');
// Retrieve heart rate value from the form
$heartRate = $_POST['heart_rate'];

// Sanitize and validate the data (you may need to customize this based on your requirements)
$heartRate = mysqli_real_escape_string($conn, $heartRate);

// Insert heart rate data into database table
$sql = "INSERT INTO heart_rate (heart_rate) VALUES ('$heartRate')";

if ($conn->query($sql) === TRUE) {
    echo "Heart rate data saved successfully";
} else {
    echo "Error: " . $sql . "<br>" . $conn->error;
}

// Close connection
$conn->close();
?>
