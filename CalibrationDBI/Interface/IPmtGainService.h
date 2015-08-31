/**
 * @file   IPmtGainService.h
 * @brief  Interface for experiment-specific service for pmt gain info
 * @author Brandon Eberly (eberly@fnal.gov)
 * @date   August 30, 2015
 */

#ifndef IPMTGAINSERVICE_H
#define IPMTGAINSERVICE_H

// LArSoft libraries
#include "CalibrationDBIFwd.h"

// Framework libraries
#include "art/Framework/Services/Registry/ServiceMacros.h"

//forward declarations
namespace lariov {
  class IPmtGainProvider;
}

namespace lariov {

  /**
   * @brief Service providing PMT Gain information
   *
   * This service provides only a simple interface to a provider class
   */
  class IPmtGainService {
      
    public:
   
      /// Destructor
      virtual ~IPmtGainService() = default;

      //
      // Actual interface here
      //

      //@{
      /// Returns a reference to the service provider
      IPmtGainProvider const& GetProvider() const
	{ return this->DoGetProvider(); }
      //@}

      //
      // end of interface
      //
    
    private:

      /// Returns a reference to the service provider
      virtual IPmtGainProvider const& DoGetProvider() const = 0;
    
    
    
  }; // class IPmtGainService
} // namespace lariov


DECLARE_ART_SERVICE_INTERFACE(lariov::IPmtGainService, LEGACY)




#if 0

#include "RawData/RawDigit.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

namespace filter {
  
  
  /** **************************************************************************
   * @brief Filters out the bad channels
   * @tparam Digits a collection of raw::RawDigit objects or pointers
   * @param digits list of raw::RawDigit pointers
   * @return a vector of pointers to good raw::RawDigit
   * 
   * This function can be used to filter out bad channels. For example:
   *     
   *     art::View<raw::RawDigit> rawdigits;
   *     evt.getView("SimWire", rawdigits);
   *     
   *     // loop only through non-bad raw digits
   *     for(const raw::RawDigit* digit: filter::SelectGoodChannels(rawdigits))
   *       ...
   *     
   * Note that this function does not work with lists of digits, but only with
   * list of digit pointers (including art::View, art::PtrVector<raw::RawDigit>,
   * std::vector<art::Ptr<raw::Digit>> ...).
   */
  template <typename Digits>
  std::vector<const raw::RawDigit*> SelectGoodChannels(const Digits& digits) {
    
    std::vector<const raw::RawDigit*> GoodDigits;
    
    const filter::IChannelFilterProvider* pFilter =
      art::ServiceHandle<lariov::IChannelFilterService>()->GetFilter();
    
    for (const auto& pDigit: digits) {
      if (!pFilter->BadChannel(pDigit->Channel()))
        GoodDigits.push_back(&*pDigit);
    } // for
    
    return GoodDigits;
  } // SelectGoodChannels()
  
} // namespace filter
#endif // 0



#endif // ICHANNELFILTERSERVICE_H
