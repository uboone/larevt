////////////////////////////////////////////////////////////////////////
//
// EmptyFilter class:
// Algorithm to produce event files with the
// blank events removed using only hit information.
//
// pagebri3@msu.edu
//
////////////////////////////////////////////////////////////////////////

/// Framework includes
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Principal/Event.h" 
#include "fhiclcpp/ParameterSet.h" 
#include "art/Framework/Principal/Handle.h" 
#include "canvas/Persistency/Common/Ptr.h" 
#include "canvas/Persistency/Common/PtrVector.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "messagefacility/MessageLogger/MessageLogger.h" 


// LArSoft includes"
#include "lardataobj/RecoBase/Hit.h"
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/WireGeo.h"

#include "TH1I.h"
#include "TH2I.h"
#include "TH2D.h"

#include <math.h>
#include <algorithm>
#include <iostream>
#include <fstream>


namespace filt {

  class EmptyFilter : public art::EDFilter  {

  public:

    explicit EmptyFilter(fhicl::ParameterSet const& );

    bool filter(art::Event& evt);
    void reconfigure(fhicl::ParameterSet const& p);
    void beginJob();


  private:

    std::string fHitsModuleLabel;
    double  fMinIonization;
    int fMinNumHits;
    TH1I * totHitHist;
    TH1I * selHitHist;
    TH1I * rejHitHist;
    TH2D * totIonSelHist;
    TH2D * totIonRejHist;
    TH1I * numEventHist;
    TH2I * resultTable;

  protected:

  }; // class EmptyFilter

  //-------------------------------------------------
  EmptyFilter::EmptyFilter(fhicl::ParameterSet const & pset)
    : EDFilter{pset}
  {
    this->reconfigure(pset);
  }

  //-------------------------------------------------
  void EmptyFilter::reconfigure(fhicl::ParameterSet const& p)
  {
    fHitsModuleLabel = p.get< std::string > ("HitsModuleLabel");
    fMinIonization =   p.get< double      > ("MinIonization");
    fMinNumHits =      p.get< int         > ("MinHits");
  }

  //-------------------------------------------------
  void EmptyFilter::beginJob()
  {
    art::ServiceHandle<art::TFileService const> tfs;
    totHitHist = tfs->make<TH1I>("totHitHist","Hit Number Per Event",750,0,1500);
    totIonSelHist = tfs->make<TH2D>("totIonSelHist","Ionization Per selected Event",500,0,20000,500,0,20000);
    totIonRejHist = tfs->make<TH2D>("totIonRejHist","Ionization Per rejected Event",500,0,20000,500,0,20000);
    selHitHist= tfs->make<TH1I>("selHitHist","Hit Number Per selected  Event",750, 0 ,1500);
    rejHitHist= tfs->make<TH1I>("rejHitHist","Hit Number Per rejected Event",750, 0 ,1500);
    numEventHist = tfs->make<TH1I>("numEventHist","Number of Events Processed and Selected",2,0,2);
    resultTable = tfs->make<TH2I>("resultTable","Event number is x axis, y axis bins 0=selected,1= hit num, 2= one plane empty, 3= too little ionization",40000,0,40000,4,0,4);

  }

  //-------------------------------------------------
  bool EmptyFilter::filter(art::Event &evt)
  {

    numEventHist->Fill(0);
    int failFlag = 0;
    double indIon(0.0), colIon(0.0);
    int event = evt.id().event();

    art::ServiceHandle<geo::Geometry const> geom;
    art::Handle< std::vector<recob::Hit> > hitHandle;
    evt.getByLabel(fHitsModuleLabel,hitHandle);
    art::PtrVector<recob::Hit> hitvec;
    for(unsigned int i = 0; i < hitHandle->size(); ++i){
      art::Ptr<recob::Hit> prod(hitHandle, i);
      hitvec.push_back(prod);
    }

    int numHits = hitvec.size();
    if( numHits> 0) {
      totHitHist->Fill(numHits);
      if(numHits < fMinNumHits) {
	mf::LogWarning("EmptyFilterModule") << "Too few hits: "<< numHits;
	failFlag=1;
      }
      if(failFlag==0) {
	//Check to see if either plane is empty
/* BB: This code section is broken.
	if(geom->Cryostat(cs).TPC(tpc).Plane(plane).SignalType() == geo::kInduction){
	  std::cout << "Induction empty." << std::endl;
	  failFlag=2;
	}
*/
	unsigned int j(0);
	//advances j to collection plane
	while(hitvec[j]->WireID().Plane == 0) {
	  indIon+=hitvec[j]->Integral();
	  j++;
	  if(j == hitvec.size()) {
	    failFlag=2;
	    mf::LogWarning("EmptyFilterModule") << "Collection empty.";
	  }
	}
	for(; j < hitvec.size(); j++){
	  colIon+=hitvec[j]->Integral();
	}
	double minIon=0;
	if((1.92*indIon)>colIon) minIon = colIon;
	else minIon=1.92*indIon;
	mf::LogWarning("EmptyFilterModule") <<"min ionization " << minIon
					    << " " << fMinIonization;
	if (minIon < fMinIonization) failFlag=3;
      }
    }
    else failFlag = 1;
    resultTable->Fill(event,failFlag);
    if(failFlag>0){
      totIonRejHist->Fill(indIon,colIon);
      rejHitHist->Fill(numHits);
      return  false;
    }
    numEventHist->Fill(1);
    totIonSelHist->Fill(indIon,colIon);
    selHitHist->Fill(numHits);

    return true;
  }


  DEFINE_ART_MODULE(EmptyFilter)

} //namespace filt
