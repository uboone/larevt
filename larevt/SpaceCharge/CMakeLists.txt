art_make(NO_PLUGINS
         LIB_LIBRARIES
           canvas
           ${FHICLCPP}
           ${ROOT_CORE}
           ${ROOT_HIST}
           ${ROOT_RIO}
           cetlib
           cetlib_except
         )

install_headers()
install_fhicl()
install_source()
