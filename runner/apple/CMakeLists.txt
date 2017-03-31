if (ENABLE_QT5)
set(OSX_ICON_FILES ${CMAKE_SOURCE_DIR}/artwork/icons/plexydesk.icns)
endif()

if (ENABLE_QT4)
set(OSX_ICON_FILES ${CMAKE_SOURCE_DIR}/plexydesk/artwork/icons/plexydesk.icns)
endif()

set(MACOSX_BUNDLE_ICON_FILE "plexydesk.icns")
set(QTMOC_SRCS ../desktopmanager.h)

if (ENABLE_QT5)
QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})
endif()

if (ENABLE_QT4)
QT4_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})
endif()

set(sourceFiles ../desktopmanager.cpp main.mm)

#Locate cococa framework on mac
INCLUDE_DIRECTORIES ( /System/Library/Frameworks )

if (APPLE AND ENABLE_QT4)
  FIND_LIBRARY(CARBON_LIBRARY Carbon)
endif()

FIND_LIBRARY(COCOA_LIBRARY Cocoa)
FIND_LIBRARY(GLUT_LIBRARY GLUT)
FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
MARK_AS_ADVANCED (CARBON_LIBRARY COCOA_LIBRARY GLUT_LIBRARY OpenGL_LIBRARY)
set(MAC_LIBS  ${CARBON_LIBRARY} ${COCOA_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY})

set(APP_TYPE MACOSX_BUNDLE)
set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})

ADD_EXECUTABLE(plexydesk ${APP_TYPE} ${sourceFiles} ${resourceFiles} ${QT_MOC_SRCS})

if (ENABLE_QT5)
SET_TARGET_PROPERTIES(plexydesk PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmakemodules/plexydesk-info.plist)
endif()

if (ENABLE_QT4)
SET_TARGET_PROPERTIES(plexydesk PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/plexydesk/cmakemodules/plexydesk-info.plist)
endif()

#rpath specific properties

TARGET_LINK_LIBRARIES(plexydesk LINK_PUBLIC jsoncpp_ck ${PLEXY_UI_CORE_LIBRARY} ${MAC_LIBS})
#QT5_USE_MODULES(plexydesk Widgets Xml Core)

install(FILES ${OSX_ICON_FILES} DESTINATION plexydesk.app/Contents/Resources)
install(TARGETS plexydesk BUNDLE DESTINATION . COMPONENT Runtime RUNTIME DESTINATION bin COMPONENT Runtime)
