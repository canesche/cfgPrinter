add_library(CfgPrinter MODULE
    cfgPrintPass.cpp
    cfgPrintPlugin.cpp
)

target_include_directories(CfgPrinter PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)
