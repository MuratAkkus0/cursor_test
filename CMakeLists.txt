cmake_minimum_required(VERSION 3.10)

project(CryptoBreaker VERSION 1.0.0 LANGUAGES CXX)

# C++ standardı ayarla
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compile flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/Utils.cpp
    src/FrequencyAnalyzer.cpp
    src/CipherBreaker.cpp
    src/CaesarBreaker.cpp
    src/SubstitutionBreaker.cpp
    src/VigenereBreaker.cpp
    src/LanguageDetector.cpp
)

# Header files
set(HEADERS
    include/Utils.h
    include/FrequencyAnalyzer.h
    include/CipherBreaker.h
    include/LanguageDetector.h
)

# Executable
add_executable(cryptobreaker ${SOURCES})

# Threading support
find_package(Threads REQUIRED)
target_link_libraries(cryptobreaker Threads::Threads)

# Install target
install(TARGETS cryptobreaker DESTINATION bin)

# Testing
enable_testing()

# Unit tests executable
set(TEST_SOURCES
    tests/unit_tests.cpp
    src/Utils.cpp
    src/FrequencyAnalyzer.cpp
    src/CipherBreaker.cpp
    src/CaesarBreaker.cpp
    src/SubstitutionBreaker.cpp
    src/VigenereBreaker.cpp
    src/LanguageDetector.cpp
)

add_executable(run_tests ${TEST_SOURCES})
target_link_libraries(run_tests Threads::Threads)

# Test discovery
add_test(NAME CryptoBreaker_Tests COMMAND run_tests)

# Custom targets
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
)

# Print configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")