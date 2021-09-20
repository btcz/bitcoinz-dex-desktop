include(vcpkg_common_functions)
set(BZIP2_VERSION 1.0.6)
vcpkg_download_distfile(ARCHIVE
    URLS "https://github.com/KomodoPlatform/bzip2-mirror/archive/v1.0.6.tar.gz"
    FILENAME "v1.0.6.tar.gz"
    SHA512 c3d904f65f90aa34dbdd9e27f0ffa40e47efce2f38e3524905b70b281d0ab4e1480f8d18f1bfde859e7f64534246b89d09b634420ea7f22612597ec8caa86ed2)
    
vcpkg_extract_source_archive_ex(
    OUT_SOURCE_PATH SOURCE_PATH
    ARCHIVE ${ARCHIVE}
    REF ${BZIP2_VERSION}
    PATCHES
        fix-import-export-macros.patch
        fix-windows-include.patch
)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt DESTINATION ${SOURCE_PATH})

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS_DEBUG
        -DBZIP2_SKIP_HEADERS=ON
        -DBZIP2_SKIP_TOOLS=ON
)

vcpkg_install_cmake()
vcpkg_copy_pdbs()

file(READ ${CURRENT_PACKAGES_DIR}/include/bzlib.h BZLIB_H)
if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    string(REPLACE "defined(BZ_IMPORT)" "0" BZLIB_H "${BZLIB_H}")
else()
    string(REPLACE "defined(BZ_IMPORT)" "1" BZLIB_H "${BZLIB_H}")
endif()
file(WRITE ${CURRENT_PACKAGES_DIR}/include/bzlib.h "${BZLIB_H}")

file(COPY ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/bzip2)
file(RENAME ${CURRENT_PACKAGES_DIR}/share/bzip2/LICENSE ${CURRENT_PACKAGES_DIR}/share/bzip2/copyright)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/usage DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})

vcpkg_test_cmake(PACKAGE_NAME BZip2 MODULE)
