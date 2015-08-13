/**
 * \file IDetPedestalProvider
 * 
 * \brief Class def header for a class IDetPedestalProvider
 *
 * @author eberly@slac.stanford.edu
 */

#ifndef IDETPEDESTALPROVIDER_H
#define IDETPEDESTALPROVIDER_H

namespace lariov {

  /**
     \class IDetPedestalProvider
     Pure abstract interface class for retrieving detector pedestals.
     Includes a feature to encourage database use: an Update method that can be used to update 
     an implementation's local state to ensure that the correct information is retrieved
  */
  class IDetPedestalProvider {
  
    public:
       
      /// Retrieve pedestal information     
      virtual float PedMean(std::uint64_t ch) const = 0;
      virtual float PedRms(std::uint64_t ch) const = 0;
      virtual float PedMeanErr(std::uint64_t ch) const = 0;
      virtual float PedRmsErr(std::uint64_t ch) const = 0;
      
      /// Update local state of implementation
      virtual bool Update(std::uint64_t ts) = 0;
  };
}//end namespace lariov

#endif
