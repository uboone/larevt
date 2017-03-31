
simple_plugin(CalWireAna "module"
                        larevt_Filters
	                lardataobj_RawData
			lardata_Utilities_LArFFT_service
			lardata_Utilities
                        ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_FRAMEWORK_SERVICES_OPTIONAL}
			${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
			art_Persistency_Common
			art_Persistency_Provenance
			art_Utilities
                        canvas
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
	                ${FHICLCPP}
			cetlib cetlib_except
                        ${ROOT_GEOM}
                        ${ROOT_XMLIO}
                        ${ROOT_GDML}
			${ROOT_BASIC_LIB_LIST}
              
)

simple_plugin(CalWire "module"
                        larevt_Filters
                        lardataobj_RawData
                        lardataobj_RecoBase
                        lardata_ArtDataHelper
                        ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_FRAMEWORK_SERVICES_OPTIONAL}
			${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
			art_Persistency_Common
			art_Persistency_Provenance
			art_Utilities
                        canvas
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
	                ${FHICLCPP}
			cetlib cetlib_except
                        ${ROOT_GEOM}
                        ${ROOT_XMLIO}
                        ${ROOT_GDML}
			${ROOT_BASIC_LIB_LIST}
              
)

simple_plugin(CalWireT962 "module"
                        larevt_Filters
                        lardataobj_RawData
                        lardataobj_RecoBase
                        lardata_ArtDataHelper
                        larcore_Geometry_Geometry_service
                        larcore_Geometry
                        ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_FRAMEWORK_SERVICES_OPTIONAL}
			${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
			art_Persistency_Common
			art_Persistency_Provenance
			art_Utilities
                        canvas
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
	                ${FHICLCPP}
			cetlib cetlib_except
                        ${ROOT_GEOM}
                        ${ROOT_XMLIO}
                        ${ROOT_GDML}
			${ROOT_BASIC_LIB_LIST}
               
)

simple_plugin(DumpWires "module"
                        larevt_Filters
	                lardataobj_RawData
	                lardataobj_RecoBase
                        ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${MF_MESSAGELOGGER}
	                ${FHICLCPP}
			cetlib cetlib_except
              
)


install_headers()
install_fhicl()
install_source()

add_subdirectory(test)
