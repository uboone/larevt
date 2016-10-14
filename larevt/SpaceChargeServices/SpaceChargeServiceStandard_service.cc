////////////////////////////////////////////////////////////////////////
// \file SpaceChargeStandard.cxx
//
// \brief implementation of class for storing/accessing space charge distortions
//
// \author mrmooney@bnl.gov
// 
////////////////////////////////////////////////////////////////////////

// C++ language includes
#include <iostream>

// LArSoft includes
#include "larevt/SpaceChargeServices/SpaceChargeServiceStandard.h"

// ROOT includes
#include "TMath.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/exception.h"

//-----------------------------------------------
spacecharge::SpaceChargeServiceStandard::SpaceChargeServiceStandard(fhicl::ParameterSet const& pset, art::ActivityRegistry &reg)
{
  fProp.reset(new spacecharge::SpaceChargeStandard(pset));

  reg.sPreBeginRun.watch(this, &SpaceChargeServiceStandard::preBeginRun);
}

//----------------------------------------------
void spacecharge::SpaceChargeServiceStandard::preBeginRun(const art::Run& run)
{
  fProp->Update(run.id().run());
}

//------------------------------------------------
void spacecharge::SpaceChargeServiceStandard::reconfigure(fhicl::ParameterSet const& pset)
{
  fProp->Configure(pset);  
  return;
}

//------------------------------------------------
DEFINE_ART_SERVICE_INTERFACE_IMPL(spacecharge::SpaceChargeServiceStandard, spacecharge::SpaceChargeService)
