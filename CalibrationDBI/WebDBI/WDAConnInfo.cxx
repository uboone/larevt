#ifndef WDACONNINFO_CXX
#define WDACONNINFO_CXX

#include "WDAConnInfo.h"

namespace lariov{

  WDAConnInfo::WDAConnInfo(const std::string  server,
			   const std::string  port,
			   const std::string  dbname,
			   const unsigned int timeout)
    : _server  ( server  )
    , _port    ( port    )
    , _dbname  ( dbname  )
    , _timeout ( timeout )
  {
    if( _server == kDEFAULT_WDA_CONN_STRING ) _server = getenv( "LIBWDA_SERVER" );
    if( _port   == kDEFAULT_WDA_CONN_STRING ) _port   = getenv( "LIBWDA_PORT"   );
    if( _dbname == kDEFAULT_WDA_CONN_STRING ) _dbname = getenv( "LIBWDA_DBNAME" );
  }

  std::string WDAConnInfo::URLPrefix() const
  {
    std::string url("http://");
    url += _server;
    if(!_port.empty()) url += ":" + _port;
    url += "/" + _dbname + "/app";
    return url;
  }

}

#endif
