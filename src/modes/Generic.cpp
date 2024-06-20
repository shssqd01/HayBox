/* Ultimate profile by Taker */
#include "modes/Generic.hpp"

#define ANALOG_STICK_MIN 0
#define ANALOG_STICK_MIN_SWITCH 28
#define ANALOG_STICK_NEUTRAL 128
#define ANALOG_STICK_MAX_SWITCH 228
#define ANALOG_STICK_MAX 255

Generic::Generic(socd::SocdType socd_type, bool is_switch) {
    _socd_pair_count = 4;
    _socd_pairs = new socd::SocdPair[_socd_pair_count]{
        socd::SocdPair{&InputState::left,    &InputState::right,   socd_type},
        socd::SocdPair{ &InputState::down,   &InputState::up,      socd_type},
        socd::SocdPair{ &InputState::c_left, &InputState::c_right, socd_type},
        socd::SocdPair{ &InputState::c_down, &InputState::c_up,    socd_type},
    };
    _is_switch = is_switch;
}

void Generic::UpdateDigitalOutputs(InputState &inputs, OutputState &outputs) {
    if (_is_switch) {
        outputs.b = inputs.a;
        outputs.a = inputs.x;
        outputs.triggerRDigital = inputs.l;
        outputs.triggerLDigital = inputs.lightshield;

        outputs.y = inputs.b;
        outputs.x = inputs.y;
        outputs.buttonR = inputs.r;
        outputs.buttonL = inputs.z;
    }
    else {
        outputs.a = inputs.a;
        outputs.b = inputs.x;
        outputs.triggerRDigital = inputs.l;
        outputs.triggerLDigital = inputs.lightshield;

        outputs.x = inputs.b;
        outputs.y = inputs.y;
        outputs.buttonR = inputs.r;
        outputs.buttonL = inputs.z;
    }

    // Turn on D-Pad layer by holding Nunchuk C button.
    if (inputs.mod_x) {
        outputs.dpadUp = inputs.up;
        outputs.dpadDown = inputs.down;
        outputs.dpadLeft = inputs.left;
        outputs.dpadRight = inputs.right;
        outputs.home = inputs.start;
        outputs.select = inputs.nunchuk_c;
        outputs.start = inputs.mod_y;
    }
    else
    {
        outputs.leftStickClick = inputs.nunchuk_c;
        outputs.rightStickClick = inputs.mod_y;
    }
}

void Generic::UpdateAnalogOutputs(InputState &inputs, OutputState &outputs) {
    // Coordinate calculations to make modifier handling simpler.
    UpdateDirections(
        inputs.left && !(inputs.mod_x),
        inputs.right && !(inputs.mod_x),
        inputs.down && !(inputs.mod_x),
        inputs.up && !(inputs.mod_x),
        inputs.c_left,
        inputs.c_right,
        inputs.c_down,
        inputs.c_up,
        _is_switch ? ANALOG_STICK_MIN_SWITCH : ANALOG_STICK_MIN,
        ANALOG_STICK_NEUTRAL,
        _is_switch ? ANALOG_STICK_MAX_SWITCH : ANALOG_STICK_MAX,
        outputs
    );

    if (directions.x != 0 && directions.y != 0)
    {
        outputs.leftStickX -= 28 * directions.x;
        outputs.leftStickY -= 28 * directions.y;
    }
    if (directions.cx != 0 && directions.cy != 0)
    {
        outputs.rightStickX -= 28 * directions.cx;
        outputs.rightStickY -= 28 * directions.cy;
    }
}