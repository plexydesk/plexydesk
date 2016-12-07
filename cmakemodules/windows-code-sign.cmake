macro(windows_code_sign_target arg ext)
if (NOT ENABLE_DEVELOPER_BUILD)
	file(TO_CMAKE_PATH ${CODE_SIGN_CERT} OUT_VALUE)
	add_custom_command(TARGET ${arg} POST_BUILD
		COMMAND ${SIGNTOOL} sign /p ${CODE_SIGN_CERT_AUTH} /f "${OUT_VALUE}" /td sha256 -fd sha256 /tr "http://timestamp.comodoca.com/authenticode" /d "PlexyDesk - Desktop NG Desktop Widgets" ${arg}.${ext}
                  )
endif()
endmacro()


macro(windows_code_sign_deps arg)
if (NOT ENABLE_DEVELOPER_BUILD)
  include(${CMAKE_BINARY_DIR}/CPackConfig.cmake)
  set(BUILD_ROOT "${CMAKE_BINARY_DIR}/_CPack_Packages/${CPACK_SYSTEM_NAME}/WIX/${CPACK_PACKAGE_FILE_NAME}/${arg}")
  file(TO_CMAKE_PATH ${CODE_SIGN_CERT} OUT_VALUE)
  file (TO_NATIVE_PATH ${BUILD_ROOT} TARGET_SIGN_FILE)
  STRING(REGEX REPLACE "\\\\" "\\\\\\\\" GOOD_PATH ${TARGET_SIGN_FILE})
  install(CODE "EXECUTE_PROCESS(COMMAND \"${SIGNTOOL}\" sign /f ${OUT_VALUE} /p ${CODE_SIGN_CERT_AUTH} /td sha256 /fd sha256 /tr \"http://timestamp.comodoca.com/authenticode\" ${GOOD_PATH})")
endif()
endmacro()
