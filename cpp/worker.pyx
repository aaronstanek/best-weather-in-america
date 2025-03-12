# distutils: language = c++

from WeatherWorker cimport WeatherWorker

cdef WeatherWorker my_worker

def read_latitude():
    return my_worker.coordinateRegister.latitude

def read_longitude():
    return my_worker.coordinateRegister.longitude

def set_location():
    my_worker.setLocation()

def add_to_history(latitude, longitude, temperature, humidity, wind, rain):
    my_worker.addToHistory(latitude, longitude, temperature, humidity, wind, rain)

def get_best_index():
    return my_worker.getBestIndex()
