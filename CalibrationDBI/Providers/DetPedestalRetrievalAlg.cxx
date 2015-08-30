#ifndef WEBDB_DETPEDESTALRETRIEVALALG_CXX
#define WEBDB_DETPEDESTALRETRIEVALALG_CXX

#include "DetPedestalRetrievalAlg.h"
#include "WebError.h"
#include "CalibrationDBI/IOVData/IOVDataConstants.h"

// art/LArSoft libraries
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "Geometry/Geometry.h"
#include "cetlib/exception.h"

namespace lariov {

  //constructors
  DetPedestalRetrievalAlg::DetPedestalRetrievalAlg(const std::string& foldername, 
      			      			   const std::string& url, 
			      			   const std::string& tag /*=""*/) : 
    DatabaseRetrievalAlg(foldername, url, tag),
    fDataSource(DataSource::Database) {
    
    fData.Clear();
    IOVTimeStamp tmp = IOVTimeStamp::MaxTimeStamp();
    tmp.SetStamp(tmp.Stamp()-1, tmp.SubStamp());
    fData.SetIoV(tmp, IOVTimeStamp::MaxTimeStamp());
  }
	
      
  DetPedestalRetrievalAlg::DetPedestalRetrievalAlg(fhicl::ParameterSet const& p) :
    DatabaseRetrievalAlg(p.get<fhicl::ParameterSet>("DatabaseRetrievalAlg")) {	
    
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
      float default_collmean     = p.get<float>("DefaultCollMean", 400.0);
      float default_collrms      = p.get<float>("DefaultCollRms", 0.3);
      float default_mean_err     = p.get<float>("DefaultMeanErr", 0.0);
      float default_rms_err      = p.get<float>("DefaultRmsErr", 0.0);
      float default_indmean      = p.get<float>("DefaultIndMean", 2048.0);
      float default_indrms       = p.get<float>("DefaultIndRms", 0.3);
      
      DetPedestal DefaultColl(0);
      DetPedestal DefaultInd(0);
      
      DefaultColl.SetPedMean(default_collmean);
      DefaultColl.SetPedMeanErr(default_mean_err);
      DefaultColl.SetPedRms(default_collrms);
      DefaultColl.SetPedRmsErr(default_rms_err);
      
      DefaultInd.SetPedMean(default_indmean);
      DefaultInd.SetPedMeanErr(default_mean_err);
      DefaultInd.SetPedRms(default_indrms);
      DefaultInd.SetPedRmsErr(default_rms_err);
      
      art::ServiceHandle<geo::Geometry> geo;
      geo::wire_id_iterator itW = geo->begin_wire_id();
      for ( ; itW != geo->end_wire_id(); ++itW) {
        DBChannelID_t ch = geo->PlaneWireToChannel(*itW);
      
        if (geo->SignalType(ch) == geo::kCollection) {
	  DefaultColl.SetChannel(ch);
	  fData.AddOrReplaceRow(DefaultColl);
	}
	else if (geo->SignalType(ch) == geo::kInduction) {
	  DefaultInd.SetChannel(ch);
	  fData.AddOrReplaceRow(DefaultInd);
	}
	else throw IOVDataError("Wire type is not collection or induction!");
      }
    }
    else if (fDataSource == DataSource::File) {
      throw cet::exception("DetPedestalRetrievalAlg")
        << "DetPedestalRetrievalAlg: input from file not implemented yet\n";
      //need to implement
    }
  }


  bool DetPedestalRetrievalAlg::Update(DBTimeStamp_t ts) {
    
    if (fDataSource != DataSource::Database) return false;
      
    if (!this->UpdateFolder(ts)) return false;

    //DBFolder was updated, so now update the Snapshot
    fData.Clear();
    fData.SetIoV(this->Begin(), this->End());

    std::vector<DBChannelID_t> channels;
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
  
  const DetPedestal& DetPedestalRetrievalAlg::Pedestal(DBChannelID_t ch) const {     
    return fData.GetRow(ch);
  }
      
  float DetPedestalRetrievalAlg::PedMean(DBChannelID_t ch) const {
    return this->Pedestal(ch).PedMean();
  }
  
  float DetPedestalRetrievalAlg::PedRms(DBChannelID_t ch) const {
    return this->Pedestal(ch).PedRms();
  }
  
  float DetPedestalRetrievalAlg::PedMeanErr(DBChannelID_t ch) const {
    return this->Pedestal(ch).PedMeanErr();
  }
  
  float DetPedestalRetrievalAlg::PedRmsErr(DBChannelID_t ch) const {
    return this->Pedestal(ch).PedRmsErr();
  }



}//end namespace lariov
	
#endif
        
