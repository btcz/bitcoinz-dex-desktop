# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/container_hash
    REF boost-1.75.0
    SHA512 4d1a8381e9b7554884597db4b1e365465845b1fd7e665ec5bc89c38f859fb4b62e024e77b2cd818f3bdf959476dedae0fc49fc43e28e4e9043e0386a366f9a43
    HEAD_REF master
)

include(${CURRENT_INSTALLED_DIR}/share/boost-vcpkg-helpers/boost-modular-headers.cmake)
boost_modular_headers(SOURCE_PATH ${SOURCE_PATH})