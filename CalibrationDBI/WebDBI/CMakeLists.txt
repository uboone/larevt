
cet_find_library( LIBWDA NAMES wda PATHS ENV LIBWDA_LIB NO_DEFAULT_PATH )

include_directories( $ENV{LIBWDA_FQ_DIR}/include )

art_make( LIBRARY_NAME  CalibrationDBI_WebDBI
          LIB_LIBRARIES
			CalibrationDBI_IOVData
			${LIBWDA}
			Geometry
			Geometry_service
        )

install_headers()
install_fhicl()
install_source()
