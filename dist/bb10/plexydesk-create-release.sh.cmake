
cp plexydesk-bb10.xml ${CMAKE_INSTALL_PREFIX}
cd ${CMAKE_INSTALL_PREFIX}
blackberry-nativepackager -package ${CMAKE_CURRENT_BINARY_DIR}/plexydesk-release-${APPLICATION_MAIN_VERSION}.bar plexydesk-bb10.xml


