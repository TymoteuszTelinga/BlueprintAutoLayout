# CMakeList.txt: projekt CMake dla elementu BlueprintAutoLayout, dołącz źródło i zdefiniuj
# tutaj logikę specyficzną dla projektu.
#
cmake_minimum_required (VERSION 3.15)

# Włącz przeładowywanie kodu XAML na gorąco dla kompilatorów MSVC, jeśli jest to obsługiwane.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("BlueprintAutoLayout")

include(FetchContent)

FetchContent_Declare(
    nfd
    GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(nfd)


find_package(raylib REQUIRED)
find_package(yaml-cpp REQUIRED)

# Dodaj źródło do pliku wykonywalnego tego projektu.
add_executable (BlueprintAutoLayout "src/main.cpp"  "src/BlueprintLayout.h" "src/BlueprintLayout.cpp" "src/BlueprintGUI.h" "src/BlueprintGUI.cpp")

target_compile_definitions(BlueprintAutoLayout PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    NOGDI
    NOUSER
)

target_link_libraries(BlueprintAutoLayout raylib)
target_link_libraries(BlueprintAutoLayout yaml-cpp::yaml-cpp)
target_link_libraries(BlueprintAutoLayout nfd)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BlueprintAutoLayout PROPERTY CXX_STANDARD 20)
endif()

# TODO: Dodaj testy i zainstaluj elementy docelowe w razie potrzeby.
