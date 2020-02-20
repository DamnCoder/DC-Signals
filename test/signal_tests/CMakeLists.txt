set(SOURCES
    test_signals.cpp
    test_signals.h
    )

add_executable(test_signals ${SOURCES})
target_link_libraries(test_signals PUBLIC dcpp_signals CONAN_PKG::gtest CONAN_PKG::spdlog)

add_test(NAME TestSignals COMMAND $<TARGET_FILE:test_signals>)
set_target_properties(test_signals PROPERTIES FOLDER "TestSignals")
