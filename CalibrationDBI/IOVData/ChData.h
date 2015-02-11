/**
 * \file ChData.h
 *
 * \ingroup IOVData
 * 
 * \brief Class def header for a class ChData
 *
 * @author kterao
 */

/** \addtogroup IOVData

    @{*/
#ifndef IOVDATA_CHDATA_H
#define IOVDATA_CHDATA_H

#include <vector>
#include <string>

namespace lariov {
  /**
     \class ChData
  */
  template <class T>
  class ChData : public std::vector<T> {
    
  public:
    
    /// Default constructor
    ChData() : std::vector<T>() {}
    
    /// Default destructor
    virtual ~ChData(){}

    void Channel(unsigned int ch) { _ch = ch;   }

    unsigned int Channel() const  { return _ch; }

    template <class U>
    inline bool operator<(const U& rhs) const
    { return _ch < rhs;}

    inline bool operator<(const ChData& ch) const
    { return _ch < ch.Channel(); }

  protected:

    unsigned int _ch;
    
  };
}

namespace std {
  template <class T>
  class less<lariov::ChData<T>*>
  {
  public:
    bool operator()( const lariov::ChData<T>* lhs, const lariov::ChData<T>* rhs )
    { return (*lhs) < (*rhs); }
  };
}

template class lariov::ChData< std::string >;
template class lariov::ChData< float  >;
template class lariov::ChData< double >;
template class lariov::ChData< short  >;
template class lariov::ChData< int    >;
template class lariov::ChData< long   >;
template class lariov::ChData< unsigned short >;
template class lariov::ChData< unsigned int   >;
template class lariov::ChData< unsigned long  >;

#endif
/** @} */ // end of doxygen group 

