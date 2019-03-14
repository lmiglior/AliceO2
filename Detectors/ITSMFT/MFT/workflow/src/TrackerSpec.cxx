// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// @file   TrackerSpec.cxx

#include "MFTWorkflow/TrackerSpec.h"
#include "MFTTracking/ROframe.h"
#include "MFTBase/GeometryTGeo.h"

#include <vector>

#include "TGeoGlobalMagField.h"

#include "Framework/ControlService.h"
#include "DataFormatsITSMFT/CompCluster.h"
#include "DataFormatsITSMFT/Cluster.h"
#include "DataFormatsMFT/TrackMFT.h"
#include "SimulationDataFormat/MCCompLabel.h"
#include "SimulationDataFormat/MCTruthContainer.h"
#include "DataFormatsITSMFT/ROFRecord.h"
#include "Field/MagneticField.h"
#include "DetectorsBase/GeometryManager.h"
#include "DetectorsBase/Propagator.h"

using namespace o2::framework;

namespace o2
{
namespace MFT
{

void TrackerDPL::init(InitContext& ic)
{
  auto filename = ic.options().get<std::string>("grp-file");
  const auto grp = o2::parameters::GRPObject::loadFrom(filename.c_str());
  if (grp) {
    mGRP.reset(grp);
    o2::Base::Propagator::initFieldFromGRP(grp);
    auto field = static_cast<o2::field::MagneticField*>(TGeoGlobalMagField::Instance()->GetField());

    o2::Base::GeometryManager::loadGeometry();
    o2::MFT::GeometryTGeo* geom = o2::MFT::GeometryTGeo::Instance();
    geom->fillMatrixCache(o2::utils::bit2Mask(o2::TransformType::T2L, o2::TransformType::T2GRot,
                                              o2::TransformType::T2G));

    mTracker = std::make_unique<o2::MFT::Tracker>();
    double origD[3] = { 0., 0., 0. };
    mTracker->setBz(field->getBz(origD));
  } else {
    LOG(ERROR) << "Cannot retrieve GRP from the " << filename.c_str() << " file !";
    mState = 0;
  }
  mState = 1;
}
  
void TrackerDPL::run(ProcessingContext& pc)
{
  if (mState != 1)
    return;

  auto compClusters = pc.inputs().get<const std::vector<o2::ITSMFT::CompClusterExt>>("compClusters");
  auto clusters = pc.inputs().get<const std::vector<o2::ITSMFT::Cluster>>("clusters");
  auto labels = pc.inputs().get<const o2::dataformats::MCTruthContainer<o2::MCCompLabel>*>("labels");
  auto rofs = pc.inputs().get<const std::vector<o2::ITSMFT::ROFRecord>>("ROframes");

  LOG(INFO) << "MFTTracker pulled " << clusters.size() << " clusters, "
            << labels->getIndexedSize() << " MC label objects , in "
            << rofs.size() << " RO frames";

  std::vector<o2::MFT::TrackMFT> tracks;
  o2::dataformats::MCTruthContainer<o2::MCCompLabel> trackLabels;
  std::vector<o2::MFT::TrackMFT> allTracks;
  o2::dataformats::MCTruthContainer<o2::MCCompLabel> allTrackLabels;

  std::uint32_t roFrame = 0;
  o2::MFT::ROframe event(0);

  bool continuous = mGRP->isDetContinuousReadOut("MFT");
  LOG(INFO) << "MFTTracker RO: continuous=" << continuous;

  if (continuous) {
  } else {
  }

  LOG(INFO) << "MFTTracker pushed " << allTracks.size() << " tracks";
  pc.outputs().snapshot(Output{ "MFT", "TRACKS", 0, Lifetime::Timeframe }, allTracks);
  pc.outputs().snapshot(Output{ "MFT", "TRACKSMCTR", 0, Lifetime::Timeframe }, allTrackLabels);

  mState = 2;
  pc.services().get<ControlService>().readyToQuit(true);
}

DataProcessorSpec getTrackerSpec()
{
  return DataProcessorSpec{
    "its-tracker",
    Inputs{
      InputSpec{ "compClusters", "MFT", "COMPCLUSTERS", 0, Lifetime::Timeframe },
      InputSpec{ "clusters", "MFT", "CLUSTERS", 0, Lifetime::Timeframe },
      InputSpec{ "labels", "MFT", "CLUSTERSMCTR", 0, Lifetime::Timeframe },
      InputSpec{ "ROframes", "MFT", "MFTClusterROF", 0, Lifetime::Timeframe } },
    Outputs{
      OutputSpec{ "MFT", "TRACKS", 0, Lifetime::Timeframe },
      OutputSpec{ "MFT", "TRACKSMCTR", 0, Lifetime::Timeframe } },
    AlgorithmSpec{ adaptFromTask<TrackerDPL>() },
    Options{
      { "grp-file", VariantType::String, "o2sim_grp.root", { "Name of the output file" } },
    }
  };
}

} // namespace MFT
} // namespace o2
