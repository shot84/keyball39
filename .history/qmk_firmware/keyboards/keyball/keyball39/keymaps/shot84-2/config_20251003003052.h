#pragma once

// ==== 分割ポイントデバイス（右ボール→左マスター）====
#define SPLIT_POINTING_ENABLE
#define POINTING_DEVICE_RIGHT

// ==== Layer数ビット幅（4層なら8bitで十分：わずかに削減）====
#define LAYER_STATE_8BIT

// ==== RGB Matrix 基本 ====
#define RGB_MATRIX_LED_COUNT 48
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 100
#define RGB_MATRIX_DEFAULT_HUE 125
#define RGB_MATRIX_DEFAULT_SAT 255
#define RGB_MATRIX_DEFAULT_VAL 100

// ==== 必要最低限の効果だけを有効化 ====
#define RGB_MATRIX_KEYPRESSES                       // Reactive系に必要
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE  // Layer0で使用
#define ENABLE_RGB_MATRIX_SOLID_COLOR               // 他レイヤの下地として利用
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE

// ==== （容量節約）ヒートマップ/フレームバッファ効果はOFF ====
// /* 余裕が出たら下を有効化：重いです（数百B～KB）
// #define RGB_MATRIX_FRAMEBUFFER_EFFECTS
// #define ENABLE_RGB_MATRIX_TYPING_HEATMAP
// #define RGB_MATRIX_TYPING_HEATMAP_SPREAD 48
// #define RGB_MATRIX_TYPING_HEATMAP_AREA_LIMIT 25
// #define RGB_MATRIX_TYPING_HEATMAP_DECREASE_DELAY_MS 25
// */

// ==== デバッグ出力は止める（CONSOLE_ENABLE=no時の保険）====
#ifndef NO_DEBUG
#  define NO_DEBUG
#endif
#if !defined(NO_PRINT) && !defined(CONSOLE_ENABLE)
#  define NO_PRINT
#endif
