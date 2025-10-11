#include QMK_KEYBOARD_H

enum dasbob_layers { _QWERTY, _SYMBOL, _NUMBER, _NAV };
// Tap Dance keycodes
// https://docs.qmk.fm/features/tap_dance#example-5
enum td_keycodes { SHIFT_NAV };

// Define a type containing as many tapdance states as you need
typedef enum { TD_NONE, TD_UNKNOWN, TD_SINGLE_TAP, TD_SINGLE_HOLD, TD_DOUBLE_SINGLE_TAP } td_state_t;

// Create a global instance of the tapdance state type
static td_state_t td_state;

// Declare your tapdance functions:

// Function to determine the current tapdance state
td_state_t cur_dance(tap_dance_state_t *state);

// `finished` and `reset` functions for each tapdance keycode
void shift_nav_finished(tap_dance_state_t *state, void *user_data);
void shift_nav_reset(tap_dance_state_t *state, void *user_data);

// Defaults match https://docs.qmk.fm/tap_hold#is-flow-tap-key
// excluding space as I use this for symbol layer so expect to use these symbols in fast typing.
bool is_flow_tap_key(uint16_t keycode) {
    if ((get_mods() & (MOD_MASK_CG | MOD_BIT_LALT)) != 0) {
        return false; // Disable Flow Tap on hotkeys.
    }
    switch (get_tap_keycode(keycode)) {
        case KC_A ... KC_Z:
        case KC_DOT:
        case KC_COMM:
        case KC_SCLN:
        case KC_SLSH:
            return true;
    }
    return false;
}
const uint16_t PROGMEM esc_combo[]  = {LCTL_T(KC_J), LSFT_T(KC_K), COMBO_END};
combo_t key_combos[] = {
    COMBO(esc_combo, KC_ESC),
};

// Ensure chordal hold works with handedness except for the thumb cluster
// TODO: Figure out why this breaks qmkfmt.
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM =
    LAYOUT_split_3x5_3(
        'L', 'L', 'L', 'L', 'L',       'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L',       'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L',       'R', 'R', 'R', 'R', 'R',
                  '*', '*', '*',       '*', '*', '*'
    );

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
     /*
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │ Q │ W │ E │ R │ T │       │ Y │ U │ I │ O │ P │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ A │ S │ D │ F │ G │       │ H │ J │ K │ L │ [ │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ Z │ X │ C │ V │ B │       │ N │ M │ , │ . │ ] │
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *           ┌───┐                   ┌───┐
      *           │XXX├───┐           ┌───┤XXX│
      *           └───┤SPC├───┐   ┌───┤SPC├───┘
      *               └───│LOW│   │ENT├───┘
      *                   └───┘   └───┘
      */


    [_QWERTY] = LAYOUT_split_3x5_3(
        KC_Q,         KC_W,         KC_E,         KC_R,         KC_T,                    KC_Y,   KC_U,                KC_I,         KC_O,         KC_P,
        LGUI_T(KC_A), LALT_T(KC_S), LSFT_T(KC_D), LCTL_T(KC_F), KC_G,                    KC_H,   LCTL_T(KC_J),        LSFT_T(KC_K), RALT_T(KC_L), RGUI_T(KC_LBRC),
        KC_Z,         KC_X,         KC_C,         KC_V,         KC_B,                    KC_N,   KC_M,                KC_COMM,      KC_DOT,       KC_RBRC,
                                    XXXXXXX,      MO(_NUMBER),  TD(SHIFT_NAV),           KC_ENT, LT(_SYMBOL, KC_SPC), XXXXXXX
    ),

    // https://docs.qmk.fm/keycodes_us_ansi_shifted
    [_SYMBOL] = LAYOUT_split_3x5_3(
        KC_EXLM, KC_LCBR, KC_RCBR, KC_SCLN, KC_QUES,           KC_GRV,  KC_AMPR, KC_HASH,      KC_AT,   KC_BSLS,
        KC_CIRC, KC_EQL,  KC_UNDS, KC_DLR,  KC_ASTR,           KC_DQUO, KC_BSPC, KC_TAB,       KC_SPC,  KC_PLUS,
        KC_LPRN, KC_PIPE, KC_MINS, KC_RPRN, KC_SLSH,           KC_QUOT, KC_DEL,  LSFT(KC_TAB), KC_TILD, XXXXXXX,
                          XXXXXXX, KC_PERC, KC_COLN,           _______, _______, XXXXXXX
    ),

    [_NUMBER] = LAYOUT_split_3x5_3(
        _______, _______, _______, _______, _______,           LSFT(KC_G), KC_7, KC_8,   KC_9, _______,
        KC_LGUI, KC_LALT, KC_LSFT, KC_LCTL, _______,           KC_K,       KC_4, KC_5,   KC_6, _______,
        _______, _______, _______, _______, _______,           KC_J,       KC_1, KC_2,   KC_3, _______,
                          _______, _______, _______,           _______,    KC_0, _______
    ),

    [_NAV] = LAYOUT_split_3x5_3(
        _______, _______, _______, _______, _______,           _______, _______, _______, _______,  _______,
        _______, _______, _______, _______, _______,           KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT, _______,
        _______, _______, _______, _______, _______,           _______, _______, _______, _______,  _______,
                          _______, _______, _______,           _______, _______, _______
    ),

};

// Determine the tapdance state to return
td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed)
            return TD_SINGLE_TAP;
        else
            return TD_SINGLE_HOLD;
    }

    if (state->count == 2)
        return TD_DOUBLE_SINGLE_TAP;
    else
        return TD_UNKNOWN; // Any number higher than the maximum state value you return above
}

// Handle the possible states for each tapdance keycode you define:

