import time
from datetime import datetime, timedelta
from pytz import timezone
import json
import os

# google-cloud-storage
from google.cloud import storage
# google-auth
from google.oauth2.service_account import Credentials

def current_time_eastern_rounded():
    eastern_tz = timezone('US/Eastern')
    eastern_time = datetime.now(eastern_tz)
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
        return [obj] + state[:23]
    except:
        return []

def upload_state(bucket, state):
    try:
        blob = bucket.blob("state.json")
        blob.upload_from_string(json.dumps(state, separators=(',', ':')))
    except:
        pass

def submit_best_weather(obj):
    obj["unixTimeSeconds"] = time.time()
    obj["easternTimeString"] = current_time_eastern_rounded()
    credentials_info = get_credentials()
    credentials = Credentials.from_service_account_info(credentials_info)
    client = storage.Client(project=credentials_info["project_id"], credentials=credentials)
    bucket = client.bucket("aaronstanek-best-weather-in-america")
    state = download_state(bucket)
    state = update_state(state, obj)
    upload_state(bucket, state)
