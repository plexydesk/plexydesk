# - Try to find the FFMPEG package -
# If found the following will be defined
#
#  FFMPEG_FOUND - FFMPEG package found on the system
#  FFMPEG_INCLUDE_DIR - Directory of the FFMPEG package include files
#  FFMPEG_LIBRARY - Where libkbfxcommon.so resides
#

FIND_PATH(FFMPEG_INCLUDE_DIR ffmpeg/avutil.h
  PATHS
  ${INCLUDE_SEARCH_PATHES}
)

FIND_LIBRARY(FFMPEG_LIBRARY NAMES libavutil 
  PATHS
  ${LIB_SEARCH_PATHES}
)

if (FFMPEG_INCLUDE_DIR AND FFMPEG_LIBRARY)
   set(FFMPEG_FOUND TRUE)
endif(FFMPEG_INCLUDE_DIR AND FFMPEG_LIBRARY)

if (FFMPEG_FOUND)
  IF(NOT FFMPEG_FIND_QUIETLY)
    MESSAGE(STATUS "Found FFMPEG package: ${FFMPEG_LIBRARY}")
  endif(NOT FFMPEG_FIND_QUIETLY)
ELSE(FFMPEG_FOUND)
  IF(FFMPEG_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find FFMPEG package! Please download and install FFMPEG from http://ffmpeg.mplayerhq.hu")
  endif(FFMPEG_FIND_REQUIRED)
endif(FFMPEG_FOUND)

