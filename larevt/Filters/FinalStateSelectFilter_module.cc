////////////////////////////////////////////////////////////////////////
//
// FinalStateSelectFilter class:
// Algoritm to produce a filtered event file having
// events with user-defined final state particles 
//
// saima@ksu.edu
//
////////////////////////////////////////////////////////////////////////
#ifndef FINALSTATESELECTFILTER_H
#define FINALSTATESELECTFILTER_H

//Framework Includes
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h" 
#include "art/Framework/Principal/Event.h" 
#include "fhiclcpp/ParameterSet.h" 
#include "art/Framework/Principal/Handle.h" 
#include "canvas/Persistency/Common/Ptr.h" 
#include "canvas/Persistency/Common/PtrVector.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art/Framework/Services/Optional/TFileService.h" 
#include "art/Framework/Services/Optional/TFileDirectory.h" 
#include "messagefacility/MessageLogger/MessageLogger.h" 


//Larsoft Includes
#include "nusimdata/SimulationBase/MCNeutrino.h"
#include "nusimdata/SimulationBase/MCTruth.h"

#include "TH1.h"
#include "TH2.h"

namespace filt {

  class FinalStateSelectFilter : public art::EDFilter  {
    
  public:
    
    explicit FinalStateSelectFilter(fhicl::ParameterSet const& ); 
    virtual ~FinalStateSelectFilter();
         
    
    bool filter(art::Event& evt);
    void reconfigure(fhicl::ParameterSet const& p);
    void beginJob();
   

  private: 
 
    std::string fGenieModuleLabel;
    bool fInclusive;      /// Returns events which contain AT LEAST the listed particles
    bool fChargedCurrent; /// Returns events which contain CC interactions
    bool fNeutralCurrent; /// Returns events which contain NC interactions
    std::vector<int> fPDG;     /// List of particle PDGs we want to keep 
    bool fCheckCount;     /// Returns events which contain N of the particles with the PDG
    std::vector<int> fPDGCount;/// List of N's for the particle PDGs  
    std::vector<int> fStatusCode;
    TH1D* fSelectedEvents;
    TH1D* fTotalEvents;

  protected: 

    bool isSubset(std::vector<int>& a, std::vector<int>& b, bool IsInclusive);
    
  }; // class FinalStateSelectFilter

}

namespace filt{

  //-------------------------------------------------
  FinalStateSelectFilter::FinalStateSelectFilter(fhicl::ParameterSet const & pset)  
  {   
    this->reconfigure(pset);
  }

  //-------------------------------------------------
  FinalStateSelectFilter::~FinalStateSelectFilter()
  {
  }
  
  //-------------------------------------------------
  void FinalStateSelectFilter::reconfigure(fhicl::ParameterSet const& p)
  {
    fGenieModuleLabel = p.get< std::string      >("GenieModuleLabel");
    fPDG              = p.get< std::vector<int> >("PDG");
    fInclusive        = p.get< bool >("isInclusive");        
    fChargedCurrent   = p.get< bool >("isChargedCurrent");        
    fNeutralCurrent   = p.get< bool >("isNeutralCurrent");        
    fCheckCount       = p.get< bool >("CheckCount"); 
    fPDGCount         = p.get< std::vector<int> >("PDGCount");

  } 

  //-------------------------------------------------
  void FinalStateSelectFilter::beginJob()
  {
    art::ServiceHandle<art::TFileService> tfs;
    fSelectedEvents = tfs->make<TH1D>("fSelectedEvents", "Number of Selected Events", 3, 0, 3); //counts the number of selected events 
    fTotalEvents = tfs->make<TH1D>("fTotalEvents", "Total Events", 3, 0, 3); //counts the initial number of events in the unfiltered root input file
  }

  //-------------------------------------------------
  bool FinalStateSelectFilter::filter(art::Event &evt)
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

    if(isSubset(fPDG, finalstateparticles,fInclusive)){
      fSelectedEvents->Fill(1);
      std::cout << "this is a selected event" << std::endl;
    }

    return isSubset(fPDG, finalstateparticles, fInclusive); // returns true if the user-defined fPDG exist(s) in the final state particles

  } // bool  
  //} // namespace
    
  //------------------------------------------------   
  
  bool FinalStateSelectFilter::isSubset(std::vector<int>& a, std::vector<int>& b, bool IsInclusive)
  {
    bool end;
    if(IsInclusive){
      end = true;
    }
    /// else it is exclusive
    else{
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
    return end;
  }
}

//--------------------------------------------------

namespace filt {

  DEFINE_ART_MODULE(FinalStateSelectFilter)

} //namespace filt

#endif // FINALSTATEPARTICLEFILTER_H
