/* Ultimate profile by Taker */
#include "modes/Ultimate.hpp"

#define ANALOG_STICK_MIN 28
#define ANALOG_STICK_NEUTRAL 128
#define ANALOG_STICK_MAX 228

Ultimate::Ultimate(socd::SocdType socd_type) {
    _socd_pair_count = 4;
    _socd_pairs = new socd::SocdPair[_socd_pair_count]{
        socd::SocdPair{ &InputState::left,   &InputState::right,   socd_type},
        socd::SocdPair{ &InputState::down,   &InputState::up,      socd_type},
        socd::SocdPair{ &InputState::c_left, &InputState::c_right, socd_type},
        socd::SocdPair{ &InputState::c_down, &InputState::c_up,    socd_type},
    };
}

void Ultimate::UpdateDigitalOutputs(InputState &inputs, OutputState &outputs) {
    outputs.a = inputs.a;
    outputs.b = inputs.b;
    outputs.x = inputs.x;
    outputs.y = inputs.y;
    outputs.triggerLDigital = inputs.l;
    outputs.triggerRDigital = inputs.r;

    // Turn on D-Pad layer by holding Nunchuk C button.
    if (inputs.nunchuk_c) {
        outputs.dpadUp = inputs.c_up;
        outputs.dpadDown = inputs.c_down;
        outputs.dpadLeft = inputs.c_left;
        outputs.dpadRight = inputs.c_right;
        outputs.leftStickClick = inputs.lightshield;
        outputs.rightStickClick = inputs.z;
        outputs.select = inputs.mod_y;
        outputs.home = inputs.start;
    }
    else
    {
        outputs.buttonL = inputs.lightshield;
        outputs.buttonR = inputs.z;
        outputs.start = inputs.start;
        outputs.select = inputs.select;
        outputs.home = inputs.home;
    }
}

