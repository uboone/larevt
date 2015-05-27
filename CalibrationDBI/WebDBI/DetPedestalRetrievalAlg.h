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
#include "DatabaseRetrievalAlg.h"

namespace lariov {

  /**
     \class DetPedestalRetrievalAlg
     User defined class DetPedestalRetrievalAlg ... these comments are used to generate
     doxygen documentation!
  */
  class DetPedestalRetrievalAlg : public DatabaseRetrievalAlg {
  
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
      /// This version accepts an art::Event object from which to form a time stamp, is meant
      /// to be a temporary workaround to address detector independence issues... (TU)
      bool Update(const art::Event& event);
      
      /// Update Snapshot and inherited DBFolder if using database.  Return true if updated
      bool Update(const IOVTimeStamp& ts);
      
      /// Set defaults for one channel
      void SetOneDefault(const DetPedestal& def);
      
      /// Retrieve pedestal information
      const DetPedestal& Pedestal(unsigned int ch);      
      float PedMean(unsigned int ch);
      float PedRms(unsigned int ch);
      float PedMeanErr(unsigned int ch);
      float PedRmsErr(unsigned int ch);
           
      //hardcoded information about database folder - useful for debugging cross checks
      const unsigned int NCOLUMNS = 5;    
      const std::vector<std::string> FIELD_NAMES = {"channel", "mean", "mean_err", "rms", "rms_err"};
      const std::vector<std::string> FIELD_TYPES = {"unsigned int", "float", "float", "float", "float"};
      
    private:
    
      bool fUseDB;
      bool fUseFile;
      bool fUseDefault;
          
      Snapshot<DetPedestal> fData; 
      DetPedestal fDefaultColl;
      DetPedestal fDefaultInd;
  };
}//end namespace lariov

#endif
 /** @} */ // end of doxygen group 
