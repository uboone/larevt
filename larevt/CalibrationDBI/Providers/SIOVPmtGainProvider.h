/**
 * \file SIOVPmtGainProvider.h
 *
 * \ingroup WebDBI
 * 
 * \brief Class def header for a class SIOVPmtGainProvider
 *
 * @author eberly@slac.stanford.edu
 */

#ifndef SIOVPMTGAINPROVIDER_H
#define SIOVPMTGAINPROVIDER_H

#include "larevt/CalibrationDBI/IOVData/PmtGain.h"
#include "larevt/CalibrationDBI/IOVData/Snapshot.h"
#include "larevt/CalibrationDBI/IOVData/IOVDataConstants.h"
#include "larevt/CalibrationDBI/Interface/PmtGainProvider.h"
#include "DatabaseRetrievalAlg.h"

namespace lariov {

  /**
   * @brief Retrieves information: pmt gain
   * 
   * Configuration parameters
   * =========================
   * 
   * - *DatabaseRetrievalAlg* (parameter set, mandatory): configuration for the
   *   database; see lariov::DatabaseRetrievalAlg
   * - *UseDB* (boolean, default: false): retrieve information from the database
   * - *UseFile* (boolean, default: false): retrieve information from a file;
   *   not implemented yet
   * - *DefaultGain* (real, default: ): Gain returned 
   *   when /UseDB/ and /UseFile/ parameters are false
   * - *DefaultGainErr* (real, default: ): Gain uncertainty returned
   *   when /UseDB/ and /UseFile/ parameters are false
   */
  class SIOVPmtGainProvider : public DatabaseRetrievalAlg, public PmtGainProvider {
  
    public:
    
      /// Constructors
      SIOVPmtGainProvider(fhicl::ParameterSet const& p);
      
      /// Reconfigure function called by fhicl constructor
      void Reconfigure(fhicl::ParameterSet const& p) override;
      
      /// Update Snapshot and inherited DBFolder if using database.  Return true if updated
      bool Update(DBTimeStamp_t ts);
      
      /// Retrieve gain information
      const PmtGain& PmtGainObject(DBChannelID_t ch) const;      
      float Gain(DBChannelID_t ch) const override;
      float GainErr(DBChannelID_t ch) const override;
      CalibrationExtraInfo const& ExtraInfo(DBChannelID_t ch) const override;
      
    private:
    
      DataSource::ds fDataSource;
          
      Snapshot<PmtGain> fData;
  };
}//end namespace lariov

#endif

