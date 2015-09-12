/**
 * @file   SIOVChannelFilterProvider.h
 * @brief  Channel quality provider with information from configuration file
 * @author Brandon Eberly (eberly@fnal.gov)
 * @date   August 24, 2015
 * @see    IChannelFilterService.h SIOVChannelFilterProvider.cpp
 */


#ifndef SIOVCHANNELFILTERPROVIDER_H
#define SIOVCHANNELFILTERPROVIDER_H 1

// LArSoft libraries
#include "CalibrationDBI/Interface/IChannelFilterProvider.h"
#include "DatabaseRetrievalAlg.h"
#include "CalibrationDBI/IOVData/ChannelStatus.h"
#include "CalibrationDBI/IOVData/Snapshot.h"

// Utility libraries
#include "fhiclcpp/ParameterSet.h"


/// Filters for channels, events, etc
namespace lariov {


  /** **************************************************************************
   * @brief Class providing information about the quality of channels
   *
   * This class serves information read from a FHiCL configuration file and/or a database.
   * 
   * LArSoft interface to this class is through the service
   * SIOVChannelFilterService.
   */
  class SIOVChannelFilterProvider: public DatabaseRetrievalAlg, public IChannelFilterProvider {
    
    public:
    
      /// Constructor
      SIOVChannelFilterProvider(fhicl::ParameterSet const& pset);
    
      ///Default destructor
      ~SIOVChannelFilterProvider() = default;

      //
      // non-interface methods
      //
      /// Returns Channel Status
      const ChannelStatus& GetChannelStatus(DBChannelID_t channel) const;

      //
      // interface methods
      //

      /// @name Single channel queries
      /// @{
      /// Returns whether the specified channel is physical and connected to wire
      bool IsPresent(DBChannelID_t channel) const override {
        return this->GetChannelStatus(channel).IsPresent();
      }

      /// Returns whether the specified channel is bad in the current run
      bool IsBad(DBChannelID_t channel) const override {
        return this->GetChannelStatus(channel).IsDead() || this->GetChannelStatus(channel).IsLowNoise() || !this->IsPresent(channel);
      }

      /// Returns whether the specified channel is noisy in the current run
      bool IsNoisy(DBChannelID_t channel) const override {
        return this->GetChannelStatus(channel).IsNoisy();
      }
      
      /// Returns whether the specified channel is physical and good
      bool IsGood(DBChannelID_t channel) const override {
	return this->GetChannelStatus(channel).IsGood(); 
      }
      /// @}
      
      unsigned short Status(DBChannelID_t channel) const override {
        return (unsigned short)this->GetChannelStatus(channel).Status();
      }


      /// @name Global channel queries
      /// @{
      /// Returns a copy of set of good channel IDs for the current run
      const DBChannelSet_t GoodChannels() const override;

      /// Returns a copy of set of bad channel IDs for the current run
      const DBChannelSet_t BadChannels() const override;

      /// Returns a copy of set of noisy channel IDs for the current run
      const DBChannelSet_t NoisyChannels() const override;
      /// @}


      /// @name Configuration functions
      /// @{
      /// Prepares the object to provide information about the specified time
      bool Update(DBTimeStamp_t) override;
      
      /// Allows a service to add to the list of noisy channels
      void AddNoisyChannel(DBChannelID_t ch);
      ///@}
    
    private:
    
      DataSource::ds fDataSource;
      Snapshot<ChannelStatus> fData;
      ChannelStatus fDefault;
      
      const DBChannelSet_t GetChannelsWithStatus(chStatus status) const;
    
  }; // class SIOVChannelFilterProvider
  
  
} // namespace lariov


#endif // SIOVCHANNELFILTERPROVIDER_H
