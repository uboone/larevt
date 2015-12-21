
art_make( 
          LIBRARY_NAME  Filters
          LIB_LIBRARIES 
                        RecoBase
                        Geometry
                        larcore_Geometry_Geometry_service
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
                        ${ROOT_BASIC_LIB_LIST}
                        ${Boost_SYSTEM_LIBRARY}
          MODULE_LIBRARIES Filters
                           RawData
                           ${SIMULATIONBASE}
          SERVICE_LIBRARIES Filters
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
