file(GLOB_RECURSE SOURCES "${CppExecutor_SOURCE_DIR}/src/*.cpp")


add_library(CppExecutor ${SOURCES})
target_include_directories(CppExecutor PUBLIC "${CppExecutor_SOURCE_DIR}/src")
target_compile_features(CppExecutor PUBLIC cxx_std_17)