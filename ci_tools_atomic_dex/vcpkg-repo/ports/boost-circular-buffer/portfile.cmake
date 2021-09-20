# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/circular_buffer
    REF boost-1.75.0
    SHA512 08a51f1d6947802097245d2e97ab23be89447fbaa47ff9538f04ce9e1a3077b6bf0b7ec8be5e21d02eb24c6721f330ed485f57d2fd2e4759021eef508690d5f2
    HEAD_REF master
)

include(${CURRENT_INSTALLED_DIR}/share/boost-vcpkg-helpers/boost-modular-headers.cmake)
boost_modular_headers(SOURCE_PATH ${SOURCE_PATH})