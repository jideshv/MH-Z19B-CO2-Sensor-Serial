set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/MH-Z19B-CO2-Sensor-Serial.h)

add_library(MHZ19B INTERFACE)

target_sources(MHZ19B INTERFACE ${header_files})

target_include_directories(MHZ19B INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
