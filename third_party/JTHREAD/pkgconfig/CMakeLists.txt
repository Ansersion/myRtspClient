configure_file(${PROJECT_SOURCE_DIR}/pkgconfig/jthread.pc.in ${PROJECT_BINARY_DIR}/pkgconfig/jthread.pc)
install(FILES ${PROJECT_BINARY_DIR}/pkgconfig/jthread.pc DESTINATION ${LIBRARY_INSTALL_DIR}/pkgconfig)

