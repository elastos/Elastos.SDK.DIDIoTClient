include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/thirdparty
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CFG_BUILD_ROOT}/include)

add_subdirectory(common)
add_subdirectory(wrapper/httpclient)

pkg_search_module(pkg-openssl REQUIRED openssl)
message(STATUS "openssl include path: ${pkg-openssl_INCLUDE_DIRS}")
message(STATUS "openssl library path: ${pkg-openssl_STATIC_LDFLAGS}")

pkg_search_module(pkg-Elastos.ORG.Wallet.Lib.C REQUIRED Elastos.ORG.Wallet.Lib.C)
message(STATUS "Elastos.ORG.Wallet.Lib.C include path: ${pkg-Elastos.ORG.Wallet.Lib.C_INCLUDE_DIRS}")
message(STATUS "Elastos.ORG.Wallet.Lib.C library path: ${pkg-Elastos.ORG.Wallet.Lib.C_STATIC_LDFLAGS}")

file( GLOB Elastos.SDK.DIDIoTClient-SOURCES "*.cpp" )
file( GLOB Elastos.SDK.DIDIoTClient-HEADERS "*.hpp" )
add_library(Elastos.SDK.DIDIoTClient ${Elastos.SDK.DIDIoTClient-SOURCES})
target_include_directories(Elastos.SDK.DIDIoTClient PRIVATE "${pkg-Elastos.ORG.Wallet.Lib.C_INCLUDE_DIRS}")
target_include_directories(Elastos.SDK.DIDIoTClient PRIVATE "${pkg-openssl_INCLUDE_DIRS}")
target_link_libraries(Elastos.SDK.DIDIoTClient ${pkg-Elastos.ORG.Wallet.Lib.C_STATIC_LDFLAGS})
target_link_libraries(Elastos.SDK.DIDIoTClient ${pkg-openssl_STATIC_LDFLAGS})
target_link_libraries(Elastos.SDK.DIDIoTClient common httpclient)
set_target_properties(Elastos.SDK.DIDIoTClient PROPERTIES PUBLIC_HEADER "${Elastos.SDK.DIDIoTClient-HEADERS}")
install(TARGETS Elastos.SDK.DIDIoTClient
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		PUBLIC_HEADER DESTINATION include/Elastos.SDK.DIDIoTClient)

if(DEFINED CFG_WITH_TEST)
	add_executable(DidApiTest test/DidApiTest.cpp)
	target_link_libraries(DidApiTest Elastos.SDK.DIDIoTClient)
endif()
