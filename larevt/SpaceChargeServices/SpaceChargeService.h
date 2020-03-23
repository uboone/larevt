////////////////////////////////////////////////////////////////////////
// \file SpaceChargeService.h
//
// \brief pure virtual service interface for space charge distortions
//
// \author mrmooney@bnl.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef SPACECHARGESERVICE_H
#define SPACECHARGESERVICE_H

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "fhiclcpp/ParameterSet.h"
#include "larcore/CoreUtils/ServiceUtil.h"
#include "larevt/SpaceCharge/SpaceCharge.h"

namespace spacecharge {
  class SpaceChargeService {

  public:
    typedef spacecharge::SpaceCharge provider_type;

  public:
    virtual ~SpaceChargeService() = default;

    virtual void reconfigure(fhicl::ParameterSet const& pset) = 0;
    virtual const spacecharge::SpaceCharge* provider() const = 0;

  }; // class SpaceChargeService
} //namespace spacecharge
DECLARE_ART_SERVICE_INTERFACE(spacecharge::SpaceChargeService, LEGACY)
#endif // SPACECHARGESERVICE_H
