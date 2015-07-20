/**
 * @file   SimpleChannelFilter_test.cxx
 * @brief  Test of SimpleChannelFilter
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 17th, 2015
 */

// Boost libraries
/*
 * Boost Magic: define the name of the module;
 * and do that before the inclusion of Boost unit test headers
 * because it will change what they provide.
 * Among the those, there is a main() function and some wrapping catching
 * unhandled exceptions and considering them test failures, and probably more.
 * This also makes fairly complicate to receive parameters from the command line
 * (for example, a random seed).
 */
#define BOOST_TEST_MODULE ( simple_channel_filter_test )
#include <boost/test/auto_unit_test.hpp> // BOOST_AUTO_TEST_CASE()
#include <boost/test/test_tools.hpp> // BOOST_CHECK(), BOOST_CHECK_EQUAL()

// LArSoft libraries
#include "Filters/SimpleChannelFilter.h"
#include "SimpleTypesAndConstants/RawTypes.h"
#include "CalibrationDBI/IOVData/IOVTimeStamp.h" // lariov::IOVTimeStamp

// framework libraries
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/coding.h"

// Boost libraries
#include <boost/any.hpp>

// C/C++ standard library
#include <iostream>
#include <ostream>
#include <set>
#include <string>
#include <memory> // std::unique_ptr<>
#include <algorithm> // std::equal(), std::transform()


namespace std {
  
  template <typename T>
  fhicl::detail::ps_sequence_t encode(std::set<T> const& s) {
    fhicl::detail::ps_sequence_t result;
    result.reserve(s.size());
    std::transform(s.begin(), s.end(), std::inserter(result, result.end()),
      [](T const& value) { return boost::any(fhicl::detail::encode(value)); }
      );
    return result;
  } // encode(set<T>)


  template <typename T>
  std::ostream& operator<< (std::ostream& out, std::set<T> const& s) {
    out << "{";
    typename std::set<T>::const_iterator begin = s.cbegin(), end = s.cend();
    if (begin != end) {
      out << " " << *begin;
      while (++begin != end) out << ", " << *begin;
      out << " ";
    } // if not empty
    out << "}";
    return out;
  } // operator<< (ostream, set<T>)

} // namespace std

//------------------------------------------------------------------------------
class FilterConfiguration {
    public:
  const raw::ChannelID_t fMaxChannel;
  const raw::ChannelID_t fMaxPresentChannel;
  const std::set<unsigned int> fBadChannels;
  const std::set<unsigned int> fNoisyChannels;
  
  FilterConfiguration(
    const raw::ChannelID_t MaxChannel,
    const raw::ChannelID_t MaxPresentChannel,
    const std::set<unsigned int> BadChannels,
    const std::set<unsigned int> NoisyChannels
    )
    : fMaxChannel(MaxChannel)
    , fMaxPresentChannel(MaxPresentChannel)
    , fBadChannels(BadChannels)
    , fNoisyChannels(NoisyChannels)
    {}
  
  std::unique_ptr<filter::SimpleChannelFilter> operator() () const
    { return CreateFilter(); }
  
  
  fhicl::ParameterSet CreateConfiguration() const {
    fhicl::ParameterSet cfg;
    
    boost::any any {fhicl::detail::encode(fNoisyChannels)};
    cfg.put("NoisyChannels", fNoisyChannels);
    cfg.put("BadChannels", fBadChannels);
    
    return cfg;
  } // CreateConfiguration()
  
  
  std::unique_ptr<filter::SimpleChannelFilter> CreateFilter() const {
    fhicl::ParameterSet config = CreateConfiguration();
    
    filter::SimpleChannelFilter* pFilter
      = new filter::SimpleChannelFilter(config);
    pFilter->Setup(fMaxChannel, fMaxPresentChannel);
    
    std::cout
      << "\nConfiguration:"
      << "\n  { " << config.to_string() << " }"
      << "\nLoaded from configuration:"
      << "\n  - " << pFilter->BadChannels().size() << " bad channels: " << pFilter->BadChannels()
      << "\n  - " << pFilter->NoisyChannels().size() << " noisy channels: " << pFilter->NoisyChannels()
      << "\n  - largest channel ID: " << pFilter->MaxChannel()
        << ", largest present: " << pFilter->MaxChannelPresent()
      << std::endl;
    
    return std::unique_ptr<filter::SimpleChannelFilter>(pFilter);
  } // CreateFilter()
  
  
}; // class FilterConfiguration


