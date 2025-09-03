/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

#include "quantum.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default (VIA)
  [0] = LAYOUT_universal(
    KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                            KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     ,
    KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                            KC_H     , KC_J     , KC_K     , KC_L     , KC_MINS  ,
    KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     ,                            KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  ,
    KC_LCTL  , KC_LGUI  , KC_LALT  ,LSFT_T(KC_LNG2),LT(1,KC_SPC),LT(3,KC_LNG1),KC_BSPC,LT(2,KC_ENT),LSFT_T(KC_LNG2),KC_RALT,KC_RGUI, KC_RSFT
  ),

  [1] = LAYOUT_universal(
    KC_F1    , KC_F2    , KC_F3    , KC_F4    , KC_RBRC  ,                            KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   ,
    KC_F5    , KC_EXLM  , S(KC_6)  ,S(KC_INT3), S(KC_8)  ,                           S(KC_INT1), KC_BTN1  , KC_PGUP  , KC_BTN2  , KC_SCLN  ,
    S(KC_EQL),S(KC_LBRC),S(KC_7)   , S(KC_2)  ,S(KC_RBRC),                            KC_LBRC  , KC_DLR   , KC_PGDN  , KC_BTN3  , KC_F11   ,
    KC_INT1  , KC_EQL   , S(KC_3)  , _______  , _______  , _______  ,      TO(2)    , TO(0)    , _______  , KC_RALT  , KC_RGUI  , KC_F12
  ),

  [2] = LAYOUT_universal(
    KC_TAB   , KC_7     , KC_8     , KC_9     , KC_MINS  ,                            KC_NUHS  , _______  , KC_BTN3  , _______  , KC_BSPC  ,
   S(KC_QUOT), KC_4     , KC_5     , KC_6     ,S(KC_SCLN),                            S(KC_9)  , KC_BTN1  , KC_UP    , KC_BTN2  , KC_QUOT  ,
    KC_SLSH  , KC_1     , KC_2     , KC_3     ,S(KC_MINS),                           S(KC_NUHS), KC_LEFT  , KC_DOWN  , KC_RGHT  , _______  ,
    KC_ESC   , KC_0     , KC_DOT   , KC_DEL   , KC_ENT   , KC_BSPC  ,      _______  , _______  , _______  , _______  , _______  , _______
  ),

  [3] = LAYOUT_universal(
    RGB_TOG  , AML_TO   , AML_I50  , AML_D50  , _______  ,                            _______  , _______  , SSNP_HOR , SSNP_VRT , SSNP_FRE ,
    RGB_MOD  , RGB_HUI  , RGB_SAI  , RGB_VAI  , SCRL_DVI ,                            _______  , _______  , _______  , _______  , _______  ,
    RGB_RMOD , RGB_HUD  , RGB_SAD  , RGB_VAD  , SCRL_DVD ,                            CPI_D1K  , CPI_D100 , CPI_I100 , CPI_I1K  , KBC_SAVE ,
    QK_BOOT  , KBC_RST  , _______  , _______  , _______  , _______  ,      _______  , _______  , _______  , _______  , KBC_RST  , QK_BOOT
  ),
};
// clang-format on

// layer_state_t layer_state_set_user(layer_state_t state)
// {
//   // レイヤーが1または3の場合、スクロールモードが有効になる
//   keyball_set_scroll_mode(get_highest_layer(state) == 3);
//   // keyball_set_scroll_mode(get_highest_layer(state) == 1 || get_highest_layer(state) == 3);

//   // レイヤーとLEDを連動させる
//   uint8_t layer = biton32(state);
//   switch (layer)
//   {
//   case 4:
//       //rgblight_sethsv(HSV_WHITE);
//       rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
//     break;

//   default:
//     //rgblight_sethsv(HSV_OFF);
//     rgb_matrix_reload_from_eeprom();
//   }
//   return state;
// }

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif

