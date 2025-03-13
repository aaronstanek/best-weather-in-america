FROM python:3.13.2-slim-bookworm AS build-stage

RUN apt update
RUN apt install -y gcc=4:12.2.0-3 g++=4:12.2.0-3
RUN pip install cython==3.0.11 setuptools==72.1.0

COPY cpp/* .
RUN python setup.py build_ext --inplace

FROM python:3.13.2-slim-bookworm AS run-stage

RUN pip install requests==2.32.3 pytz==2024.1 google-cloud-storage==1.31.0 google-auth==1.33.0

COPY py/* .
COPY --from=build-stage *.so .

CMD ["python", "web_server.py"]
