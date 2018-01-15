set(FILTER_NAME AngleToServo)

# Specify the filter module target
adtf_add_filter(${FILTER_NAME}
                stdafx.h
                ../includes/ADTF_Helper.h
                ../includes/ADTF_Helper.cpp
                AngleToServo.h
                AngleToServo.cpp
)

# set folder (only needed and available for MSVC)
adtf_set_folder(${FILTER_NAME} controller)

# Specify where it should be installed to
adtf_install_plugin(${FILTER_NAME} ${CMAKE_INSTALL_BINARY})
