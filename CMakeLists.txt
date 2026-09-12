cmake_minimum_required(VERSION 3.15)
project(VoltBill VERSION 1.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.c
    src/utils.c
    src/ui.c
    src/banner.c
    src/tariff.c
    src/customer.c
    src/billing.c
    src/payment.c
    src/analytics.c
    src/storage.c
)

# Executable target
add_executable(voltbill ${SOURCES})

# Compiler warnings and options
if(MSVC)
    target_compile_definitions(voltbill PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(voltbill PRIVATE /W3 /O2)
else()
    target_compile_options(voltbill PRIVATE -Wall -Wextra -O2)
    target_link_libraries(voltbill PRIVATE m)
endif()

# Installation target
install(TARGETS voltbill DESTINATION bin)
install(DIRECTORY config/ DESTINATION etc/voltbill OPTIONAL)

# CPack configuration for cross-platform binary releases
set(CPACK_PACKAGE_NAME "voltbill")
set(CPACK_PACKAGE_VENDOR "Akshar Miyani")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VoltBill: Console Electricity Billing & Utility Management System in C")
set(CPACK_PACKAGE_VERSION "1.1.0")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/miyaniakshar1234/VoltBill")
set(CPACK_PACKAGE_CONTACT "https://github.com/miyaniakshar1234/VoltBill")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ")
else()
    set(CPACK_GENERATOR "DEB;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Akshar Miyani <https://github.com/miyaniakshar1234/VoltBill>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.27)")
endif()

include(CPack)
