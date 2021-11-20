set(RANGES_TEST_SRC
    ranges-consumer.cpp
    ranges-producer.cpp
)

set(RANGES_TEST_HDR_STANDALONE
)

set(RANGES_TEST_HDR)
foreach(HDR ${RANGES_TEST_HDR_STANDALONE})
    set(HDR "${CMAKE_CURRENT_SOURCE_DIR}/${HDR}")
    list(APPEND RANGES_TEST_HDR ${HDR})
endforeach()


foreach(SRC ${RANGES_TEST_SRC})
    string(REGEX REPLACE "\.cpp$" ".h" HDR ${SRC})
    set(HDR "${CMAKE_CURRENT_SOURCE_DIR}/${HDR}")
    list(APPEND RANGES_TEST_HDR ${HDR})
endforeach()

add_executable(ranges-test ${RANGES_TEST_SRC} ${RANGES_TEST_HDR})

target_link_libraries(ranges-test nanorange fmt)