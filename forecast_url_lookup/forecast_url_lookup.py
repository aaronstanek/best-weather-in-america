import requests
import time
import json

def read_wikipedia_list():
    with open("wikipedia_list.txt", "r") as file:
        return file.read()

def parse_city_name(s):
    return s.split('[')[0]

def parse_coordinates(s):
    coordinate_list = []
    float_characters_list = []
    for c in s:
        n = ord(c)
        if (n >= 48 and n <= 57) or c == '.':
            float_characters_list.append(c)
        if c == '°':
            coordinate_list.append("".join(float_characters_list))
            float_characters_list = []
    if len(coordinate_list) != 2:
        raise Exception(f"coordinate_list does not have expected length: {coordinate_list} {float_characters_list} {s}")
    return f"{coordinate_list[0]},-{coordinate_list[1]}"

def parse_wikipedia_list(lst):
    lines_in = lst.split('\n')
    lines_out = []
    for line in lines_in:
        line_sections = line.split('\t')
        if len(line_sections) < 3:
            continue
        lines_out.append([
            parse_city_name(line_sections[0]),
            line_sections[1],
            parse_coordinates(line_sections[-1])
        ])
    return lines_out

def add_smaller_cities(lst):
    smaller_cities = [
        ["Wilmington", "DE", "39.7458,-75.5467"],
        ["Portland", "ME", "43.66,-70.255"],
        ["Burlington", "VT", "44.4758,-73.2119"],
        ["Charleston", "WV", "38.3498,-81.6326"],
        ["Cheyenne", "WY", "41.14,-104.8203"]
    ]
    return lst + smaller_cities

request_headers = {
    "User-Agent": "(aaronstanek.com, amr.stanek@gmail.com)"
}

def url_retry_wrapper(url):
    retries_remaining = 3
    while retries_remaining > 0:
        try:
            response = requests.get(url, headers=request_headers).text
            obj = json.loads(response)
            return obj["properties"]["forecastHourly"]
        except:
            retries_remaining -= 1
            time.sleep(0.1)
    raise Exception(f"Unable to load url {url}")

def query_forecast_urls(lst_in):
    lst_out = []
    for city in lst_in:
        print(city[0], city[1])
        lst_out.append([
            city[0],
            city[1],
            url_retry_wrapper(f"https://api.weather.gov/points/{city[2]}")
        ])
    return lst_out

def save_cities_list(lst):
    with open("cities.json", "w") as file:
        json.dump(lst, file, separators=(',',':'))

def main():
    x = read_wikipedia_list()
    x = parse_wikipedia_list(x)
    x = add_smaller_cities(x)
    x = query_forecast_urls(x)
    save_cities_list(x)

main()
