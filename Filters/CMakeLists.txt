
art_make( BASENAME_ONLY
          MODULE_LIBRARIES 
			Filters
                        RecoBase
	                Geometry
	                Geometry_service
			Utilities
		    	RawData
			Simulation
			Geometry
                        LArProperties_service
			${SIMULATIONBASE}
                        ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_FRAMEWORK_SERVICES_BASIC}
			${ART_FRAMEWORK_SERVICES_OPTIONAL}
			${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
			${ART_PERSISTENCY_COMMON}
			${ART_PERSISTENCY_PROVENANCE}
			${ART_UTILITIES}
#			${ROOT_GEOM}
#                        ${ROOT_XMLIO}
#                        ${ROOT_GDML}
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
	                ${FHICLCPP}
			${CETLIB}
		      	${ROOT_BASIC_LIB_LIST}

          LIB_LIBRARIES Filters
                        RecoBase
	                Geometry
	                Geometry_service
			Utilities
		    	RawData
			Simulation
			Geometry
                        LArProperties_service
			${SIMULATIONBASE}
                        ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_FRAMEWORK_SERVICES_BASIC}
			${ART_FRAMEWORK_SERVICES_OPTIONAL}
			${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
			${ART_PERSISTENCY_COMMON}
			${ART_PERSISTENCY_PROVENANCE}
			${ART_UTILITIES}
#			${ROOT_GEOM}
#                        ${ROOT_XMLIO}
#                        ${ROOT_GDML}
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
	                ${FHICLCPP}
			${CETLIB}
		      	${ROOT_BASIC_LIB_LIST}

        )

install_headers()
install_fhicl()
install_source()
