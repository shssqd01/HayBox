#include "comms/B0XXInputViewer.hpp"
#include "comms/DInputBackend.hpp"
#include "comms/GamecubeBackend.hpp"
#include "comms/N64Backend.hpp"
#include "comms/NintendoSwitchBackend.hpp"
#include "comms/XInputBackend.hpp"
#include "config/mode_selection.hpp"
#include "core/CommunicationBackend.hpp"
#include "core/InputMode.hpp"
#include "core/KeyboardMode.hpp"
#include "core/pinout.hpp"
#include "core/socd.hpp"
#include "core/state.hpp"
#include "input/GpioButtonInput.hpp"
#include "input/NunchukInput.hpp"
#include "joybus_utils.hpp"
#include "modes/Melee20Button.hpp"
#include "stdlib.hpp"

#include <pico/bootrom.h>

CommunicationBackend **backends = nullptr;
size_t backend_count;
KeyboardMode *current_kb_mode = nullptr;
bool is_switch_backend = false;

GpioButtonMapping button_mappings[] = {
    { &InputState::mod_x,       5  },
    { &InputState::left,        4  },
    { &InputState::up,          3  },
    { &InputState::right,       2  },

    { &InputState::down,        6  },
    { &InputState::nunchuk_c,   7  }, //Dpad Toggle

    { &InputState::select,      10 },
    { &InputState::start,       0  },
    { &InputState::home,        11 },

    { &InputState::c_left,      13 },
    { &InputState::c_up,        12 },
    { &InputState::mod_y,       15 },
    { &InputState::c_down,      14 },
    { &InputState::c_right,     16 },

    { &InputState::a,           26 },
    { &InputState::x,           21 },
    { &InputState::l,           19 }, //Left Trigger
    { &InputState::lightshield, 17 }, //Left Bumper

    { &InputState::b,           27 },
    { &InputState::y,           22 },
    { &InputState::r,           20 }, //Right Trigger
    { &InputState::z,           18 }, //Right Bumper
};
size_t button_count = sizeof(button_mappings) / sizeof(GpioButtonMapping);

const Pinout pinout = {
    .joybus_data = 28,
    .mux = -1,
    .nunchuk_detect = -1,
    .nunchuk_sda = -1,
    .nunchuk_scl = -1,
};

void setup() {
    // Create GPIO input source and use it to read button states for checking button holds.
    GpioButtonInput *gpio_input = new GpioButtonInput(button_mappings, button_count);

    InputState button_holds;
    gpio_input->UpdateInputs(button_holds);

    // Bootsel button hold as early as possible for safety.
    if (button_holds.start) {
        reset_usb_boot(0, 0);
    }

    // Turn on LED to indicate firmware booted.
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 1);

    // Create array of input sources to be used.
    static InputSource *input_sources[] = { gpio_input };
    size_t input_source_count = sizeof(input_sources) / sizeof(InputSource *);

    ConnectedConsole console = detect_console(pinout.joybus_data);

    /* Select communication backend. */
    CommunicationBackend *primary_backend;
    if (console == ConnectedConsole::NONE) {
        if (button_holds.b) {
            // If B is held on plugin, then use Switch backend with Ultimate mode.
            is_switch_backend = true;
            NintendoSwitchBackend::RegisterDescriptor();
            backend_count = 1;
            primary_backend = new NintendoSwitchBackend(input_sources, input_source_count);
            backends = new CommunicationBackend *[backend_count] { primary_backend };
            primary_backend->SetGameMode(new Ultimate(socd::SOCD_NEUTRAL));
            return;
        } else if (button_holds.y) {
            // If Y is held on plugin, then use DInput backend with FGC mode.
            TUGamepad::registerDescriptor();
            TUKeyboard::registerDescriptor();
            backend_count = 2;
            primary_backend = new DInputBackend(input_sources, input_source_count);
            backends = new CommunicationBackend *[backend_count] {
                primary_backend, new B0XXInputViewer(input_sources, input_source_count)
            };
            primary_backend->SetGameMode(new FgcMode(socd::SOCD_NEUTRAL, socd::SOCD_NEUTRAL, false));
            return;
        } else {
            // Default to XInput backend with Generic controller mode.
            backend_count = 2;
            primary_backend = new XInputBackend(input_sources, input_source_count);
            backends = new CommunicationBackend *[backend_count] {
                primary_backend, new B0XXInputViewer(input_sources, input_source_count)
            };
            primary_backend->SetGameMode(new Generic(socd::SOCD_NEUTRAL, false));
            return;
        }
    } else {
        if (console == ConnectedConsole::GAMECUBE) {
            primary_backend = new GamecubeBackend(input_sources, input_source_count, pinout.joybus_data);
        } else if (console == ConnectedConsole::N64) {
            primary_backend = new N64Backend(input_sources, input_source_count, pinout.joybus_data);
        }

        // If console then only using 1 backend (no input viewer).
        backend_count = 1;
        backends = new CommunicationBackend *[backend_count] { primary_backend };
        primary_backend->SetGameMode(
            new Melee20Button(socd::SOCD_NEUTRAL, { .crouch_walk_os = false })
        );
        return;
    }
}

void loop() {
    select_mode(backends[0], is_switch_backend);

    for (size_t i = 0; i < backend_count; i++) {
        backends[i]->SendReport();
    }

    if (current_kb_mode != nullptr) {
        current_kb_mode->SendReport(backends[0]->GetInputs());
    }
}

/* Nunchuk code runs on the second core */
NunchukInput *nunchuk = nullptr;

void setup1() {
    while (backends == nullptr) {
        tight_loop_contents();
    }

    // Create Nunchuk input source.
    nunchuk = new NunchukInput(Wire, pinout.nunchuk_detect, pinout.nunchuk_sda, pinout.nunchuk_scl);
}

void loop1() {
    if (backends != nullptr) {
        nunchuk->UpdateInputs(backends[0]->GetInputs());
        busy_wait_us(50);
    }
}
