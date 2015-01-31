CONFIGURE_FILE(com.flikr.api.xml ${CMAKE_BINARY_DIR}/base/social/share/social/com.flikr.api.xml)
CONFIGURE_FILE(com.flickr.json.api.xml ${CMAKE_BINARY_DIR}/base/social/share/social/com.flickr.json.api.xml)
CONFIGURE_FILE(com.dropbox.api.v2.xml ${CMAKE_BINARY_DIR}/base/social/share/social/com.dropbox.api.v2.xml)

#todo: install this file to prefix.
IF (NOT APPLE)
INSTALL(FILES com.flikr.api.xml DESTINATION share/social/)
INSTALL(FILES com.flickr.json.api.xml DESTINATION share/social/)
INSTALL(FILES com.dropbox.api.v2.xml DESTINATION share/social/)
ENDIF (NOT APPLE)

IF (APPLE)
    INSTALL(FILES com.flikr.api.xml  DESTINATION plexydesk.app/Contents/share/social/)
    INSTALL(FILES com.flickr.json.api.xml DESTINATION plexydesk.app/Contents/share/social/)
    INSTALL(FILES com.dropbox.api.v2.xml DESTINATION plexydesk.app/Contents/share/social/)
ENDIF(APPLE)
