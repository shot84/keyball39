#pragma once

// ==== キータップとVIA層数 ====
#define TAP_CODE_DELAY 5
#define DYNAMIC_KEYMAP_LAYER_COUNT 5

// ==== 分割ポイントデバイス（右ボール→左マスター）====
#define SPLIT_POINTING_ENABLE
#define POINTING_DEVICE_RIGHT
#define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#define AUTO_MOUSE_DEFAULT_LAYER 3

// ==== RGB Matrix 基本 ====
#define RGB_MATRIX_LED_COUNT 48
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 100
#define RGB_MATRIX_DEFAULT_HUE 125
#define RGB_MATRIX_DEFAULT_SAT 255
#define RGB_MATRIX_DEFAULT_VAL 100

// ==== Layer0で使う純正エフェクト ====
#define RGB_MATRIX_KEYPRESSES
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE

// ==== （容量節約）ヒートマップ系は既定でOFF ====
// 余裕があれば、以下の #if 0 を 1 にして有効化してください。
#if 0
  #define RGB_MATRIX_FRAMEBUFFER_EFFECTS
  #define ENABLE_RGB_MATRIX_TYPING_HEATMAP
  #define RGB_MATRIX_TYPING_HEATMAP_SPREAD 48
  #define RGB_MATRIX_TYPING_HEATMAP_AREA_LIMIT 25
  #define RGB_MATRIX_TYPING_HEATMAP_DECREASE_DELAY_MS 25
#endif

// ==== デバッグ出力を止めて数百B削減 ====
#ifndef NO_DEBUG
#  define NO_DEBUG
#endif
#if !defined(NO_PRINT) && !defined(CONSOLE_ENABLE)
#  define NO_PRINT
#endif
