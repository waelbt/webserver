<?php
    // Set a cookie
    setcookie('test_cookie', 'vendetta', time() + (86400 * 30), "/"); // 86400 = 1 day
    
    // Check if cookie is set
    if(!isset($_COOKIE['test_cookie'])) {
        echo "Cookie named 'test_cookie' is not set!";
    } else {
        echo "Cookie 'test_cookie' is set!<br>";
        echo "Value is: " . $_COOKIE['test_cookie'];
    }
?>
