cmake_minimum_required(VERSION 3.10)

set(FRANKENTAR_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/frankentar.h

	${CMAKE_CURRENT_LIST_DIR}/frankentar/read.h
	${CMAKE_CURRENT_LIST_DIR}/frankentar/write.h
PARENT_SCOPE)
