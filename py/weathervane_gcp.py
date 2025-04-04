import time
from datetime import datetime
from pytz import timezone
import json
import os

# google-cloud-storage
from google.cloud import storage
# google-auth
from google.oauth2.service_account import Credentials

def get_rounded_unix_time_seconds():
    return int(round(time.time() / 3600)) * 3600

def current_time_eastern_rounded(rounded_unix_time_seconds):
    eastern_tz = timezone('US/Eastern')
    eastern_time = datetime.fromtimestamp(rounded_unix_time_seconds, eastern_tz)
    time_string = eastern_time.strftime("%I %p")
    if time_string[0] == "0":
        return time_string[1:]
    return time_string

def get_credentials():
    return json.loads(os.environ["GOOGLE_STORAGE_CREDENTIALS"])

def download_state(bucket):
    try:
        blob = bucket.blob("state.json")
        contents = blob.download_as_text()
        return json.loads(contents)
    except:
        return []

def update_state(state, obj):
    try:
        one_day_ago_unix_seconds = obj["unixTimeSeconds"] - 86400
        return [obj] + list(filter(lambda x: x["unixTimeSeconds"] >= one_day_ago_unix_seconds, state))
    except:
        return []

def upload_state(bucket, state):
    try:
        blob = bucket.blob("state.json")
        blob.upload_from_string(json.dumps(state, separators=(',', ':')))
    except:
        pass

def submit_best_weather(obj):
    rounded_unix_time_seconds = get_rounded_unix_time_seconds()
    obj["unixTimeSeconds"] = rounded_unix_time_seconds
    obj["easternTimeString"] = current_time_eastern_rounded(rounded_unix_time_seconds)
    credentials_info = get_credentials()
    credentials = Credentials.from_service_account_info(credentials_info)
    client = storage.Client(project=credentials_info["project_id"], credentials=credentials)
    bucket = client.bucket("aaronstanek-best-weather-in-america")
    state = download_state(bucket)
    state = update_state(state, obj)
    upload_state(bucket, state)
