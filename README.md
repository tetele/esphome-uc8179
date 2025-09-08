# UC8179 component for using E-paper displays in ESPHome

This is a custom [ESPHome](http://esphome.io) component for using UC8179 e-paper display drivers, embedded in many popular e-ink screens from Waveshare/GoodDisplay.

Including the component is done with [`external_component`](https://esphome.io/components/external_components.html#external-components) syntax:

```yaml
external_components:
  - source: github://tetele/esphome-uc8179@main
    components: [ uc8179 ]
```

## Configuration

The setup is split in 2 parts: one concerning the actual driver and one where you have to declare a display using the driver.

```yaml
spi:
  # Required as the UC8719 communicates over SPI
  clk_pin: GPIOXX
  mosi_pin: GPIOXX

uc8179:
  # IC config
  cs_pin: GPIOXX # Required
  dc_pin: GPIOXX # Required. Signals whether the SPI bus is used for commands or data
  busy_pin: GPIOXX # Optional, but recommended
  reset_pin: GPIOXX # Optional, but recommended

display:
  # Display config
  - platform: uc8179
    id: eink_display
    model: GDEY075T7
    # ...
```

### Configuration variables

#### uc8719

- **id** (*Optional*, ID): The ID of the component. Useful if there are multiple displays attached
- **dc_pin** (**Required**, [Pin Schema](https://esphome.io/guides/configuration-types#config-pin_schema)): The pin mapping for the Data/Command signal used in SPI
- **reset_pin** (*Optional*, [Pin Schema](https://esphome.io/guides/configuration-types#config-pin_schema)): The pin used to reset the driver IC
- **busy_pin** (*Optional*, [Pin Schema](https://esphome.io/guides/configuration-types#config-pin_schema)): An output pin where the IC will maintain a low signal for as long as it is performing a long operation, then it returns to high

#### display

- **id** (*Optional*, ID): The ID of the display. Useful if there are multiple displays attached
- **driver_id** (*Optional*, ID): The ID of the UC8719 driver comopnent. Required only if there are multiple drivers declared, otherwise it can be omitted as it will use the single declared UC8719 driver
- **model** (**Required**): A string representing the model of the actual display, list below

## Supported display models

|Name|Config value|Manufacturer|Details|
|---|---|---|---|
|GDEY075T7|GDEY075T7|GoodDisplay|A 7.5", 800x480px, 4-color grayscale display.|
|GDEY075T7|GDEY075T7_BW|GoodDisplay|The same GDEY075T7, but used in black//white mode only (i.e. no greyscale)|

## Usage

The easiest way you can use the component is to simply set up a display and use its `lambda` function to draw on it. Just like for [other `display`s](https://esphome.io/components/display/), you have an `it` variable exposed in the lambda function to reference the drawing methods.

```yaml
display:
  - platform: uc8179
    id: eink_display
    model: GDEY075T7
    update_interval: 120s
    rotation: 90°
    lambda: |-
      // Fill background.
      it.fill(id(color_bg));

      it.filled_rectangle(40, 40, 40, 40, id(color_white));
      it.filled_rectangle(40, 80, 40, 40, id(color_light_gray));
      it.filled_rectangle(40, 120, 40, 40, id(color_dark_gray));
      it.filled_rectangle(40, 160, 40, 40, id(color_black));
```
