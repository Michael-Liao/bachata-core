### common operations ###
find_package(OpenCV REQUIRED)
# get the location of node-addon-api
execute_process(COMMAND node -p "require('node-addon-api').include"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE NODE_ADDON_API_DIR)
# remove the excess strings like line change or quotes
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

###
# execution to test functionality
###
add_executable(chroma
    "main.cpp"
    "chromaremove.cpp")

target_link_libraries(chroma
    ${OpenCV_LIBS})


add_library(bachata SHARED
    "bachata.cpp"
    "chromaremove.cpp"
    ${CMAKE_JS_SRC})

# normally a shared library will be compiled into a name with prefix `lib` and suffix `.so`
# i.e. `libsilhouette.so`
# we want ours to be silhouette.node
set_target_properties(bachata PROPERTIES
    PREFIX ""
    SUFFIX ".node")

if(DEFINED ${CMAKE_JS_INC})
# this block will be executed by cmake-js because -DCMAKE_JS_INC is declared
# for more information run `cmake-js print-configure`
target_include_directories(bachata BEFORE
    PRIVATE ${CMAKE_JS_INC}
    PRIVATE ${NODE_ADDON_API_DIR})

else()

# this is just to help vscode configure intellisense better
# this is currently defaulted to use the electron runtime
target_include_directories(bachata BEFORE
    PRIVATE "/home/michael/.cmake-js/electron-x64/v9.1.1/include/node"
    PRIVATE ${NODE_ADDON_API_DIR})

endif()

target_link_libraries(bachata
    ${OpenCV_LIBS}
    ${CMAKE_JS_LIB})

# use c++11 for compiling and using the library
target_compile_features(bachata
    PUBLIC cxx_std_11)

# use napi of v3 (maximum compatibility)
target_compile_definitions(bachata
    PUBLIC NAPI_VERSION=3)