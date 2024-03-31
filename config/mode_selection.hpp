#ifndef _CONFIG_MODE_SELECTION_HPP
#define _CONFIG_MODE_SELECTION_HPP

#include <typeinfo>
#include "core/state.hpp"
#include "modes/DefaultKeyboardMode.hpp"
#include "modes/FgcMode.hpp"
#include "modes/Melee20Button.hpp"
#include "modes/ProjectM.hpp"
#include "modes/RivalsOfAether.hpp"
#include "modes/Ultimate.hpp"
#include "modes/Generic.hpp"

extern KeyboardMode *current_kb_mode;

void set_mode(CommunicationBackend *backend, ControllerMode *mode) {
    // Delete keyboard mode in case one is set, so we don't end up getting both controller and
    // keyboard inputs.
    delete current_kb_mode;
    current_kb_mode = nullptr;

    // Set new controller mode.
    backend->SetGameMode(mode);
}

void set_mode(CommunicationBackend *backend, KeyboardMode *mode) {
    // Delete and reassign current keyboard mode.
    delete current_kb_mode;
    current_kb_mode = mode;

    // Unset the current controller mode so backend only gives neutral inputs.
    backend->SetGameMode(nullptr);
}

void select_mode(CommunicationBackend *backend, bool is_switch_backend = false) {
    InputState &inputs = backend->GetInputs();
    if (inputs.mod_y && !inputs.mod_x && inputs.start) {
        if (inputs.c_right) {
            set_mode(backend, new Ultimate(socd::SOCD_NEUTRAL));
        } else if (inputs.c_up) {
            set_mode(backend, new Generic(socd::SOCD_NEUTRAL, is_switch_backend));
        } else if (inputs.c_left) {
            set_mode(backend, new FgcMode(socd::SOCD_NEUTRAL, socd::SOCD_NEUTRAL, false));
        } else if (inputs.c_down) {
            set_mode(backend, new FgcMode(socd::SOCD_NEUTRAL, socd::SOCD_NEUTRAL, true));
        }
    }
}

#endif
