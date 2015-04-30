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
    fUseDB(true),
    fUseFile(false),
    fUseDefault(false),
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

    fUseDB      = p.get<bool>("UseDB", false);
    fUseFile   = p.get<bool>("UseTable", false);
    fUseDefault = p.get<bool>("UseDefault", true);

    //priority:  (1) use db, (2) use table, (3) use defaults
    //If none are specified, use defaults
    if ( fUseDB ) {
      fUseFile = false;
      fUseDefault = false;
    }
    else if (fUseFile) fUseDefault = false;
    else if (!fUseDefault) fUseDefault = true;

    if (fUseDefault) {
      float default_mean     = p.get<float>("DefaultCollMean", 400.0);
      float default_rms      = p.get<float>("DefaultCollRms", 0.3);
      float default_mean_err = p.get<float>("DefaultMeanErr", 0.0);
      float default_rms_err  = p.get<float>("DefaultRmsErr", 0.0);

      fDefaultColl.SetPedMean(default_mean);
      fDefaultColl.SetPedMeanErr(default_mean_err);
      fDefaultColl.SetPedRms(default_rms);
      fDefaultColl.SetPedRmsErr(default_rms_err);
      
      default_mean     = p.get<float>("DefaultIndMean", 400.0);
      default_rms      = p.get<float>("DefaultIndRms", 0.3);
      
      fDefaultInd.SetPedMean(default_mean);
      fDefaultInd.SetPedMeanErr(default_mean_err);
      fDefaultInd.SetPedRms(default_rms);
      fDefaultInd.SetPedRmsErr(default_rms_err);

    }
    else if (fUseFile) {
      //need to implement
    }
  }

  bool DetPedestalRetrievalAlg::Update(const IOVTimeStamp& ts) {
    if (fUseDB && !fData.IsValid(ts)) {
      
      if (!this->DatabaseRetrievalAlg::Update(ts)) {
        throw WebError("DetPedestal DB cache update failed!");
      }
      
      fData.Clear();
      fData.SetIoV(this->Begin(), this->End());
     
      std::vector<unsigned int> channels;
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
    
    //Not using the DB or no need to update
    return false;
  }
  
  void DetPedestalRetrievalAlg::SetOneDefault(const DetPedestal& def) {
    if (fUseDefault) fData.AddOrReplaceRow(def);
  }
  
  const DetPedestal& DetPedestalRetrievalAlg::Pedestal(unsigned int ch) {  
    try {
      return fData.GetRow(ch);
    }
    catch(IOVDataError& e) {
      if (fUseDefault) {
        art::ServiceHandle<geo::Geometry> geo;
      
        DetPedestal tmp_ped(ch);
	if (geo->SignalType(ch) == geo::kCollection) {
	  tmp_ped.SetPedMean( fDefaultColl.PedMean() );
          tmp_ped.SetPedMeanErr( fDefaultColl.PedMeanErr() );
	  tmp_ped.SetPedRms( fDefaultColl.PedRms() );
	  tmp_ped.SetPedRmsErr( fDefaultColl.PedRmsErr() );
	}
	else if (geo->SignalType(ch) == geo::kInduction) {
	  tmp_ped.SetPedMean( fDefaultInd.PedMean() );
          tmp_ped.SetPedMeanErr( fDefaultInd.PedMeanErr() );
          tmp_ped.SetPedRms( fDefaultInd.PedRms() );
          tmp_ped.SetPedRmsErr( fDefaultInd.PedRmsErr() );
	}
	else throw e;
	
	fData.AddOrReplaceRow(tmp_ped);
	return fData.GetRow(ch);
      }
      else throw e;
    }
  }
      
  float DetPedestalRetrievalAlg::PedMean(unsigned int ch) {
    return this->Pedestal(ch).PedMean();
  }
  
  float DetPedestalRetrievalAlg::PedRms(unsigned int ch) {
    return this->Pedestal(ch).PedRms();
  }
  
  float DetPedestalRetrievalAlg::PedMeanErr(unsigned int ch) {
    return this->Pedestal(ch).PedMeanErr();
  }
  
  float DetPedestalRetrievalAlg::PedRmsErr(unsigned int ch) {
    return this->Pedestal(ch).PedRmsErr();
  }



}//end namespace lariov
	
#endif
        
