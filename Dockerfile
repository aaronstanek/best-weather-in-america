FROM python:3.13.2-slim-bookworm AS build-stage

RUN apt update
RUN apt install -y gcc=4:12.2.0-3 g++=4:12.2.0-3

RUN pip install cython==3.0.11
RUN pip install setuptools==72.1.0

COPY cpp/* .
RUN python setup.py build_ext --inplace
