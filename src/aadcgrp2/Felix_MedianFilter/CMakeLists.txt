set(FILTER_NAME MedianFilter)

# Specify the filter module target
adtf_add_filter(${FILTER_NAME}
                median.h
                median.cpp
                stdafx.h
                ../includes/StdFilter.h
                ../includes/StdFilter.cpp
                MedianFilter.h
                MedianFilter.cpp
)

# set folder (only needed and available for MSVC)
adtf_set_folder(${FILTER_NAME} controller)

# Specify where it should be installed to
adtf_install_plugin(${FILTER_NAME} ${CMAKE_INSTALL_BINARY})
