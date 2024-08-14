cmake_minimum_required(VERSION 3.5)

set(PICO_BOARD pico_w)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(path_navigation_wifi_distance)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
if (TARGET tinyusb_device)
	add_executable(path_navigation_wifi_distance
        main_entry_wifi.c server_wifi.c make_move.c
        2enginesmove.c move_encoder.c
        move_commands.c string_list.c
        moving-sensor_ioe_sr05.c
        path_navigation.c move_list.c
        )
	target_compile_definitions(path_navigation_wifi_distance PRIVATE
        WIFI_SSID=\"$ENV{WIFI_SSID}\"
        WIFI_PASSWORD=\"$ENV{WIFI_PASSWORD}\"
	)
	target_include_directories(path_navigation_wifi_distance PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
	pico_generate_pio_header(path_navigation_wifi_distance ${CMAKE_CURRENT_LIST_DIR}/uart_rx.pio)
	# Pull in our pico_stdlib which pulls in commonly used features
	target_link_libraries(path_navigation_wifi_distance 
		pico_stdlib pico_printf
		hardware_pwm  pico_mem_ops 
		pico_cyw43_arch_lwip_threadsafe_background
		pico_multicore pico_sync hardware_pio
	)

	# enable usb output, disable uart output
    pico_enable_stdio_usb(path_navigation_wifi_distance 1)
    pico_enable_stdio_uart(path_navigation_wifi_distance 0)

	# create map/bin/hex file etc.
	pico_add_extra_outputs(path_navigation_wifi_distance)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building interract_usb because TinyUSB submodule is not initialized in the SDK")
endif()
	
