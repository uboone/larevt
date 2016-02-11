
art_make( 
          
          LIB_LIBRARIES 
                        lardata_RecoBase
                        larcore_Geometry
                        larcore_Geometry_Geometry_service
                        lardata_Utilities
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
                        ${ROOT_BASIC_LIB_LIST}
                        ${Boost_SYSTEM_LIBRARY}
          MODULE_LIBRARIES larevt_Filters
                           lardata_RawData
                           lardata_Utilities_LArProperties_service
                           ${SIMULATIONBASE}
          SERVICE_LIBRARIES larevt_Filters
                            ${ART_FRAMEWORK_PRINCIPAL}
                            ${ART_FRAMEWORK_SERVICES_REGISTRY}
                            ${MF_MESSAGELOGGER}
                            ${MF_UTILITIES}
                            ${FHICLCPP}
                            ${CETLIB}
                            ${ROOT_BASIC_LIB_LIST}
        )

install_headers()
install_fhicl()
install_source()
