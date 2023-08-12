<?php

if (!extension_loaded('imagick')) {
    die("The Imagick extension is not installed on the server.");
}

try {

    // Validate user input 
    if (!isset($_FILES['image']['tmp_name']) || $_FILES['image']['error'] != UPLOAD_ERR_OK) {
        throw new Exception("File upload failed.");
    }

    // Check file size
    if ($_FILES['image']['size'] > 10000000) { // 10 MB limit
        throw new Exception("File size is too large.");
    }

    // Create a unique output file name
    $outputFileName = 'output_' . uniqid() . '.jpg';

    // Create an Imagick object
    $imagick = new Imagick();

    // Read the uploaded HEIC image
    $imagick->readImage($_FILES['image']['tmp_name']);

    // Check image format
    if ($imagick->getImageFormat() != 'HEIC') {
        throw new Exception("Invalid image format.");
    }

    // Convert to JPEG
    $imagick->setImageFormat('jpg');

    // Set JPEG quality if needed
    $imagick->setImageCompressionQuality(90);

    // Write the JPEG image to a user-specific directory
    $imagick->writeImage('user_files/' . $outputFileName);

    // Clear the Imagick object to free up resources
    $imagick->clear();
    $imagick->destroy();

    echo "Conversion successful. File saved as " . $outputFileName;

} 
catch (Exception $e) {
    // Log error internally (e.g., to a file or database)
    error_log("An error occurred: " . $e->getMessage());

    // Show generic error message to the user
    echo "An error occurred while processing your request.";
}
?>
