<?php

// Get the query string
$query = $_SERVER['QUERY_STRING'];

// Extract the path from the query string
$path = parse_url($query, PHP_URL_PATH);

// Output the path
echo "Path: " . $path;

// You can perform additional operations on the path here

?>