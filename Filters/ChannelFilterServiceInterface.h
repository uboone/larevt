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

// C/C++ standard libraries
#include <cstdlib> // std::uint32_t
#include <set>
#include <memory> // std::unique_ptr<>
#include <type_traits> // std::is_same<>, std::enable_if<>

// Framework libraries
#include "fhiclcpp/ParameterSet.h" // for convenience to the including services
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Principal/Run.h"

// LArSoft libraries
#include "RawData/RawDigit.h"
#include "Filters/ChannelFilterBaseInterface.h"


/// Filters for channels, events, etc
namespace filter {

  /**
   * @brief Service providing information about the quality of channels
   *
   * This service provides only a simple interface.
   * Experiments need to implement and configure their own service implementing
   * this interface.
   * The simplest implementation is provided in LArSoft:
   * ConfigurableChannelFilterService.
   * 
   * Currently, the service provides interface for the following information:
   * - goodness of the channel: good or bad (dead or unusable)
   * - noisiness of the channel: good or noisy (or compromised in some way)
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
   *      const filter::ChannelFilterBaseInterface* chanFilt
   *        = art::ServiceHandle<filter::ChannelFilterServiceInterface>()->GetFilter();
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
    using channel_type = std::uint32_t; ///< type of channel ID
    using channel_list_type = std::set<channel_type>;
      ///< type of a list of channel IDs
    
    /// Copy constructor
    ChannelFilterServiceInterface(const ChannelFilterServiceInterface&)
      = default;
    
    /// Move constructor
    ChannelFilterServiceInterface(ChannelFilterServiceInterface&&)
      = default;
    
    /// Copy assignment
    ChannelFilterServiceInterface& operator=
      (const ChannelFilterServiceInterface&) = default;
    
    /// Move assignment
    ChannelFilterServiceInterface& operator=
      (ChannelFilterServiceInterface&&) = default;
    
    
    /// Destructor
    virtual ~ChannelFilterServiceInterface() = default;
    
    //
    // Actual interface here
    //
    
    /// Returns whether the specified channel is bad in the current run
    virtual bool BadChannel(channel_type channel) const
      { return SetOfBadChannels().count(channel) > 0; }
    
    /// Returns whether the specified channel is noisy in the current run
    virtual bool NoisyChannel(channel_type channel) const
      { return SetOfNoisyChannels().count(channel) > 0; }
    
    /// Returns a copy of set of bad channel IDs for the current run
    virtual channel_list_type SetOfBadChannels() const
      { return pFilter->SetOfBadChannels(); }
    
    /// Returns a copy of set of noisy channel IDs for the current run
    virtual channel_list_type SetOfNoisyChannels() const
      { return pFilter->SetOfNoisyChannels(); }
    
    //@{
    /// Returns a pointer to the information provider (nullptr if not available)
    virtual ChannelFilterBaseInterface const* GetFilter() const
      { return pFilter.get(); }
    virtual ChannelFilterBaseInterface* GetFilter()
      { return pFilter.get(); }
    //@}
    
    //
    // end of interface
    //
    
      protected:
    /// Default constructor: binds the begin-of-run update
    ChannelFilterServiceInterface
      (fhicl::ParameterSet const&, art::ActivityRegistry& reg)
      {
        reg.sPreBeginRun.watch
          (this, &ChannelFilterServiceInterface::preBeginRun);
      } // ChannelFilterServiceInterface()
    
    
    /// Triggers automatic update
    void preBeginRun(art::Run const& run) { SetRun(run); }
    
    /// Updates the underlying object to the new run
    virtual void SetRun(art::Run const& run)
      { if (pFilter) pFilter->SetRun(run.run()); }
    
    
    //@{
    /// Sets the filter object, deleting the old one
    void SetFilter(ChannelFilterBaseInterface* new_filter)
      { pFilter.reset(new_filter); }
    void SetFilter(std::unique_ptr<ChannelFilterBaseInterface> new_filter)
      { pFilter = std::move(new_filter); }
    //@}
    
      private:
    /// pointer to our actual info provider
    std::unique_ptr<ChannelFilterBaseInterface> pFilter;
    
  }; // class ChannelFilterServiceInterface
} // namespace filter


DECLARE_ART_SERVICE_INTERFACE(filter::ChannelFilterServiceInterface, LEGACY)


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




#endif // CHANNELFILTERSERVICEINTERFACE_H
