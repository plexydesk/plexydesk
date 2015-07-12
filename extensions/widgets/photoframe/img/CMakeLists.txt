FILE(GLOB PNGFILES *.svg *.png )

if (NOT APPLE)
INSTALL(FILES ${PNGFILES}  DESTINATION share/plexy/skins/default/flick)
endif (NOT APPLE)
