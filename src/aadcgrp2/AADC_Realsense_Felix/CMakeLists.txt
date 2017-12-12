# declare our filter
if(UNIX)
	set(FILTER_NAME aadc_felix_realsensefilter)

	# add the include directory of the filter
	include_directories(/home/aadc/ADTF/include
				${REALSENSE_INCLUDE_DIRS}
				${GLFW_INCLUDE_DIRS}
				${OpenCV_INCLUDE_DIRS})

	link_directories(${REALSENSE_LIBRARY_DIRS})

	#c++11 support for librealsense
	#if we use std=c++11 we have some warnings from ADTF an we supress them here
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-literal-suffix")

	adtf_add_filter(${FILTER_NAME}
		stdafx.h

		cRealsenseCameraFelix.h
		cRealsenseCameraFelix.cpp
		Realsensehelper.h
	)

	adtf_set_folder(${FILTER_NAME} camera)

	target_link_libraries(${FILTER_NAME} ${REALSENSE_LIBS} ${OPENGL_LIBRARIES} ${GLFW_LIBS} ${LIBUSB1_LIBRARIES} ${OpenCV_LIBS})
	# Specify where it should be installed to
	adtf_install_plugin(${FILTER_NAME} ${CMAKE_INSTALL_BINARY})
endif(UNIX)
