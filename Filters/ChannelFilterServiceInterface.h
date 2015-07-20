/**
 * @file   ChannelFilterServiceInterface.h
 * @brief  Interface for experiment-specific service for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 *
 * The schema is the same as for ExptGeoHelperInterface in Geometry library
 * (larcore repository).
 * The service is meant to be directly used.
 */


#ifndef CHANNELFILTERSERVICEINTERFACE_H
#define CHANNELFILTERSERVICEINTERFACE_H

// LArSoft libraries
#include "SimpleTypesAndConstants/RawTypes.h"
// #include "RawData/RawDigit.h"
#include "Filters/ChannelFilterBaseInterface.h"

// Framework libraries
#include "fhiclcpp/ParameterSet.h" // for convenience to the including services
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art/Framework/Services/Registry/ServiceMacros.h"

// C/C++ standard libraries
#include <memory> // std::unique_ptr<>


/// Filters for channels, events, etc
namespace filter {

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
   *      art::ServiceHandle<filter::ChannelFilterServiceInterface> chanFilt;
   *      
   * (include files Filters/ChannelFilterServiceInterface.h instead of
   * Filters/ChannelFilter.h) or
   *      
   *      filter::ChannelFilterBaseInterface const& chanFilt
   *        = art::ServiceHandle<filter::ChannelFilterServiceInterface>()
   *          ->GetFilter();
   *      
   * (include files Filters/ChannelFilterServiceInterface.h and
   * Filters/ChannelFilterBaseInterface.h instead of Filters/ChannelFilter.h).
   * The latter object can in principle be passed to algorithms that are not
   * art-aware.
   * 
   * 
   * @details
   * This class is not directly instanciable, although it can be copied or moved
   * from other instances. It provides basic functionality out of the box to
   * interface with a generic ChannelFilterBaseInterface object. The derived
   * class that wants to use the out-of-the-box features will have at least to
   * instanciate and initialize the concrete filter object as pFilter.
   */
  class ChannelFilterServiceInterface {
      public:
    ///< type of a list of channel IDs
    using ChannelSet_t = ChannelFilterBaseInterface::ChannelSet_t;
    
    /// Destructor
    virtual ~ChannelFilterServiceInterface() = default;
    
    
    //
    // Actual interface here
    //
    
    //@{
    /// Returns a reference to the service provider
    virtual ChannelFilterBaseInterface const& GetFilter() const
      { return *GetFilterPtr(); }
    ChannelFilterBaseInterface const& GetProvider() const
      { return GetFilter(); }
    //@}
    
    //@{
    /// Returns a pointer to the service provider (nullptr if not available)
    virtual ChannelFilterBaseInterface const* GetFilterPtr() const
      { return pFilter.get(); }
    ChannelFilterBaseInterface const* GetProviderPtr() const
      { return GetFilterPtr(); }
    //@}
    
    virtual bool hasProvider() const { return bool(pFilter); }
    
    //
    // end of interface
    //
    
      protected:
    
    //@{
    /// Sets the filter object, deleting the old one
    void SetFilter(ChannelFilterBaseInterface* new_filter)
      { pFilter.reset(new_filter); }
    void SetFilter(std::unique_ptr<ChannelFilterBaseInterface> new_filter)
      { pFilter = std::move(new_filter); }
    //@}
    
      private:
    /// pointer to our actual service provider
    std::unique_ptr<ChannelFilterBaseInterface> pFilter;
    
  }; // class ChannelFilterServiceInterface
} // namespace filter


DECLARE_ART_SERVICE_INTERFACE(filter::ChannelFilterServiceInterface, LEGACY)


#if 0
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
    
    const filter::ChannelFilterBaseInterface* pFilter =
      art::ServiceHandle<filter::ChannelFilterServiceInterface>()->GetFilter();
    
    for (const auto& pDigit: digits) {
      if (!pFilter->BadChannel(pDigit->Channel()))
        GoodDigits.push_back(&*pDigit);
    } // for
    
    return GoodDigits;
  } // SelectGoodChannels()
  
} // namespace filter
#endif // 0



#endif // CHANNELFILTERSERVICEINTERFACE_H
