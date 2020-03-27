#ifndef DATABASERETRIEVALALG_CXX
#define DATABASERETRIEVALALG_CXX 1

#include "DatabaseRetrievalAlg.h"

namespace lariov {

  /// Configure using fhicl::ParameterSet
  void DatabaseRetrievalAlg::Reconfigure(fhicl::ParameterSet const& p) {

    std::string foldername = p.get<std::string>("DBFolderName");
    std::string url        = p.get<std::string>("DBUrl");
    std::string url2       = p.get<std::string>("DBUrl2", "");
    std::string tag        = p.get<std::string>("DBTag", "");
    bool usesqlite         = p.get<bool>("UseSQLite", false);
    bool testmode          = p.get<bool>("TestMode", false);
    fFolder.reset(new DBFolder(foldername, url, url2, tag, usesqlite, testmode));
  }
}
#endif
