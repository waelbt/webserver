<?php

// Set a cookie
setcookie('test_cookie', 'Hello, Cookie!', time() + 3600); // Cookie expires in 1 hour

// Get the value of the cookie, if set
$cookieValue = $_COOKIE['test_cookie'] ?? 'Cookie not set';

// Output the cookie value
echo "Cookie Value: " . $cookieValue;

?>