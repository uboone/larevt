////////////////////////////////////////////////////////////////////////
//
// EventTimeFilter class
// 
// author: Aidan Reynolds
// email: aidan.reynolds@physics.ox.ac.uk
//
// - A filter to select events between given start and end dates and times.
//    - Dates should be passed in the form YYYYMMDD
//    - Times should be passes in the form HHMMSS
//    - All dates and times should be in UTC
//
////////////////////////////////////////////////////////////////////////

// C++
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
}
#include <math.h>
#include <algorithm>
#include <iostream>
#include <fstream>

// ROOT
#include "TMath.h"
#include "TTimeStamp.h"

/// Framework 
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Principal/Event.h" 
#include "fhiclcpp/ParameterSet.h" 
#include "art/Framework/Principal/Handle.h" 
#include "canvas/Persistency/Common/Ptr.h" 
#include "canvas/Persistency/Common/PtrVector.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art/Framework/Services/Optional/TFileService.h" 
#include "art/Framework/Services/Optional/TFileDirectory.h" 
#include "messagefacility/MessageLogger/MessageLogger.h" 

///filters for events, etc
namespace filter {

	class EventTimeFilter : public art::EDFilter  {

	public:

		explicit EventTimeFilter(fhicl::ParameterSet const& ); 
		
		bool filter(art::Event& evt);
		void reconfigure(fhicl::ParameterSet const& p);

	private:

		UInt_t fDateRangeLow; 
		UInt_t fTimeRangeLow; 
		UInt_t fDateRangeHigh; 
		UInt_t fTimeRangeHigh; 

	}; //class EventTimeFilter

}

void filter::EventTimeFilter::reconfigure(fhicl::ParameterSet const& p) {

	fDateRangeLow   = p.get<UInt_t>("DateRangeLow", 0);   // YYYYMMDD
	fTimeRangeLow   = p.get<UInt_t>("TimeRangeLow", 0);   // HHMMSS
	fDateRangeHigh  = p.get<UInt_t>("DateRangeHigh", 0);  // YYYYMMDD
	fTimeRangeHigh  = p.get<UInt_t>("TimeRangeHigh", 0);  // HHMMSS

}

filter::EventTimeFilter::EventTimeFilter(fhicl::ParameterSet const& pset)
  : EDFilter{pset}
{
	this->reconfigure(pset);
}

bool filter::EventTimeFilter::filter(art::Event &evt) {   

	// Check that input date is in correct format
	if (fDateRangeHigh > 99999999 || fDateRangeLow > 99999999) {
		std::cout << "Warning: please provide date in format YYYYMMDD, event time "
		          << "filter returning false." << std::endl; 
		return false;
	}
	if (fDateRangeHigh > 0 && fDateRangeHigh < 10000000) {
		std::cout << "Warning: please provide date in format YYYYMMDD, event time "
		          << "filter returning false." << std::endl; 
		return false;
	}
	if (fDateRangeLow > 0 && fDateRangeLow < 10000000) {
		std::cout << "Warning: please provide date in format YYYYMMDD, event time "
		          << "filter returning false." << std::endl; 
		return false;
	}

	// Check that input times are in correct format
	if (fTimeRangeHigh > 999999 || fTimeRangeLow > 999999) {
		std::cout << "Warning: please provide time in format HHMMSS, event time "
		          << "filter returning false." << std::endl; 
		return false;
	}
	if (fTimeRangeHigh > 0 && fTimeRangeHigh < 100000) {
		std::cout << "Warning: please provide time in format HHMMSS, event time "
		          << "filter returning false." << std::endl; 
		return false;
	}
	if (fTimeRangeLow > 0 && fTimeRangeLow < 100000) {
		std::cout << "Warning: please provide time in format HHMMSS, event time "
		          << "filter returning false." << std::endl; 
		return false;
	}

	// Event time
	art::Timestamp evtTime = evt.time();
	TTimeStamp * evtTTS;
	if (evtTime.timeHigh() == 0) { evtTTS = new TTimeStamp(evtTime.timeLow()); }
	else { evtTTS = new TTimeStamp(evtTime.timeHigh(), evtTime.timeLow()); }

	std::cout << "Event time:  " << evtTTS -> AsString() << std::endl;

	// Requested time range lower end
	TTimeStamp * ttsLow(nullptr); 
	if (fDateRangeLow != 0) {
		if (fTimeRangeLow != 0) { 
			ttsLow = new TTimeStamp(fDateRangeLow, fTimeRangeLow, 0u); 
		}
		else { 
			ttsLow = new TTimeStamp(fDateRangeLow, 0u, 0u); 
			std::cout << "Warning: No start time given for event time filter, "
			          << "assuming 00:00:00" << std::endl;
		}
	}

	// Requested time range higher end
	TTimeStamp * ttsHigh(nullptr);
	if (fDateRangeHigh != 0) {
		if (fTimeRangeHigh != 0) { 
			ttsHigh = new TTimeStamp(fDateRangeHigh, fTimeRangeHigh, 0u); 
		}
		else { 
			std::cout << "Warning: No end time given for event time filter, assuming "
			          << "23:59:59" << std::endl;
			ttsHigh = new TTimeStamp(fDateRangeHigh, 235959u, 0u); 
		}
	}

	// Filter decision
	if (ttsLow == nullptr && ttsHigh == nullptr) {
		std::cout << "Warning: No date range requested for event time filter, "
		          << "returning false." << std::endl;
		return false;
	} 
	else if (ttsLow == nullptr) {
		std::cout << "Warning: No lower limit requested for event time filter, "
		          << "taking all events before " << ttsHigh -> AsString() 
		          << std::endl;
		if (evtTTS -> GetSec() < ttsHigh -> GetSec()) { return true; }
		else { return false;}
	} 
	else if (ttsHigh == nullptr) {
		std::cout << "Warning: No lower limit requested for event time filter, "
		          << "taking all events after " << ttsLow -> AsString() 
		          << std::endl;
		if (evtTTS -> GetSec() > ttsLow -> GetSec()) { return true; }
		else { return false;}
	} 
	else { 
		if (evtTTS -> GetSec() > ttsLow -> GetSec() && 
		    evtTTS -> GetSec() < ttsHigh -> GetSec()) { return true; }
		else { return false; }
	}

}

namespace filter { DEFINE_ART_MODULE(EventTimeFilter) }
