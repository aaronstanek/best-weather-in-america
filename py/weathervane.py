import worker
import weathervane_gcp

import time
import threading
import queue
import json

import requests

request_headers = {
    "User-Agent": "(aaronstanek.com, amr.stanek@gmail.com)"
}

def url_retry_wrapper(url):
    retries_remaining = 3
    while retries_remaining > 0:
        try:
            return requests.get(url, headers=request_headers).text
        except:
            retries_remaining -= 1
            time.sleep(0.1)
    return ""

def url_reader(to_main, to_url_reader):
    v = {}
    while True:
        v["params"] = to_url_reader.get(block=True, timeout=None)
        v["response"] = url_retry_wrapper(v["params"]["url"])
        to_main.put({
            "ok": True,
            "params": v["params"],
            "response": v["response"]
        }, block=True, timeout=None)
        v = {}

def nice_coordinate(n):
    if n < 0:
        return f"-{nice_coordinate(-n)}"
    s = str(int(round(n * 1e4)))
    while len(s) < 5:
        s = "0" + s
    return f"{s[:-4]}.{s[-4:]}"

def decode_points_api(response):
    try:
        obj = json.loads(response)
        return {
            "forecast_url": obj["properties"]["forecastHourly"],
            "city": obj["properties"]["relativeLocation"]["properties"]["city"],
            "state": obj["properties"]["relativeLocation"]["properties"]["state"]
        }
    except:
        return None

def decode_forecast_api(response):
    try:
        obj = json.loads(response)
        return {
            "temperature": obj["properties"]["periods"][0]["temperature"],
            "humidity": obj["properties"]["periods"][0]["relativeHumidity"]["value"],
            "wind": int(obj["properties"]["periods"][0]["windSpeed"].split(" ")[0]),
            "rain": obj["properties"]["periods"][0]["probabilityOfPrecipitation"]["value"]
        }
    except:
        None

def weathervane():
    to_main = queue.Queue()
    to_url_reader = queue.Queue()
    url_reader_count = 2
    threads = []
    for i in range(url_reader_count):
        threads.append(threading.Thread(target=url_reader, args=[to_main, to_url_reader], daemon=True))
        threads[-1].start()
    target_count = 44
    history = []
    outstanding_requests = 0
    failed_requests = 0
    while len(history) < target_count:
        if failed_requests > 10:
            return None
        while outstanding_requests < min(url_reader_count, target_count - len(history)):
            worker.set_location()
            latitude = worker.read_latitude()
            longitude = worker.read_longitude()
            to_url_reader.put({
                "url": f"https://api.weather.gov/points/{nice_coordinate(latitude)},{nice_coordinate(longitude)}",
                "type": "points",
                "latitude": latitude,
                "longitude": longitude
            }, block=True, timeout=None)
            outstanding_requests += 1
        web_data = to_main.get(block=True, timeout=None)
        if not web_data["ok"]:
            del web_data
            outstanding_requests -= 1
            failed_requests += 1
            continue
        if web_data["params"]["type"] == "points":
            location = decode_points_api(web_data["response"])
            if location is None:
                del web_data
                outstanding_requests -= 1
                failed_requests += 1
                continue
            to_url_reader.put({
                "url": location["forecast_url"],
                "type": "forecast",
                "latitude": web_data["params"]["latitude"],
                "longitude": web_data["params"]["longitude"],
                "city": location["city"],
                "state": location["state"]
            }, block=True, timeout=None)
            del web_data
        else:
            forecast = decode_forecast_api(web_data["response"])
            if forecast is None:
                del web_data
                outstanding_requests -= 1
                failed_requests += 1
                continue
            history.append({
                "city": web_data["params"]["city"],
                "state": web_data["params"]["state"],
                "temperature": f"{forecast["temperature"]}°F",
                "humidity": f"{forecast["humidity"]}%",
                "wind": f"{forecast["wind"]} mph",
                "rain": f"{forecast["rain"]}% chance"
            })
            worker.add_to_history(
                web_data["params"]["latitude"],
                web_data["params"]["longitude"],
                forecast["temperature"],
                forecast["humidity"],
                forecast["wind"],
                forecast["rain"]
            )
            del web_data
            outstanding_requests -= 1
    return history[worker.get_best_index()]

t1 = time.time()
best_location = weathervane()
weathervane_gcp.submit_best_weather(best_location)
t2 = time.time()
print(f"Delta {t2-t1}")
print(best_location)
