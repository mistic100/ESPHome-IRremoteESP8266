# IRremoteESP8266 components for ESPHome

This is a collection for `climate` implementations using the awesome [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266) library.

For now only some protocols are implemented, please open an issue or an PR to add more.

It does NOT support receive mode.

**Supported platforms:**
- [fujitsu](#fujitsu)
- [fujitsu-264](#fujitsu-264)
- [panasonic](#panasonic)
- [electra](#electra)
- [sharp](#sharp)

## Usage

```yaml
esp32:
  framework:
    type: arduino

external_components:
  - source:
      type: git
      url: https://github.com/mistic100/ESPHome-IRremoteESP8266
    components: [ ir_remote_base, <platform_name> ]

remote_transmitter:
  pin: GPIOXX
  carrier_duty_percent: 50%

climate:
  - platform: <platform_name>
    name: 'Living Room AC'
```

Replace `<platform_name>` by the name of one of the platforms available.

It supports other options of [climate_ir](https://esphome.io/components/climate/climate_ir.html) like `sensor` and `transmitter_id`.

> [!NOTE]
> For platforms with a `model` option, please refer to the [supported Protocols page](https://github.com/crankyoldgit/IRremoteESP8266/blob/master/SupportedProtocols.md) ("A/C Model" column).

> [!WARNING]
> Only Arduino Framework is supported

## fujitsu

```yaml
climate:
  - platform: fujitsu
    model: XXXXXX
    name: 'Living Room AC'
```

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

## fujitsu-264

This platform implements the special Fujitsu protocol of the `AR-RLB2J` remote.

```yaml
climate:
  - platform: fujitsu_264
    name: 'Living Room AC'
```

#### Toggle powerful

You can call the `toggle_powerful()` methods on the climate controller.

```yaml
button:
  - platform: template
    name: 'Toggle powerful'
    on_press:
      then:
        - lambda: |-
            id(my_climate).toggle_powerful();
```

#### Set fan angle

You can call the `set_fan_angle()` method on the climate controller.

```yaml
button:
  - platform: template
    name: 'Set fan angle'
    on_press:
      then:
        - lambda: |-
            // Acceptable values are 1-7 and 15(means stay)
            id(my_climate).set_fan_angle(1);
```

## panasonic

```yaml
climate:
  - platform: panasonic
    model: XXXXXX
    name: 'Living Room AC'
```

## electra

Also known as Aux.

```yaml
climate:
  - platform: electra
    name: 'Living Room AC'
```

## sharp

```yaml
climate:
  - platform: sharp
    model: XXXXXX
    name: 'Living Room AC'
```


## Changelog

- **2026.02.21**: Add Sharp platform
- **2026.02.19**: Use latest version of IRremoteESP8266
- **2025.07.28**: Add fujitsu-264 platform
- **2025.07.21**: Compatibility with ESPHome 2025.7
- **2025.07.07**: Add Electra platform
- **2025.07.06**: Add Panasonic platform
- **2025.06.04**: Add `step_vertical()` and `step_horizontal()` methods to Fujitsu platform
- **2025.05.22**: Compatibility with ESPHome 2025.5
