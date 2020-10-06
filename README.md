# get-weather-on-c
  
This software is just get current data of openweathermap with json.

# Build
```
bash ./build.sh
```

# Dependents
[libhttp_connector](https://github.com/madosuki/libhttp_connector/)  
[libopenweathermap_client](https://github.com/madosuki/libopenweathermap_client)

# Usage

config file directory: ${HOME}/..openweather_map_client  

config file name: "config file directory"/openweather_map_account.json  

your api key set to openweather_map_account.json. Do reference to sample file in repository root.
And this software not collect api key and other data.

then build  
then can do run
```
./build/sample CityName
```
