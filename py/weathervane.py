import time
import threading
import queue
import json
import random

import requests

def get_cities():
    with open("cities.json", "r") as file:
        return json.load(file)

def seed_url_queue(to_url_reader):
    cities = get_cities()
    random.shuffle(cities)
    for city in cities:
        to_url_reader.put({
            "city": city[0],
            "state": city[1],
            "url": city[2]
        }, block=True, timeout=None)
    return len(cities)

request_headers = {
    "User-Agent": "(aaronstanek.com, amr.stanek@gmail.com)"
}

def url_retry_wrapper(url):
    retries_remaining = 3
    while retries_remaining > 0:
        try:
            return requests.get(url, headers=request_headers, timeout=3).text
        except:
            retries_remaining -= 1
            time.sleep(0.1)
    return ""

def url_reader(to_main, to_url_reader):
    while True:
        element = to_url_reader.get(block=True, timeout=None)
        response = url_retry_wrapper(element["url"])
        to_main.put({
            "city": element["city"],
            "state": element["state"],
            "response": response
        }, block=True, timeout=None)

def decode_forecast_api(response):
    try:
        obj = json.loads(response)
        period = obj["properties"]["periods"][1]
        return {
            "temperature": period["temperature"],
            "humidity": period["relativeHumidity"]["value"],
            "wind": int(period["windSpeed"].split(" ")[0]),
            "rain": period["probabilityOfPrecipitation"]["value"]
        }
    except:
        None

def compute_badness_score(forecast):
    score = 0.0002753038612347609 * (forecast["temperature"] - 70) ** 2
    score += 0.00010046751836904414 * (forecast["humidity"] - 50) ** 2
    score += 0.00014421437280512476 * (forecast["wind"] - 5) ** 2
    score += 0.07129086834186059 * forecast["rain"] ** 0.3739738949262469
    return score

def weathervane():
    to_main = queue.Queue()
    to_url_reader = queue.Queue()
    city_count = seed_url_queue(to_url_reader)
    url_reader_count = 4
    threads = []
    for i in range(url_reader_count):
        threads.append(threading.Thread(target=url_reader, args=[to_main, to_url_reader], daemon=True))
        threads[-1].start()
    forecasts_processed_count = 0
    best_score_so_far = None
    best_city_so_far = None
    while forecasts_processed_count < city_count:
        web_data = to_main.get(block=True, timeout=None)
        forecast = decode_forecast_api(web_data["response"])
        forecasts_processed_count += 1
        if forecast is None:
            continue
        score = compute_badness_score(forecast)
        if best_score_so_far is None or score < best_score_so_far:
            best_score_so_far = score
            best_city_so_far = {
                "city": web_data["city"],
                "state": web_data["state"],
                "temperature": f"{forecast["temperature"]}°F",
                "humidity": f"{forecast["humidity"]}%",
                "wind": f"{forecast["wind"]} mph",
                "rain": f"{forecast["rain"]}% chance"
            }
    if best_city_so_far is None:
        raise Exception("Unable to find best weather conditions")
    return best_city_so_far
