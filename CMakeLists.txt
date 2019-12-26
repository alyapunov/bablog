CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(banlog)

SET(DEFAULT_BUILD_TYPE "Debug")
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
ENDIF()
SET(EXPECTED_BUILD_TYPES Debug Release RelWithDebInfo MinSizeRel)
IF("${CMAKE_BUILD_TYPE}" IN_LIST EXPECTED_BUILD_TYPES)
    MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
ELSE()
    MESSAGE(WARNING "Unexcpected build type: ${CMAKE_BUILD_TYPE}")
ENDIF()

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 17)
SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")
SET(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -Werror")

INCLUDE_DIRECTORIES(.)

SET(SOURCE_FILES main.cpp FileReader.hpp IndexedBitset.hpp)

ADD_EXECUTABLE(banlog ${SOURCE_FILES})

ADD_EXECUTABLE(IndexedBitsetUnitTest IndexedBitsetUnitTest.cpp IndexedBitset.hpp)
ADD_EXECUTABLE(FileReaderUnitTest FileReaderUnitTest.cpp FileReader.hpp)
ADD_EXECUTABLE(StringFinderUnitTest StringFinderUnitTest.cpp StringFinder.hpp)
ADD_EXECUTABLE(CompactCharSetUnitTest CompactCharSetUnitTest.cpp CompactCharSet.hpp CompactCharSetTestUtils.hpp)
ADD_EXECUTABLE(CompactCharSetPerfTest CompactCharSetPerfTest.cpp CompactCharSet.hpp CompactCharSetTestUtils.hpp)

ENABLE_TESTING()
ADD_TEST(NAME IndexedBitsetUnitTest COMMAND IndexedBitsetUnitTest)
ADD_TEST(NAME FileReaderUnitTest COMMAND FileReaderUnitTest)
ADD_TEST(NAME StringFinderUnitTest COMMAND StringFinderUnitTest)
ADD_TEST(NAME CompactCharSetUnitTest COMMAND CompactCharSetUnitTest)
