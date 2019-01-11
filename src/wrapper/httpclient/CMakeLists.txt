pkg_search_module(pkg-curl REQUIRED libcurl)
message(STATUS "curl include path: ${pkg-curl_INCLUDE_DIRS}")
message(STATUS "curl library path: ${pkg-curl_STATIC_LDFLAGS}")

file(GLOB HTTPCLIENT_SOURCES "*.cpp")
add_library(httpclient STATIC ${HTTPCLIENT_SOURCES})
target_include_directories(httpclient PRIVATE "${pkg-curl_INCLUDE_DIRS}")
target_link_libraries(httpclient ${pkg-curl_STATIC_LDFLAGS})
target_link_libraries(httpclient common)

if(DEFINED CFG_WITH_TEST)
	add_executable(test-httpget test/HttpGet.cpp)
	target_link_libraries(test-httpget httpclient)

	add_executable(test-httppost test/HttpPost.cpp)
	target_link_libraries(test-httppost httpclient)
endif()
