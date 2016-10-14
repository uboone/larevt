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
#include "larcore/Geometry/Geometry.h"
#include "WebError.h"
#include "larevt/CalibrationDBI/IOVData/IOVDataConstants.h"

// C/C++ standard libraries
#include <vector>
#include <algorithm> // std::copy()
#include <iterator> // std::inserter()
#include <utility> // std::pair<>
#include <fstream>


namespace lariov {
  
  
  //----------------------------------------------------------------------------
  SIOVChannelStatusProvider::SIOVChannelStatusProvider(fhicl::ParameterSet const& pset)
    : DatabaseRetrievalAlg(pset.get<fhicl::ParameterSet>("DatabaseRetrievalAlg"))
    , fDefault(0)
  {

    bool UseDB    = pset.get<bool>("UseDB", false);
    bool UseFile  = pset.get<bool>("UseFile", false);
    std::string fileName = pset.get<std::string>("FileName", "");
    
    //priority:  (1) use db, (2) use table, (3) use defaults
    //If none are specified, use defaults
    if ( UseDB )      fDataSource = DataSource::Database;
    else if (UseFile) fDataSource = DataSource::File;
    else              fDataSource = DataSource::Default;
    
    if (fDataSource == DataSource::Default) {
      std::cout << "Using default channel status value: "<<kGOOD<<"\n";
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
    else if (fDataSource == DataSource::File) {
      std::cout << "Using channel statuses from local file: "<<fileName<<"\n";
      std::ifstream file(fileName);
      if (!file) {
        throw cet::exception("SIOVChannelStatusProvider")
	  << "File "<<fileName<<" is not found.";
      }
      
      std::string line;
      ChannelStatus cs(0);
      while (std::getline(file, line)) {
        DBChannelID_t ch = (DBChannelID_t)std::stoi(line.substr(0, line.find(',')));
	int status = std::stoi(line.substr(line.find(',')+1));
	
	cs.SetChannel(ch);
	cs.SetStatus( ChannelStatus::GetStatusFromInt(status) );
	fData.AddOrReplaceRow(cs);
      }
    } // if source from file
    else {
      std::cout << "Using channel statuses from conditions database\n";
    }
  }
  
  bool SIOVChannelStatusProvider::Update(DBTimeStamp_t ts) {
    
    fNewNoisy.Clear();
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
      cs.SetStatus( ChannelStatus::GetStatusFromInt((int)status) );

      fData.AddOrReplaceRow(cs);
    }
    return true;
  }   
  
  
  //----------------------------------------------------------------------------
  const ChannelStatus& SIOVChannelStatusProvider::GetChannelStatus(raw::ChannelID_t ch) const {
    if (fNewNoisy.HasChannel(rawToDBChannel(ch))) {
      return fNewNoisy.GetRow(rawToDBChannel(ch));
    }
    else { 
      return fData.GetRow(rawToDBChannel(ch));
    }
  } 
  
  
  //----------------------------------------------------------------------------
  SIOVChannelStatusProvider::ChannelSet_t
  SIOVChannelStatusProvider::GetChannelsWithStatus(chStatus status) const {
    
    ChannelSet_t retSet;
    retSet.clear();
    DBChannelID_t maxChannel = art::ServiceHandle<geo::Geometry>()->Nchannels() - 1;
    if (fDataSource == DataSource::Default) {
      if (fDefault.Status() == status) {
	std::vector<DBChannelID_t> chs;
	for (DBChannelID_t ch=0; ch != maxChannel; ++ch) {
	  chs.push_back(ch);
	}
	retSet.insert(chs.begin(), chs.end());
      }
    }
    else {
      std::vector<DBChannelID_t> chs;
      for (DBChannelID_t ch=0; ch != maxChannel; ++ch) {
	if (this->GetChannelStatus(ch).Status() == status) chs.push_back(ch);
      }

      retSet.insert(chs.begin(), chs.end());
    }
    return retSet;  
  }
  
  
  //----------------------------------------------------------------------------
  SIOVChannelStatusProvider::ChannelSet_t
  SIOVChannelStatusProvider::GoodChannels() const {
    return GetChannelsWithStatus(kGOOD);
  }

  
  //----------------------------------------------------------------------------
  SIOVChannelStatusProvider::ChannelSet_t
  SIOVChannelStatusProvider::BadChannels() const {
    ChannelSet_t dead = GetChannelsWithStatus(kDEAD);
    ChannelSet_t ln = GetChannelsWithStatus(kLOWNOISE);
    dead.insert(ln.begin(),ln.end());
    return dead;
  }

  
  //----------------------------------------------------------------------------
  SIOVChannelStatusProvider::ChannelSet_t
  SIOVChannelStatusProvider::NoisyChannels() const {
    return GetChannelsWithStatus(kNOISY); 
  }
  
  
  //----------------------------------------------------------------------------
  void SIOVChannelStatusProvider::AddNoisyChannel(raw::ChannelID_t ch) {
    
    register DBChannelID_t const dbch = rawToDBChannel(ch);
    if (!this->IsBad(dbch) && this->IsPresent(dbch)) {
      ChannelStatus cs(dbch);
      cs.SetStatus(kNOISY);
      fNewNoisy.AddOrReplaceRow(cs);
    }
  }

  
  
  //----------------------------------------------------------------------------
  
} // namespace lariov

#endif
