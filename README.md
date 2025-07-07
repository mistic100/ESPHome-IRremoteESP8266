# IRremoteESP8266 components for ESPHome

This is a collection for `climate` implementations using the awesome [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266) library.

For now only some protocols are implemented, please open an issue or an PR to add more.

It does NOT support receive mode.

**Supported platforms:**
- [Fujitsu](#fujitsu)
- [Panasonic](#panasonic)


## Usage

> Only Arduino Framework is supported

```yaml
esp32:
  framework:
    type: arduino
```

It supports other options of [climate_ir](https://esphome.io/components/climate/climate_ir.html) like `sensor` and `transmitter_id`.

### Fujitsu

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/mistic100/ESPHome-IRremoteESP8266
    components: [ fujitsu, ir_remote_base ]

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

#### Control fan direction

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

### Panasonic

_untested_

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/mistic100/ESPHome-IRremoteESP8266
    components: [ panasonic, ir_remote_base ]

remote_transmitter:
  pin: GPIOXX
  carrier_duty_percent: 50%

climate:
  - platform: panasonic
    model: DKE
    name: 'Living Room AC'
```

`model` is one of the remote model name supported by IRremoteESP8266:

- LKE
- NKE
- DKE
- JKE
- CKP
- RKR


## Changelog

- **2025.07.06**: Add Panasonic platform
- **2025.06.04**: Add `step_vertical()` and `step_horizontal()` methods to Fujitsu platform
- **2025.05.22**: Compatibility with ESPHome 2025.5
