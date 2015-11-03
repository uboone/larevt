/**
 * @file   IChannelStatusService.h
 * @brief  Interface for experiment-specific service for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 *
 * The schema is the same as for ExptGeoHelperInterface in Geometry library
 * (larcore repository).
 * The service is meant to be directly used.
 */

#ifndef ICHANNELSTATUSSERVICE_H
#define ICHANNELSTATUSSERVICE_H

// LArSoft libraries
#include "CalibrationDBIFwd.h"

// Framework libraries
#include "art/Framework/Services/Registry/ServiceMacros.h"

//forward declarations
namespace lariov {
  class IChannelStatusProvider;
}

namespace lariov {

  /**
   * @brief Service providing information about the quality of channels
   *
   * This service provides only a simple interface.
   * Experiments need to implement and configure their own service implementing
   * this interface.
   * The simplest implementation is provided in LArSoft:
   * SimpleChannelFilterService.
   * 
   * Currently, the service provides interface for the following information:
   * - goodness of the channel: good or bad (dead or unusable)
   * - noisiness of the channel: good or noisy (or compromised in some way)
   * - presence of the channel: connected to a wire or not
   * 
   * The use of this service replaces the deprecated ChannelFilter class.
   * An algorithm that used to use ChannelFilter class can be updated. From:
   *      
   *      filter::ChannelFilter* chanFilt = new filter::ChannelFilter();
   *      
   * to
   *      
   *      art::ServiceHandle<lariov::IChannelStatusService> chanFilt;
   *      
   * (include files CalibrationDBI/Interface/IChannelStatusService.h instead of
   * Filters/ChannelFilter.h) or
   *      
   *      lariov::IChannelStatusProvider const& chanFilt
   *        = art::ServiceHandle<lariov::IChannelStatusService>()
   *          ->GetFilter();
   *      
   * (include files CalibrationDBI/Interface/IChannelStatusService.h and
   * CalibrationDBI/Interface/IChannelStatusProvider.h instead of Filters/ChannelFilter.h).
   * The latter object can in principle be passed to algorithms that are not
   * art-aware.
   * 
   */
  class IChannelStatusService {
      
    public:
   
      /// Destructor
      virtual ~IChannelStatusService() = default;

      //
      // Actual interface here
      //

      //@{
      /// Returns a reference to the service provider
      IChannelStatusProvider const& GetFilter() const
	{ return DoGetFilter(); }
      //@}

      //@{
      /// Returns a pointer to the service provider
      IChannelStatusProvider const* GetFilterPtr() const
	{ return DoGetFilterPtr(); }
      //@}

      //
      // end of interface
      //
    
    private:
    
      /// Returns a pointer to the service provider 
      virtual IChannelStatusProvider const* DoGetFilterPtr() const = 0;

      /// Returns a reference to the service provider
      virtual IChannelStatusProvider const& DoGetFilter() const = 0;
    
    
    
  }; // class IChannelStatusService
} // namespace lariov


DECLARE_ART_SERVICE_INTERFACE(lariov::IChannelStatusService, LEGACY)




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
    
    const filter::IChannelStatusProvider* pFilter =
      art::ServiceHandle<lariov::IChannelStatusService>()->GetFilter();
    
    for (const auto& pDigit: digits) {
      if (!pFilter->BadChannel(pDigit->Channel()))
        GoodDigits.push_back(&*pDigit);
    } // for
    
    return GoodDigits;
  } // SelectGoodChannels()
  
} // namespace filter
#endif // 0



#endif // ICHANNELSTATUSSERVICE_H
