/**
 * \file IOVDataError.h
 *
 * \ingroup IOVData
 * 
 * \brief Collection of exception classes for IOVData
 *
 * @author kterao
 */

/** \addtogroup IOVData

    @{*/
#ifndef IOVDATA_IOVDATAERROR_H
#define IOVDATA_IOVDATAERROR_H

#include <iostream>
#include <exception>

namespace lariov {

  /**
     \class IOVDataError
  */
  class IOVDataError : public std::exception{

  public:

    IOVDataError(std::string msg="") : std::exception()
    {
      _msg = "\033[93m";
      _msg += msg;
      _msg += "\033[00m";
    }
    
    virtual ~IOVDataError() throw(){};
    virtual const char* what() const throw()
    { return _msg.c_str(); }

  private:
    std::string _msg;
  };

}

#endif
/** @} */ // end of doxygen group 

