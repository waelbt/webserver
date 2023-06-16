<?php
/**
* Website: www.TutorialsClass.com
**/

if(!empty($_POST["remember"])) {
	setcookie ("username",$_POST["username"],time() + (86400 * 30));
	setcookie ("password",$_POST["password"],time() + (86400 * 30));
	echo "Cookies Set Successfuly";
} else {
	setcookie("username","");
	setcookie("password","");
	echo "Cookies Not Set";
}

?>

<p><a href="page1.php"> Go to Login Page </a> </p>