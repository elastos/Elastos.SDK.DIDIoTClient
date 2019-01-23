cmake_minimum_required(VERSION 3.12)

include(config/cmake/CMakeLists.txt)

project(Elastos.SDK.DIDIoTClient)
add_subdirectory(src)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Elastos.SDK.DIDIoTClient.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/Elastos.SDK.DIDIoTClient.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Elastos.SDK.DIDIoTClient.pc DESTINATION lib/pkgconfig)
