if (LINUX)
    #message(STATUS "Linux")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/bundled/linux)
    install(SCRIPT ${CMAKE_SOURCE_DIR}/cmake/install/linux/linux_post_install.cmake)
endif()