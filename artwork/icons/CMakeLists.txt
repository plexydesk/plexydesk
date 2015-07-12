#ADD_SUBDIRECTORY(oxygen)

#FILE(GLOB PNGFILES *.png)

#INSTALL(FILES ${PNGFILES} DESTINATION share/plexy/skins/default)

if (NOT APPLE)
INSTALL(FILES "plexydesk.png" DESTINATION share/plexy)
endif (NOT APPLE)
