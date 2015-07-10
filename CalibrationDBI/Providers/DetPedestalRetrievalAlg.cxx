#ifndef WEBDB_DETPEDESTALRETRIEVALALG_CXX
#define WEBDB_DETPEDESTALRETRIEVALALG_CXX

#include "DetPedestalRetrievalAlg.h"
#include "WebError.h"
#include "CalibrationDBI/IOVData/IOVDataConstants.h"

// art/LArSoft libraries
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "Geometry/Geometry.h"

namespace lariov {

  //constructors
  DetPedestalRetrievalAlg::DetPedestalRetrievalAlg(const std::string& foldername, 
      			      			   const std::string& url, 
			      			   const std::string& tag /*=""*/) : 
    DatabaseRetrievalAlg(foldername, url, tag),
    fDataSource(DataSource::Database),
    fDefaultColl(0),
    fDefaultInd(0) {
    
    fData.Clear();
    IOVTimeStamp tmp = IOVTimeStamp::MaxTimeStamp();
    tmp.SetStamp(tmp.Stamp()-1, tmp.SubStamp());
    fData.SetIoV(tmp, IOVTimeStamp::MaxTimeStamp());
  }
	
      
  DetPedestalRetrievalAlg::DetPedestalRetrievalAlg(fhicl::ParameterSet const& p) :
    DatabaseRetrievalAlg(p.get<fhicl::ParameterSet>("DatabaseRetrievalAlg")),
    fDefaultColl(0),
    fDefaultInd(0) {	
    
    this->Reconfigure(p);
  }
      
  void DetPedestalRetrievalAlg::Reconfigure(fhicl::ParameterSet const& p) {
    
    this->DatabaseRetrievalAlg::Reconfigure(p.get<fhicl::ParameterSet>("DatabaseRetrievalAlg"));
    fData.Clear();
    IOVTimeStamp tmp = IOVTimeStamp::MaxTimeStamp();
    tmp.SetStamp(tmp.Stamp()-1, tmp.SubStamp());
    fData.SetIoV(tmp, IOVTimeStamp::MaxTimeStamp());

    bool UseDB      = p.get<bool>("UseDB", false);
    bool UseFile   = p.get<bool>("UseFile", false);

    //priority:  (1) use db, (2) use table, (3) use defaults
    //If none are specified, use defaults
    if ( UseDB )      fDataSource = DataSource::Database;
    else if (UseFile) fDataSource = DataSource::File;
    else              fDataSource = DataSource::Default;

    if (fDataSource == DataSource::Default) {
      float default_mean     = p.get<float>("DefaultCollMean", 400.0);
      float default_rms      = p.get<float>("DefaultCollRms", 0.3);
      float default_mean_err = p.get<float>("DefaultMeanErr", 0.0);
      float default_rms_err  = p.get<float>("DefaultRmsErr", 0.0);

      fDefaultColl.SetPedMean(default_mean);
      fDefaultColl.SetPedMeanErr(default_mean_err);
      fDefaultColl.SetPedRms(default_rms);
      fDefaultColl.SetPedRmsErr(default_rms_err);
      
      default_mean     = p.get<float>("DefaultIndMean", 2048.0);
      default_rms      = p.get<float>("DefaultIndRms", 0.3);
      
      fDefaultInd.SetPedMean(default_mean);
      fDefaultInd.SetPedMeanErr(default_mean_err);
      fDefaultInd.SetPedRms(default_rms);
      fDefaultInd.SetPedRmsErr(default_rms_err);

    }
    else if (fDataSource == DataSource::File) {
      //need to implement
    }
  }


  bool DetPedestalRetrievalAlg::Update(std::uint64_t ts) {
    
    if (fDataSource != DataSource::Database) return false;
      
    if (!this->UpdateFolder(ts)) return false;

    //DBFolder was updated, so now update the Snapshot
    fData.Clear();
    fData.SetIoV(this->Begin(), this->End());

    std::vector<std::uint64_t> channels;
    fFolder->GetChannelList(channels);
    for (auto it = channels.begin(); it != channels.end(); ++it) {

      double mean, mean_err, rms, rms_err;
      fFolder->GetNamedChannelData(*it, "mean",     mean);
      fFolder->GetNamedChannelData(*it, "mean_err", mean_err);
      fFolder->GetNamedChannelData(*it, "rms",      rms);
      fFolder->GetNamedChannelData(*it, "rms_err",  rms_err);

      DetPedestal pd(*it);
      pd.SetPedMean( (float)mean );
      pd.SetPedMeanErr( (float)mean_err );
      pd.SetPedRms( (float)rms );
      pd.SetPedRmsErr( (float)rms_err );

      fData.AddOrReplaceRow(pd);
    }

    return true;

  }
  
  const DetPedestal& DetPedestalRetrievalAlg::Pedestal(std::uint64_t ch) const {  
    try {
      return fData.GetRow(ch);
    }
    catch(IOVDataError& e) {
      if (fDataSource == DataSource::Default) {
        art::ServiceHandle<geo::Geometry> geo;
      
        DetPedestal tmp_ped(ch);
	if (geo->SignalType(ch) == geo::kCollection) {
	  tmp_ped = fDefaultColl;
	}
	else if (geo->SignalType(ch) == geo::kInduction) {
	  tmp_ped = fDefaultInd;
	}
	else throw e;
	
	fData.AddOrReplaceRow(tmp_ped);
	return fData.GetRow(ch);
      }
      else throw e;
    }
  }
      
  float DetPedestalRetrievalAlg::PedMean(std::uint64_t ch) const {
    return this->Pedestal(ch).PedMean();
  }
  
  float DetPedestalRetrievalAlg::PedRms(std::uint64_t ch) const {
    return this->Pedestal(ch).PedRms();
  }
  
  float DetPedestalRetrievalAlg::PedMeanErr(std::uint64_t ch) const {
    return this->Pedestal(ch).PedMeanErr();
  }
  
  float DetPedestalRetrievalAlg::PedRmsErr(std::uint64_t ch) const {
    return this->Pedestal(ch).PedRmsErr();
  }



}//end namespace lariov
	
#endif
        
