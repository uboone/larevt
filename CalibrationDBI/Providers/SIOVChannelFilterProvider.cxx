/**
 * @file   SIOVChannelFilterProvider.cxx
 * @brief  Channel quality provider with information from configuration file
 * @author Gianluca Petrillo (petrillo@fnal.gov) Brandon Eberly (eberly@fnal.gov)
 * @date   November 25th, 2014
 * @see    SIOVChannelFilterProvider.h
 */

#ifndef SIOVCHANNELFILTERPROVIDER_CXX
#define SIOVCHANNELFILTERPROVIDER_CXX 1

// Our header
#include "SIOVChannelFilterProvider.h"

// LArSoft libraries
#include "Geometry/Geometry.h"
#include "Geometry/GeometryCore.h"
#include "WebError.h"
#include "CalibrationDBI/IOVData/IOVDataConstants.h"

// C/C++ standard libraries
#include <vector>
#include <algorithm> // std::copy()
#include <iterator> // std::inserter()
#include <utility> // std::pair<>


namespace lariov {
  
  
  //----------------------------------------------------------------------------
  SIOVChannelFilterProvider::SIOVChannelFilterProvider(fhicl::ParameterSet const& pset)
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
    
    fDefault.SetStatus(kGOOD);   
  }
  
  bool SIOVChannelFilterProvider::Update(DBTimeStamp_t ts) {
    
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
	case kBAD          : cs.SetStatus(kBAD);
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
  
  const ChannelStatus& SIOVChannelFilterProvider::GetChannelStatus(DBChannelID_t ch) const {
    
    if (fDataSource == DataSource::Default) {
      return fDefault;
    }
    else {
      return fData.GetRow(ch);
    }
  } 
  
  
  //----------------------------------------------------------------------------
  const DBChannelSet_t SIOVChannelFilterProvider::GetChannelsWithStatus(chStatus status) const {
    
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
  
 const DBChannelSet_t SIOVChannelFilterProvider::GoodChannels() const {
   return GetChannelsWithStatus(kGOOD);
 }
 
 const DBChannelSet_t SIOVChannelFilterProvider::BadChannels() const {
   return GetChannelsWithStatus(kBAD);
 }
 
 const DBChannelSet_t SIOVChannelFilterProvider::NoisyChannels() const {
   return GetChannelsWithStatus(kNOISY); 
 }
 
 void SIOVChannelFilterProvider::AddNoisyChannel(DBChannelID_t ch) {  
   ChannelStatus cs(ch);
   cs.SetStatus(kNOISY);
   fData.AddOrReplaceRow(cs);
 }
  
  
  //----------------------------------------------------------------------------
  
} // namespace lariov

#endif