void shift_nav_finished(tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);
    switch (td_state) {
        case TD_SINGLE_TAP:
            set_oneshot_mods(MOD_BIT(KC_LSFT));
            break;
        case TD_SINGLE_HOLD:
            layer_on(_NAV);
            break;
        case TD_DOUBLE_SINGLE_TAP:
            caps_word_on();
            break;
        default:
            break;
    }
}

void shift_nav_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state) {
        case TD_SINGLE_TAP:
            break;
        case TD_SINGLE_HOLD:
            clear_oneshot_mods();
            layer_off(_NAV);
            break;
        case TD_DOUBLE_SINGLE_TAP:
            // Do nothing for caps word disable
            break;
        default:
            break;
    }
}

// Define `ACTION_TAP_DANCE_FN_ADVANCED()` for each tapdance keycode, passing in `finished` and `reset` functions
tap_dance_action_t tap_dance_actions[] = {[SHIFT_NAV] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, shift_nav_finished, shift_nav_reset)};
#ifdef OLED_ENABLE

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_0;
}
bool oled_task_user(void) {
    if (is_keyboard_master()) {
        static const char PROGMEM runqmk_logo[] = {0, 0, 0, 0, 152, 152, 152, 152, 152, 152, 24,  24, 24, 152, 152, 24, 24,  24,  152, 152, 24,  24, 152, 152, 24, 24, 24,  152, 152, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 127, 13, 13, 29, 63, 103, 64, 0,  63, 127, 96, 96, 96, 127, 63, 0,  0,  127, 127, 7,  12, 56, 127, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                   0, 0, 0, 0, 120, 252, 134, 198, 198, 252, 120, 0,  0,  254, 254, 60, 224, 224, 60,  254, 254, 0,  254, 254, 16, 24, 124, 230, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24,  24,  25, 25, 25, 25, 27,  24, 24, 25, 25,  24, 25, 25, 24,  25, 25, 24, 25,  25,  24, 24, 24, 24,  25,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        oled_write_raw_P(runqmk_logo, sizeof(runqmk_logo));

        led_t led_usb_state = host_keyboard_led_state();
        oled_set_cursor(6, 3);
        oled_write_P(led_usb_state.num_lock ? PSTR("NUM") : PSTR(""), false);
        oled_set_cursor(6, 2);
        oled_write_P(PSTR("WPM: "), false);
        oled_write(get_u8_str(get_current_wpm(), '0'), false);
        oled_set_cursor(6, 0);
        oled_write_P(PSTR("by GroooveBob"), false);
        oled_set_cursor(6, 1);
        oled_write_P(PSTR("Layer: "), false);

        switch (get_highest_layer(layer_state)) {
            case _QWERTY:
                oled_set_cursor(12, 1);
                oled_write_P(PSTR("Default\n"), false);
                break;
            case _SYMBOL:
                oled_set_cursor(12, 1);
                oled_write_P(PSTR("Symbol\n"), false);
                break;
            case _NUMBER:
                oled_set_cursor(12, 1);
                oled_write_P(PSTR("Number\n"), false);
                break;
            case _NAV:
                oled_set_cursor(12, 1);
                oled_write_P(PSTR("Navigation\n"), false);
                break;
            default:
                // Or use the write_ln shortcut over adding '\n' to the end of your string
                oled_set_cursor(6, 1);
                oled_write_ln_P(PSTR("Undefined"), false);
        }
    } else {
        static const char PROGMEM dasbob_logo[] = {
            0,   0, 0,   0,   0,   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 0,   0,   0,   0,   0,   128, 128, 128, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 128, 128, 0, 0, 0,  0,   0,   0,   0,   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 0,   0,   0,   0,   0,   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 0,   0,   0,   128, 128, 128, 128, 128, 192, 192, 192, 192, 192, 0,   0,   0,   128, 128, 128, 128, 128, 128, 128, 128, 128, 0, 0,  0,  0,   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 0,   0,  0, 0, 126, 127, 255, 255, 255, 255, 255, 255, 255, 199, 247, 255, 255, 255, 255, 255, 255, 255, 255, 127, 252, 255, 255, 255, 255, 255, 255, 255, 255, 255, 15,  127, 255, 255, 255, 255, 255, 255, 255, 255, 254,
            248, 0, 126, 127, 255, 255, 255, 255, 255, 255, 255, 199, 247, 255, 255, 255, 255, 255, 255, 255, 255, 127, 126, 255, 255, 255, 255, 255, 255, 255, 255, 239, 239, 239, 239, 207, 223, 223, 223, 159, 63,  0, 7, 63, 255, 255, 255, 255, 255, 255, 255, 254, 14,  254, 255, 255, 255, 255, 255, 255, 255, 63,  241, 252, 254, 255, 255, 255, 255, 255, 255, 255, 31,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 128, 0,   0,   0,   126, 255, 255, 255, 255, 255, 255, 255, 255, 227, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 15, 63, 127, 255, 255, 255, 255, 255, 255, 255, 248, 255, 255, 255, 255, 255, 255, 255, 255, 127, 63, 7, 0, 126, 255, 255, 255, 255, 255, 255, 255, 255, 227, 255, 255, 255, 255, 255, 255, 255, 255, 255, 128, 252, 253, 249, 249, 251, 251, 251, 251, 251, 255, 255, 255, 255, 255, 255, 255, 255, 127, 30,  0,   0,
            0,   3, 31,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 7,   0,   63,  255, 255, 255, 255, 255, 255, 255, 255, 255, 248, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0,  0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0, 0,  0,  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,  0, 0, 0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,
        };
        oled_write_raw_P(dasbob_logo, sizeof(dasbob_logo));
    }
    return false;
}
#endif