// led_config_t g_led_config = {
//     {// Key Matrix to LED Index
//      /*
//      {14,  10,   6,   3,   0,               43,    40,    37,    34,  30}, \
//      {15,  11,   7,   4,   1,               44,    41,    38,    35,  31}, \
//      {16,  12,   8,   5,   2,               45,    42,    39,    36,  32}, \
//      {17,  13,   9,  21,  22,  23,     24,  25,NO_LED,NO_LED,NO_LED,  33}, \
//      //   {18,  19,  20,                      ,    26,    27,    28,  29}  //
//      backlights
//      */
//      {14, 10, 6, 3, 0, 18},
//      {15, 11, 7, 4, 1, 19},
//      {16, 12, 8, 5, 2, 20},
//      {17, 13, 9, 21, 22, 23},
//      {30, 34, 37, 40, 43, 26},
//      {31, 35, 38, 41, 44, 27},
//      {32, 36, 39, 42, 45, 28},
//      {33, NO_LED, NO_LED, 29, 25, 24}},
//     {
//         // LED Index to Physical Position
//         // {x,y}: x = 224 / (NUMBER_OF_COLS - 1) * COL_POSITION , y =  64 /
//         // (NUMBER_OF_ROWS - 1) * ROW_POSITION
//         {60, 0},   // 0
//         {60, 21},  // 1
//         {60, 43},  // 2
//         {45, 0},   // 3
//         {45, 21},  // 4
//         {45, 43},  // 5
//         {30, 0},   // 6
//         {30, 21},  // 7
//         {30, 43},  // 8
//         {30, 64},  // 9
//         {15, 0},   // 10
//         {15, 21},  // 11
//         {15, 43},  // 12
//         {15, 64},  // 13
//         {0, 0},    // 14
//         {0, 21},   // 15
//         {0, 43},   // 16
//         {0, 64},   // 17
//         {45, 0},   // 18
//         {0, 0},    // 19
//         {0, 43},   // 20
//         {30, 43},  // 21
//         {60, 64},  // 22
//         {75, 64},  // 23
//         {149, 64}, // 24
//         {164, 64}, // 25
//         {187, 43}, // 26
//         {224, 43}, // 27
//         {224, 0},  // 28
//         {179, 0},  // 29
//         {224, 0},  // 30
//         {224, 21}, // 31
//         {224, 43}, // 32
//         {224, 64}, // 33
//         {209, 0},  // 34
//         {209, 21}, // 35
//         {209, 43}, // 36
//         {194, 0},  // 37
//         {194, 21}, // 38
//         {194, 43}, // 39
//         {179, 0},  // 40
//         {179, 21}, // 41
//         {179, 43}, // 42
//         {164, 0},  // 43
//         {164, 21}, // 44
//         {164, 43}, // 45
//         {0, 0},    // 46
//         {0, 0},    // 47
//     },
//     {
//         // LED Index to Flag
//         LED_FLAG_KEYLIGHT, // 0
//         LED_FLAG_KEYLIGHT, // 1
//         LED_FLAG_KEYLIGHT, // 2
//         LED_FLAG_KEYLIGHT, // 3
//         LED_FLAG_KEYLIGHT, // 4
//         LED_FLAG_KEYLIGHT, // 5
//         LED_FLAG_KEYLIGHT, // 6
//         LED_FLAG_KEYLIGHT, // 7
//         LED_FLAG_KEYLIGHT, // 8
//         LED_FLAG_KEYLIGHT, // 9
//         LED_FLAG_KEYLIGHT, // 10
//         LED_FLAG_KEYLIGHT, // 11
//         LED_FLAG_KEYLIGHT, // 12
//         LED_FLAG_KEYLIGHT, // 13
//         LED_FLAG_KEYLIGHT, // 14
//         LED_FLAG_KEYLIGHT, // 15
//         LED_FLAG_KEYLIGHT, // 16
//         LED_FLAG_KEYLIGHT, // 17
//         LED_FLAG_KEYLIGHT, // 18
//         LED_FLAG_KEYLIGHT, // 19
//         LED_FLAG_KEYLIGHT, // 20
//         LED_FLAG_KEYLIGHT, // 21
//         LED_FLAG_KEYLIGHT, // 22
//         LED_FLAG_KEYLIGHT, // 23
//         LED_FLAG_KEYLIGHT, // 24
//         LED_FLAG_KEYLIGHT, // 25
//         LED_FLAG_KEYLIGHT, // 26
//         LED_FLAG_KEYLIGHT, // 27
//         LED_FLAG_KEYLIGHT, // 28
//         LED_FLAG_KEYLIGHT, // 29
//         LED_FLAG_KEYLIGHT, // 30
//         LED_FLAG_KEYLIGHT, // 31
//         LED_FLAG_KEYLIGHT, // 32
//         LED_FLAG_KEYLIGHT, // 33
//         LED_FLAG_KEYLIGHT, // 34
//         LED_FLAG_KEYLIGHT, // 35
//         LED_FLAG_KEYLIGHT, // 36
//         LED_FLAG_KEYLIGHT, // 37
//         LED_FLAG_KEYLIGHT, // 38
//         LED_FLAG_KEYLIGHT, // 39
//         LED_FLAG_KEYLIGHT, // 40
//         LED_FLAG_KEYLIGHT, // 41
//         LED_FLAG_KEYLIGHT, // 42
//         LED_FLAG_KEYLIGHT, // 43
//         LED_FLAG_KEYLIGHT, // 44
//         LED_FLAG_KEYLIGHT, // 45
//         LED_FLAG_NONE,     // 46
//         LED_FLAG_NONE      // 47
//     }};

//     void keyboard_post_init_user(void) {
//   // Call the post init code.
//   rgb_matrix_reload_from_eeprom();
// }

