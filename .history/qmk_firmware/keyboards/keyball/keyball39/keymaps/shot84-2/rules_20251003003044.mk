# --- size optimization (QMK公式の推奨) ---
LTO_ENABLE        = yes        # リンク時最適化で数KB削減（ケース多）  ※最重要
CONSOLE_ENABLE    = no         # 端末デバッグを切る
COMMAND_ENABLE    = no         # コマンド機能を切る
MAGIC_ENABLE      = no         # Magic機能を切る
MOUSEKEY_ENABLE   = no         # マウスキーは未使用（Pointing Deviceは別機構）
SPACE_CADET_ENABLE= no         # 使っていなければ切る
GRAVE_ESC_ENABLE  = no         # 使っていなければ切る
AVR_USE_MINIMAL_PRINTF = yes   # printfを使う場合の軽量版（~数百B）  ※使っていなければ無害

# --- 必要機能 ---
RGB_MATRIX_ENABLE     = yes
POINTING_DEVICE_ENABLE= yes
SPLIT_KEYBOARD        = yes
OLED_ENABLE           = yes         # ご要望どおり有効のまま
# VIA_ENABLE          = no          # ※未使用ならno推奨（1～2KB級の節約）

# 他で使っていなければ
# BOOTMAGIC_ENABLE   = no
# NKRO_ENABLE        = no
# MIDI_ENABLE        = no
# AUDIO_ENABLE       = no
