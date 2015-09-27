/**
 * @file   SIOVChannelStatusProvider.cxx
 * @brief  Channel quality provider with information from configuration file
 * @author Brandon Eberly (eberly@fnal.gov)
 * @date   August 24, 2015
 * @see    SIOVChannelStatusProvider.h
 */

#ifndef SIOVCHANNELSTATUSPROVIDER_CXX
#define SIOVCHANNELSTATUSPROVIDER_CXX 1

// Our header
#include "SIOVChannelStatusProvider.h"

// LArSoft libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "Geometry/Geometry.h"
#include "WebError.h"
#include "CalibrationDBI/IOVData/IOVDataConstants.h"

// C/C++ standard libraries
#include <vector>
#include <algorithm> // std::copy()
#include <iterator> // std::inserter()
#include <utility> // std::pair<>


namespace lariov {
  
  
  //----------------------------------------------------------------------------
  SIOVChannelStatusProvider::SIOVChannelStatusProvider(fhicl::ParameterSet const& pset)
    : DatabaseRetrievalAlg(pset.get<fhicl::ParameterSet>("DatabaseRetrievalAlg"))
    , fDefault(0)
  {

    bool UseDB    = pset.get<bool>("UseDB", false);
    bool UseFile  = pset.get<bool>("UseFile", false);
    
    //priority:  (1) use db, (2) use table, (3) use defaults
    //If none are specified, use defaults
    if ( UseDB )      fDataSource = DataSource::Database;
    else if (UseFile) fDataSource = DataSource::File;
    else              fDataSource = DataSource::Default;
    
    if (fDataSource == DataSource::Default) {
      ChannelStatus cs(0);
      cs.SetStatus(kGOOD);
      
      art::ServiceHandle<geo::Geometry> geo;
      geo::wire_id_iterator itW = geo->begin_wire_id();
      for ( ; itW != geo->end_wire_id(); ++itW) {
        DBChannelID_t ch = geo->PlaneWireToChannel(*itW);
	cs.SetChannel(ch);
	fData.AddOrReplaceRow(cs);
      }
    } 
  }
  
  bool SIOVChannelStatusProvider::Update(DBTimeStamp_t ts) {
    if (fDataSource != DataSource::Database) return false;
    
    if (!this->UpdateFolder(ts)) return false;
    
    //DBFolder was updated, so now update the Snapshot
    fData.Clear();
    fData.SetIoV(this->Begin(), this->End());

    std::vector<DBChannelID_t> channels;
    fFolder->GetChannelList(channels);
    for (auto it = channels.begin(); it != channels.end(); ++it) {

      long status;
      fFolder->GetNamedChannelData(*it, "status", status);

      ChannelStatus cs(*it);
      
      switch(status)
      {
        case kDISCONNECTED : cs.SetStatus(kDISCONNECTED);
	                     break;
	case kDEAD         : cs.SetStatus(kDEAD);
	                     break;
	case kLOWNOISE     : cs.SetStatus(kLOWNOISE);
	                     break;
	case kNOISY        : cs.SetStatus(kNOISY);
	                     break;
	case kGOOD         : cs.SetStatus(kGOOD);
	                     break;
	default            : cs.SetStatus(kUNKNOWN);
      }

      fData.AddOrReplaceRow(cs);
    }
    return true;
  }   
  
  
  //----------------------------------------------------------------------------
  const ChannelStatus& SIOVChannelStatusProvider::GetChannelStatus(DBChannelID_t ch) const {
    return fData.GetRow(ch);
  } 
  
  
  //----------------------------------------------------------------------------
  const DBChannelSet_t SIOVChannelStatusProvider::GetChannelsWithStatus(chStatus status) const {
    
    DBChannelSet_t retSet;
    retSet.clear();
    if (fDataSource == DataSource::Default) {
      if (fDefault.Status() == status) {
	std::vector<DBChannelID_t> chs;
	DBChannelID_t maxChannel = art::ServiceHandle<geo::Geometry>()->Nchannels() - 1;
	for (DBChannelID_t ch=0; ch != maxChannel; ++ch) {
	  chs.push_back(ch);
	}
	retSet.insert(chs.begin(), chs.end());
      }
    }
    else {
      const std::vector<ChannelStatus>& snapshot_data = fData.Data();
      std::vector<DBChannelID_t> chs;
      for (auto itC = snapshot_data.begin(); itC != snapshot_data.end(); ++itC) {
	if (itC->Status() == status) chs.push_back(itC->Channel());
      }

      retSet.insert(chs.begin(), chs.end());
    }
    return retSet;  
  }
  
  
  //----------------------------------------------------------------------------
  const DBChannelSet_t SIOVChannelStatusProvider::GoodChannels() const {
    return GetChannelsWithStatus(kGOOD);
  }

  
  //----------------------------------------------------------------------------
  const DBChannelSet_t SIOVChannelStatusProvider::BadChannels() const {
    DBChannelSet_t dead = GetChannelsWithStatus(kDEAD);
    DBChannelSet_t ln = GetChannelsWithStatus(kLOWNOISE);
    dead.insert(ln.begin(),ln.end());
    return dead;
  }

  
  //----------------------------------------------------------------------------
  const DBChannelSet_t SIOVChannelStatusProvider::NoisyChannels() const {
    return GetChannelsWithStatus(kNOISY); 
  }
  
  
  //----------------------------------------------------------------------------
  void SIOVChannelStatusProvider::AddNoisyChannel(DBChannelID_t ch) {  

    if (!this->IsBad(ch) && this->IsPresent(ch)) {
      ChannelStatus cs(ch);
      cs.SetStatus(kNOISY);
      fData.AddOrReplaceRow(cs);
    }
  }

  
  
  //----------------------------------------------------------------------------
  
} // namespace lariov

#endif
