project(ImageTest)

SET(COMPILE_DEFINITIONS -Werror)

find_package( OpenCV REQUIRED )

include_directories(/opt/vc/include)

include_directories(/opt/vc/include/interface/vmcs_host)

include_directories(/opt/vc/include/interface/vmcs_host/linux)

include_directories(/usr/local/include)

include_directories(USERLAND_DIR)

link_directories(/opt/vc/lib)

link_directories(/opt/vc/src/hello_pi/libs/vgfont)

link_directories(/usr/local/lib)

add_executable(main src/main.cpp)

target_link_libraries(main mmal_core raspicam_cv raspicam mmal_util mmal_vc_client bcm_host ${OpenCV_LIBS})
