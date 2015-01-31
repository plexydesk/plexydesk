FILE (GLOB HTML_FILES *.html)
FILE (GLOB CSS_FILES *.css)

IF (NOT APPLE)
    INSTALL(FILES ${HTML_FILES} DESTINATION share/plexy/facebook/ui/data/)
    INSTALL(FILES ${CSS_FILES} DESTINATION share/plexy/facebook/ui/data/)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(FILES ${HTML_FILES} DESTINATION plexydesk.app/Contents/share/plexy/facebook/ui/data/)
   INSTALL(FILES ${CSS_FILES} DESTINATION plexydesk.app/Contents/share/plexy/facebook/ui/data/)
ENDIF(APPLE)
