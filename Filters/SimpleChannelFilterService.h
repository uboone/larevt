/**
 * @file   SimpleChannelFilterService.h
 * @brief  Service for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    IChannelFilterService.h SimpleChannelFilter.h
 *
 * Service serving a SimpleChannelFilter.
 */


#ifndef SIMPLECHANNELFILTERSERVICE_H
#define SIMPLECHANNELFILTERSERVICE_H

// LArSoft libraries
#include "Filters/SimpleChannelFilter.h"
#include "CalibrationDBI/Interface/IChannelFilterService.h"

// C/C++ standard libraries
#include <memory> //std::unique_ptr

namespace art {
  class ActivityRegistry;
} // namespace art


namespace lariov {

  /**
   * @brief Service providing information about the quality of channels
   *
   * This is a simple implementation of the IChannelFilterService
   * service interface.
   * Channel lists are passed by FHiCL configuration.
   * Note that there is no support for conditions varying with time.
   * 
   * Configuration parameters
   * =========================
   * 
   * In addition to the parameters supported by filter::SimpleChannelFilter,
   * this service supports:
   * 
   * - *service_type* (string): must be set to "SimpleChannelFilterService"
   * 
   */
  class SimpleChannelFilterService: public IChannelFilterService {
      public:
    
    /// Constructor: reads the channel IDs from the configuration
    SimpleChannelFilterService
      (fhicl::ParameterSet const& pset, art::ActivityRegistry&);
    
    
      private:
      
    IChannelFilterProvider const& DoGetFilter() const override 
     { return *GetFilterPtr(); }
    IChannelFilterProvider const* DoGetFilterPtr() const override 
     { return fFilter.get(); } 
    
    std::unique_ptr<SimpleChannelFilter> fFilter;
    
    
    
  }; // class SimpleChannelFilterService
  
  
} // namespace lariov

DECLARE_ART_SERVICE_INTERFACE_IMPL(lariov::SimpleChannelFilterService, lariov::IChannelFilterService, LEGACY)

#endif // SIMPLECHANNELFILTERSERVICE_H
