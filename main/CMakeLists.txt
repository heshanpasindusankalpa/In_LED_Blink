idf_component_register(SRCS "helloworld.c"
                            "../packages/boards/my_board/src/gpio_config.c"
                            "../packages/boards/my_board/src/btn_config.c"
                            "../packages/boards/my_board/src/led_config.c"
                            "../packages/boards/my_board/src/pin_config.c"
                            "../packages/pheripheral/gpio/src/Gpio.c"
                            "../packages/devices/led/src/led.c"
                            "../packages/boards/esp/src/gpio_port.c"
                            "../packages/pheripheral/gpio/src/Gpio.c"
                            "../packages/devices/button/src/btn.c"
                            "../packages/services/src/platform.c"
                            

                            
                    INCLUDE_DIRS "."
                                "../packages/boards/my_board/inc"
                                "../packages/pheripheral/gpio/inc"
                                "../packages/devices/led/inc"
                                "../packages/boards/esp/inc"
                                "../packages/boards/my_board/inc"
                                "../packages/pheripheral/gpio/inc"
                                "../packages/devices/button/inc"
                                "../packages/services/inc"
                                

                    REQUIRES
                        )
