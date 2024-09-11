/* Ultimate profile by Taker */
#include "modes/Ultimate_Kazuya.hpp"

#define ANALOG_STICK_MIN 28
#define ANALOG_STICK_NEUTRAL 128
#define ANALOG_STICK_MAX 228
#define ANALOG_STICK_TILT_MIN 64
#define ANALOG_STICK_TILT_MAX 192

Ultimate_Kazuya::Ultimate_Kazuya(socd::SocdType socd_type) {
    _socd_pair_count = 4;
    _socd_pairs = new socd::SocdPair[_socd_pair_count]{
        socd::SocdPair{ &InputState::left,   &InputState::right,   socd_type},
        socd::SocdPair{ &InputState::down,   &InputState::up,      socd_type},
        socd::SocdPair{ &InputState::c_left, &InputState::c_right, socd_type},
        socd::SocdPair{ &InputState::c_down, &InputState::c_up,    socd_type},
    };
}

void Ultimate_Kazuya::UpdateDigitalOutputs(InputState &inputs, OutputState &outputs) {
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

void Ultimate_Kazuya::UpdateAnalogOutputs(InputState &inputs, OutputState &outputs) {
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
        ANALOG_STICK_TILT_MIN,
        ANALOG_STICK_NEUTRAL,
        ANALOG_STICK_TILT_MAX,
        outputs
    );

    bool shield_button_pressed = inputs.l || inputs.r;

    if (inputs.mod_x) {
        if (directions.diagonal) {
            outputs.leftStickX = 128 + (directions.x * 100);
            outputs.leftStickY = 128 + (directions.y * 100);
        }
        // Angled fsmash/ftilt with C-Stick + MX
        else if (directions.cx != 0) {
            outputs.rightStickX = 128 + (directions.cx * 72);
            outputs.rightStickY = 128 + (directions.y * 59);
        }
        // MX + Horizontal = 6625 = 53
        else if (directions.horizontal) {
            // Horizontal Shield tilt = 51
            outputs.leftStickX = 128 + (directions.x * 100);
        }
        // MX + Vertical = 44
        else if (directions.vertical) {
            // Vertical Shield Tilt = 51
            outputs.leftStickY = 128 + (directions.y * 100);
        }
    }
    else if (inputs.mod_y) {
        if (directions.diagonal) {
            if (inputs.a) {
                // MY Pivot Uptilt/Dtilt
                outputs.leftStickX = 128 + (directions.x * 34);
                outputs.leftStickY = 128 + (directions.y * 38);
            }
        }
        // MY + Horizontal (even if shield is held) = 41
        else if (directions.horizontal) {
            // MY Horizontal Tilts
            outputs.leftStickX = 128 + (directions.x * 41);
        }
        // MY + Vertical (even if shield is held) = 53
        else if (directions.vertical) {
            // MY Vertical Tilts
            outputs.leftStickY = 128 + (directions.y * 53);
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