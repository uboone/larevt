////////////////////////////////////////////////////////////////////////
//
// FinalStateParticleFilter class:
// Algoritm to produce a filtered event file having
// events with user-defined final state particles
//
// saima@ksu.edu
//
////////////////////////////////////////////////////////////////////////

//Framework Includes
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h" 
#include "art/Framework/Principal/Event.h" 
#include "fhiclcpp/ParameterSet.h" 
#include "art/Framework/Principal/Handle.h" 
#include "canvas/Persistency/Common/Ptr.h" 
#include "canvas/Persistency/Common/PtrVector.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "messagefacility/MessageLogger/MessageLogger.h" 


//Larsoft Includes
#include "nusimdata/SimulationBase/MCNeutrino.h"
#include "nusimdata/SimulationBase/MCTruth.h"

#include "TH1.h"
#include "TH2.h"

namespace filt {

  class FinalStateParticleFilter : public art::EDFilter  {

  public:

    explicit FinalStateParticleFilter(fhicl::ParameterSet const& );

    bool filter(art::Event& evt);
    void reconfigure(fhicl::ParameterSet const& p);
    void beginJob();


  private:

    std::string fGenieModuleLabel;
    std::vector<int> fPDG;
    std::vector<int> fStatusCode;
    TH1D* fSelectedEvents;
    TH1D* fTotalEvents;

    bool isSubset(std::vector<int>& a, std::vector<int>& b);

  }; // class FinalStateParticleFilter

}

namespace filt{

  //-------------------------------------------------
  FinalStateParticleFilter::FinalStateParticleFilter(fhicl::ParameterSet const & pset)
    : EDFilter{pset}
  {
    this->reconfigure(pset);
  }

  //-------------------------------------------------
  void FinalStateParticleFilter::reconfigure(fhicl::ParameterSet const& p)
  {
    fGenieModuleLabel = p.get< std::string      >("GenieModuleLabel");
    fPDG              = p.get< std::vector<int> >("PDG");
  }

  //-------------------------------------------------
  void FinalStateParticleFilter::beginJob()
  {
    art::ServiceHandle<art::TFileService const> tfs;
    fSelectedEvents = tfs->make<TH1D>("fSelectedEvents", "Number of Selected Events", 3, 0, 3); //counts the number of selected events
    fTotalEvents = tfs->make<TH1D>("fTotalEvents", "Total Events", 3, 0, 3); //counts the initial number of events in the unfiltered root input file
  }

  //-------------------------------------------------
  bool FinalStateParticleFilter::filter(art::Event &evt)
  {

    //const TDatabasePDG* databasePDG = TDatabasePDG::Instance();

    art::Handle< std::vector<simb::MCTruth> > mclist;
    evt.getByLabel(fGenieModuleLabel,mclist);
    art::Ptr<simb::MCTruth> mc(mclist,0);

    fTotalEvents->Fill(1);

    std::vector<int> finalstateparticles;

    //get a vector of final state particles
    for(int i = 0; i < mc->NParticles(); ++i){
      simb::MCParticle part(mc->GetParticle(i));
      if(part.StatusCode()== 1)
	finalstateparticles.push_back(part.PdgCode());
    }

    if(isSubset(fPDG, finalstateparticles)){
      fSelectedEvents->Fill(1);
      std::cout << "this is a selected event" << std::endl;
    }

    return isSubset(fPDG, finalstateparticles); // returns true if the user-defined fPDG exist(s) in the final state particles

  } // bool
  //} // namespace

//------------------------------------------------

bool FinalStateParticleFilter::isSubset(std::vector<int>& a, std::vector<int>& b)
{
  for (std::vector<int>::iterator i = a.begin(); i != a.end(); i++){
    bool found = false;
    for (std::vector<int>::iterator j = b.begin(); j != b.end(); j++){
      if (*i == *j){
	found = true;
	break;
      }
    }

    if (!found){
      return false;
    }
  }
  return true;
}
}

//--------------------------------------------------

namespace filt {

  DEFINE_ART_MODULE(FinalStateParticleFilter)

} //namespace filt
