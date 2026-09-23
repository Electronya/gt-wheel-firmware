# GT Wheel Firmware

![Build](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/JudeBake/6aa3080953d92cad7d9ec93b8606c348/raw/gt-wheel-firmware-build.json)
![Tests](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/JudeBake/6aa3080953d92cad7d9ec93b8606c348/raw/gt-wheel-firmware-tests.json)
![Coverage](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/JudeBake/6aa3080953d92cad7d9ec93b8606c348/raw/gt-wheel-firmware-coverage.json)

Electronya DIY GT Wheel firmware, built on Zephyr RTOS and the
[embedded-services](https://gitea.main-server.home/Electronya/embedded-services) module library.

## Status

This app is currently a bare skeleton, being rebuilt from scratch for the new multi-app
Electronya Zephyr workspace. The previous custom board port (a custom STM32F303-based wheel PCB)
and its `buttonMngr`/`clutchReader`/`ledCtrl` modules were removed; they'll come back as proper
`embedded-services`-based modules once that work starts. USB HID is planned but not yet
implemented.

## Getting Started

This app carries its own west manifest, switchable within the shared Electronya Zephyr
workspace:

```bash
./activate.sh                # activate this app, default board (b_g474e_dpow1)
./activate.sh -b <board>      # activate with a specific board
./activate.sh -d              # also apply prj_dev.conf as an EXTRA_CONF_FILE
```

Then build normally from the workspace root:

```bash
west build apps/gt-wheel-firmware
west flash
```

## Modules

### App Info

Shell commands (`app name`, `app version`, `app info`) reporting basic firmware
identification. See `src/appInfo/`.

## Testing

Tests run via Zephyr's `twister` on the native simulator — no hardware needed.

```bash
# All tests with coverage report
./run-tests-coverage.sh

# A single module's tests
./run-tests-coverage.sh tests/<module>
```

## CI

Gitea workflows:
- `.gitea/workflows/build.yml` — builds the firmware for the target board.
- `.gitea/workflows/test-and-coverage.yml` — runs unit tests on `native_sim` and publishes
  coverage.
