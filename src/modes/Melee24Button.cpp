#include "modes/Melee24Button.hpp"

//unified angles

#define ANALOG_STICK_MIN 48
#define ANALOG_STICK_NEUTRAL 128
#define ANALOG_STICK_MAX 208

Melee24Button::Melee24Button(socd::SocdType socd_type, Melee24ButtonOptions options) {
    socd_type = MELEE_SOCD;
    _socd_pair_count = 4;
    _socd_pairs = new socd::SocdPair[_socd_pair_count]{
        socd::SocdPair{&InputState::left,    &InputState::right,   socd_type},
        socd::SocdPair{ &InputState::down,   &InputState::up,      socd_type},
        socd::SocdPair{ &InputState::c_left, &InputState::c_right, socd_type},
        socd::SocdPair{ &InputState::c_down, &InputState::c_up,    socd_type},
    };

    _options = options;
    _horizontal_socd = false;
}

void Melee24Button::HandleSocd(InputState &inputs) {
    _horizontal_socd = inputs.left && inputs.right;
    InputMode::HandleSocd(inputs);
}

void Melee24Button::UpdateDigitalOutputs(InputState &inputs, OutputState &outputs) {
    outputs.a = inputs.a;
    outputs.b = inputs.b;
    outputs.x = inputs.x;
    outputs.y = inputs.y;
    outputs.buttonR = inputs.z;
    outputs.triggerLDigital = inputs.l;
    outputs.triggerRDigital = inputs.r;
    outputs.start = inputs.start;
    
    // Activate D-Pad layer by holding D-Pad mod
    if (inputs.dpad_mod) {
        outputs.dpadUp = inputs.dpad_up;
        outputs.dpadDown = inputs.mod_y;
        outputs.dpadLeft = inputs.mod_x;
        outputs.dpadRight = inputs.dpad_right;
    }    
}

