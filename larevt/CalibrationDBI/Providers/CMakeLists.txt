cet_find_library( LIBWDA NAMES wda PATHS ENV LIBWDA_LIB NO_DEFAULT_PATH )

include_directories( $ENV{LIBWDA_FQ_DIR}/include )

art_make( LIBRARY_NAME  CalibrationDBI_Providers
          LIB_LIBRARIES
			CalibrationDBI_IOVData
			${LIBWDA}
	                ${FHICLCPP}
			${CETLIB}
			Geometry
			larcore_Geometry_Geometry_service
                        ${Boost_SYSTEM_LIBRARY}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ROOT_BASIC_LIB_LIST}
			${ART_UTILITIES}
        )

install_headers()
