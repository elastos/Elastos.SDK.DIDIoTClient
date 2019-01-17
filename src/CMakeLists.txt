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

file( GLOB Elastos.SDK.DID.IOT-SOURCES "*.cpp" )
file( GLOB Elastos.SDK.DID.IOT-HEADERS "*.h" )
add_library(Elastos.SDK.DID.IOT ${Elastos.SDK.DID.IOT-SOURCES})
target_include_directories(Elastos.SDK.DID.IOT PRIVATE "${pkg-Elastos.ORG.Wallet.Lib.C_INCLUDE_DIRS}")
target_include_directories(Elastos.SDK.DID.IOT PRIVATE "${pkg-openssl_INCLUDE_DIRS}")
target_link_libraries(Elastos.SDK.DID.IOT ${pkg-Elastos.ORG.Wallet.Lib.C_STATIC_LDFLAGS})
target_link_libraries(Elastos.SDK.DID.IOT ${pkg-openssl_STATIC_LDFLAGS})
target_link_libraries(Elastos.SDK.DID.IOT common httpclient)
set_target_properties(Elastos.SDK.DID.IOT PROPERTIES PUBLIC_HEADER "${Elastos.SDK.DID.IOT-HEADERS}")
install(TARGETS Elastos.SDK.DID.IOT
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		PUBLIC_HEADER DESTINATION include/Elastos.SDK.DID.IOT)

if(DEFINED CFG_WITH_TEST)
	add_executable(DidApiTest test/DidApiTest.cpp)
	target_link_libraries(DidApiTest Elastos.SDK.DID.IOT)
endif()
