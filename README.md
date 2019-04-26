A dead simple webserver for ESP8266 and ESP32 boards to remotely trigger pins for a variable amount of time.

# How to use

Create `secrets.h` with your network's WiFi information and a secret passcode of your choosing:
```c++
const char *ssid = "...";
const char *wifiPassword = "...";
const char *secretPasscode = "...";
```

Flash the firmware to your board. Find its IP using either the serial output or another method.

Send a POST request to the device's IP on port 80 with the following headers:

* `pin` (integer): The pin to set to `HIGH`.
* `timeout` (integer): The amount of time to wait before resetting the specified `pin` back to `LOW`.
* `passcode`(string): The passcode specified in `secrets.h`.
