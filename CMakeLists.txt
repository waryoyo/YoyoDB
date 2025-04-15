# CMakeList.txt : CMake project for YoyoDB, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

project ("YoyoDB")

# Add source to this project's executable.
include_directories(YoyoDB PUBLIC include)
add_executable (YoyoDB "YoyoDB.cpp" "YoyoDB.h" "src/database.cpp" "src/storage.cpp" "src/cacheManager.cpp" "src/storageIndex.cpp" "src/query/condition.cpp" "src/query/queryParser.cpp" "src/query/queryExecutor.cpp" "src/fieldIndex/uniqueFieldIndex.cpp" "src/fieldIndex/baseFieldIndex.cpp")
target_link_libraries(YoyoDB nlohmann_json::nlohmann_json)



if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET YoyoDB PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
