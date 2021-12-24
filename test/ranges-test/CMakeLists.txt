set(RANGES_TEST_SRC
    split-test.cpp
)

add_executable(split-test ${RANGES_TEST_SRC})

target_link_libraries(split-test jevmachopp nanorange fmt)