#ifndef WEBDB_WEBREADER_CXX
#define WEBDB_WEBREADER_CXX

#include "WebReader.h"
#include <TStopwatch.h>
#include <limits>
#include <sstream>
#include <wda.h>

#include "CalibrationDBI/IOVData/UtilFunc.h"
#include "WebError.h"
namespace lariov {
  
  template <class T>
  WebReader<T>::WebReader(std::string  server,
			  std::string  port,
			  std::string  dbname,
			  unsigned int timeout)
    : _timeout (timeout)
  {
    _server = (server != "default" ? server : getenv("LIBWDA_SERVER") );
    _port   = (port   != "default" ? port   : getenv("LIBWDA_PORT"  ) );
    _dbname = (dbname != "default" ? dbname : getenv("LIBWDA_DBNAME") );
  }

  template <class T>
  const Snapshot<T>& WebReader<T>::Request(const std::string& name,
					   const TTimeStamp&  ts,
					   const std::string tag)
  {
    auto iter = _data_m.find(name);
    if(iter != _data_m.end() && (*iter).second.Valid(ts))
      return (*iter).second;

    std::string url("http://");
    url += _server;
    if(!_port.empty()) url += ":" + _port;
    url += "/" + _dbname + "/app";
    url += "/data?f=" + name;
    url += "&t=" + std::to_string(ts.GetSec()) + "." + std::to_string(int(ts.GetNanoSec()/1.e9));
    
    int err_code=0;
    TStopwatch fWatch;
    fWatch.Start();
    auto data = getDataWithTimeout(url.c_str(),NULL,_timeout,&err_code);
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

    if(nrows < 4) {
      releaseDataset(data);
      std::ostringstream msg;
      if(nrows)
	msg << "Not enough information loaded (#rows = " << nrows << " < 4)";
      else {
	auto time_taken = fWatch.RealTime();
	if(time_taken > (double)(_timeout))
	  msg << "No data ... likely connection timed-out!" ;
	else
	  msg << "No data ... likely incorrect folder name / invalid timestamp!" ;

	msg << std::endl
	    << "URL: " << url << std::endl
	    << "Time taken: " << time_taken << " / " << _timeout << " [s]" << std::endl;
	throw WebError(msg.str());
      }
    }

    TTimeStamp start, end;
    ChData<T> values;
    std::string str_value;
    std::vector<std::string> str_array;
    std::vector<std::string> field_name;
    std::vector<std::string> field_type;
    char indefinite[30]="-";
    char buf[30];
    int ch_column = -1;
    for(size_t row=0; row<nrows; ++row) {
      auto tup = getTuple(data,row);
      str_array.resize(getNfields(tup),"");

      if(row==0) {
	double t = getDoubleValue(tup,0,&err_code);
	if(err_code) {
	  releaseTuple(tup);
	  releaseDataset(data);
	  throw WebError("Failed to parse the IOV start time stamp!");
	}
	start.SetSec(int(t));
	start.SetNanoSec(int((t - start.GetSec())*1.e9));

      }
      else if(row==1) {
	int str_size = getStringValue(tup,0,buf,sizeof(buf),&err_code);
	if(err_code) {
	  releaseTuple(tup);
	  releaseDataset(data);
	  throw WebError("Failed to parse the IOV end time stamp!");
	}
	if(!(strncmp(buf,indefinite,str_size)))
	  end.SetSec(std::numeric_limits<int>::max());
	else{
	  double t = getDoubleValue(tup,0,&err_code);
	  if(err_code) {
	    releaseTuple(tup);
	    releaseDataset(data);
	    throw WebError("Failed to parse the IOV end time stamp!");
	  }
	  end.SetSec(int(t));
	  end.SetNanoSec(int((t - end.GetSec())*1.e9));	  
	}
      }
      else{
	for(size_t column=0; column<(size_t)(getNfields(tup)); ++column) {
	  int str_size = getStringValue(tup,column,buf,sizeof(buf),&err_code);
	  if(err_code) {
	    releaseTuple(tup);
	    releaseDataset(data);
	    throw WebError("Failed to parse the table field names!");
	  }
	  str_array[column].assign(buf,str_size);
	}

	if (!field_name.size()) {
	  field_name.reserve(str_array.size());
	  for(size_t i=0; i<str_array.size(); ++i) {
	    auto const& str = str_array[i];
	    if(str == "channel") ch_column = i;
	    else field_name.push_back(str);
	  }
	  if( ch_column < 0)
	    throw WebError("Channel field not found!");
	}
	else if (!field_type.size()) {
	  field_type.reserve(str_array.size());
	  for(size_t i=0; i<str_array.size(); ++i) {
	    auto const& str = str_array[i];
	    if(ch_column == ((int)i)) continue;
	    field_type.push_back(str);
	  }
	  if(iter==_data_m.end()) 
	    iter = _data_m.insert(std::make_pair(name,Snapshot<T>(name,field_name,field_type))).first;
	  (*iter).second.Compat(field_name,field_type);
	  (*iter).second.Reset(start, end, tag);
	  (*iter).second.reserve(nrows-4);
	}
	else{
	  values.resize(str_array.size()-1);
	  for(size_t column=0; column<str_array.size(); ++column) {
	    auto const& str = str_array[column];
	    try{
	      if(ch_column == ((int)column)) values.Channel(FromString<unsigned int>(str));
	      else values[column-1] = FromString<T>(str);
	    }catch(const std::exception& e) {
	      std::ostringstream msg;
	      msg<<"Failed to parse the string: "<<str.c_str()<<std::endl;
	      throw WebError(msg.str());
	    }
	  }
	  (*iter).second.push_back(values);
	}
      }
      releaseTuple(tup);
    }
    releaseDataset(data);
    return (*iter).second;
  }
}

#endif
