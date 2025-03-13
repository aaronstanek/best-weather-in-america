from weathervane import weathervane
from weathervane_gcp import submit_best_weather

from http.server import BaseHTTPRequestHandler, HTTPServer

class MyRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        try:
            submit_best_weather(weathervane())
            self.send_response(200)
        except:
            self.send_response(500)

httpd = HTTPServer(('', 8080), MyRequestHandler)
httpd.serve_forever()
