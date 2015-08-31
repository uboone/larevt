#ifndef SIOVPMTGAINPROVIDER_CXX
#define SIOVPMTGAINPROVIDER_CXX

#include "SIOVPmtGainProvider.h"
#include "WebError.h"
#include "CalibrationDBI/IOVData/IOVDataConstants.h"

// art/LArSoft libraries
#include "cetlib/exception.h"

namespace lariov {

  //constructor      
  SIOVPmtGainProvider::SIOVPmtGainProvider(fhicl::ParameterSet const& p) :
    DatabaseRetrievalAlg(p.get<fhicl::ParameterSet>("DatabaseRetrievalAlg")),
    fDefault(0) {	
    
    this->Reconfigure(p);
  }
      
  void SIOVPmtGainProvider::Reconfigure(fhicl::ParameterSet const& p) {
    
    this->DatabaseRetrievalAlg::Reconfigure(p.get<fhicl::ParameterSet>("DatabaseRetrievalAlg"));
    fData.Clear();
    IOVTimeStamp tmp = IOVTimeStamp::MaxTimeStamp();
    tmp.SetStamp(tmp.Stamp()-1, tmp.SubStamp());
    fData.SetIoV(tmp, IOVTimeStamp::MaxTimeStamp());

    bool UseDB      = p.get<bool>("UseDB", false);
    bool UseFile    = p.get<bool>("UseFile", false);

    //priority:  (1) use db, (2) use table, (3) use defaults
    //If none are specified, use defaults
    if ( UseDB )      fDataSource = DataSource::Database;
    else if (UseFile) fDataSource = DataSource::File;
    else              fDataSource = DataSource::Default;

    if (fDataSource == DataSource::Default) {
      float default_height     = p.get<float>("DefaultSPEHeight");
      float default_height_err = p.get<float>("DefaultSPEHeightErr");
      float default_width      = p.get<float>("DefaultSPEWidth");
      float default_width_err  = p.get<float>("DefaultSPEWidthErr");
      float default_area       = p.get<float>("DefaultSPEArea");
      float default_area_err   = p.get<float>("DefaultSPEAreaErr");

      fDefault.SetSpeHeight(default_height);
      fDefault.SetSpeHeightErr(default_height_err);
      fDefault.SetSpeWidth(default_width);
      fDefault.SetSpeWidthErr(default_width_err);
      fDefault.SetSpeArea(default_area);
      fDefault.SetSpeAreaErr(default_area_err);      
    }
    else if (fDataSource == DataSource::File) {
      throw cet::exception("SIOVPmtGainProvider")
        << "SIOVPmtGainProvider: input from file not implemented yet\n";
      //need to implement
    }
  }


  bool SIOVPmtGainProvider::Update(DBTimeStamp_t ts) {
    
    if (fDataSource != DataSource::Database) return false;
      
    if (!this->UpdateFolder(ts)) return false;

    //DBFolder was updated, so now update the Snapshot
    fData.Clear();
    fData.SetIoV(this->Begin(), this->End());

    std::vector<DBChannelID_t> channels;
    fFolder->GetChannelList(channels);
    for (auto it = channels.begin(); it != channels.end(); ++it) {

      double height, height_err, width, width_err, area, area_err;
      fFolder->GetNamedChannelData(*it, "spe_height",     height);
      fFolder->GetNamedChannelData(*it, "spe_height_err", height_err);
      fFolder->GetNamedChannelData(*it, "spe_width",      width);
      fFolder->GetNamedChannelData(*it, "spe_width_err",  width_err);
      fFolder->GetNamedChannelData(*it, "spe_area",       area);
      fFolder->GetNamedChannelData(*it, "spe_area_err",   area_err);      

      PmtGain pg(*it);
      pg.SetSpeHeight( (float)height );
      pg.SetSpeHeightErr( (float)height_err );
      pg.SetSpeWidth( (float)width );
      pg.SetSpeWidthErr( (float)width_err );
      pg.SetSpeArea( (float)area );
      pg.SetSpeAreaErr( (float)area_err );

      fData.AddOrReplaceRow(pg);
    }

    return true;

  }
  
  const PmtGain& SIOVPmtGainProvider::ChannelInfo(DBChannelID_t ch) const { 
    if (fDataSource == DataSource::Default) {
      fDefault.SetChannel(ch);
      return fDefault;
    }
    else return fData.GetRow(ch);
  }
      
  float SIOVPmtGainProvider::SpeHeight(DBChannelID_t ch) const {
    return this->ChannelInfo(ch).SpeHeight();
  }
  
  float SIOVPmtGainProvider::SpeHeightErr(DBChannelID_t ch) const {
    return this->ChannelInfo(ch).SpeHeightErr();
  }
  
  float SIOVPmtGainProvider::SpeWidth(DBChannelID_t ch) const {
    return this->ChannelInfo(ch).SpeWidth();
  }
  
  float SIOVPmtGainProvider::SpeWidthErr(DBChannelID_t ch) const {
    return this->ChannelInfo(ch).SpeWidthErr();
  }

  float SIOVPmtGainProvider::SpeArea(DBChannelID_t ch) const {
    return this->ChannelInfo(ch).SpeArea();
  }
  
  float SIOVPmtGainProvider::SpeAreaErr(DBChannelID_t ch) const {
    return this->ChannelInfo(ch).SpeAreaErr();
  }

}//end namespace lariov
	
#endif
        
