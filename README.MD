# Get Started
To get started create a secrets.h file with the following definitions.

```c++
#pragma once
#define WIFI_SSID "SSID"
#define WIFI_PWD "Password"
```

# Wiring
Connect the Grbl control to the ESP32 as follows:

| ESP32 | GRBL |
|---|---|
| VIN | 5V |
| GND | GND |
| RX2 | TX |
| TX2 | RX |


