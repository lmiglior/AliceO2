// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
///
/// \file Tracker.h
/// \brief
///

#ifndef O2_MFT_TRACKER_H_
#define O2_MFT_TRACKER_H_

#include "MFTTracking/ROframe.h"
#include "DataFormatsMFT/TrackMFT.h"
#include "SimulationDataFormat/MCCompLabel.h"
#include "SimulationDataFormat/MCTruthContainer.h"

namespace o2
{
namespace MFT
{

class Tracker
{

 public:
  Tracker();

  Tracker(const Tracker&) = delete;
  Tracker& operator=(const Tracker&) = delete;
  ~Tracker();

  void setBz(float bz) { mBz = bz; }
  float getBz() const { return mBz; }

  std::vector<TrackMFT>& getTracks();
  o2::dataformats::MCTruthContainer<MCCompLabel>& getTrackLabels();

  void clustersToTracks(const ROframe&, std::ostream& = std::cout);

  void setROFrame(std::uint32_t f) { mROFrame = f; }
  std::uint32_t getROFrame() const { return mROFrame; }

 private:
  void findTracks(const ROframe& ev);
  float mBz = 5.f;
  std::uint32_t mROFrame = 0;
  std::vector<TrackMFT> mTracks;
  o2::dataformats::MCTruthContainer<MCCompLabel> mTrackLabels;
};

inline std::vector<TrackMFT>& Tracker::getTracks()
{
  return mTracks;
}

inline o2::dataformats::MCTruthContainer<MCCompLabel>& Tracker::getTrackLabels()
{
  return mTrackLabels;
}

} // namespace MFT
} // namespace o2

#endif /* O2_MFT_TRACKER_H_ */
