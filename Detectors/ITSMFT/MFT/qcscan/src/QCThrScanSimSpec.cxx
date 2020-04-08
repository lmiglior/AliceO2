// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// @file   QCThrScanSimSpec.cxx

#include <vector>

#include <TRandom.h>

#include "MFTQcscan/QCThrScanSimSpec.h"

#include "TTree.h"
#include "Framework/ControlService.h"
#include "Framework/ConfigParamRegistry.h"
#include "DataFormatsITSMFT/Digit.h"
#include "SimulationDataFormat/MCCompLabel.h"
#include "SimulationDataFormat/MCTruthContainer.h"
#include "DataFormatsITSMFT/ROFRecord.h"

using namespace o2::framework;
using namespace o2::itsmft;

namespace o2
{
namespace mft
{

void QCThrScanSim::init(framework::InitContext& ic)
{
  LOG(INFO) << " In SimpleThresholdScanSimulatorMFT::init ... entering ";
  
  // read options defining the scan
  mMinDAC = ic.options().get<float>("mft-min-dac");
  mMaxDAC = ic.options().get<float>("mft-max-dac");
  mMaxInject = ic.options().get<float>("mft-max-inj");    
  mScannedChip = ic.options().get<int>("mft-scanned-chip");
  mNScannedRows = ic.options().get<int>("mft-n-scanned-rows");
  mFirstScannedRow = ic.options().get<int>("mft-first-scanned-row");    
  
  // read options defining the S-curve
  mScanSigma = ic.options().get<float>("mft-scan-sig");
  mScanThreshold = ic.options().get<float>("mft-scan-thr");        
  
  // initialise scan
  mCurDAC = mMinDAC;
  mCurInject = 0.; // number of injections per DAC level

  LOG(INFO) << "Will store in " << mFileName << ":";
  LOG(INFO) << "Tree " << mTreeNameDig << " with MFT digits";
  
  mOutFile = std::make_unique<TFile>(mFileName.c_str(), "RECREATE");
  if (!mOutFile || mOutFile->IsZombie()) {
    LOG(ERROR) << "Failed to open " << mFileName << " output file";
  } else {
    LOG(INFO) << "Opened " << mFileName << " output file";
  }

  mState = 1;
}

void QCThrScanSim::run(ProcessingContext& pc)
{
  if (mState != 1)
    return;

  // get expected value from ScanFunction
  float q = scanFunction(mCurDAC, mScanSigma, mScanThreshold, mMaxInject);

  // vector of digits in this ROF
  std::vector<o2::itsmft::Digit> digits, *digitsPtr = &digits;;

  TTree tree("o2sim", "Tree with MFT scan digits");
  tree.Branch("MFTDigit", &digits);
  
  // loop over rows and columns
  // and if needed create a digit and add it to the ROF
  for(unsigned short iRow = 0; iRow < mNScannedRows; iRow++) {
    unsigned short row = iRow + mFirstScannedRow;
    for(unsigned short iCol = 0; iCol < 1024; iCol++) {
      // get random number to decide if digit will be created
      if (q < gRandom->Uniform(0, mMaxInject)) continue;
      // create digit and emplace it back
      Digit dig(mScannedChip, row, iCol, mCurDAC);
      digits.emplace_back(dig);
    }
  }

  // fill in the output
  LOG(INFO) << " SimpleThresholdScanSimulatorMFT::run. In this ROF there are  " << digits.size() << " digits";

  //pc.outputs().snapshot(Output{ "MFT", "DIGITS", 0, Lifetime::Timeframe }, digits);
 
  // prepare for next injection
  if (mCurInject < mMaxInject) {
    mCurInject++;
  } else {
    mCurInject = 0;
    mCurDAC++;
  }

  // finish scan when maxDAC is reached
  if (mCurDAC == mMaxDAC) {
    tree.Fill();
    tree.Write();
    tree.Reset(); // delete the trees before closing the file
    mOutFile->Close();
    mState = 2;
    
    LOG(INFO) << " SimpleThresholdScanSimulatorMFT::run. End of scan reached";
    pc.services().get<ControlService>().endOfStream();
    pc.services().get<ControlService>().readyToQuit(QuitRequest::Me);
    return;
  }
}

DataProcessorSpec getQCThrScanSimSpec()
{
  std::vector<OutputSpec> outputs;
  outputs.emplace_back("MFT", "DIGITS", 0, Lifetime::Timeframe);

  return DataProcessorSpec{
    "simple-threshold-scan-simulator-mft",
    Inputs{},
    outputs,
    AlgorithmSpec{ adaptFromTask<QCThrScanSim>() },
    Options{
      {"mft-min-dac", VariantType::Float, 0.0f, {"minimum DAC in scan"}},
      {"mft-max-dac", VariantType::Float, 50.0f, {"maximum DAC in scan" }},
      {"mft-max-inj", VariantType::Float, 50.0f, {"number of injections per DAC in scan"}},
      {"mft-scanned-chip", VariantType::Int, 0, {"index of chip being tested"}},
      {"mft-n-scanned-rows", VariantType::Int, 1, {"how many subsequent rows"}},
      {"mft-first-scanned-row", VariantType::Int, 0, {"index of the first row"}},
      {"mft-scan-sig", VariantType::Float, 3.0f, {"sigma parameter of S-curve"}},
      {"mft-scan-thr", VariantType::Float, 25.0f, {"threshold parameter of S-curve"}}}};
}
  
} // namespace mft
} // namespace o2
