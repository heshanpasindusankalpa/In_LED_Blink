idf_component_register(SRCS "helloworld.c"
                            "../packages/boards/my_board/src/gpio_config.c"
                            "../packages/boards/my_board/src/btn_config.c"
                            "../packages/boards/my_board/src/led_config.c"
                            "../packages/boards/my_board/src/pin_config.c"
                            "../packages/boards/my_board/src/i2c_config.c"
                            "../packages/boards/my_board/src/bq27427_config.c"
                            "../packages/devices/bq27427/src/bq27427.c"
                            "../packages/pheripheral/gpio/src/Gpio.c"
                            "../packages/devices/led/src/led.c"
                            "../packages/devices/lmp91000/src/lmp91000.c"
                            "../packages/boards/esp/src/gpio_port.c"
                            "../packages/boards/esp/src/i2c_port.c"
                            "../packages/devices/button/src/btn.c"
                            "../packages/services/src/platform.c"
                            "../packages/services/src/svc_button.c"
                            "../packages/services/src/svc_led.c"
                            "../packages/pheripheral/i2c/src/I2c.c"
                            

                            
                    INCLUDE_DIRS "."
                                "../packages/boards/my_board/inc"
                                "../packages/devices/led/inc"
                                "../packages/devices/bq27427/inc"
                                "../packages/boards/my_board/include"
                                "../packages/boards/esp/inc"
                                "../packages/pheripheral/gpio/inc"
                                "../packages/devices/button/inc"
                                "../packages/devices/lmp91000/inc"
                                "../packages/services/inc"
                                "../packages/pheripheral/i2c/inc"
                                
                                

                    REQUIRES
                        )
