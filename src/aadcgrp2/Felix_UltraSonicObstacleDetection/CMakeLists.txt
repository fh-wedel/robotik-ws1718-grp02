set(FILTER_NAME UltraSonicObstacleDetection)

# Specify the filter module target
adtf_add_filter(${FILTER_NAME}
                stdafx.h
                ../Felix_MedianFilter/median.h
                ../Felix_MedianFilter/median.cpp
                UltraSonicObstacleDetection.h
                UltraSonicObstacleDetection.cpp
)

# set folder (only needed and available for MSVC)
adtf_set_folder(${FILTER_NAME} controller)

# Specify where it should be installed to
adtf_install_plugin(${FILTER_NAME} ${CMAKE_INSTALL_BINARY})
