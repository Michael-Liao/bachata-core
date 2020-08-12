# SET(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)
# get the location of node-addon-api
execute_process(COMMAND node -p "require('node-addon-api').include"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE NODE_ADDON_API_DIR)
# remove the excess strings like line change or quotes
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

###
# Node.js Addon
###
add_library(silhouette SHARED
    "entry.cpp"
    "chromaremove.cpp"
    ${CMAKE_JS_SRC})

# normally a shared library will be compiled into a name with prefix `lib` and suffix `.so`
# i.e. `libsilhouette.so`
# we want ours to be silhouette.node
set_target_properties(silhouette PROPERTIES
    PREFIX ""
    SUFFIX ".node")


target_include_directories(silhouette BEFORE
    PRIVATE ${CMAKE_JS_INC}
    PRIVATE ${NODE_ADDON_API_DIR})

target_link_libraries(silhouette
    ${OpenCV_LIBS}
    ${CMAKE_JS_LIB})

# use c++11 for compiling and using the library
target_compile_features(silhouette
    PUBLIC cxx_std_11)

# use napi of v3 (maximum compatibility)
target_compile_definitions(silhouette
    PUBLIC NAPI_VERSION=3)

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
    ${CMAKE_JS_SRC})

# normally a shared library will be compiled into a name with prefix `lib` and suffix `.so`
# i.e. `libsilhouette.so`
# we want ours to be silhouette.node
set_target_properties(bachata PROPERTIES
    PREFIX ""
    SUFFIX ".node")

target_include_directories(bachata BEFORE
    PRIVATE ${CMAKE_JS_INC}
    PRIVATE ${NODE_ADDON_API_DIR})

target_link_libraries(bachata
    ${OpenCV_LIBS}
    ${CMAKE_JS_LIB})

# use c++11 for compiling and using the library
target_compile_features(bachata
    PUBLIC cxx_std_11)

# use napi of v3 (maximum compatibility)
target_compile_definitions(bachata
    PUBLIC NAPI_VERSION=3
    PUBLIC NAPI_EXPERIMENTAL)