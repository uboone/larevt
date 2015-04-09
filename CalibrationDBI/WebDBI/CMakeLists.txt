
set( WDA_LIB $ENV{LIBWDA_LIB}/libwda.so )

include_directories( $ENV{LIBWDA_FQ_DIR}/include )

art_make( LIBRARY_NAME  CalibrationDBI_WebDBI
          LIB_LIBRARIES
			CalibrationDBI_IOVData
			${WDA_LIB}
			Geometry
			Geometry_service
        )

install_headers()
install_fhicl()
install_source()
