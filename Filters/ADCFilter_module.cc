////////////////////////////////////////////////////////////////////////
//
// ADCFilter class:
// Algorithm to ignore events with no ADC values 
// above user-defined threshold.
//
// msoderbe@syr.edu
//
////////////////////////////////////////////////////////////////////////
#ifndef ADCFILTER_H
#define ADCFILTER_H

#include <algorithm>
#include <vector>
#include <stdint.h>

//Framework Includes
#include "fhiclcpp/ParameterSet.h" 
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h" 
#include "art/Framework/Principal/Handle.h" 
#include "art/Framework/Principal/View.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art/Persistency/Common/Ptr.h" 

//Larsoft Includes
#include "RawData/raw.h"
#include "RawData/RawDigit.h"
#include "CalibrationDBI/Interface/IChannelStatusProvider.h"
#include "CalibrationDBI/Interface/IChannelStatusService.h"


namespace filter {

   class ADCFilter : public art::EDFilter  {
    
   public:
    
      explicit ADCFilter(fhicl::ParameterSet const& ); 
      virtual ~ADCFilter();
         
    
      bool filter(art::Event& evt);
      void reconfigure(fhicl::ParameterSet const& p);
      void beginJob();
   

   private: 
 
      std::string fDigitModuleLabel;
      double      fMinADC;  
 
   protected: 
    
   }; // class ADCFilter

   //-------------------------------------------------
   ADCFilter::ADCFilter(fhicl::ParameterSet const & pset)  
   {   
      this->reconfigure(pset);
   }

   //-------------------------------------------------
   ADCFilter::~ADCFilter()
   {
   }
  
   //-------------------------------------------------
   void ADCFilter::reconfigure(fhicl::ParameterSet const& p)
   {
      fDigitModuleLabel = p.get< std::string > ("DigitModuleLabel"); 
      fMinADC           = p.get< double      > ("MinADC");         
   } 

   //-------------------------------------------------
   void ADCFilter::beginJob()
   {
   }

   //-------------------------------------------------
   bool ADCFilter::filter(art::Event &evt)
   { 
      //Read in raw data
      art::View<raw::RawDigit> rawdigitView;
      evt.getView(fDigitModuleLabel, rawdigitView);
      
      if(!rawdigitView.size()) return false;
      
      lariov::IChannelStatusProvider const& channelFilter
        = art::ServiceHandle<lariov::IChannelStatusService>()->GetFilter();

      // look through the good channels
//      for(const raw::RawDigit* digit: filter::SelectGoodChannels(rawdigitView))
      for(const raw::RawDigit* digit: rawdigitView)
      {
         if (!channelFilter.IsGood(digit->Channel())) continue;
         //get ADC values after decompressing
         std::vector<short> rawadc(digit->Samples());
         raw::Uncompress(digit->ADCs(),rawadc,digit->Compression());
         short max = *std::max_element(rawadc.begin(),rawadc.end()) - digit->GetPedestal();
         if(max>=fMinADC) return true;//found one ADC value above threshold, pass filter  
      }

      return false;//didn't find ADC above threshold

   }

   DEFINE_ART_MODULE(ADCFilter)

} //namespace filter

#endif // ADCFILTER_H
