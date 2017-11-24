set(FILTER_NAME RadiusToAngle)

# Specify the filter module target
adtf_add_filter(${FILTER_NAME}
                stdafx.h
                RadiusToAngle.h
                RadiusToAngle.cpp
)

# set folder (only needed and available for MSVC)
adtf_set_folder(${FILTER_NAME} controller)

# Specify where it should be installed to
adtf_install_plugin(${FILTER_NAME} ${CMAKE_INSTALL_BINARY})
