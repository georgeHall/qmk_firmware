/*
 * Copyright 2022 Uthol
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "helix.h"

enum uthol_layers { _QWERTY, _COLEMAK, _LOWER, _RAISE, _SETTINGS };

#define QWERTY DF(_QWERTY)
#define COLEMAK DF(_COLEMAK)
#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)
#define SETTINGS MO(_SETTINGS)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY]   = LAYOUT(KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_DEL, KC_ESC, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_ENT, KC_LCTL, SETTINGS, KC_LGUI, KC_LALT, RAISE, KC_SPC, LOWER, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT),
    [_COLEMAK]  = LAYOUT(KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC, KC_TAB, KC_Q, KC_W, KC_F, KC_P, KC_B, KC_J, KC_L, KC_U, KC_Y, KC_SCLN, KC_DEL, KC_ESC, KC_A, KC_R, KC_S, KC_T, KC_G, KC_M, KC_N, KC_E, KC_I, KC_O, KC_QUOT, KC_LSFT, KC_Z, KC_X, KC_C, KC_D, KC_V, KC_K, KC_H, KC_COMM, KC_DOT, KC_SLSH, KC_ENT, KC_LCTL, SETTINGS, KC_LGUI, KC_LALT, RAISE, KC_SPC, LOWER, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT),
    [_LOWER]    = LAYOUT(KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_MINS, KC_EQL, KC_INS, KC_TRNS, KC_P7, KC_P8, KC_P9, KC_PMNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LBRC, KC_RBRC, KC_BSLS, KC_P0, KC_P4, KC_P5, KC_P6, KC_PPLS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MSTP, KC_TRNS, KC_TRNS, KC_P1, KC_P2, KC_P3, KC_PEQL, KC_TRNS, KC_TRNS, KC_TRNS, KC_HOME, KC_PGDN, KC_PGUP, KC_END, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT),
    [_RAISE]    = LAYOUT(KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_TRNS, KC_P7, KC_P8, KC_P9, KC_PMNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MPLY, KC_BSLS, KC_P0, KC_P4, KC_P5, KC_P6, KC_PPLS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MSTP, KC_TRNS, KC_TRNS, KC_P1, KC_P2, KC_P3, KC_EQL, KC_TRNS, KC_TRNS, KC_TRNS, KC_HOME, KC_PGDN, KC_PGUP, KC_END, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_SPC, KC_TRNS, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT),
    [_SETTINGS] = LAYOUT(QWERTY, COLEMAK, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, QK_BOOT, UG_TOGG, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_SLEP, KC_PWR, KC_NO, KC_SCRL, KC_NO, KC_NO, KC_PSCR, KC_NO, KC_NO, KC_NO, KC_NO, UG_HUED, UG_HUEU, KC_NO, KC_NO, KC_NO, KC_NUM, KC_CAPS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, UG_SATD, UG_SATU, RGB_M_P, KC_NO, KC_TRNS, KC_NO, KC_NO, KC_TRNS, KC_TRNS, KC_TRNS, UG_PREV, UG_VALD, UV_VALU, UG_NEXT)
};

#define ANIM_SIZE 1024           // number of bytes in array, minimize for adequate firmware size, max is 1024
#define ANIM_FRAMES 50           // Number of frames
#define ANIM_FRAME_DURATION 160  // How long each frame lasts in ms

uint32_t anim_timer         = 0;
uint32_t anim_sleep         = 0;
uint8_t  current_idle_frame = 0;

static void render_anim(void) {
    // assumes 1 frame prep stage
    void animation_phase(void) {
        current_idle_frame = (current_idle_frame + 1) % ANIM_FRAMES;
        oled_write_raw_P(animation[abs((ANIM_FRAMES - 1) - current_idle_frame)], ANIM_SIZE);
    }
    if (get_current_wpm() != 000) {
        oled_on();
        if (timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
            anim_timer = timer_read32();
            animation_phase();
        }
        anim_sleep = timer_read32();
    } else {
        if (timer_elapsed32(anim_sleep) > OLED_TIMEOUT) {
            oled_off();
        } else {
            if (timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
                anim_timer = timer_read32();
                animation_phase();
            }
        }
    }
}

// Used to draw on to the oled screen
bool oled_task_user(void) {
    render_anim();
    oled_write_P(PSTR("Layer:"), false);
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("QWERTY\n"), false);
            break;
        case _LOWER:
            oled_write_P(PSTR("Lower\n"), false);
            break;
        case _RAISE:
            oled_write_P(PSTR("Raise\n"), false);
            break;
        case _COLEMAK:
            oled_write_P(PSTR("Colemak\n"), false);
            break;
        case _SETTINGS:
            oled_write_P(PSTR("Settings\n"), false);
            break;
        default:
            oled_write_ln_P(PSTR("Undefined"), false);
    }
    oled_write_P(PSTR("WPM: "), false);
    oled_write(get_u8_str(get_current_wpm(), '0'), false);

    return false;
}
