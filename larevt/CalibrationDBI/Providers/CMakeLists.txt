cet_find_library( LIBWDA NAMES wda PATHS ENV LIBWDA_LIB NO_DEFAULT_PATH )

include_directories( $ENV{LIBWDA_FQ_DIR}/include )

art_make( 
          LIB_LIBRARIES
			larevt_CalibrationDBI_IOVData
			${LIBWDA}
                        ${MF_MESSAGELOGGER}
                        ${MF_UTILITIES}
	                ${FHICLCPP}
			cetlib cetlib_except
			larcorealg_Geometry
			larcore_Geometry_Geometry_service
                        ${Boost_SYSTEM_LIBRARY}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ROOT_BASIC_LIB_LIST}
			art_Utilities canvas
        )

install_headers()
install_source()
