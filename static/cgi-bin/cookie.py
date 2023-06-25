from http import cookies
import os

# Create a cookie
cookie = cookies.SimpleCookie()
cookie['test_cookie'] = 'test_value'
cookie['test_cookie']['path'] = '/'
cookie['test_cookie']['max-age'] = 86400 * 30  # 30 days

print(cookie, '\r')  # Sends the HTTP header

# Get the cookie value
if 'HTTP_COOKIE' in os.environ:
    received_cookies = cookies.SimpleCookie(os.environ['HTTP_COOKIE'])
    if 'test_cookie' in received_cookies:
        print("Content-Type: text/html\r")
        print("\r")
        print(f"Cookie 'test_cookie' is set!<br>")
        print(f"Value is: {received_cookies['test_cookie'].value}")
else:
	print("Content-Type: text/html\r")
	print("\r")
	print("Cookie named 'test_cookie' is not set!")
