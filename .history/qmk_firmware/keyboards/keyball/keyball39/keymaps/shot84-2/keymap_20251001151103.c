#include QMK_KEYBOARD_H
#include "pointing_device.h"
#include "rgb_matrix.h"
#include "timer.h"

// --- ユーザー設定・状態 --- //
enum {
    L_BASE = 0,   // Layer0: 純正 SOLID_REACTIVE_MULTIWIDE
    L_ICE  = 1,   // Layer1: アイスウェーブ
    L_STAT = 2,   // Layer2: スタティック
    L_MOUSE= 3,   // Layer3: マウスムーブ
    L_SCROLL=4    // Layer4: スクロールムーブ
};

// ノーマル/レインボー（OFF->ONでトグル）
static bool g_rainbow_mode = false;
static bool g_last_enabled = true;

// スタティック用の色相（Hue）
static uint8_t g_user_hue = 160; // 薄い青緑あたり

// マウス／スクロール用の簡易カーソル
typedef struct {
    float x;
    float y;
} cursor_t;

static cursor_t g_cursor = { .x = 112.0f, .y = 32.0f }; // だいたい中央開始（x:0-224, y:0-64相当のつもり）
static uint16_t g_anim_timer = 0;
static uint16_t g_last_scroll_ms = 0;
static int8_t   g_scroll_band = 0; // スクロールで動かす帯のインデックス

// RGBユーティリティ
static inline void hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b) {
    HSV hsv = { .h = h, .s = s, .v = v };
    RGB rgb = hsv_to_rgb(hsv);
    *r = rgb.r; *g = rgb.g; *b = rgb.b;
}

// レインボー調色
static inline void apply_palette_rainbow(uint16_t t, uint8_t base_h, uint8_t *h_out) {
    *h_out = base_h + (t / 10); // 緩やかに色相を進める
}

// g_led_config を使ってLED座標を取得
extern const led_config_t g_led_config;

// 近傍性に基づき、カーソル近辺を光らせる簡易ヘルパ
static void light_trail_around_cursor(uint8_t val, uint8_t sat, bool horizontally_wrap, bool rainbowish) {
    uint16_t now = timer_read();
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        // LEDの実座標
        uint8_t lx = g_led_config.point[i].x; // 0..224
        uint8_t ly = g_led_config.point[i].y; // 0..64

        // 距離計算
        float dx = (float)lx - g_cursor.x;
        float dy = (float)ly - g_cursor.y;

        // 横方向の環状（スクロールモード想定）なら短距離補正
        if (horizontally_wrap) {
            if (dx > 112)  dx -= 224;
            if (dx < -112) dx += 224;
        }

        float d2 = dx*dx + dy*dy;

        // 半径に応じて明るさを減衰（閾値は適当チューニング）
        uint8_t v = 0;
        if (d2 < 200.0f)       v = val;
        else if (d2 < 600.0f)  v = (uint8_t)((float)val * 0.5f);
        else if (d2 < 1300.0f) v = (uint8_t)((float)val * 0.2f);
        else                   v = 0;

        if (v > 0) {
            uint8_t h = g_user_hue;
            if (rainbowish) {
                apply_palette_rainbow(now + (i * 16), g_user_hue, &h);
            }
            uint8_t r,g,b; hsv_to_rgb(h, sat, v, &r, &g, &b);
            rgb_matrix_set_color(i, r, g, b);
        }
    }
}

// アイスウェーブ：白基調に青帯が走る
static void render_icewave(void) {
    uint16_t t = timer_read();
    // ベースを淡い白
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        rgb_matrix_set_color(i, 30, 30, 30);
    }
    // 青グラデーションの波
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        uint8_t lx = g_led_config.point[i].x;
        uint16_t phase = (uint16_t)(lx + (t / 8));
        uint8_t band = (uint8_t)((sinf((float)phase * 0.03f) + 1.0f) * 100.0f); // 0..200
        uint8_t add = band > 150 ? (band - 150) : 0;                            // 山の上だけ足す
        uint8_t r = 20, g = 20, b = 20 + add;
        if (g_rainbow_mode) {
            uint8_t h; apply_palette_rainbow(t + lx*3, 160, &h);
            hsv_to_rgb(h, 200, 30 + add, &r, &g, &b);
        }
        rgb_matrix_set_color(i, r, g, b);
    }
}

