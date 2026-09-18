# Automatic RC Car

A 3D-printed car on a Raspberry Pi that you drive with a PS4 controller over
Bluetooth, and that can also follow a tennis ball on its own using the Pi
camera and an HSV colour filter (no neural network).

**The story, photos and architecture diagram are on the case-study page:
https://kalpkan.com/projects/rc-car**

## What is in this repository

| Path | What it is |
|---|---|
| `ps4_controller_integrated.cpp` | The controller program: one thread reads button presses from the Linux input device, another polls the sticks through SDL2 at 20 Hz (0.1 dead-zone) and prints throttle / steering. Build and usage: [`README_PS4_INTEGRATED.md`](README_PS4_INTEGRATED.md). |
| `ps4_joystick_test.cpp`, `ps4_joystick_test.py`, `button_testing.cpp`, `ps4_testing.cpp` | Earlier, smaller tests the integrated program grew out of. Build notes: [`README_CPP_BUILD.md`](README_CPP_BUILD.md). |
| `CMakeLists.txt` | Builds the two C++ programs (SDL2 on Linux/macOS, XInput on Windows). |
| `data_prep_scripts/` | The Python/OpenCV side: `green_object_realtime_demo.py` is the live webcam demo with HSV sliders; the `data_prep_*.py` scripts sorted two open image sets into green / non-green, resized them to 320×240, wrote bounding boxes and split train/val. |

## Build the controller program

```bash
sudo apt-get install libsdl2-dev cmake build-essential   # Raspberry Pi / Debian
mkdir build && cd build && cmake .. && make
sudo ./ps4_controller_integrated                          # /dev/input/event3 by default
```

## Why cloning this repo downloads 943 MB, and what is being done about it

The image sets used to tune the green filter (`data/`: 64,273 JPEGs, about
900 MB) were committed by mistake in July 2025. The last commit on `main`
(`08819992`, "Remove training data from git tracking") removed them from the
working tree and added `data/` to `.gitignore`, so **no current file is
large**, but git keeps every old version, so a clone still pulls the whole
history.

Shrinking the clone needs the history to be rewritten, which changes every
commit id and requires a force-push. That is documented, with the exact
command, in the pull request that added this README, and it will only be run
when Kalp approves it. Until then, a quick way to work with the code without
the history is:

```bash
git clone --depth 1 https://github.com/KalpKan/Automatic-RC-Car.git
```

## Not in this repository

- The print files (RPi mounts, N20 motor mounts, 14-tooth spur gears, wheels)
  and the build/driving videos live on Kalp's machine; the case study shows
  frames from the videos.
- The steering-command mapping (ball position → steering angle) and the Pi
  Zero 2W port were done on the bench and are not yet committed as one
  program.
