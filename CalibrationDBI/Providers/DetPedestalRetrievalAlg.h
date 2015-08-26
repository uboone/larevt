/**
 * \file DetPedestalRetrievalAlg.h
 *
 * \ingroup WebDBI
 * 
 * \brief Class def header for a class DetPedestalRetrievalAlg
 *
 * @author eberly@slac.stanford.edu
 */

/** \addtogroup WebDBI

    @{*/
#ifndef WEBDBI_DETPEDESTALRETRIEVALALG_H
#define WEBDBI_DETPEDESTALRETRIEVALALG_H

#include "art/Framework/Principal/Event.h"
#include "CalibrationDBI/IOVData/DetPedestal.h"
#include "CalibrationDBI/IOVData/Snapshot.h"
#include "CalibrationDBI/IOVData/IOVDataConstants.h"
#include "CalibrationDBI/Interface/IDetPedestalProvider.h"
#include "DatabaseRetrievalAlg.h"

namespace lariov {

  /**
     \class DetPedestalRetrievalAlg
     User defined class DetPedestalRetrievalAlg ... these comments are used to generate
     doxygen documentation!
  */
  class DetPedestalRetrievalAlg : public DatabaseRetrievalAlg, public IDetPedestalProvider {
  
    public:
    
      /// Constructors
      DetPedestalRetrievalAlg(const std::string& foldername, 
      			      const std::string& url, 
			      const std::string& tag="");
	
      DetPedestalRetrievalAlg(fhicl::ParameterSet const& p);
      
      /// Reconfigure function called by fhicl constructor
      void Reconfigure(fhicl::ParameterSet const& p);
      
      /// Default destructor
      ~DetPedestalRetrievalAlg() {}
      
      /// Update Snapshot and inherited DBFolder if using database.  Return true if updated
      bool Update(DBTimeStamp_t ts) override;
      
      /// Retrieve pedestal information
      const DetPedestal& Pedestal(DBChannelID_t ch) const;      
      float PedMean(DBChannelID_t ch) const override;
      float PedRms(DBChannelID_t ch) const override;
      float PedMeanErr(DBChannelID_t ch) const override;
      float PedRmsErr(DBChannelID_t ch) const override;
           
      //hardcoded information about database folder - useful for debugging cross checks
      const unsigned int NCOLUMNS = 5;    
      const std::vector<std::string> FIELD_NAMES = {"channel", "mean", "mean_err", "rms", "rms_err"};
      const std::vector<std::string> FIELD_TYPES = {"unsigned int", "float", "float", "float", "float"};
      
    private:
    
      DataSource::ds fDataSource;
          
      Snapshot<DetPedestal> fData;
      DetPedestal fDefaultColl;
      DetPedestal fDefaultInd;
  };
}//end namespace lariov

#endif
 /** @} */ // end of doxygen group 