// スタティック：単色（Hue可変／レインボーなら緩やか推移）
static void render_static(void) {
    uint16_t t = timer_read();
    uint8_t h = g_user_hue;
    if (g_rainbow_mode) apply_palette_rainbow(t, h, &h);
    uint8_t r,g,b; hsv_to_rgb(h, 200, 120, &r, &g, &b);
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        rgb_matrix_set_color(i, r, g, b);
    }
}

// マウスムーブ：カーソル付近に軌跡（虹 or 単色）
static void render_mousemove(void) {
    light_trail_around_cursor(170, 220, false, g_rainbow_mode);
}

// スクロールムーブ：横帯がスクロール方向に移動（虹 or 単色）
static void render_scrollmove(void) {
    // 横方向の環状を想定してバンド中心xを決める
    float center_x = (float)((g_scroll_band * 224) / 12); // 12バンド想定
    g_cursor.x = center_x;
    // yは中央固定
    g_cursor.y = 32.0f;
    light_trail_around_cursor(180, 220, true, g_rainbow_mode);
}

// --- Layer 切替：Layer0は純正、他レイヤーはカスタム --- //
layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t top = get_highest_layer(state);
    switch (top) {
        case L_BASE:
            // 純正エフェクト（config.h 既定：SOLID_REACTIVE_MULTIWIDE）
            // ここで明示しておくと確実
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE);
            break;
        case L_ICE:
        case L_STAT:
        case L_MOUSE:
        case L_SCROLL:
            // カスタム描画は rgb_matrix_indicators_user() で上書きする
            // 背景効果は薄い単色にセット（上書きの見栄え安定化）
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            break;
        default:
            break;
    }
    return state;
}

// --- ライティング OFF→ON で ノーマル↔レインボー 切替 --- //
void matrix_scan_user(void) {
    bool enabled = rgb_matrix_is_enabled();
    if (enabled && !g_last_enabled) {
        // OFF→ON遷移でトグル
        g_rainbow_mode = !g_rainbow_mode;
    }
    g_last_enabled = enabled;
}

// --- キー入力処理：Hue微調整など（必要に応じて任意のキーで調整） --- //
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) return true;

    switch (keycode) {
        case KC_F13: // 例：Hueアップ（お好みで割り当て）
            g_user_hue += 8;
            return false;
        case KC_F14: // 例：Hueダウン
            g_user_hue -= 8;
            return false;
        default:
            break;
    }
    return true;
}

// --- ポインティングデバイスからカーソル位置/スクロールバンドを更新 --- //
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // 右手の動作が左マスターに送られてくる（SPLIT_POINTING_ENABLE 前提）:contentReference[oaicite:5]{index=5}
    uint16_t now = timer_read();
    // マウス移動
    if (mouse_report.x != 0 || mouse_report.y != 0) {
        g_cursor.x += mouse_report.x * 3.0f;
        g_cursor.y -= mouse_report.y * 2.5f;
        if (g_cursor.x < 0) g_cursor.x += 224.0f;
        if (g_cursor.x >= 224.0f) g_cursor.x -= 224.0f;
        if (g_cursor.y < 0) g_cursor.y = 0;
        if (g_cursor.y > 64.0f) g_cursor.y = 64.0f;
        g_anim_timer = now;
    }
    // スクロール
    if (mouse_report.v != 0) {
        // 12分割の横帯を上下スクロールで移動
        g_scroll_band += (mouse_report.v > 0) ? 1 : -1;
        if (g_scroll_band < 0) g_scroll_band = 11;
        if (g_scroll_band > 11) g_scroll_band = 0;
        g_last_scroll_ms = now;
    }
    return mouse_report;
}

// --- カスタム描画（各フレームで呼ばれ、純正効果の後に上書き） --- //
void rgb_matrix_indicators_user(void) {
    uint8_t top = get_highest_layer(layer_state);

    // Layer0は純正（SOLID_REACTIVE_MULTIWIDE）を尊重：上書きしない
    if (top == L_BASE) return;

    switch (top) {
        case L_ICE:
            render_icewave();
            break;
        case L_STAT:
            render_static();
            break;
        case L_MOUSE:
            render_mousemove();
            break;
        case L_SCROLL:
            render_scrollmove();
            break;
        default:
            break;
    }
}
