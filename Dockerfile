FROM python:3.13.2-slim-bookworm AS run-stage

RUN pip install requests==2.32.3 pytz==2024.1 google-cloud-storage==1.31.0 google-auth==1.33.0 Flask==3.1.0

COPY py/* .
COPY forecast_url_lookup/cities.json .

CMD ["python", "web_server.py"]
