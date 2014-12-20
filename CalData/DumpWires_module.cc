/**
 * @file   DumpWires_module.cc
 * @brief  Dumps on screen the content of the wires
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 31st, 2014
 */

// C//C++ standard libraries
#include <string>
#include <algorithm> // std::min(), std::copy_n()
#include <ios> // std::fixed
#include <iomanip> // std::setprecision(), std::setw()
#include <memory> // std::unique_ptr<>

// support libraries
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// art libraries
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

// LArSoft includes
#include "SimpleTypesAndConstants/geo_types.h"
// #include "RawData/RawDigit.h"
#include "RecoBase/Wire.h"
#include "Filters/ChannelFilter.h"

namespace {
	
	/**
	 * @brief Records the minimum and maximum of a value
	 * @param T type of the values to record
	 */
	template <typename T>
	class ExtremaAccumulator {
			public:
		using Data_t = T; ///< type of the extrema to be accumulated
		
		/// Default constructor: nothing accumulated yet
		ExtremaAccumulator(): minimum(), maximum(), n(0) {}
		
		///@{
		///@name Accessors
		Data_t samples() const { return n; }
		Data_t min() const { return hasStats()? minimum: T(); }
		Data_t max() const { return hasStats()? maximum: T(); }
		///@}
		
		/// Returns whether this object has accumulated any statistics
		bool hasStats() const { return samples() > 0; }
		
		/// Returns whether all the accumulated objects have the same value
		bool isConstant() const { return !hasStats() || (min() == max()); }
		
		/**
		 * @brief Adds a single value to the sample
		 * @param value the value to be recorded
		 * @return this object
		 */
		ExtremaAccumulator& add(const Data_t& value)
			{
				if (n == 0) minimum = maximum = value;
				else {
					if (minimum > value) minimum = value;
					else if (maximum < value) maximum = value;
				}
				++n;
				return *this;
			} // add(Data_t)
		
		/**
		 * @brief Adds a sequence of values to the sample
		 * @param ITER type of iterator
		 * @param begin (constant) iterator pointing to the first element
		 * @param end (constant) iterator pointing after the last element
		 * @return this object
		 */
		template <typename ITER>
		ExtremaAccumulator& add(ITER begin, ITER end)
			{
				for (ITER iter = begin; iter != end; ++iter) add(*iter);
				return *this;
			} // add(ITER)
		
			private:
		Data_t minimum, maximum; ///< extrema recorded so far
		unsigned int n; ///< number of recorded values
	}; // class ExtremaAccumulator<>
	
} // local namespace


namespace caldata {

  /**
   * @brief Prints the content of all the wires on screen
   *
   * This analyser prints the content of all the wires into the
   * LogInfo/LogVerbatim stream.
   * 
   * <b>Configuration parameters</b>
   * 
   * - <b>CalWireModuleLabel</b> (string, default: "caldata"): label of the
   *   producer used to create the recob::Wire collection to be dumped
   * - <b>DetSimModuleLabel</b> (string, default: "daq"): label of the
   *   producer used to create the raw::RawDigits collection the wires are based
   *   on (not used)
   * - <b>OutputCategory</b> (string, default: "DumpWires"): the category used
   *   for the output (useful for filtering)
   * - <b>DigitsPerLine</b> (integer, default: 20): the dump of digits and ticks
   *   will put this many of them for each line
   * - <b>IgnoreFilters</b> (boolean, default: false): if true, channel filters
   *   will be ignored; by default, only wires on channels that are not bad are
   *   printed out
   *
   */
  class DumpWires : public art::EDAnalyzer {
      public:
    
    /// Default constructor
    explicit DumpWires(fhicl::ParameterSet const& pset); 
    
    /// Does the printing
    void analyze (const art::Event& evt);

      private:

    std::string fCalWireModuleLabel; ///< name of module that produced the wires
    std::string fDetSimModuleLabel; ///< name of module that produced the digits
    std::string fOutputCategory; ///< category for LogInfo output
    unsigned int fDigitsPerLine; ///< ticks/digits per line in the output
    bool bIgnoreFilters; ///< use all the wires, don't filter them

  }; // class DumpWires

} // End caldata namespace.


namespace caldata {

  //-------------------------------------------------
  DumpWires::DumpWires(fhicl::ParameterSet const& pset) 
    : EDAnalyzer         (pset) 
    , fCalWireModuleLabel(pset.get<std::string>("CalWireModuleLabel", "caldata"))
    , fDetSimModuleLabel (pset.get<std::string>("DetSimModuleLabel", "daq"))
    , fOutputCategory    (pset.get<std::string>("OutputCategory", "DumpWires"))
    , fDigitsPerLine     (pset.get<unsigned int>("DigitsPerLine", 20))
    , bIgnoreFilters     (pset.get<bool>("IgnoreFilters", false))
    {}


