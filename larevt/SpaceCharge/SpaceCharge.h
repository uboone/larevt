////////////////////////////////////////////////////////////////////////
// \file SpaceCharge.h
//
// \brief pure virtual base interface for space charge distortions
//
// \author mrmooney@bnl.gov
// 
////////////////////////////////////////////////////////////////////////
#ifndef SPACECHARGE_SPACECHARGE_H
#define SPACECHARGE_SPACECHARGE_H

// C/C++ standard libraries
#include <vector>


namespace spacecharge{
  
  class SpaceCharge {
    public:

      SpaceCharge(const SpaceCharge &) = delete;
      SpaceCharge(SpaceCharge &&) = delete;
      SpaceCharge& operator = (const SpaceCharge &) = delete;
      SpaceCharge& operator = (SpaceCharge &&) = delete;
      virtual ~SpaceCharge() = default;

      virtual bool EnableSimSpatialSCE() const = 0;
      virtual bool EnableSimEfieldSCE() const = 0;
      virtual bool EnableCorrSCE() const = 0;
      virtual std::vector<double> GetPosOffsets(double xVal, double yVal, double zVal) const = 0;
      virtual std::vector<double> GetEfieldOffsets(double xVal, double yVal, double zVal) const = 0;

    protected:

      SpaceCharge() = default;
      
    }; // class SpaceCharge
} //namespace spacecharge

#endif // SPACECHARGE_SPACECHARGE_H