void test_simple_filter() {
  
  FilterConfiguration filterCreator(
           /* MaxChannel */ 20,
    /* MaxPresentChannel */ 15,
          /* BadChannels */ { 1, 9, 13 },
        /* NoisyChannels */ { 6, 8, 10, 11, 12, 13 }
    );
  
  BOOST_TEST_CHECKPOINT("Creating simple filter");
  std::unique_ptr<filter::SimpleChannelFilter> FilterOwner
    = filterCreator.CreateFilter();
  
  // Update() is always true, no matter what time is specified in
  BOOST_CHECK(FilterOwner->Update(lariov::IOVTimeStamp(0, 0)));
  
  filter::SimpleChannelFilter const* pSimpleFilter = FilterOwner.get();
  
  // check the values of the extremes
  BOOST_CHECK_EQUAL(pSimpleFilter->MaxChannel(), filterCreator.fMaxChannel);
  BOOST_CHECK_EQUAL
    (pSimpleFilter->MaxChannelPresent(), filterCreator.fMaxPresentChannel);
  
  // downcast to the interface to test interface stuff
  filter::ChannelFilterBaseInterface const* pFilter = pSimpleFilter;
  
  /**
   *
   * Public interface:
   * 
   * bool isPresent(raw::ChannelID_t channel) const
   * 
   * bool isGood(raw::ChannelID_t channel) const
   * 
   * bool isBad(raw::ChannelID_t channel) const
   * 
   * bool isNoisy(raw::ChannelID_t channel) const
   * 
   * ChannelSet_t GoodChannels() const
   * 
   * ChannelSet_t BadChannels() const
   * 
   * ChannelSet_t NoisyChannels() const
   * 
   * bool Update(lariov::IOVTimeStamp const& ts)
   *   ("checked" above)
   */
  
  // ChannelFilterBaseInterface::BadChannels()
  std::set<raw::ChannelID_t> FilterBadChannels = pFilter->BadChannels();
  BOOST_CHECK_EQUAL
    (FilterBadChannels.size(), filterCreator.fBadChannels.size());
  BOOST_CHECK_EQUAL(FilterBadChannels, filterCreator.fBadChannels);
  
  // ChannelFilterBaseInterface::NoisyChannels()
  std::set<raw::ChannelID_t> FilterNoisyChannels = pFilter->NoisyChannels();
  BOOST_CHECK_EQUAL
    (FilterNoisyChannels.size(), filterCreator.fNoisyChannels.size());
  BOOST_CHECK_EQUAL(FilterNoisyChannels, filterCreator.fNoisyChannels);
  
  std::set<raw::ChannelID_t> GoodChannels;
  
  for (raw::ChannelID_t channel = 0; channel <= filterCreator.fMaxChannel;
    ++channel
  ) {
    
    const bool bPresent = raw::isValidChannelID(channel)
      && (channel <= filterCreator.fMaxPresentChannel);
    const bool bBad = (filterCreator.fBadChannels.count(channel) > 0);
    const bool bNoisy = (filterCreator.fNoisyChannels.count(channel) > 0);
    const bool bGood = bPresent && !bBad && !bNoisy;
    
    if (bGood) GoodChannels.insert(channel);
    
    BOOST_CHECK_EQUAL(pFilter->isPresent(channel), bPresent);
    BOOST_CHECK_EQUAL(pFilter->isBad(channel), bBad);
    BOOST_CHECK_EQUAL(pFilter->isNoisy(channel), bNoisy);
    
    BOOST_CHECK_EQUAL(pFilter->isGood(channel), bGood);
    
  } // for channel
  
  // ChannelFilterBaseInterface::GoodChannels()
  std::set<raw::ChannelID_t> FilterGoodChannels = pFilter->GoodChannels();
  BOOST_CHECK_EQUAL(FilterGoodChannels.size(), GoodChannels.size());
  BOOST_CHECK_EQUAL(FilterGoodChannels, GoodChannels);
  
} // test_simple_filter()


//
// test
//
BOOST_AUTO_TEST_CASE(SimpleFilterTest) {
  test_simple_filter();
}

