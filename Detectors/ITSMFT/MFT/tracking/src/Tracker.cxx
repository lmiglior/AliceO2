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
/// \file Tracker.cxx
/// \brief
///

#include "MFTTracking/Tracker.h"

#include "ReconstructionDataFormats/Track.h"

namespace o2
{
namespace MFT
{

Tracker::Tracker()
{
  //assert(mTracks != nullptr);
}

Tracker::~Tracker()
{
}

void Tracker::clustersToTracks(const ROframe& event, std::ostream& timeBenchmarkOutputStream)
{
  mTracks.clear();
  mTrackLabels.clear();

  findTracks(event);
}

void Tracker::findTracks(const ROframe& event)
{
}

} // namespace MFT
} // namespace o2
