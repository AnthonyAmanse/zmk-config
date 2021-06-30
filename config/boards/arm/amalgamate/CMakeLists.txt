set_property(GLOBAL APPEND PROPERTY extra_post_build_commands
  COMMAND ${PYTHON_EXECUTABLE} ${ZEPHYR_BASE}/../tools/uf2/utils/uf2conv.py
  -c 
  -b 0x26000
  -f 0xADA52840
  -o ${PROJECT_BINARY_DIR}/${CONFIG_KERNEL_BIN_NAME}.uf2
  ${PROJECT_BINARY_DIR}/${CONFIG_KERNEL_BIN_NAME}.bin
)

target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE custom_status_screen.c)
target_sources_ifdef(CONFIG_ZMK_DISPLAY app PRIVATE custom_battery_status.c)
target_sources_ifdef(CONFIG_ZMK_DISPLAY app PRIVATE custom_charge_status.c)
target_include_directories(app PRIVATE include)