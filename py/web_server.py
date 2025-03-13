from weathervane import weathervane
from weathervane_gcp import submit_best_weather

from http.server import BaseHTTPRequestHandler, HTTPServer

class MyRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        try:
            submit_best_weather(weathervane())
            self.send_response(200)
            self.send_header("Content-type", "text/plain")
            self.send_header("Content-Length", "2")
            self.end_headers()
            self.wfile.write(b"OK")
        except:
            self.send_response(500)

httpd = HTTPServer(('', 8080), MyRequestHandler)
httpd.serve_forever()