void Ultimate::UpdateAnalogOutputs(InputState &inputs, OutputState &outputs) {
    // Coordinate calculations to make modifier handling simpler.
    UpdateDirections(
        inputs.left,
        inputs.right,
        inputs.down,
        inputs.up,
        inputs.c_left,
        inputs.c_right,
        inputs.c_down,
        inputs.c_up,
        ANALOG_STICK_MIN,
        ANALOG_STICK_NEUTRAL,
        ANALOG_STICK_MAX,
        outputs
    );

    bool shield_button_pressed = inputs.l || inputs.r;

    if (inputs.mod_x) {
        if (directions.diagonal) {
            if (inputs.a) {
                // Angled Ftilts
                outputs.leftStickX = 128 + (directions.x * 36);
                outputs.leftStickY = 128 + (directions.y * 30);
            }
            else if (shield_button_pressed) {
                // MX + L, R, LS, and MS + q1/2/3/4 = 6375 3750 = 51 30
                outputs.leftStickX = 128 + (directions.x * 51);
                outputs.leftStickY = 128 + (directions.y * 30);
            }
            else if (inputs.c_down) {
                // 37.5 degrees
                outputs.leftStickX = 128 + (directions.x * 79);
                outputs.leftStickY = 128 + (directions.y * 61);
            }
            else if (inputs.c_left) {
                // 30 degrees
                outputs.leftStickX = 128 + (directions.x * 87);
                outputs.leftStickY = 128 + (directions.y * 50);
            }
            else if (inputs.c_up) {
                // 15 degrees
                outputs.leftStickX = 128 + (directions.x * 97);
                outputs.leftStickY = 128 + (directions.y * 26);
            }
            else if (inputs.c_right) {
                // 7.5 degrees
                outputs.leftStickX = 128 + (directions.x * 99);
                outputs.leftStickY = 128 + (directions.y * 13);
            }
            else {
                // 22.5 degrees
                outputs.leftStickX = 128 + (directions.x * 92);
                outputs.leftStickY = 128 + (directions.y * 38);
            }
        }
        // Angled fsmash/ftilt with C-Stick + MX
        else if (directions.cx != 0) {
            outputs.rightStickX = 128 + (directions.cx * 72);
            outputs.rightStickY = 128 + (directions.y * 59);
        }
        // MX + Horizontal = 6625 = 53
        else if (directions.horizontal) {
            // Horizontal Shield tilt = 51
            if (shield_button_pressed) {
                outputs.leftStickX = 128 + (directions.x * 51);
            }
            else {
                outputs.leftStickX = 128 + (directions.x * 64);
            }
        }
        // MX + Vertical = 44
        else if (directions.vertical) {
            // Vertical Shield Tilt = 51
            if (shield_button_pressed) {
                outputs.leftStickY = 128 + (directions.y * 51);
            }
            else {
                outputs.leftStickY = 128 + (directions.y * 64);
            }
        }
    }
    else if (inputs.mod_y) {
        if (directions.diagonal) {
            if (inputs.a) {
                // MY Pivot Uptilt/Dtilt
                outputs.leftStickX = 128 + (directions.x * 34);
                outputs.leftStickY = 128 + (directions.y * 38);
            }
            else if (shield_button_pressed) {
                // MY + L, R, LS, and MS + q1/2 = 38 70
                outputs.leftStickX = 128 + (directions.x * 38);
                outputs.leftStickY = 128 + (directions.y * 70);
                // MY + L, R, LS, and MS + q3/4 = 40 68
                if (directions.x == -1) {
                    outputs.leftStickX = 128 + (directions.x * 40);
                    outputs.leftStickY = 128 + (directions.y * 68);
                }
            }
            else if (inputs.c_down) {
                // 52.5 degrees
                outputs.leftStickX = 128 + (directions.x * 61);
                outputs.leftStickY = 128 + (directions.y * 79);
            }
            else if (inputs.c_left) {
                // 60 degrees
                outputs.leftStickX = 128 + (directions.x * 50);
                outputs.leftStickY = 128 + (directions.y * 87);
            }
            else if (inputs.c_up) {
                // 75 degrees
                outputs.leftStickX = 128 + (directions.x * 26);
                outputs.leftStickY = 128 + (directions.y * 97);
            }
            else if (inputs.c_right) {
                // 82.5 degrees
                outputs.leftStickX = 128 + (directions.x * 13);
                outputs.leftStickY = 128 + (directions.y * 99);
            }
            else {
                // 67.5 degrees
                outputs.leftStickX = 128 + (directions.x * 38);
                outputs.leftStickY = 128 + (directions.y * 92);
            }
        }
        // MY + Horizontal (even if shield is held) = 41
        else if (directions.horizontal) {
            // MY Horizontal Tilts
            if (inputs.a) {
                outputs.leftStickX = 128 + (directions.x * 36);
            }
            else {
                outputs.leftStickX = 128 + (directions.x * 41);
            }
        }
        // MY + Vertical (even if shield is held) = 53
        else if (directions.vertical) {
            // MY Vertical Tilts
            if (inputs.a) {
                outputs.leftStickY = 128 + (directions.y * 36);
            }
            else {
                outputs.leftStickY = 128 + (directions.y * 53);
            }
        }
    }

    // C-stick ASDI Slideoff angle overrides any other C-stick modifiers (such as
    // angled fsmash).
    if (directions.cx != 0 && directions.cy != 0) {
        // 5250 8500 = 42 68
        outputs.rightStickX = 128 + (directions.cx * 42);
        outputs.rightStickY = 128 + (directions.cy * 68);
    }

    if (inputs.l) {
        outputs.triggerLAnalog = 140;
    }

    if (inputs.r) {
        outputs.triggerRAnalog = 140;
    }

    // Shut off C-stick when using D-Pad layer.
    if (inputs.nunchuk_c) {
        outputs.rightStickX = 128;
        outputs.rightStickY = 128;
    }

    // Nunchuk overrides left stick.
    if (inputs.nunchuk_connected) {
        outputs.leftStickX = inputs.nunchuk_x;
        outputs.leftStickY = inputs.nunchuk_y;
    }
}