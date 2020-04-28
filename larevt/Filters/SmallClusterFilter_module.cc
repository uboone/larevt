////////////////////////////////////////////////////////////////////////
//
// \file SmallClusterFilter_module.cc
//
// \author corey.adams@yale.edu
//
// \brief Filter out events and only keep those with a few hits.
/*
	This filter is meant to search for radioactive decay products in "empty" frames.

	It will run on microboone, I claim, though it's really meant for argoneut.

	The algorithm is quite simple, it just makes a cut on the total number of hits (all
	planes combined) and also a hit on the number of hits in each individual plane.  Both
	of those numbers are parameters from the .fcl file filters.fcl.
	-Corey
*/
//
//
///////////////////////////////////////////////////////////////////////

// include the proper bit of the framework
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "larcore/Geometry/Geometry.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardata/Utilities/GeometryUtilities.h"
#include "lardataobj/RecoBase/Hit.h"

namespace cluster {

  class SmallClusterFilter : public art::EDFilter {

  public:
    /**METHODS global*/
    explicit SmallClusterFilter(fhicl::ParameterSet const& pset);
    void beginJob();
    /**Routine that finds the cluster and sets the dTdW of the 2D shower*/
    bool filter(art::Event& evt);

  private:
    void ClearandResizeVectors(unsigned int nHits);

    void GetPlaneAndTPC(art::Ptr<recob::Hit> a,
                        unsigned int& p,
                        unsigned int& cs,
                        unsigned int& t,
                        unsigned int& w);

    art::ServiceHandle<geo::Geometry const> geom; ///< handle to geometry service
    const detinfo::DetectorProperties* detp = lar::providerFrom<
      detinfo::DetectorPropertiesService>(); ///< const ptr to det properties data provider
    util::GeometryUtilities gser;            ///< geometry utilities
    std::vector<unsigned int> fNWires;       ///< Number of wires on each plane

    //input parameters
    std::string fHitFinderModuleLabel; ///< label of module making hits
    std::vector<int> fMaxHitsByPlane;  ///< maximum hits on each plane
    int fMaxTotalHits;                 ///< maximum number of hits allowed
    unsigned int fNPlanes;             ///< number of planes

    std::vector<std::vector<art::Ptr<recob::Hit>>>
      hitlistbyplane; ///< list of all hits on each plane

  }; // class SmallAngleFinder

}

cluster::SmallClusterFilter::SmallClusterFilter(fhicl::ParameterSet const& pset) : EDFilter{pset}
{
  fHitFinderModuleLabel = pset.get<std::string>("HitFinderModuleLabel");
  fMaxHitsByPlane = pset.get<std::vector<int>>("MaxHitsByPlane");
  fMaxTotalHits = pset.get<int>("MaxTotalHits");

  //fRadiusSizePar is used to exclude hits from a cluster outside of a certain size
  //fNHitsInClust ensures the clusters don't get too big
  //max hits by plane filters this event if the hits on that plane is too big
  //max hits total checks against the sum of all hits found.
}

//-----------------------------------------------

// ***************** //
void
cluster::SmallClusterFilter::beginJob()
{
  // this will not change on a run per run basis.
  fNPlanes = geom->Nplanes(); //get the number of planes in the TPC
}

// ************************************* //
// Clear and resize - exactly what it sounds like?
// Don't know why it takes the number of clusters...
void
cluster::SmallClusterFilter::ClearandResizeVectors(unsigned int /* nClusters */)
{

  ///////////////
  //   fMinWire.clear();
  //   fMaxWire.clear();
  //   fMinTime.clear();
  //   fMaxTime.clear();
  //
  //   fRMS_wire.clear();
  //   fRMS_time.clear();
  //
  //   mcwirevertex.resize(fNPlanes);  // wire coordinate of vertex for each plane
  //   mctimevertex.resize(fNPlanes);  // time coordinate of vertex for each plane
  //
  //   fRMS_wire.resize(fNPlanes);
  //   fRMS_time.resize(fNPlanes);
  //
  ////////////////
  hitlistbyplane.clear();
  hitlistbyplane.resize(fNPlanes);
  return;
}

// ***************** //
// This method actually makes the clusters.
bool
cluster::SmallClusterFilter::filter(art::Event& evt)
{
  //Check the size of the maxHitsByPlane vector against fNPlanes
  if (fMaxHitsByPlane.size() != fNPlanes) return false;

  /**Get Clusters*/

  //Get the hits for this event:
  art::Handle<std::vector<recob::Hit>> HitListHandle;
  evt.getByLabel(fHitFinderModuleLabel, HitListHandle);

  //A vector to hold hits, not yet filled:
  std::vector<art::Ptr<recob::Hit>> hitlist;

  //How many hits in this event?  Tell user:
  mf::LogVerbatim("SmallClusterFilter")
    << " ++++ Hitsreceived received " << HitListHandle->size() << " +++++ ";

  //Catch the case were there are no hits in the event:
  if (HitListHandle->size() == 0) {
    mf::LogWarning("SmallClusterFilter") << " no hits received! exiting ";
    return false;
  }
  if (HitListHandle->size() > (unsigned int)fMaxTotalHits) {
    mf::LogWarning("SmallClusterFinder") << "Not an empty event, exiting.";
    return false;
  }

  ClearandResizeVectors(HitListHandle->size());
  // resizing once cluster size is known.

  art::Ptr<recob::Hit> theHit;

  bool collFound = false;
  //add all of the hits to the hitlist, and sort them into hits by plane
  for (unsigned int iHit = 0; iHit < HitListHandle->size(); iHit++) {

    theHit = art::Ptr<recob::Hit>(HitListHandle, iHit);

    unsigned int p(0), w(0), t(0), cs(0); //c=channel, p=plane, w=wire, t=TPC
    GetPlaneAndTPC(theHit, p, cs, t, w);  //Find out what plane this hit is on.

    //Do a check to catch crazy hits:
    if (theHit->Integral() > 500) continue;

    //add this hit to the total list
    hitlist.push_back(theHit);

    //add this hit to the list specific to this plane
    hitlistbyplane[p].push_back(theHit);
    //hitlistleftover[p].push_back(theHit);
    //Just for searching for Ar39:

    if (theHit->SignalType() == geo::kCollection) collFound = true;

  } // End loop on hits.

  //Check against each plane:
  for (unsigned int i = 0; i < fNPlanes; i++) {
    if (hitlistbyplane[i].size() > (unsigned int)fMaxHitsByPlane[i]) return false;
  }

  //check that there is at least 1 hit on collection:
  if (!collFound) return false;

  mf::LogVerbatim("SmallClusterFilter")
    << "\nPassing with " << hitlistbyplane[fNPlanes - 1].size() << " hit(s) on collection.\n";

  return true;
}

// ******************************* //
// ******************************* //
void cluster::SmallClusterFilter::GetPlaneAndTPC(art::Ptr<recob::Hit> a, //the hit
                                                 unsigned int& p,        //plane
                                                 unsigned int& cs,       //cryostat
                                                 unsigned int& t,        //tpc
                                                 unsigned int& w)        //wire
{
  cs = a->WireID().Cryostat;
  p = a->WireID().Plane;
  t = a->WireID().TPC;
  w = a->WireID().Wire;
}

namespace cluster {

  DEFINE_ART_MODULE(SmallClusterFilter)

}
