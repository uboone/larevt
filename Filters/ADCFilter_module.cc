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

#include <math.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>

//Framework Includes
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h" 
#include "fhiclcpp/ParameterSet.h" 
#include "art/Framework/Principal/Handle.h" 
#include "art/Persistency/Common/Ptr.h" 
#include "art/Persistency/Common/PtrVector.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art/Framework/Services/Optional/TFileService.h" 
#include "art/Framework/Services/Optional/TFileDirectory.h" 
#include "messagefacility/MessageLogger/MessageLogger.h"

//Larsoft Includes
#include "RawData/raw.h"
#include "RawData/RawDigit.h"
#include "Filters/ChannelFilter.h"

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
      art::Handle< std::vector<raw::RawDigit> > rawdigitHandle;
      evt.getByLabel(fDigitModuleLabel,rawdigitHandle);

      //Make a ChannelFilter
      filter::ChannelFilter *chanFilt = new filter::ChannelFilter();

      if(!rawdigitHandle->size()) return false;

      for(unsigned int i = 0; i<rawdigitHandle->size(); ++i){
         art::Ptr<raw::RawDigit> digit(rawdigitHandle,i);
         uint32_t channel = digit->Channel();
         if(chanFilt->BadChannel(channel)) continue;
         
         //get ADC values after decompressing
         std::vector<short> rawadc(digit->Samples());
         raw::Uncompress(digit->ADCs(),rawadc,digit->Compression());
         short max = *std::max_element(rawadc.begin(),rawadc.end()) - digit->GetPedestal();
         if(max>=fMinADC) return true;//found one ADC value above threshold, pass filter  
      }

      return false;//didn't find ADC above threshold

   }

   DEFINE_ART_MODULE(ADCFilter)

} //namespace filt

#endif // ADCFILTER_H
