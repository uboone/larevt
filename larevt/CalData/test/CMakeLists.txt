cet_enable_asserts()

simple_plugin(FFTTest "module"
			lardata_Utilities_LArFFT_service
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_ROOT_IO_TFILE_SUPPORT} ${ROOT_CORE}
			${ART_ROOT_IO_TFILESERVICE_SERVICE}
			art_Persistency_Common
			art_Persistency_Provenance
	 		art_Utilities
                        canvas
			${MF_MESSAGELOGGER}
			
	                ${FHICLCPP}
			cetlib cetlib_except
			${ROOT_BASIC_LIB_LIST}
              
)

install_headers()
install_fhicl()
install_source()

