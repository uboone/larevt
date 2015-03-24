#ifndef DATABASERETRIEVALALG_CXX
#define DATABASERETRIEVALALG_CXX 1

#include "DatabaseRetrievalAlg.h"

namespace lariov {

  /// Configure using fhicl::ParameterSet
  void DatabaseRetrievalAlg::Reconfigure(fhicl::ParameterSet const& p) {

    std::string foldername = p.get<std::string>("DBFolderName", "detpedestals");
    std::string url        = p.get<std::string>("DBUrl", "http://dbdata0.fnal.gov:8086/uboonecon_dev/app/");
    std::string tag        = p.get<std::string>("DBTag", "");
    fFolder.reset(new DBFolder(foldername, url, tag));
  }
}
#endif
