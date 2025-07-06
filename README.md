# IRremoteESP8266 components for ESPHome

This is a collection for `climate` implementations using the awesome [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266) library.

For now only the "Fujitsu" and "Electra (aux)" protocol is implemented, please open an issue or an PR to add more.

It does NOT support receive mode.

## Usage

> Only Arduino Framework is supported

```yaml
esp32:
  framework:
    type: arduino
```

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

### Electra

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/mistic100/ESPHome-IRremoteESP8266
    components: [ electra ]

remote_transmitter:
  pin: GPIOXX
  carrier_duty_percent: 50%

climate:
  - platform: electra
    name: 'Living Room AC'
```


It supports other options of [climate_ir](https://esphome.io/components/climate/climate_ir.html) like `sensor` and `transmitter_id`.

### Control fan direction

You can call the `step_vertical()` and `step_horizontal()` (if supported) methods on the climate controller.

```yaml
button:
  - platform: template
    name: 'Step vertical'
    on_press:
      then:
        - lambda: |-
            id(my_climate).step_vertical();
```

## Changelog

- **2025.07.06**: Add Electra platform
- **2025.06.04**: Add `step_vertical()` and `step_horizontal()` methods to Fujitsu platform
- **2025.05.22**: Compatibility with ESPHome 2025.5
