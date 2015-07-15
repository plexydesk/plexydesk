set(sourceFiles
	template.cpp
	templateinterface.cpp
	)

set(headerFiles
	template.h
	templateinterface.h
	)

set(QTMOC_SRCS
	template.h
	templateinterface.h
	)

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
	${sourceFiles}
	${headerFiles}
	)

set(libs
	${QT_QTCORE_LIBRARY}
	)

ADD_LIBRARY(templateengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
	SET_TARGET_PROPERTIES(templateengine PROPERTIES
		IMPORT_SUFFIX ".lib"
		IMPORT_PREFIX ""
		PREFIX ""
		)
endif(MINGW)

TARGET_LINK_LIBRARIES(templateengine
	${PLEXY_CORE_LIBRARY}
	${libs}
	)

QT5_USE_MODULES(templateengine Widgets Xml  Core   )

if (NOT APPLE)
	INSTALL(TARGETS templateengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
	INSTALL(FILES template.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
	INSTALL(TARGETS templateengine LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
	INSTALL(FILES template.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif(APPLE)
