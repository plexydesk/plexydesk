add_subdirectory(icons)

if (QNX)
add_subdirectory(themepacks/bb10)
endif (QNX)

if (NOT QNX)
#IF (NOT APPLE)
add_subdirectory(themepacks/default)
#ENDIF (NOT APPLE)

#IF (APPLE)
#ADD_SUBDIRECTORY(themepacks/mac)
#ENDIF (APPLE)
endif (NOT QNX)

#IF (NOT APPLE)
#ADD_SUBDIRECTORY(widgets/base-widget)
#ENDIF (NOT APPLE)
