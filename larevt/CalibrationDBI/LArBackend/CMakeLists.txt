
#art_make( 
#	  EXCLUDE WebReaderService_service.cc
#	  
#          MODULE_LIBRARIES
#		   CalibrationDBI_WebDBI
#                   WebReaderService_service
#		   ${ART_FRAMEWORK_CORE}
#		   ${ART_FRAMEWORK_PRINCIPAL}
#                   ${ART_FRAMEWORK_BASIC}
#		   ${ART_FRAMEWORK_SERVICES_REGISTRY}
#		   ${ART_ROOT_IO_TFILE_SUPPORT} ${ROOT_CORE}
#		   ${ART_ROOT_IO_TFILESERVICE_SERVICE}
#		   art_Persistency_Common canvas
#		   art_Persistency_Provenance canvas
#		   art_Utilities canvas
#		   ${MF_MESSAGELOGGER}
#		   
#		   ${FHICLCPP}
#		   cetlib cetlib_except
#		   ${ROOT_GEOM}
#		   ${ROOT_XMLIO}
#		   ${ROOT_GDML}
#		   ${ROOT_BASIC_LIB_LIST}
#		)

#simple_plugin(WebReaderService "service"
#	      CalibrationDBI_WebDBI
#              ${ART_FRAMEWORK_SERVICES_REGISTRY}
#              ${FHICLCPP}
#              
#)

add_subdirectory(job)

#install_headers()
install_fhicl()
#install_source()
