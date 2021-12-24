set(UNIT_TESTS_CATCH2_SRC
    ARM64DisasmTest.cpp
    AtomicTest.cpp
    HexTest.cpp
    LEB128Test.cpp
    RingBufferTest.cpp
)

add_executable(jevmachopp-unit-tests-catch2 ${UNIT_TESTS_CATCH2_SRC})

target_link_libraries(jevmachopp-unit-tests-catch2 jevmachopp nanorange fmt Catch2 Catch2WithMain)
