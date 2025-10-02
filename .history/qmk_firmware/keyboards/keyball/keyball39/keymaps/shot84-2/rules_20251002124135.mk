# --- size optimization ---
LTO_ENABLE = yes
CONSOLE_ENABLE = no
COMMAND_ENABLE = no
MAGIC_ENABLE = no
MOUSEKEY_ENABLE = no
AVR_USE_MINIMAL_PRINTF = yes

# 機能系（必要なら残す）
RGB_MATRIX_ENABLE = yes
POINTING_DEVICE_ENABLE = yes
SPLIT_KEYBOARD = yes
# VIA/Vialを使うなら残す（使っていないなら no にすると縮む）
# VIA_ENABLE = yes