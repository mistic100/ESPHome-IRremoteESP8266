# IRremoteESP8266 components for ESPHome

This is a collection for `climate` implementations using the awesome [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266) library.

For now only some protocols are implemented, please open an issue or an PR to add more.

It does NOT support receive mode.

## Usage

### Fujitsu

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/mistic100/ESPHome-IRremoteESP8266
    components: [ fujitsu ]

remote_transmitter:
  pin: GPIOXX
  carrier_duty_percent: 50%

climate:
  - platform: fujitsu
    model: ARREB1E
    name: 'Living Room AC'
```

`model` is one of the remote model name supported by IRremoteESP8266:

- ARDB1
- ARJW2
- ARRAH2E
- ARREB1E
- ARREW4E
- ARRY4

### Goodweather

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/mistic100/ESPHome-IRremoteESP8266
    components: [ goodweather ]

remote_transmitter:
  pin: GPIOXX
  carrier_duty_percent: 50%

climate:
  - platform: goodweather
    name: 'Living Room AC'
```

It supports other options of [climate_ir](https://esphome.io/components/climate/climate_ir.html) like `sensor` and `transmitter_id`.

## TODO

- [ ] Make a generic class for the communication between ClimateIR and IRremoteESP8266
- [ ] Implements IR receiver mode
- [ ] Add [more platforms](https://github.com/crankyoldgit/IRremoteESP8266/blob/master/SupportedProtocols.md)
