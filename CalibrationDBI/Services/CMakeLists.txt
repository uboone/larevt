art_make( BASENAME_ONLY
	  SERVICE_LIBRARIES CalibrationDBI_WebDBI
	                    CalibrationDBI_IOVData
			    ${ART_FRAMEWORK_SERVICES_REGISTRY}
			    ${FHICLCPP}
)

install_headers()
install_source()

#add_subdirectory(test)
