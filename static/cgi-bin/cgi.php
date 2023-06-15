#!/usr/bin/php
<?php
// Send the HTTP headers
echo "Status: 200\r\n";
echo "Content-type: text/html\r\n\r\n";


// Generate the HTML response
echo "<!DOCTYPE html>\n";
echo "<html>\n";
echo "<head>\n";
echo "<title>PHP CGI Script</title>\n";
echo "</head>\n";
echo "<body>\n";
echo "<h1>Hello, this is a PHP CGI script!</h1>\n";

// Display some server and request information
echo "<h2>Server Information</h2>\n";
echo "<pre>\n";
print_r($_SERVER);
echo "</pre>\n";

echo "<h2>Request Information</h2>\n";
echo "<pre>\n";
print_r($_REQUEST);
echo "</pre>\n";

echo "</body>\n";
echo "</html>\n";
?>
