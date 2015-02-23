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

namespace lariov {
  /**
     \class ChData
  */
  class ChData {
    
    public:

      /// Constructor
      ChData(unsigned int ch) : fChannel(ch) {}

      /// Default destructor
      virtual ~ChData(){}

      unsigned int Channel() const  { return fChannel; }

      inline bool operator<(const U& rhs) const
      { return fChannel < rhs;}

      inline bool operator<(const ChData& ch) const
      { return fChannel < ch.Channel(); }

    protected:

      unsigned int fChannel;
    
  };
}

namespace std {
  class less<lariov::ChData*>
  {
    public:
      bool operator()( const lariov::ChData* lhs, const lariov::ChData* rhs )
      { return (*lhs) < (*rhs); }
  };
}


#endif
/** @} */ // end of doxygen group 

