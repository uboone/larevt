/**
 * @file   SimpleChannelFilterTest_module.cc
 * @brief  Integration test for SimpleChannelFilterService
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 25th, 2014
 */

// C/C++ standard library
#include <vector>
#include <string>

// Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

// LArSoft includes
#include "Filters/ChannelFilterServiceInterface.h"



namespace art { class Event; } // art::Event declaration

///tracking algorithms
namespace filter {
  /**
   * @brief Tests an instanciation of the ChannelFilterService
   * 
   * Configuration parameters
   * =========================
   * 
   * - **TestBadChannels** (list of integers, default: empty): test singularly
   *   these channels and verify they are bad
   * - **TestGoodChannels** (list of integers, default: empty): test singularly
   *   these channels and verify they are not bad nor noisy
   * - **TestNoisyChannels** (list of integers, default: empty): test singularly
   *   these channels and verify they are noisy
   */
  class SimpleChannelFilterTest: public art::EDAnalyzer {
      public:
    explicit SimpleChannelFilterTest(fhicl::ParameterSet const& pset);

    virtual void analyze(art::Event const&) override {}
    virtual void beginRun(art::Run const& run) override;

      private:
    std::vector<unsigned int> KnownGoodChannels;
    std::vector<unsigned int> KnownNoisyChannels;
    std::vector<unsigned int> KnownBadChannels;
    
    template <typename Obj>
    unsigned int testObject(Obj* pFilter) const;
    
    
    static const std::vector<unsigned int> EmptyVect; ///< for initializations
  }; // class SimpleChannelFilterTest
  
} // namespace filter


//------------------------------------------------------------------------------


namespace filter {
  
  const std::vector<unsigned int> SimpleChannelFilterTest::EmptyVect;
  
  //......................................................................
  SimpleChannelFilterTest::SimpleChannelFilterTest
    (fhicl::ParameterSet const& pset) 
    : EDAnalyzer(pset)
    , KnownGoodChannels
        (pset.get<std::vector<unsigned int>>("TestGoodChannels", EmptyVect))
    , KnownNoisyChannels
        (pset.get<std::vector<unsigned int>>("TestNoisyChannels", EmptyVect))
    , KnownBadChannels
        (pset.get<std::vector<unsigned int>>("TestBadChannels", EmptyVect))
  {
    mf::LogInfo log("SimpleChannelFilterTest");
    
    log << "Channels known as good:  " << KnownGoodChannels.size();
    if (!KnownGoodChannels.empty()) {
      log << " (";
      for (unsigned int chId: KnownGoodChannels) log << " " << chId;
      log << ")";
    } // if known good channels
    
    log << "\nChannels known as noisy: " << KnownNoisyChannels.size();
    if (!KnownNoisyChannels.empty()) {
      log << " (";
      for (unsigned int chId: KnownNoisyChannels) log << " " << chId;
      log << ")";
    } // if known noisy channels
    
    log << "\nChannels known as bad:   " << KnownBadChannels.size();
    if (!KnownBadChannels.empty()) {
      log << " (";
      for (unsigned int chId: KnownBadChannels) log << " " << chId;
      log << ")";
    } // if known bad channels
    
  } // SimpleChannelFilterTest::SimpleChannelFilterTest()
  
  
  //......................................................................
  void SimpleChannelFilterTest::beginRun(art::Run const& run) {
    
    mf::LogInfo("SimpleChannelFilterTest") << "New run: " << run.run();
    
    unsigned int nErrors = 0;
    
    //---
    mf::LogVerbatim("SimpleChannelFilterTest")
      << "\nTesting service interface...";
    art::ServiceHandle<filter::ChannelFilterServiceInterface> FilterSrvHandle;
    const filter::ChannelFilterServiceInterface* pFilterSrv = &*FilterSrvHandle;
    nErrors = testObject(pFilterSrv);
    if (nErrors > 0) {
      throw art::Exception(art::errors::LogicError)
        << nErrors << " errors while testing ChannelFilterServiceInterface!";
    } // if errors
    
    //---
    mf::LogVerbatim("SimpleChannelFilterTest")
      << "\nTesting base interface...";
    filter::ChannelFilterBaseInterface* pFilter = FilterSrvHandle->GetFilter();
    pFilter->SetRun(run.run());
    nErrors = testObject(pFilter);
    if (nErrors > 0) {
      throw art::Exception(art::errors::LogicError)
        << nErrors << " errors while testing ChannelFilterBaseInterface!";
    } // if errors
    
  } // SimpleChannelFilterTest::beginRun()
  
  
  //......................................................................
  template <typename Obj>
  unsigned int SimpleChannelFilterTest::testObject(Obj* pFilter) const
  {
    // 1. print all the channels marked non-good
    {
      mf::LogInfo log("SimpleChannelFilterTest");
      
      
      // this is a copy of the list;
      // to avoid creating temporary objects, check channels one by one
      auto BadChannels = pFilter->SetOfBadChannels();
      log << "\nChannels marked as bad:   " << BadChannels.size();
      if (!BadChannels.empty()) {
        log << " (";
        for (unsigned int chId: BadChannels) log << " " << chId;
        log << ")";
      } // if bad channels
      
      auto NoisyChannels = pFilter->SetOfNoisyChannels();
      log << "\nChannels marked as noisy: " << NoisyChannels.size();
      if (!NoisyChannels.empty()) {
        log << " (";
        for (unsigned int chId: NoisyChannels) log << " " << chId;
        log << ")";
      } // if noisy channels
    } // print test
    
    // 2. test the channels as in the configuration
    unsigned int nErrors = 0;
    for (const auto chId: KnownBadChannels) {
      if (!pFilter->BadChannel(chId)) {
        mf::LogError("SimpleChannelFilterTest")
          << "channel #" << chId << " is not bad as it should";
        ++nErrors;
      }
    } // for knwon bad channels
    
    for (const auto chId: KnownNoisyChannels) {
      if (!pFilter->NoisyChannel(chId)) {
        mf::LogError("SimpleChannelFilterTest")
          << "channel #" << chId << " is not noisy as it should";
        ++nErrors;
      }
    } // for knwon noisy channels
    
    for (const auto chId: KnownGoodChannels) {
      if (pFilter->BadChannel(chId)) {
        mf::LogError("SimpleChannelFilterTest")
          << "channel #" << chId << " is bad, while it should not";
        ++nErrors;
      }
      if (pFilter->NoisyChannel(chId)) {
        mf::LogError("SimpleChannelFilterTest")
          << "channel #" << chId << " is noisy, while it should not";
        ++nErrors;
      }
    } // for knwon good channels
    
    return nErrors;
  } // SimpleChannelFilterTest::testObject()
  
  
  DEFINE_ART_MODULE(SimpleChannelFilterTest)
  
} // namespace filter
