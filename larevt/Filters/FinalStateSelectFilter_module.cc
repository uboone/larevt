////////////////////////////////////////////////////////////////////////
//
// FinalStateSelectFilter class:
// Algoritm to produce a filtered event file having
// events with user-defined final state particles 
// Now with more control! 
//
// Joseph Zennamo, UChicago, 2016 (almost 2017 really, happy holidays!)
// jzennamo@uchicago.edu
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
    std::vector<int> fPDG;     /// List of particle PDGs we want to keep 
    std::vector<int> fPDGCount;/// List of N's for the particle PDGs  
    std::vector<bool> fPDGCountExclusive;/// If true:  Only select events with EXACTLY  that number of particles
                                         /// If false:      select events with AT LEAST that number of particles  
    std::vector<int> fStatusCode;
    TH1D* fSelectedEvents;
    TH1D* fTotalEvents;

  protected: 

    bool isSubset(std::vector<int>& a, std::vector<int>& aN, std::vector<bool> aNex, std::vector<int>& b, bool IsInclusive);
    
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
    fGenieModuleLabel  = p.get< std::string      >("GenieModuleLabel");
    fPDG               = p.get< std::vector<int> >("PDG");
    fInclusive         = p.get< bool >("isInclusive");        
    fPDGCount          = p.get< std::vector<int> >("PDGCount");
    fPDGCountExclusive = p.get< std::vector<bool> >("PDGCountExclusivity");


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
    
    //Guard agaist lazy people not including this 
    if(fPDGCount.size() != 0 || fPDGCountExclusive.size() != 0){
      //Make sure we have all that we need
      if(fPDGCount.size() != fPDG.size() ||
	 fPDGCountExclusive.size() != fPDGCount.size() || 
	 fPDGCountExclusive.size() != fPDG.size()){
	std::cout << "PDG, PDGCount, and PDGCountExclusivity Vector size mismatch." << std::endl;
	return false;
      }
    }
   
    //get a vector of final state particles   
    for(int i = 0; i < mc->NParticles(); ++i){
      simb::MCParticle part(mc->GetParticle(i));
      if(part.StatusCode()== 1)
	finalstateparticles.push_back(part.PdgCode());
    }

    if(isSubset(fPDG, fPDGCount, fPDGCountExclusive, finalstateparticles,fInclusive)){
      fSelectedEvents->Fill(1);
      std::cout << "this is a selected event" << std::endl;
    }

    // returns true if the user-defined fPDG exist(s) in the final state particles
    return isSubset(fPDG, fPDGCount, fPDGCountExclusive, finalstateparticles, fInclusive); 

  } // bool  
  //} // namespace
    
  //------------------------------------------------   
  
  bool FinalStateSelectFilter::isSubset(std::vector<int>& a, std::vector<int>& aN, std::vector<bool> aNex, std::vector<int>& b, bool IsInclusive)
  {
    bool end;

    // check if the analyzer wants an includive final state
    if(IsInclusive){
      // Sweet, OK, so things get a bit annoying because we want this 
      // to be fairly generic. What I am trying to do is let analyzers 
      // create complicated inclusive final states with exclusive or inclusive 
      // numbers of particles. This requires this "exclusivity" vector
      //
      // an example final state would be NC 1 photon inclusive, 
      // all the analyzer wants is events with EXACTLY 1 photon in the final 
      // state. 

      // Create a vector for the exclusivity check;
      std::vector< int > counts(a.size());

      //Particle Checking
      //  Iterate through all the wanted PDGs (vector a) and compare
      //  to what we have in the final state (vecotr b). 
      for(unsigned int i = 0; i < a.size(); i++){
	if(std::find(b.begin(), b.end(), a[i]) != b.end()){
	  counts[i] += 1;
	}	       	
      }
      
      //Verify we have the final state we want
      //  Iterate through the full vector and make sure we have at least
      //  as many particles we wanted, also check the exclusivity
      if(aN.size() != 0){
	for(unsigned int i = 0; i < a.size(); i++){
	  if(counts[i] <= aN[i]){
	    end = false;
	    break;
	  }
	  if(aNex[i] = true && counts[i] != aN[i]){
	    end = false;
	    break;
	  }
	}
      }
      // If the count vector size is zero then the user was being lazy...
      // but no one is perfect so I will try to accommodate this. I make the 
      // assumption that they only are asking for final state which contain
      // ANY NUMBER of the requested PDG. If this isn't true then the user
      // should specify all the arguments
      else{

	for(unsigned int i = 0; i < a.size(); i++){
	  if(std::find(b.begin(), b.end(), a[i]) == b.end()){
	    end = false;
	    break;
	  }
	}		
      }
      // if things didn't break then things went well.
      end = true;
    }
    /// else it is exclusive
    else{
      //Exclusive filter is fine too, though I am less excited.

      std::vector< int > counts(a.size());

      //Particle Checking
      //  Iterate through all the wanted PDGs (vector a) and compare
      //  to what we have in the final state (vecotr b). Need to count
      //  how many matches we since this is supposed to be the exclusive
      //  portion of the code. 
      for(unsigned int i = 0; i < a.size(); i++){
	
	//Search in b vector is there is the wanted PDG!
	//   If we find it iterate the counter so that we can
	//   check against the wanted counts later, BUT if that
	//   PDG we want isn't in the final state (vector b) then
	//   we want to ditch the event! 
	if(std::find(b.begin(), b.end(), a[i]) != b.end()){
	  counts[i] += 1;
	}
	// else, ditch the event
	else{
	  return false;
	}
      }
      
      
      // Exclusive Check
      //   iterate through the count vectors and make sure they 
      //   are the same, I am sure there is a smart computer-ing
      //   way to do this but I wrote it this way, if it turns out
      //   to be too slow we can address this. 
      bool AllGood = true; 
      for(unsigned int i = 0; i < aN.size(); i++){	
	if(aN[i] != counts[i]){
	  AllGood = false;
	  break;
	}
      }
   
      //Make the final return what I think the final exclusive check is 
      end = AllGood; 
    }          
    return end;
  }
}

//--------------------------------------------------

namespace filt {

  DEFINE_ART_MODULE(FinalStateSelectFilter)

} //namespace filt

#endif // FINALSTATEPARTICLEFILTER_H
