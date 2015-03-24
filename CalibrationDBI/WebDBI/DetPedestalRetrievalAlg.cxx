#ifndef WEBDB_DETPEDESTALRETRIEVALALG_CXX
#define WEBDB_DETPEDESTALRETRIEVALALG_CXX

#include "DetPedestalRetrievalAlg.h"
#include "WebError.h"
#include "CalibrationDBI/IOVData/IOVDataConstants.h"

namespace lariov {

  //constructors
  DetPedestalRetrievalAlg::DetPedestalRetrievalAlg(const std::string& foldername, 
      			      			   const std::string& url, 
			      			   const std::string& tag /*=""*/) : 
    DatabaseRetrievalAlg(foldername, url, tag),
    fUseDB(true),
    fUseFile(false),
    fUseDefault(false),
    fDefault(0) {
    
    fData.clear();
    IOVTimeStamp tmp(kMAX_TIME.Stamp()-1, kMAX_TIME.SubStamp());
    fData.SetIoV(tmp, kMAX_TIME);
  }
	
      
  DetPedestalRetrievalAlg::DetPedestalRetrievalAlg(fhicl::ParameterSet const& p) :
    DatabaseRetrievalAlg(p.get<fhicl::ParameterSet>("DatabaseRetrievalAlg")),
    fDefault(0) {	
    
    this->Reconfigure(p);
  }
      
  void DetPedestalRetrievalAlg::Reconfigure(fhicl::ParameterSet const& p) {
    
    this->DatabaseRetrievalAlg::Reconfigure(p.get<fhicl::ParameterSet>("DatabaseRetrievalAlg"));
    fData.clear();
    IOVTimeStamp tmp(kMAX_TIME.Stamp()-1, kMAX_TIME.SubStamp());
    fData.SetIoV(tmp, kMAX_TIME);

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
      float default_mean     = p.get<float>("DefaultMean", 400.0);
      float default_rms      = p.get<float>("DefaultRms", 0.3);
      float default_mean_err = p.get<float>("DefaultMeanErr", 0.1);
      float default_rms_err  = p.get<float>("DefaultRmsErr", 0.03);

      fDefault.SetPedMean(default_mean);
      fDefault.SetPedMeanErr(default_mean_err);
      fDefault.SetPedRms(default_rms);
      fDefault.SetPedRmsErr(default_rms_err);
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
    fData.AddOrReplaceRow(def);
  }

  const DetPedestal& DetPedestalRetrievalAlg::Pedestal(unsigned int ch) {  
    try {
      return fData.GetRow(ch);
    }
    catch(IOVDataError& e) {
      if (fUseDefault) {
        DetPedestal tmp_ped(ch);
	tmp_ped.SetPedMean( fDefault.PedMean() );
        tmp_ped.SetPedMeanErr( fDefault.PedMeanErr() );
	tmp_ped.SetPedRms( fDefault.PedRms() );
	tmp_ped.SetPedRmsErr( fDefault.PedRmsErr() );
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
        
