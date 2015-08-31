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

#include "CalibrationDBI/IOVData/PmtGain.h"
#include "CalibrationDBI/IOVData/Snapshot.h"
#include "CalibrationDBI/IOVData/IOVDataConstants.h"
#include "CalibrationDBI/Interface/IPmtGainProvider.h"
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
   * - *DefaultSPEHeight* (real, default: ): SPE height returned 
   *   when /UseDB/ and /UseFile/ parameters are false
   * - *DefaultSPEHeightErr* (real, default: ): SPE height uncertainty returned
   *   when /UseDB/ and /UseFile/ parameters are false
   * - *DefaultSPEWidth* (real, default: ): SPE width returned
   *   when /UseDB/ and /UseFile/ parameters are false
   * - *DefaultSPEWidthErr* (real, default: 0.3): SPE width uncertainty returned
   *   when /UseDB/ and /UseFile/ parameters are false
   * - *DefaultSPEArea* (real, default: 0.0): SPE area returned
   *   when /UseDB/ and /UseFile/ parameters are false
   * - *DefaultSPEAreaErr* (real, default: 0.0): SPE area uncertainty returned
   *   when /UseDB/ and /UseFile/ parameters are false
   */
  class SIOVPmtGainProvider : public DatabaseRetrievalAlg, public IPmtGainProvider {
  
    public:
    
      /// Constructors
      SIOVPmtGainProvider(fhicl::ParameterSet const& p);
      
      /// Reconfigure function called by fhicl constructor
      void Reconfigure(fhicl::ParameterSet const& p);
      
      /// Default destructor
      ~SIOVPmtGainProvider() {}
      
      /// Update Snapshot and inherited DBFolder if using database.  Return true if updated
      bool Update(DBTimeStamp_t ts) override;
      
      /// Retrieve pedestal information
      const PmtGain& ChannelInfo(DBChannelID_t ch) const;      
      float SpeHeight(DBChannelID_t ch) const override;
      float SpeHeightErr(DBChannelID_t ch) const override;
      float SpeWidth(DBChannelID_t ch) const override;
      float SpeWidthErr(DBChannelID_t ch) const override;
      float SpeArea(DBChannelID_t ch) const override;
      float SpeAreaErr(DBChannelID_t ch) const override;
           
      //hardcoded information about database folder - useful for debugging cross checks
      const unsigned int NCOLUMNS = 7;    
      const std::vector<std::string> FIELD_NAMES = {"channel", "spe_height", "spe_height_err", "spe_width", "spe_width_err", "spe_area", "spe_area_err"};
      const std::vector<std::string> FIELD_TYPES = {"unsigned int", "float", "float", "float", "float", "float", "float"};
      
    private:
    
      DataSource::ds fDataSource;
          
      Snapshot<PmtGain> fData;
      mutable PmtGain   fDefault;
  };
}//end namespace lariov

#endif

