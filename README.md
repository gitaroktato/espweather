# An ESP8266 -based Weather Station

## Realtime Measurements
[![View it on ThikgSpeak](https://github.com/gitaroktato/espweather/assets/1140629/1ec4722a-0eb8-45fa-9a29-158e1f37708d)](https://thingspeak.com/channels/2390966)

## Schematic
![schematic](img/Schematic_espweather_2024-01-21.png)

## Look & Feel
<img src="img/1705861432740.jpg" width=300>
<img src="img/1723120591143.jpg" width=300>
<img src="img/1704799735185.jpg" width=300>
<img src="img/1723542902367.jpg" width=300>

## Energy Consumption and Battery Life
Powered by a [18650 battery with the capacity of 3300mAh](https://www.hestore.hu/prod_10039174.html?lang=en).

| State | Consumption | Period |
| -- | -- | -- |
| Wake - with onboard LEDs ON | 100mA | ~1s |
| Sleep - with onboard LEDs ON | 30mA | ~1s |
| Wake - with onboard LEDs OFF | 80mA | ~5s |
| Deep Sleep - with onboard LEDs OFF | 2mA | ~15 min |


### Battery life estimation
Over an hour period we expect to have 4 times `5s` spent on reporting. During these intervals the energy consumption is `20mA`.
In the remaining time we expect to consume `2mA`. This period lasts for `3600s - (4 * 5s)) = 3580s`.

The total battery consumption over an hour is around `80mA * 5s * 4` for the reporting periods, and `2mA * (3600s - (4 * 5s))` for the deep sleep.
**Consumption over an hour is** `(1600mA + 7160mA) / 3600s ~= 2.44mA`

On average expected reporting time should be **56 days (3300mAh / ~2.44mA / 24h)**!

- Current reporting time without deep sleep and both LEDs blinking ~22 hrs
- Last reporting time with deep sleep ~3 days (on balcony, but suspecting continuous connection problems. See issue #12)

### Hardware Modifications
In order to reduce power consumption Node MCU requires hardware modifications. This is mainly due to the fact, that the CP2102 UART bridge is getting powered even if we use the 3V3 pin. The estimated power consumption is around `~15mA`. This can be done with an exacto knife.

<img src="img/uart_bridge_cutoff/schematics.png" width=300>

First we scrape of the resin from the surface to reveal the ciruit.

<img src="img/uart_bridge_cutoff/1724917553473.jpg" width=300>

Next we break the connection.

<img src="img/uart_bridge_cutoff/1724917553461.jpg" width=300>


- More about this topic is available [over here](https://tinker.yeoman.digital/2016/05/29/running-nodemcu-on-a-battery-esp8266-low-power-consumption-revisited/).
- See the schematics [over here](https://github.com/nodemcu/nodemcu-devkit-v1.0/blob/master/NODEMCU_DEVKIT_V1.0.PDF).

### Measurements
Measurements made using an oscilloscope through a shunt resistor of 1 Ohm. See the setup below:

<img src="img/deep_sleep/measuring_power_usage.jpg" width=300>

Wake - with onboard LEDs ON shows 60mA usage on average.

<img src="img/deep_sleep/wake_led_on.jpg" width=300>

Deep sleep with periodic wake times - shows `17mA` usage on average with 80mA peak. See the table above with estimated battery life.

<img src="img/deep_sleep/deep_sleep_and_wake.jpg" width=300>

Deep sleep after hardware modifications - shows `2mA` usage on average with 80mA peak. See the table above with estimated battery life.

<img src="img/deep_sleep/deep_sleep_hardware_mod.jpg" width=300>

## References

- https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
- https://newbiely.com/tutorials/esp8266/esp8266-dht11
- https://arduinomodules.info/ky-015-temperature-humidity-sensor-module/
- https://nothans.com/thingspeak-tutorials/esp8266/building-the-internet-of-things-with-the-esp8266-wi-fi-module-and-thingspeak
- https://github.com/nothans/thingspeak-esp-examples/blob/master/examples/RSSI_to_ThingSpeak.ino

### 3D Designs
- https://www.thingiverse.com/thing:144665
- https://www.thingiverse.com/thing:4805867
- https://www.thingiverse.com/thing:1718334
- https://www.thingiverse.com/thing:1985125

### Powering ESP8266
- https://hackaday.io/project/167731-testing-cheap-linear-li-ion-chargers-for-solar
- https://forum.arduino.cc/t/how-to-power-esp8266-ch304-with-18650-li-ion-battery/1179338/14
- https://randomnerdtutorials.com/esp8266-voltage-regulator-lipo-and-li-ion-batteries/
- https://randomnerdtutorials.com/power-esp32-esp8266-solar-panels-battery-level-monitoring/
- https://www.youtube.com/watch?v=f2yMs-JAyQM
- https://www.youtube.com/watch?v=ttyKZnVzic4
- https://youtu.be/6DY767kQnUM?si=SXygTiLH4zx0RgAH

### Power Consumption Comparison
- https://blog.voneicken.com/2018/lp-wifi-esp-comparison/

### Deep Sleep
- https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/
- https://www.instructables.com/ESP8266-Pro-Tips
- https://arduinodiy.wordpress.com/2020/01/18/very-deepsleep-and-energy-saving-on-esp8266/

### Photocells
- https://www.mouser.com/datasheet/2/737/photocells-932884.pdf

### FTDI Upload
https://community.platformio.org/t/esp32-pio-unified-debugger/4541/19
