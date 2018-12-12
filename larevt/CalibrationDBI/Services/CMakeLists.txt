art_make( 
	  SERVICE_LIBRARIES larevt_CalibrationDBI_Providers
	                    larevt_CalibrationDBI_IOVData
			    ${ART_FRAMEWORK_SERVICES_REGISTRY}
			    art_Framework_Principal
			    ${FHICLCPP}
)

install_headers()
install_source()
