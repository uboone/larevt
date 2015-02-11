
art_make( BASENAME_ONLY
	  EXCLUDE WebReaderService_service.cc
	  LIBRARY_NAME CalibrationDBI_LArBackend
          MODULE_LIBRARIES
		   CalibrationDBI_WebDBI
                   WebReaderService_service
		   ${ART_FRAMEWORK_CORE}
		   ${ART_FRAMEWORK_PRINCIPAL}
                   ${ART_FRAMEWORK_BASIC}
		   ${ART_FRAMEWORK_SERVICES_REGISTRY}
		   ${ART_FRAMEWORK_SERVICES_OPTIONAL}
		   ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
		   ${ART_PERSISTENCY_COMMON}
		   ${ART_PERSISTENCY_PROVENANCE}
		   ${ART_UTILITIES}
		   ${MF_MESSAGELOGGER}
		   ${MF_UTILITIES}
		   ${FHICLCPP}
		   ${CETLIB}
		   ${ROOT_GEOM}
		   ${ROOT_XMLIO}
		   ${ROOT_GDML}
		   ${ROOT_BASIC_LIB_LIST}
		)

simple_plugin(WebReaderService "service"
	      CalibrationDBI_WebDBI
              ${ART_FRAMEWORK_SERVICES_REGISTRY}
              ${FHICLCPP}
              BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()
