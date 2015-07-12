FILE(GLOB PNGFILES *.png )

if (NOT APPLE)
INSTALL(FILES ${PNGFILES}  DESTINATION share/plexy/skins/default/clock)
endif (NOT APPLE)
