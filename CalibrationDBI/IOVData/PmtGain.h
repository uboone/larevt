/**
 * \file PmtGain.h
 *
 * \ingroup IOVData
 * 
 * \brief Class def header for a class PmtGain
 *
 * @author eberly@slac.stanford.edu
 */

/** \addtogroup IOVData

    @{*/
#ifndef IOVDATA_PMTGAIN_H
#define IOVDATA_PMTGAIN_H 1

#include "ChData.h"

namespace lariov {
  /**
     \class PmtGain
  */
  class PmtGain : public ChData {
    
    public:
    
      /// Constructor
      PmtGain(unsigned int ch) : ChData(ch) {}
      
      /// Default destructor
      ~PmtGain() {}
            
      float SpeHeight()    const { return fHeight; }
      float SpeHeightErr() const { return fHeightErr; }
      float SpeWidth()     const { return fWidth; }
      float SpeWidthErr()  const { return fWidthErr; }
      float SpeArea()      const { return fArea; }
      float SpeAreaErr()   const { return fAreaErr; }
      
      void SetSpeHeight(float v)    { fHeight    = v; }
      void SetSpeHeightErr(float v) { fHeightErr = v; }
      void SetSpeWidth(float v)     { fWidth     = v; }
      void SetSpeWidthErr(float v)  { fWidthErr  = v; }
      void SetSpeArea(float v)      { fArea      = v; }
      void SetSpeAreaErr(float v)   { fAreaErr   = v; }
      
    private:
    
      float fHeight;
      float fHeightErr;
      float fWidth;
      float fWidthErr;
      float fArea;
      float fAreaErr;
      
  }; // end class
} // end namespace lariov

#endif
/** @} */ // end of doxygen group 
