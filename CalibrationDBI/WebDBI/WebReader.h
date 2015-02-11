/**
 * \file WebReader.h
 *
 * \ingroup WebDBI
 * 
 * \brief Class def header for a class WebReader
 *
 * @author kterao
 */

/** \addtogroup WebDBI

    @{*/
#ifndef WEBDBI_WEBREADER_H
#define WEBDBI_WEBREADER_H

#include "CalibrationDBI/IOVData/Snapshot.h"
#include <map>
namespace lariov {

  /**
     \class WebReader
     User defined class WebReader ... these comments are used to generate
     doxygen documentation!
  */
  template <class T>
  class WebReader{
    
  public:
    
    /// Default constructor
    WebReader(std::string  server  = "default",
	      std::string  port    = "default",
	      std::string  dbname  = "default",
	      unsigned int timeout = 10);
    
    /// Default destructor
    virtual ~WebReader(){}

    const Snapshot<T>& Request(const std::string& name,
			       const TTimeStamp&  ts,
			       const std::string tag="");
    
  private:

    std::string  _server;  ///< HTML server dns
    std::string  _port;    ///< Port ID
    std::string  _dbname;  ///< HTML server db name
    unsigned int _timeout; ///< Query timeout period [s]
    std::map<std::string, lariov::Snapshot<T> > _data_m;

  };
}

template class lariov::WebReader< std::string >;
template class lariov::WebReader< float  >;
template class lariov::WebReader< double >;
template class lariov::WebReader< short  >;
template class lariov::WebReader< int    >;
template class lariov::WebReader< long   >;
template class lariov::WebReader< unsigned short >;
template class lariov::WebReader< unsigned int   >;
template class lariov::WebReader< unsigned long  >;

#endif
/** @} */ // end of doxygen group 

