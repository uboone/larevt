cet_enable_asserts()

simple_plugin ( SimpleChannelFilterTest "module"
                    Filters
#                     ChannelFilterServiceInterface_service
                    ${MF_MESSAGELOGGER}
                    ${MF_UTILITIES}
                    ${FHICLCPP}
                    ${CETLIB}
                    ${ROOT_BASIC_LIB_LIST}
                BASENAME_ONLY
              )

cet_test(ChannelFilterServiceTest HANDBUILT
  DATAFILES test_channel_filter.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./test_channel_filter.fcl
)

cet_test(SimpleChannelFilter_test
  SOURCES SimpleChannelFilter_test.cxx
  LIBRARIES Filters
            CalibrationDBI_IOVData
            ${FHICLCPP}
  USE_BOOST_UNIT
)

# install_headers()
# install_fhicl()
# install_source()
