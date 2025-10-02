/* keymap.c — size-optimized (no float, no sinf) */
#include QMK_KEYBOARD_H
#include "quantum.h"
#include "rgb_matrix.h"
#include "pointing_device.h"
#include "timer.h"

// ------- レイヤ定義 -------
enum {
    L_BASE = 0,   // Layer0: 純正 SOLID_REACTIVE_MULTIWIDE
    L_ICE  = 1,   // Layer1: アイスウェーブ
    L_STAT = 2,   // Layer2: スタティック
    L_MOUSE= 3    // Layer3: マウス/スクロールムーブ
};

static bool    g_rainbow_mode = false;      // OFF→ONでトグル
static bool    g_last_enabled = true;
static uint8_t g_user_hue     = 160;        // スタティック色

typedef struct { int16_t x; int16_t y; } cursor_t;
static cursor_t g_cursor = {112, 32};       // 224×64座標系の中央
static uint16_t g_last_scroll_ms = 0;
static int8_t   g_scroll_band    = 0;       // 0..11

extern led_config_t g_led_config;           // 既存宣言（constなし）

// ---- HSV→RGB（QMK標準APIラッパ：命名衝突回避）----
static inline void hsv_to_rgb_bytes(uint8_t h, uint8_t s, uint8_t v,
                                    uint8_t *r, uint8_t *g, uint8_t *b) {
    HSV hsv = (HSV){ h, s, v };
    RGB rgb = hsv_to_rgb(hsv);
    *r = rgb.r; *g = rgb.g; *b = rgb.b;
}
static inline void apply_palette_rainbow(uint16_t t, uint8_t base_h, uint8_t *h_out) {
    *h_out = base_h + (t / 10);
}
// 8bit三角波（0..254）
static inline uint8_t tri8(uint16_t x) {
    uint8_t y = x & 0xFF; if (y & 0x80) y = 255 - y; return (uint8_t)(y << 1);
}
static inline uint16_t uabs16(int16_t v){ return (v < 0) ? (uint16_t)(-v) : (uint16_t)v; }

// 近傍減衰（整数）
static void light_trail(uint8_t val, uint8_t sat, bool wrap_x, bool rainbowish) {
    uint16_t now = timer_read();
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        uint8_t lx = g_led_config.point[i].x, ly = g_led_config.point[i].y;
        int16_t dx = (int16_t)lx - g_cursor.x;
        if (wrap_x) { if (dx > 112) dx -= 224; else if (dx < -112) dx += 224; }
        int16_t dy = (int16_t)ly - g_cursor.y;
        uint16_t md = uabs16(dx) + uabs16(dy);

        uint8_t v = 0;
        if      (md < 12) v = val;
        else if (md < 24) v = (uint8_t)((val * 3) / 5);
        else if (md < 40) v = (uint8_t)(val / 4);

        if (v) {
            uint8_t h = g_user_hue;
            if (rainbowish) apply_palette_rainbow(now + (i * 16), g_user_hue, &h);
            uint8_t r,g,b; hsv_to_rgb_bytes(h, sat, v, &r, &g, &b);
            rgb_matrix_set_color(i, r, g, b);
        }
    }
}

// (1) アイスウェーブ（白基調＋青帯／整数）
static void render_icewave(void) {
    uint16_t t = timer_read(), t8 = (t >> 2);
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        uint8_t lx = g_led_config.point[i].x;
        uint8_t band = tri8((uint16_t)lx + t8);             // 0..254
        uint8_t add  = (band > 192) ? (uint8_t)(band - 192) : 0; // 0..62
        uint8_t r=20, g=20, b=(uint8_t)(20 + add);
        if (g_rainbow_mode) { uint8_t h; apply_palette_rainbow(t + (uint16_t)lx * 3, 160, &h);
            hsv_to_rgb_bytes(h, 200, (uint8_t)(30 + add), &r, &g, &b); }
        rgb_matrix_set_color(i, r, g, b);
    }
}
// (2) スタティック
static void render_static(void) {
    uint16_t t = timer_read(); uint8_t h = g_user_hue;
    if (g_rainbow_mode) apply_palette_rainbow(t, h, &h);
    uint8_t r,g,b; hsv_to_rgb_bytes(h, 200, 120, &r, &g, &b);
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) rgb_matrix_set_color(i, r, g, b);
}
// (3) マウスムーブ & (4) スクロールムーブ
static void render_mousemove(void){ light_trail(170, 220, false, g_rainbow_mode); }
static void render_scrollmove(void){ g_cursor.x = (int16_t)((g_scroll_band * 224) / 12); g_cursor.y = 32; light_trail(180,220,true,g_rainbow_mode); }

// OFF→ONでモードトグル
void matrix_scan_user(void){
    bool en = rgb_matrix_is_enabled();
    if (en && !g_last_enabled) g_rainbow_mode = !g_rainbow_mode;
    g_last_ena
