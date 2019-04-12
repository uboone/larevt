////////////////////////////////////////////////////////////////////////
// \file SpaceChargeServiceStandard.h
//
// \brief header of service for storing/accessing space charge distortions
//
// \author mrmooney@bnl.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef SPACECHARGESERVICESTANDARD_H
#define SPACECHARGESERVICESTANDARD_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Principal/Run.h"
#include "larevt/SpaceCharge/SpaceChargeStandard.h"
#include "larevt/SpaceChargeServices/SpaceChargeService.h"


namespace spacecharge{
  class SpaceChargeServiceStandard : public SpaceChargeService {
    public:

      // this enables art to print the configuration help:
      //using Parameters = art::ServiceTable<spacecharge::SpaceChargeStandard::ConfigurationParameters_t>;

      SpaceChargeServiceStandard(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);

      virtual void   reconfigure(fhicl::ParameterSet const& pset) override;
      void   preBeginRun(const art::Run& run);

      virtual const  provider_type* provider() const override { return fProp.get();}

    private:

      std::unique_ptr<spacecharge::SpaceChargeStandard> fProp;

    }; // class SpaceChargeServiceStandard
} //namespace spacecharge
DECLARE_ART_SERVICE_INTERFACE_IMPL(spacecharge::SpaceChargeServiceStandard, spacecharge::SpaceChargeService, LEGACY)
#endif // SPACECHARGESERVICESTANDARD_H
