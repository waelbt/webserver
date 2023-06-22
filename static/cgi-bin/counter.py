#!/usr/bin/env python3
import os
import http.cookies as Cookie
import cgi

print("Content-Type: text/html\n")
print("\r\n\r\n")

form = cgi.FieldStorage()
if 'increment' in form:
    if 'HTTP_COOKIE' in os.environ:
        cookie_string = os.environ.get('HTTP_COOKIE')
        c = Cookie.SimpleCookie()
        c.load(cookie_string)
        try:
            counter = int(c['counter'].value)
            counter += 1
            print(f'Set-Cookie: counter={counter}')
        except (Cookie.CookieError, KeyError):
            print('Set-Cookie: counter=1')
    else:
        print('Set-Cookie: counter=1')

else:
    if 'HTTP_COOKIE' in os.environ:
        cookie_string = os.environ.get('HTTP_COOKIE')
        c = Cookie.SimpleCookie()
        c.load(cookie_string)

        try:
            counter = int(c['counter'].value)
        except (Cookie.CookieError, KeyError):
            counter = 0
    else:
        counter = 0


print("""
<!DOCTYPE html>
<html>
<head>
    <title>Increment Counter Example</title>
</head>
<body>
    <h1>Counter: {}</h1>
    <form method="post">
        <button type="submit" name="increment">Increment Counter</button>
    </form>
</body>
</html>
""".format(counter))
