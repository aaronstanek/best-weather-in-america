cdef extern from "randomGenerator.h":
    pass

cdef extern from "randomGenerator.cpp":
    pass

cdef extern from "geometry.h":
    pass

cdef extern from "boundaries.cpp":
    pass

cdef extern from "boundaries.h":
    pass

cdef extern from "randomPointInUs.cpp":
    pass

cdef extern from "randomPointInUs.h":
    pass

cdef extern from "WeatherWorker.cpp":
    pass

cdef extern from "WeatherWorker.h":
    cdef struct Coordinate:
        double latitude
        double longitude
    cdef cppclass WeatherWorker:
        Coordinate coordinateRegister
        WeatherWorker() except +
        void setLocation()
        void addToHistory(double, double, double, double, double, double) except +
        size_t getBestIndex() except +
