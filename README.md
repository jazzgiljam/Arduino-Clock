# Servo Test Sketch: 7-Segment Display (PCA9685 + ESP32-S3)

A small Arduino sketch for testing and calibrating the 7 servos of a single servo-driven 7-segment display. It moves every servo between its **minimum** position and its **middle** position every 3 seconds, so you can check wiring, direction and travel before building the full clock.

This is a stepping stone toward a 4-display clock (28 servos on two PCA9685 boards).

---

## Features

- Drives 7 servos (one per segment) through a PCA9685 16-channel PWM driver
- Per-servo calibration (min/max) via arrays, since segments are often mounted differently
- Controls the PCA9685 `OE` pin so outputs are explicitly enabled
- Serial output so you can see the sketch is running
- Configurable I2C pins for ESP32-S3 boards

---

## Hardware

| Part | Notes |
|---|---|
| ESP32-S3 Super Mini | Any ESP32 works, I2C pins are configurable |
| PCA9685 16-channel PWM driver | Default I2C address `0x40` |
| 7x SG90 micro servos | One per segment (a-g) |
| 5 V power supply | Separate supply for the servos (V+), shared GND with the ESP32 |

### Wiring

| ESP32-S3 | PCA9685 |
|---|---|
| GND | GND |
| 3V3 | VCC |
| GPIO5 | SDA |
| GPIO6 | SCL |
| GPIO3 | OE |

Servos connect to PCA9685 channels `0`-`6`. Servo power (5 V) goes to the **V+** terminal of the PCA9685, **not** to the ESP32's 3V3 pin.

> **Tip:** If the sketch runs but nothing moves, run an I2C scanner. No device at `0x40` almost always means the SDA/SCL pins in the sketch do not match your wiring.

---

## Dependencies

Install via the Arduino Library Manager:

- [Adafruit PWM Servo Driver Library](https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library)

`Wire.h` is built in.

---

## Code

```cpp
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

const int SDA_PIN = 5;   // pas aan naar jouw bedrading
const int SCL_PIN = 6;
const int OE_PIN  = 3;

const int NUM_SERVOS = 7;

// Per servo eigen min/max (SG90 veilig: ~110-490)
int servoMin[NUM_SERVOS] = {150, 150, 150, 150, 150, 475, 150};
int servoMax[NUM_SERVOS] = {450, 450, 450, 450, 450, 520, 450};

void setAll(bool midden) {
  for (int i = 0; i < NUM_SERVOS; i++) {
    int target = midden ? (servoMin[i] + servoMax[i]) / 2 : servoMin[i];
    pwm.setPWM(i, 0, target);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, LOW);   // outputs aan (OE is active-low)

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);
  delay(500);
  Serial.println("Servo test gestart");
}

void loop() {
  Serial.println("Minimum");
  setAll(false);
  delay(3000);

  Serial.println("Midden");
  setAll(true);
  delay(3000);
}
```

---

## How it works

1. **Setup:** I2C starts on the configured pins, `OE` is pulled LOW to enable outputs, the PCA9685 is initialized, and the PWM frequency is set to 50 Hz (a 20 ms servo frame).
2. **Loop:** all servos move to their minimum, wait 3 s, move to the midpoint of their individual min/max, wait 3 s, and repeat.
3. **Per-servo ranges:** `servoMin[]` and `servoMax[]` hold one value per channel, so a segment that is mounted differently (like channel 5) gets its own range.

---

## Understanding the PWM values

The PCA9685 has 4096 ticks per period. At 50 Hz (20 ms), one tick is about **4.88 µs**:

```
pulse width (µs) = ticks x 4.88
```

| Ticks | Pulse width | Comment |
|---|---|---|
| ~100 | ~0.5 ms | Typical SG90 lower limit |
| 150 | ~0.73 ms | |
| ~300 | ~1.5 ms | Typical center |
| ~490 | ~2.4 ms | Typical SG90 upper limit |
| 600 | ~2.93 ms | **Beyond** safe SG90 range |
| 800 | ~3.9 ms | **Far beyond** safe range |

### Warning

SG90 servos expect roughly **0.5-2.4 ms** pulses. Values above ~490 ticks push the servo against its mechanical end stop. It will buzz, draw stall current and wear out the gears. If a servo only looks correct because it hits the end stop, recalibrate it.

---

## Calibration

1. Start with conservative values (`min ~150`, `max ~450`).
2. Increase or decrease one servo's value in small steps (5-10 ticks).
3. Stop as soon as the servo **buzzes** or stops moving further. Back off by 10-20 ticks.
4. Note the working values per segment in `servoMin[]` / `servoMax[]`.
5. Repeat for each of the 7 servos.

---

## Troubleshooting

| Symptom | Likely cause |
|---|---|
| Nothing moves | Wrong SDA/SCL pins, no servo power on V+, or `OE` not LOW |
| I2C scan finds no device | Wiring or pin mismatch, missing GND, VCC not connected |
| Servo buzzes at an end position | Pulse width beyond the servo's range; reduce the value |
| Servos jitter or ESP resets | Weak or shared 5 V supply; use a separate supply with common GND |
| Slightly wrong angles/timing | PCA9685 oscillator is not exactly 25 MHz; tune `setOscillatorFrequency()` |

---

## Roadmap

- [ ] Calibrate SHOW/HIDE positions for all segments
- [ ] Add the second PCA9685 (`0x41`, A0 soldered) for 14 servos
- [ ] Scale to 4 displays (28 servos) showing real time
- [ ] Run the startup servo test only on power-up

---

## License

Add your license of choice (for example MIT) here.
