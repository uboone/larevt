#ifndef WEBDB_WEBREADER_CXX
#define WEBDB_WEBREADER_CXX

#include "WebReader.h"
#include <TStopwatch.h>
#include <limits>
#include <sstream>
#include <wda.h>
#include "WebError.h"

namespace lariov {


  void WebReader::SetConnInfo(const WDAConnInfo& conn, 
  			      const std::string& folder, 
			      const std::string& tag /*=0*/) {
    if (fConn != conn) fConn = conn;
    fFolder = folder;
    fTag = tag;
  }
      
  void* WebReader::Request(const TTimeStamp& ts, DBHeader& header) const {
    
    header.fields.clear();
    header.types.clear();
    
    std::string url = fConn.URLPrefix();
    url += "/data?f=" + fFolder;
    url += "&t=" + std::to_string(ts.GetSec()) + "." + std::to_string(int(ts.GetNanoSec()/1.e9));
    
    int err_code=0;
    TStopwatch fWatch;
    fWatch.Start();
    
    void* data = getDataWithTimeout(url.c_str(),NULL,fConn._timeout,&err_code);
    if(err_code) {
      std::cerr << "\033[93m" 
		<< __FUNCTION__
		<< "\033[00m" << std::endl
		<< "  Error code: " << err_code << std::endl
		<< "  Failed to execute a query: " << url.c_str() << std::endl
		<< "  Log: " << getHTTPmessage(data) << std::endl;
      releaseDataset(data);
      throw WebError("Failed to fetch Snapshot!");
    }

    size_t nrows = getNtuples(data);

    if(nrows < kNUMBER_HEADER_ROWS) {
      releaseDataset(data);
      std::ostringstream msg;
      if(nrows)
	msg << "Not enough information loaded (#rows = " << nrows << " < "<<kNUMBER_HEADER_ROWS<<")";
      else {
	auto time_taken = fWatch.RealTime();
	if(time_taken > (double)(fConn._timeout))
	  msg << "No data ... likely connection timed-out!" ;
	else
	  msg << "No data ... likely incorrect folder name / invalid timestamp!" ;

	msg << std::endl
	    << "URL: " << url << std::endl
	    << "Time taken: " << time_taken << " / " << fConn._timeout << " [s]" << std::endl;
	throw WebError(msg.str());
      }
    }

    char indefinite[30]="-";
    char buf[30];
    for(size_t row=0; row<kNUMBER_HEADER_ROWS; ++row) {
      
      auto tup = getTuple(data,row);
      
      switch (row) {
        
	case 0: //beginning timestamp
        {
	  double t = getDoubleValue(tup,0,&err_code);
	  if(err_code) {
	    releaseTuple(tup);
	    releaseDataset(data);
	    throw WebError("Failed to parse the IOV start time stamp!");
	  }
	  header.begin.SetSec(int(t));
	  header.begin.SetNanoSec(int((t - header.begin.GetSec())*1.e9));
	  
	  break;
	}
	case 1: //end timestamp
        {
	  int str_size = getStringValue(tup,0,buf,sizeof(buf),&err_code);
	  if(err_code) {
	    releaseTuple(tup);
	    releaseDataset(data);
	    throw WebError("Failed to parse the IOV end time stamp!");
	  }
	  if(!(strncmp(buf,indefinite,str_size)))
	    header.end.SetSec(std::numeric_limits<int>::max());
	  else{
	    double t = getDoubleValue(tup,0,&err_code);
	    if(err_code) {
	      releaseTuple(tup);
	      releaseDataset(data);
	      throw WebError("Failed to parse the IOV end time stamp!");
	    }
	    header.end.SetSec(int(t));
	    header.end.SetNanoSec(int((t - header.end.GetSec())*1.e9));	  
	  }
	  
	  break;
        }
        default: //field names, types
        {
          if (row>3) {
	    throw WebError("Read too many rows in WebReader!");
	    break;
	  }

	  int ch_column = -1;
	  for(size_t column=0; column < (size_t)getNfields(tup); ++column) {
	    int str_size = getStringValue(tup,column,buf,sizeof(buf),&err_code);
	    if(err_code) {
	      releaseTuple(tup);
	      releaseDataset(data);
	      throw WebError("Failed to parse the table field names!");
	    }
	    
	    std::string tmp_str(buf,str_size);
	    if (tmp_str == "channel") ch_column = column;
	    
	    if      (row==2) header.fields.push_back(tmp_str);
	    else if (row==3) header.types.push_back(tmp_str);
	  }
	  if (row==2 && ch_column < 0)
	    throw WebError("Channel field not found!");
	    
	  break;
	}
      }//end switch(row)
    }//loop over header rows
    
    return data;
  } //end WebReader::Request
} //end namespace lariov

#endif
