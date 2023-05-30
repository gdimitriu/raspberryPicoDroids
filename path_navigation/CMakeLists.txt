cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(path_navigation)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
if (TARGET tinyusb_device)
	add_executable(path_navigation
        encoder_ble.c 2enginesmove.c string_list.c
        )

	# Pull in our pico_stdlib which pulls in commonly used features
	target_link_libraries(path_navigation pico_stdlib  hardware_pwm pico_printf pico_mem_ops)

	# enable usb output, disable uart output
    pico_enable_stdio_usb(path_navigation 1)
    pico_enable_stdio_uart(path_navigation 0)

	# create map/bin/hex file etc.
	pico_add_extra_outputs(path_navigation)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building interract_usb because TinyUSB submodule is not initialized in the SDK")
endif()
	
