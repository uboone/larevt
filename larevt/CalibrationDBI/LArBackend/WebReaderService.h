////////////////////////////////////////////////////////////////////////
//
// WebReaderService
//
////////////////////////////////////////////////////////////////////////
#ifndef WEBREADERSERVICE_H
#define WEBREADERSERVICE_H

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Persistency/Common/Ptr.h"
#include "art/Persistency/Common/PtrVector.h"
#include "art/Persistency/RootDB/SQLite3Wrapper.h"

#include "CalibrationDBI/WebDBI/WebReader.h"

namespace lariov{

  class WebReaderService {

  public:

    WebReaderService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);

    virtual ~WebReaderService(){}

    template <class T>
    WebReader<T>& GetWebReader()
    {
      return WebReader<T>::GetME(_server,_port,_db,_timeout);
    }

  private:
    std::string  _server;
    std::string  _port;
    std::string  _db;
    unsigned int _timeout;
  }; // class WebReaderService

} //namespace lariov

DECLARE_ART_SERVICE(lariov::WebReaderService, LEGACY)

#endif 
