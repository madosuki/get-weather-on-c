# get-weather-on-c
  
This software is just get current data of openweathermap with json.

# Build
```
git submodule init && git submodule update
```
then
```
mkdir -p build && cd build && cmake .. && cmake --build .
```

# Dependents
[libhttp_connector](https://github.com/madosuki/libhttp_connector/)  
[libopenweathermap_client](https://github.com/madosuki/libopenweathermap_client)
