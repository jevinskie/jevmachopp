set(UNIT_TESTS_SRC
    ARM64DisasmTest.cpp
    HexTest.cpp
)

add_executable(jevmachopp-unit-tests ${UNIT_TESTS_SRC})

target_link_libraries(jevmachopp-unit-tests jevmachopp nanorange fmt Catch2 Catch2WithMain)