  //-------------------------------------------------
  void DumpWires::analyze(const art::Event& evt) {
    
    // fetch the data to be dumped on screen
  //  art::ValidHandle<std::vector<recob::RawDigit>> Digits
  //    = evt.getValidHandle<std::vector<recob::RawDigit>>(fDetSimModuleLabel);
    art::ValidHandle<std::vector<recob::Wire>> Wires
      = evt.getValidHandle<std::vector<recob::Wire>>(fCalWireModuleLabel);
    
    // channel filter: create one only if requested
    std::unique_ptr<filter::ChannelFilter> filter;
    if (!bIgnoreFilters) {
      filter.reset(new filter::ChannelFilter);
    }
    
    mf::LogInfo(fOutputCategory) << "The event contains " << Wires->size() << " wires";
    
    // a portable version of code dumping a wire is in
    // uboonecode/uboone/CalWireROI_module.cc
    
    std::vector<float> DigitBuffer(fDigitsPerLine), LastBuffer;
    for (const recob::Wire& wire: *Wires) {
      const recob::Wire::RegionsOfInterest_t& RoIs = wire.SignalROI();
      
      // print a header for the wire
      { // limit the scope of out:
        mf::LogVerbatim out(fOutputCategory);
        out << "  #" << wire.Channel() << ":";
        if (filter && filter->BadChannel(wire.Channel())) {
          out << " bad channel";
          continue;
        }
        switch (wire.View()) {
          case geo::kU:       out << " on U plane"; break;
          case geo::kV:       out << " on V plane"; break;
          case geo::kZ:       out << " on Z plane"; break;
          case geo::k3D:      out << " in space"; break;
          case geo::kUnknown: out << " in hyperspace"; break;
        } // switch view
        out << "; " << wire.NSignal() << " time ticks";
        if (wire.NSignal() != RoIs.size())
          out << " [!!! EXPECTED " << RoIs.size() << "]";
        if (RoIs.n_ranges() == 0) {
          out << " with nothing in them";
          continue;
        }
        out << " with " << RoIs.n_ranges() << " regions of interest:";
      } // block
      
      // print the list of regions of interest
      for (const lar::sparse_vector<double>::datarange_t& RoI: RoIs.get_ranges())
      {
        mf::LogVerbatim(fOutputCategory)
          << "    from " << RoI.offset << " for " << RoI.size() << " ticks";
      } // for
      
      // print the content of the wire
      if (fDigitsPerLine > 0) {
        unsigned int repeat_count = 0; // additional lines like the last one
        unsigned int index = 0;
        LastBuffer.clear();
        ExtremaAccumulator<float> Extrema;
        mf::LogVerbatim(fOutputCategory)
          << "  content of the wire (" << fDigitsPerLine << " ticks per line):";
        recob::Wire::RegionsOfInterest_t::const_iterator iTick = RoIs.begin(),
          tend = RoIs.end();
        while (iTick != tend) {
          // the next line will show at most fDigitsPerLine ticks
          unsigned int line_size
            = std::min(fDigitsPerLine, (unsigned int) RoIs.size() - index);
          if (line_size == 0) break; // no more ticks
          
          // fill the new buffer (iTick will move forward)
          DigitBuffer.resize(line_size);
          std::vector<float>::iterator iBuf = DigitBuffer.begin(),
            bend = DigitBuffer.end();
          while ((iBuf != bend) && (iTick != tend))
            Extrema.add(*(iBuf++) = *(iTick++));
          index += line_size;
          
          // if the new buffer is the same as the old one, just mark it
          if (DigitBuffer == LastBuffer) {
            repeat_count += 1;
            continue;
          }
          
          // if there are previous repeats, write that on screen
          // before the new, different line
          if (repeat_count > 0) {
            mf::LogVerbatim(fOutputCategory)
              << "      [ ... repeated " << repeat_count << " more times ]";
            repeat_count = 0;
          }
          
          // dump the new line of ticks
          mf::LogVerbatim line_out(fOutputCategory);
          line_out << "   " << std::fixed << std::setprecision(3);
          for (float digit: DigitBuffer) line_out << std::setw(8) << digit;
          
          // quick way to assign DigitBuffer to LastBuffer
          // (we don't care we lose the former)
          std::swap(LastBuffer, DigitBuffer);
          
        } // while
        if (repeat_count > 0) {
          mf::LogVerbatim(fOutputCategory)
            << "      [ ... repeated " << repeat_count << " more times to the end]";
        }
        if (!Extrema.isConstant()) {
          mf::LogVerbatim(fOutputCategory)
            << "    range of " << Extrema.samples()
            << " samples: [" << Extrema.min() << ";" << Extrema.max() << "]";
        }
      } // if dumping the ticks
      
    } // for wire
    
  } // DumpWires::analyze()

  DEFINE_ART_MODULE(DumpWires)

} // namespace caldata
