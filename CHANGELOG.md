# Changelog

All notable changes to this project. Versions follow [Semantic Versioning](https://semver.org).
The version lives in [`VERSION`](VERSION); the sketch prints it at start-up and
`face_fan.py` has it as `__version__`.

## [1.0.0] - 2026-11-23

First working release, tested on a real ELEGOO Mega 2560.

### Added
- `face_fan_mega.ino`: servo follows the face (pin 3), L293D fan speed by PWM
  (pins 10/9/8), kick-start for the motor, smooth turning, the L LED shows when a
  face is seen, fan stops after 2 s and the head centres after 5 s without a face.
- `face_fan.py`: OpenCV face tracking with the laptop webcam, sends
  `position,speed` over USB at 115200 baud.
- Wiring diagram, 3D step-by-step wiring pictures and an interactive 3D wiring page.
- USB-power wiring (no battery) for testing.
- Demo videos, README with setup and troubleshooting.
- GitHub Actions: compile the sketch, check the Python script, automatic releases.

[1.0.0]: https://github.com/PhamAnhHoangg/ELEGOO-2560-fan-OpenCV/releases/tag/v1.0.0
