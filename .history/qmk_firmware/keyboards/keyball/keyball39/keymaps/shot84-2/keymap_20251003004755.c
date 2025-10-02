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
    g_last_enabled = en;
}
// Hue微調整（例：F13/F14）
bool process_record_user(uint16_t keycode, keyrecord_t *record){
    if (!record->event.pressed) return true;
    switch (keycode){ case KC_F13: g_user_hue += 8; return false; case KC_F14: g_user_hue -= 8; return false; default: return true; }
}
// Pointing device（右→左マスター）
report_mouse_t pointing_device_task_user(report_mouse_t m){
    if (m.x || m.y){
        g_cursor.x += (int16_t)m.x * 3; g_cursor.y -= (int16_t)m.y * 2;
        if (g_cursor.x < 0) g_cursor.x += 224; else if (g_cursor.x >= 224) g_cursor.x -= 224;
        if (g_cursor.y < 0) g_cursor.y = 0; else if (g_cursor.y > 64) g_cursor.y = 64;
    }
    if (m.v){ g_scroll_band += (m.v > 0) ? 1 : -1; if (g_scroll_band < 0) g_scroll_band = 11; if (g_scroll_band > 11) g_scroll_band = 0; g_last_scroll_ms = timer_read(); }
    return m;
}

// 純正/カスタムの切替（Layer0は純正を尊重）
bool rgb_matrix_indicators_user(void){
    uint8_t top = get_highest_layer(layer_state);
    if (top == L_BASE) return true;
    switch (top){
        case L_ICE:  render_icewave(); break;
        case L_STAT: render_static();  break;
        case L_MOUSE: (timer_elapsed(g_last_scroll_ms) < 400) ? render_scrollmove() : render_mousemove(); break;
        default: break;
    }
    return true;
}

// ---- あなたの keymaps / g_led_config は現行のまま ----
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_universal(
    KC_Q, KC_W, KC_E, KC_R, KC_T,                       KC_Y, KC_U, KC_I, KC_O, KC_P,
    KC_A, KC_S, KC_D, KC_F, KC_G,                       KC_H, KC_J, KC_K, KC_L, KC_MINS,
    KC_Z, KC_X, KC_C, KC_V, KC_B,                       KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH,
    KC_LCTL, KC_LGUI, KC_LALT, LSFT_T(KC_LNG2), LT(1,KC_SPC), LT(3,KC_LNG1), KC_BSPC, LT(2,KC_ENT), LSFT_T(KC_LNG2), KC_RALT, KC_RGUI, KC_RSFT
  ),
  [1] = LAYOUT_universal(
    KC_F1, KC_F2, KC_F3, KC_F4, KC_RBRC,                KC_F6, KC_F7, KC_F8, KC_F9, KC_F10,
    KC_F5, KC_EXLM, S(KC_6), S(KC_INT3), S(KC_8),       S(KC_INT1), KC_BTN1, KC_PGUP, KC_BTN2, KC_SCLN,
    S(KC_EQL), S(KC_LBRC), S(KC_7), S(KC_2), S(KC_RBRC), KC_LBRC, KC_DLR, KC_PGDN, KC_BTN3, KC_F11,
    KC_INT1, KC_EQL, S(KC_3), _______, _______, _______, TO(2), TO(0), _______, KC_RALT, KC_RGUI, KC_F12
  ),
  [2] = LAYOUT_universal(
    KC_TAB, KC_7, KC_8, KC_9, KC_MINS,                  KC_NUHS, _______, KC_BTN3, _______, KC_BSPC,
    S(KC_QUOT), KC_4, KC_5, KC_6, S(KC_SCLN),           S(KC_9), KC_BTN1, KC_UP, KC_BTN2, KC_QUOT,
    KC_SLSH, KC_1, KC_2, KC_3, S(KC_MINS),              S(KC_NUHS), KC_LEFT, KC_DOWN, KC_RGHT, _______,
    KC_ESC, KC_0, KC_DOT, KC_DEL, KC_ENT, KC_BSPC,      _______, _______, _______, _______, _______, _______
  ),
  [3] = LAYOUT_universal(
    RGB_TOG, AML_TO, AML_I50, AML_D50, _______,         _______, _______, SSNP_HOR, SSNP_VRT, SSNP_FRE,
    RGB_MOD, RGB_HUI, RGB_SAI, RGB_VAI, SCRL_DVI,       _______, _______, _______, _______, _______,
    RGB_RMOD, RGB_HUD, RGB_SAD, RGB_VAD, SCRL_DVD,      CPI_D1K, CPI_D100, CPI_I100, CPI_I1K, KBC_SAVE,
    QK_BOOT, KBC_RST, _______, _______, _______, _______, _______, _______, _______, _______, KBC_RST, QK_BOOT
  ),
};

layer_state_t layer_state_set_user(layer_state_t state){
  keyball_set_scroll_mode(get_highest_layer(state) == L_MOUSE);
  switch (biton32(state)){
    case L_BASE:  rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE); break;
    case L_ICE:
    case L_STAT:
    case L_MOUSE: rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR); break;
    default: break;
  }
  return state;
}

#ifdef OLED_ENABLE
#  include "lib/oledkit/oledkit.h"
void oledkit_render_info_user(void){
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif

// あなたのg_led_config（現行のまま）
led_config_t g_led_config = {
    {/* matrix->LED */ {14,10,6,3,0,18},{15,11,7,4,1,19},{16,12,8,5,2,20},{17,13,9,21,22,23},{30,34,37,40,43,26},{31,35,38,41,44,27},{32,36,39,42,45,28},{33,NO_LED,NO_LED,29,25,24}},
    {/* pos */ {60,0},{60,21},{60,43},{45,0},{45,21},{45,43},{30,0},{30,21},{30,43},{30,64},{15,0},{15,21},{15,43},{15,64},{0,0},{0,21},{0,43},{0,64},{45,0},{0,0},{0,43},{30,43},{60,64},{75,64},{149,64},{164,64},{187,43},{224,43},{224,0},{179,0},{224,0},{224,21},{224,43},{224,64},{209,0},{209,21},{209,43},{194,0},{194,21},{194,43},{179,0},{179,21},{179,43},{164,0},{164,21},{164,43},{0,0},{0,0},},
    {/* flags */ LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_KEYLIGHT,LED_FLAG_NONE,LED_FLAG_NONE,}
};

void keyboard_post_init_user(void){ rgb_matrix_reload_from_eeprom(); }
