if(NOT TARGET_TRIPLET STREQUAL _HOST_TRIPLET)
    message(FATAL_ERROR "vcpkg-cmake-config is a host-only port; please mark it as a host port in your dependencies.")
endif()

file(INSTALL
    "${CMAKE_CURRENT_LIST_DIR}/vcpkg_cmake_config_fixup.cmake"
    "${CMAKE_CURRENT_LIST_DIR}/vcpkg-port-config.cmake"
    "${CMAKE_CURRENT_LIST_DIR}/copyright"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

set(VCPKG_POLICY_EMPTY_PACKAGE enabled)
