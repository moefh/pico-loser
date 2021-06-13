add_executable(pico_loser
  main.c
  game_data.c
  screen.c
  control.c
  character.c
  collision.c
  joy_wii_i2c.c
  lib/vga_6bit.c
  lib/vga_font.c
  lib/vga_image.c
  lib/wii_i2c.c
)

pico_generate_pio_header(pico_loser ${CMAKE_CURRENT_LIST_DIR}/lib/vga_6bit.pio)

pico_enable_stdio_usb(pico_loser 1)

target_link_libraries(pico_loser
  pico_stdlib
  pico_multicore
  hardware_pio
  hardware_dma
  hardware_i2c
)

pico_add_extra_outputs(pico_loser)
