/**
 * @file   IChannelStatusService.h
 * @brief  Interface for experiment-specific service for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 *
 * The schema is the same as for ExptGeoHelperInterface in Geometry library
 * (larcore repository).
 * The implementations of this interface can be directly used as art services.
 */

#ifndef ICHANNELSTATUSSERVICE_H
#define ICHANNELSTATUSSERVICE_H

// LArSoft libraries
#include "CoreUtils/ServiceUtil.h" // ServiceRequirementsChecker<>
#include "CalibrationDBI/Interface/IChannelStatusProvider.h"

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
   * SimpleChannelStatusService.
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
   *          ->GetProvider();
   *      
   * (include files CalibrationDBI/Interface/IChannelStatusService.h and
   * CalibrationDBI/Interface/IChannelStatusProvider.h instead of
   * Filters/ChannelFilter.h).
   * The latter object can in principle be passed to algorithms that are not
   * art-aware.
   * 
   */
  class IChannelStatusService {
    
    public:
      
      using provider_type = IChannelStatusProvider;
      
      /// Destructor
      virtual ~IChannelStatusService() = default;

      //
      // Actual interface here
      //

      //@{
      /// Returns a reference to the service provider
      IChannelStatusProvider const& GetProvider() const
        { return DoGetProvider(); }
      // will be deprecated:
      IChannelStatusProvider const& GetFilter() const { return GetProvider(); }
      //@}

      //@{
      /// Returns a pointer to the service provider
      IChannelStatusProvider const* GetProviderPtr() const
        { return DoGetProviderPtr(); }
      // will be deprecated:
      IChannelStatusProvider const* GetFilterPtr() const
        { return GetProviderPtr(); }
      //@}
      
      
      IChannelStatusProvider const* provider() const
        { return GetProviderPtr(); }
      
      //
      // end of interface
      //
    
    private:
    
      /// Returns a pointer to the service provider 
      virtual IChannelStatusProvider const* DoGetProviderPtr() const = 0;

      /// Returns a reference to the service provider
      virtual IChannelStatusProvider const& DoGetProvider() const = 0;
    
    
    
  }; // class IChannelStatusService
  
} // namespace lariov


DECLARE_ART_SERVICE_INTERFACE(lariov::IChannelStatusService, LEGACY)


// check that the requirements for lariov::IChannelStatusService are satisfied
template class lar::details::ServiceRequirementsChecker<lariov::IChannelStatusService>;


#endif // ICHANNELSTATUSSERVICE_H