void Melee24Button::UpdateAnalogOutputs(InputState &inputs, OutputState &outputs) {
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
        /*ANALOG_STICK_MIN*/ANALOG_STICK_NEUTRAL - 112,
        ANALOG_STICK_NEUTRAL,
        /*ANALOG_STICK_MAX*/ANALOG_STICK_NEUTRAL + 112,
        outputs
    );

    bool shield_button_pressed = inputs.l || inputs.r || inputs.lightshield || inputs.midshield;
    if (directions.diagonal) {
        // q1/2 = 7000 7000
        // actually 6875 7125 to account for randomness
        outputs.leftStickX = 128 + (directions.x * 55);
        outputs.leftStickY = 128 + (directions.y * 57);
        // L, R, LS, and MS + q3/4 = 7125 6875 (For vanilla shield drop. Gives 44.5
        // degree wavedash). Also used as default q3/4 diagonal if crouch walk option select is
        // enabled.
        // actually 7250 6750 to account for randomness
        if (directions.y == -1 && (shield_button_pressed || _options.crouch_walk_os)) {
            outputs.leftStickX = 128 + (directions.x * 58);
            outputs.leftStickY = 128 + (directions.y * 54);
        }
    }

    if (inputs.mod_x) {
        // MX + Horizontal (even if shield is held) = 6625 = 53
        if (directions.horizontal) {
            outputs.leftStickX = 128 + (directions.x * 53);
        }
        // MX + Vertical (even if shield is held) = 5375 = 43
        // y=-0.5500 (44) is solo nana ice block, so we reduce this by one
        // MX + Vertical (even if shield is held) = 5250 = 42
        if (directions.vertical) {
            outputs.leftStickY = 128 + (directions.y * 42);
        }
        /* no more wavedash nerf
        if (directions.diagonal && shield_button_pressed) {
            // MX + L, R, LS, and MS + q1/2/3/4 = 6375 3750 = 51 30
            outputs.leftStickX = 128 + (directions.x * 51);
            outputs.leftStickY = 128 + (directions.y * 30);
        }
        */

        /* Up B angles */
        if (directions.diagonal /*&& !shield_button_pressed*/) {
            if (!inputs.b) {
                // 6750 3125 - 24.84deg - 54 25 - modX
                // 6750 3625 - 28.24deg - 54 29 - modX + cDown
                // 6625 4125 - 31.91deg - 53 33 - modX + cLeft
                // 6375 4625 - 35.96deg - 51 37 - modX + cUp
                // 6125 5125 - 39.92deg - 49 41 - modX + cRight
                outputs.leftStickX = 128 + (directions.x * 54);
                outputs.leftStickY = 128 + (directions.y * 25);
                if (inputs.c_down) {
                    outputs.leftStickX = 128 + (directions.x * 54);
                    outputs.leftStickY = 128 + (directions.y * 29);
                }
                if (inputs.c_left) {
                    outputs.leftStickX = 128 + (directions.x * 53);
                    outputs.leftStickY = 128 + (directions.y * 33);
                }
                if (inputs.c_up) {
                    outputs.leftStickX = 128 + (directions.x * 51);
                    outputs.leftStickY = 128 + (directions.y * 37);
                }
                if (inputs.c_right) {
                    outputs.leftStickX = 128 + (directions.x * 49);
                    outputs.leftStickY = 128 + (directions.y * 41);
                }
            } else {
                /* Extended Up B Angles */
                // 9000 4125 - 24.62deg - 72 33 - modX + B
                // 8750 4750 - 28.50deg - 70 38 - modX + B + cDown
                // 8500 5250 - 31.70deg - 68 42 - modX + B + cLeft
                // 7250 5250 - 35.91deg - 58 42 - modX + B + cUp
                // 6375 5250 - 39.47deg - 51 42 - modX + B + cRight
                outputs.leftStickX = 128 + (directions.x * 72);
                outputs.leftStickY = 128 + (directions.y * 33);
                if (inputs.c_down) {
                    outputs.leftStickX = 128 + (directions.x * 70);
                    outputs.leftStickY = 128 + (directions.y * 38);
                }
                if (inputs.c_left) {
                    outputs.leftStickX = 128 + (directions.x * 68);
                    outputs.leftStickY = 128 + (directions.y * 42);
                }
                if (inputs.c_up) {
                    outputs.leftStickX = 128 + (directions.x * 58);
                    outputs.leftStickY = 128 + (directions.y * 42);
                }
                if (inputs.c_right) {
                    outputs.leftStickX = 128 + (directions.x * 51);
                    outputs.leftStickY = 128 + (directions.y * 42);
                }
            }
        }

        // Angled fsmash
        if (directions.cx != 0 && directions.y != 0) {
            // 8500 5250 = 68 42
            outputs.rightStickX = 128 + (directions.cx * 68);
            outputs.rightStickY = 128 + (directions.y * 42);
        }
    }

    if (inputs.mod_y) {
        // MY + Horizontal (even if shield is held) = 3375 = 27
        if (directions.horizontal) {
            outputs.leftStickX = 128 + (directions.x * 27);
        }
        // Turnaround neutral B nerf
        /* no more turnaround neutral b nerf
        if (inputs.b) {
            outputs.leftStickX = 128 + (directions.x * 80);
        }
        */
        // MY + Vertical (even if shield is held) = 7375 = 59
        if (directions.vertical) {
            outputs.leftStickY = 128 + (directions.y * 59);
        }
        /* no more wavedash nerf
        if (directions.diagonal && shield_button_pressed) {
            // MY + L, R, LS, and MS + q1/2 = 4750 8750 = 38 70
            outputs.leftStickX = 128 + (directions.x * 38);
            outputs.leftStickY = 128 + (directions.y * 70);
            // MY + L, R, LS, and MS + q3/4 = 5000 8500 = 40 68
            if (directions.y == -1) {
                outputs.leftStickX = 128 + (directions.x * 40);
                outputs.leftStickY = 128 + (directions.y * 68);
            }
        }
        */

        /* Up B angles */
        if (directions.diagonal /*&& !shield_button_pressed*/) {
            if (!inputs.b) {
                // 3250 7625 - 23.09deg - 26 61 - modY
                // 3625 7000 - 27.38deg - 29 56 - modY + cDown
                // 4375 7000 - 32.01deg - 35 56 - modY + cLeft
                // 5125 7000 - 36.21deg - 41 56 - modY + cUp
                // 5750 7125 - 38.90deg - 46 57 - modY + cRight
                outputs.leftStickX = 128 + (directions.x * 26);
                outputs.leftStickY = 128 + (directions.y * 61);
                if (inputs.c_down) {
                    outputs.leftStickX = 128 + (directions.x * 29);
                    outputs.leftStickY = 128 + (directions.y * 56);
                }
                if (inputs.c_left) {
                    outputs.leftStickX = 128 + (directions.x * 35);
                    outputs.leftStickY = 128 + (directions.y * 56);
                }
                if (inputs.c_up) {
                    outputs.leftStickX = 128 + (directions.x * 41);
                    outputs.leftStickY = 128 + (directions.y * 56);
                }
                if (inputs.c_right) {
                    outputs.leftStickX = 128 + (directions.x * 46);
                    outputs.leftStickY = 128 + (directions.y * 57);
                }
            } else {
                /* Extended Up B Angles */
                // 3875 9125 - 23.01deg - 31 73 - modY + B
                // 4625 8750 - 27.86deg - 37 70 - modY + B + cDown
                // 5250 8500 - 31.70deg - 42 68 - modY + B + cLeft
                // 5750 7875 - 36.14deg - 46 63 - modY + B + cUp
                // 5750 7125 - 38.90deg - 46 57 - modY + B + cRight
                outputs.leftStickX = 128 + (directions.x * 31);
                outputs.leftStickY = 128 + (directions.y * 73);
                if (inputs.c_down) {
                    outputs.leftStickX = 128 + (directions.x * 37);
                    outputs.leftStickY = 128 + (directions.y * 70);
                }
                if (inputs.c_left) {
                    outputs.leftStickX = 128 + (directions.x * 42);
                    outputs.leftStickY = 128 + (directions.y * 68);
                }
                if (inputs.c_up) {
                    outputs.leftStickX = 128 + (directions.x * 46);
                    outputs.leftStickY = 128 + (directions.y * 63);
                }
                if (inputs.c_right) {
                    outputs.leftStickX = 128 + (directions.x * 46);
                    outputs.leftStickY = 128 + (directions.y * 57);
                }
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

    /*
    // Horizontal SOCD overrides X-axis modifiers (for ledgedash maximum jump
    // trajectory).
    if (_horizontal_socd && !directions.vertical) {
        outputs.leftStickX = 128 + (directions.x * 80);
    }
    */

    if (inputs.lightshield) {
        outputs.triggerRAnalog = 49;
    }
    if (inputs.midshield) {
        outputs.triggerRAnalog = 94;
    }

    if (outputs.triggerLDigital) {
        outputs.triggerLAnalog = 140;
    }
    if (outputs.triggerRDigital) {
        outputs.triggerRAnalog = 140;
    }

    // Shut off C-stick when using D-Pad layer.
    if ((inputs.mod_x && inputs.mod_y) || inputs.nunchuk_c) {
        outputs.rightStickX = 128;
        outputs.rightStickY = 128;
    }
}