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

#include "WDAConnInfo.h"

#include <string>
#include <vector>
#include <TTimeStamp.h>


namespace lariov {

  typedef struct {
    TTimeStamp begin;
    TTimeStamp end;
    std::vector<std::string> fields;
    std::vector<std::string> types;
  } DBHeader;

  /**
     \class WebReader
     User defined class WebReader ... these comments are used to generate
     doxygen documentation!
  */
  class WebReader{

    public:
      
      /// Constructors
      WebReader() {}
      WebReader(const WDAConnInfo& conn, const std::string& folder, const std::string& tag="") : 
        fConn(conn), fFolder(folder), fTag(tag) {} 


      /// Default destructor
      virtual ~WebReader(){}

      /// Connection info updater
      void SetConnInfo(const WDAConnInfo& conn, const std::string& folder, const std::string& tag="");
      
      /// Anticipate derived class caching database info
      virtual void Update() = 0;  

    protected:
    
      /// Single useful function to retrieve pointer to DB contents and parse the header information
      void* Request(const TTimeStamp&  ts, DBHeader& header) const;
   
    private:
      
      WDAConnInfo fConn;   ///< DB Connection information
      std::string fFolder; ///< Folder Name
      std::string fTag;    ///< DB Tag
      
  };
}


#endif
/** @} */ // end of doxygen group 

