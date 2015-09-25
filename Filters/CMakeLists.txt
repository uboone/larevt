

art_make( BASENAME_ONLY
	  LIBRARY_NAME  Filters
        LIB_LIBRARIES 
                      RecoBase
	                Geometry
	                Geometry_service
			Utilities
                      ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
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
                        ${Boost_SYSTEM_LIBRARY}
                      ${ROOT_GEOM}
                      ${ROOT_XMLIO}
                      ${ROOT_GDML}
			${ROOT_BASIC_LIB_LIST}
        MODULE_LIBRARIES Filters
		    	   RawData
                         LArPropertiesService_service
			   ${SIMULATIONBASE}
        )

install_headers()
install_fhicl()
install_source()
