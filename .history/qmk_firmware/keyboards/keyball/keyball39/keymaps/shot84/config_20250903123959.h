/*
This is the c configuration file for the keymap

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

#pragma once

#ifdef RGBLIGHT_ENABLE
#    define RGBLIGHT_EFFECT_BREATHING
//#    define RGBLIGHT_EFFECT_RAINBOW_MOOD
//#    define RGBLIGHT_EFFECT_RAINBOW_SWIRL
//#    define RGBLIGHT_EFFECT_SNAKE
//#    define RGBLIGHT_EFFECT_KNIGHT
//#    define RGBLIGHT_EFFECT_CHRISTMAS
// #    define RGBLIGHT_EFFECT_STATIC_GRADIENT
//#    define RGBLIGHT_EFFECT_RGB_TEST
//#    define RGBLIGHT_EFFECT_ALTERNATING
//#    define RGBLIGHT_EFFECT_TWINKLE
// #endif

#define TAP_CODE_DELAY 5

#define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#define AUTO_MOUSE_DEFAULT_LAYER 3
#define DYNAMIC_KEYMAP_LAYER_COUNT 5

// #define DRIVER_LED_TOTAL 48
// #define RGB_MATRIX_LED_COUNT 48
// #define RGB_MATRIX_MAXIMUM_BRIGHTNESS 100

// #define RGB_MATRIX_DEFAULT_HUE 100
// #define RGB_MATRIX_DEFAULT_SAT 255
// #define RGB_MATRIX_DEFAULT_VAL 100
// #define RGB_MATRIX_DEFAULT_SPD 50

// #define RGB_MATRIX_FRAMEBUFFER_EFFECTS
// #define ENABLE_RGB_MATRIX_TYPING_HEATMAP
// #define RGB_MATRIX_TYPING_HEATMAP_SPREAD 48
// #define RGB_MATRIX_TYPING_HEATMAP_AREA_LIMIT 25
// #define RGB_MATRIX_TYPING_HEATMAP_DECREASE_DELAY_MS 25

// #define RGB_MATRIX_KEYPRESSES // reacts to keypresses
// #define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE

// #define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE