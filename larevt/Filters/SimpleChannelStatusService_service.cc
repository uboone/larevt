/**
 * @file   SimpleChannelStatusService_service.cc
 * @brief  Service for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    SimpleChannelStatusService.h
 */

// Our header
#include "larevt/Filters/SimpleChannelStatusService.h"

// LArSoft libraries
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"

// Framework libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"



namespace lariov {

  //----------------------------------------------------------------------------
  SimpleChannelStatusService::SimpleChannelStatusService
    (fhicl::ParameterSet const& pset, art::ActivityRegistry& reg)
  {
    SimpleChannelStatus* simple_filter = new SimpleChannelStatus(pset);
    
    raw::ChannelID_t MaxChannel
      = raw::ChannelID_t(art::ServiceHandle<geo::Geometry>()->Nchannels() - 1);
    
    simple_filter->Setup(MaxChannel);
    
    fProvider.reset(simple_filter);
    
    mf::LogInfo("SimpleChannelStatusService") << "Loaded from configuration:"
      << "\n  - " << fProvider->BadChannels().size() << " bad channels"
      << "\n  - " << fProvider->NoisyChannels().size() << " noisy channels"
      << "\n  - largest channel ID: " << fProvider->MaxChannel()
        << ", largest present: " << fProvider->MaxChannelPresent()
      ;
    
    // register a callback to be executed when a new run starts
    reg.sPostBeginRun.watch(this, &SimpleChannelStatusService::postBeginRun);
    
  } // SimpleChannelStatusService::SimpleChannelStatusService()
  
  
  //----------------------------------------------------------------------------
  void SimpleChannelStatusService::postBeginRun(art::Run const&) {
    
    // on a new run, geometry might have changed;
    // this makes sense only if the geometry configuration was wrong in the
    // first place
    UpdateChannelRange();
    
  } // SimpleChannelStatusService::postBeginRun()
  
  
  //----------------------------------------------------------------------------
  void SimpleChannelStatusService::UpdateChannelRange() {
    
    raw::ChannelID_t MaxChannel
      = raw::ChannelID_t(art::ServiceHandle<geo::Geometry>()->Nchannels() - 1);
    
    if (MaxChannel != fProvider->MaxChannel()) {
      
      throw art::Exception(art::errors::Configuration)
        << "SimpleChannelStatusService does not support dynamic configuration, "
        << "but it appears that the channel configuration has sensibly changed."
        ;
      
    /* // this is if we had wanted to be forgiving
      simple_filter->Setup(MaxChannel);
      
      mf::LogInfo("SimpleChannelStatusService") << "Updated channel range:"
        << "\n  - largest channel ID: " << fProvider->MaxChannel()
          << ", largest present: " << fProvider->MaxChannelPresent()
        ;
    */
    } // if updated
    
  } // SimpleChannelStatusService::SimpleChannelStatusService()
  
  
  //----------------------------------------------------------------------------
  DEFINE_ART_SERVICE_INTERFACE_IMPL
    (lariov::SimpleChannelStatusService, lariov::IChannelStatusService)
  
} // namespace filter
