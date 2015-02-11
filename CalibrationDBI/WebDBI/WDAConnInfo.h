/**
 * \file WDAConnInfo.h
 *
 * \ingroup WebDBI
 * 
 * \brief Class def header for a class WDAConnInfo
 *
 * @author kterao
 */

/** \addtogroup WebDBI

    @{*/
#ifndef WEBDBI_WDACONNINFO_H
#define WEBDBI_WDACONNINFO_H

#include <string>
#include "WebDBIConstants.h"
namespace lariov {
  /**
     \class WDAConnInfo
  */
  class WDAConnInfo{
    
  public:
    
    /// Default constructor

    WDAConnInfo(const std::string  server  = kDEFAULT_WDA_CONN_STRING,
		const std::string  port    = kDEFAULT_WDA_CONN_STRING,
		const std::string  dbname  = kDEFAULT_WDA_CONN_STRING,
		const unsigned int timeout = kDEFAULT_WDA_TIMEOUT_SEC);
    
    /// Default destructor
    ~WDAConnInfo(){}

    std::string URLPrefix() const;

    std::string  _server;
    std::string  _port;
    std::string  _dbname;
    unsigned int _timeout;

    inline bool operator== ( const WDAConnInfo& rhs) const
    { return (_server==rhs._server && _port==rhs._port && _dbname==rhs._dbname); }

    inline bool operator!= ( const WDAConnInfo& rhs) const
    { return !((*this)==rhs); }
    
  };
}

#endif
/** @} */ // end of doxygen group 

