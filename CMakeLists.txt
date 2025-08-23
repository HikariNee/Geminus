CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
INCLUDE(FetchContent)

SET(CMAKE_CXX_STANDARD 23)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GNUTLS REQUIRED IMPORTED_TARGET gnutls)

PROJECT(Geminus VERSION 1.0)

SET(SOURCES
  src/socket.cpp
  src/tls/gtls_raii.cpp
  src/main.cpp
  src/utilities.cpp
  src/connection.cpp
)

SET(HEADERS
  src/socket.hpp
  src/tls/tls.hpp
  src/tls/gtls_raii.hpp
  src/tls/tls_errors.hpp
  src/utilities.hpp
  src/connection.hpp
)

ADD_EXECUTABLE(Geminus ${HEADERS} ${SOURCES})

TARGET_COMPILE_OPTIONS(Geminus PUBLIC -Wall -Wextra -Werror)
TARGET_LINK_LIBRARIES(Geminus PRIVATE ${GNUTLS_LINK_LIBRARIES})
TARGET_COMPILE_OPTIONS(Geminus PUBLIC ${GNUTLS_CFLAGS})
