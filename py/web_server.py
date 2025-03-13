from weathervane import weathervane
from weathervane_gcp import submit_best_weather

from flask import Flask, Response

app = Flask(__name__)

@app.route("/")
def hello():
    try:
        submit_best_weather(weathervane())
        return Response("OK", status=200, mimetype="text/plain")
    except:
        return Response("ERR", status=500, mimetype="text/plain")

if __name__ == "__main__":
    app.run(debug=True, port=8080, host='0.0.0.0')
