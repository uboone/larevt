///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file  SpaceCharge.h
/// \brief Implements offset in reconstructed position of ionization electrons due to SCE (Space Charge Effect)
///
/// \author  Michael Mooney (mooney@bnl.gov)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// This service provides tools to calculate the offset in reconstructed position of ioniziation electrons 
/// deposited within the TPC due to the space charge effect, which is utilized in the LArVoxelReadout class.  
/// The offsets in X (T), Y, and Z are calculated using an external program and it is expected that they will
/// be provided in a variety of different forms (parametric, matrix, interpolation engine, etc.) which can be
/// selected between via the ParameterSet.  For now the offsets are encoded in parametric form using a set of
/// TGraphs stored in a ROOT file.

/// This service is a temporary "dirty" hack and will eventually be superceded with a database solution located
/// elsewhere in the framework.

#ifndef SpaceCharge_h
#define SpaceCharge_h

#include <vector>
#include <string>

#include "TGraph.h"
#include "TF1.h"
#include "TFile.h"

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace spacecharge {

  class SpaceCharge 
  {
  public:

    SpaceCharge(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
    ~SpaceCharge();

    void reconfigure(fhicl::ParameterSet const& pset);

    bool EnableSimulationSCE() const;
    bool EnableCorrectionsSCE() const;
    std::vector<double> GetPosOffsets(double xVal, double yVal, double zVal) const;
    std::vector<double> GetPosOffsetsParametric(double xVal, double yVal, double zVal) const;
    double GetOnePosOffsetParametric(double xVal, double yVal, double zVal, std::string axis) const;
    double TransformX(double xVal) const;
    double TransformY(double yVal) const;
    double TransformZ(double zVal) const;

  private:

    bool fEnableSimulationSCE; // ONLY SIMULATION FOR NOW
    bool fEnableCorrectionsSCE; // CORRECTIONS TO BE IMPLEMENTED

    std::string fRepresentationType;
    std::string fInputFilename;

    TGraph **g1_x = new TGraph*[7];
    TGraph **g2_x = new TGraph*[7];
    TGraph **g3_x = new TGraph*[7];
    TGraph **g4_x = new TGraph*[7];
    TGraph **g5_x = new TGraph*[7];

    TGraph **g1_y = new TGraph*[7];
    TGraph **g2_y = new TGraph*[7];
    TGraph **g3_y = new TGraph*[7];
    TGraph **g4_y = new TGraph*[7];
    TGraph **g5_y = new TGraph*[7];
    TGraph **g6_y = new TGraph*[7];

    TGraph **g1_z = new TGraph*[7];
    TGraph **g2_z = new TGraph*[7];
    TGraph **g3_z = new TGraph*[7];
    TGraph **g4_z = new TGraph*[7];

    TF1 *f1_x = new TF1("f1_x","pol6");
    TF1 *f2_x = new TF1("f2_x","pol6");
    TF1 *f3_x = new TF1("f3_x","pol6");
    TF1 *f4_x = new TF1("f4_x","pol6");
    TF1 *f5_x = new TF1("f5_x","pol6");
    TF1 *fFinal_x = new TF1("fFinal_x","pol4");

    TF1 *f1_y = new TF1("f1_y","pol5");
    TF1 *f2_y = new TF1("f2_y","pol5");
    TF1 *f3_y = new TF1("f3_y","pol5");
    TF1 *f4_y = new TF1("f4_y","pol5");
    TF1 *f5_y = new TF1("f5_y","pol5");
    TF1 *f6_y = new TF1("f6_y","pol5");
    TF1 *fFinal_y = new TF1("fFinal_y","pol5");

    TF1 *f1_z = new TF1("f1_z","pol4");
    TF1 *f2_z = new TF1("f2_z","pol4");
    TF1 *f3_z = new TF1("f3_z","pol4");
    TF1 *f4_z = new TF1("f4_z","pol4");
    TF1 *fFinal_z = new TF1("fFinal_z","pol3");
  };

} // namespace spacecharge

DECLARE_ART_SERVICE(spacecharge::SpaceCharge, LEGACY)
#endif // SpaceCharge_h
