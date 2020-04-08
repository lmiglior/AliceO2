// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// @file   QCThrScanSimSpec.h

#ifndef O2_MFT_QCTHRSCANSIM_H_
#define O2_MFT_QCTHRSCANSIM_H_

#include "Framework/DataProcessorSpec.h"
#include "Framework/Task.h"

#include <TMath.h>

namespace o2
{
namespace mft
{

class QCThrScanSim : public o2::framework::Task
{
 public:
  QCThrScanSim() = default;
  ~QCThrScanSim() override = default;
  void init(o2::framework::InitContext& ic) final;
  void run(o2::framework::ProcessingContext& pc) final;

 private:
  float scanFunction(float q, float q_sig, float q_thr, float fmax) const;

  // read options defining the scan
  float mMinDAC;
  float mMaxDAC;
  float mMaxInject;    
  float mCurDAC;
  float mCurInject;
  int mScannedChip;
  int mNScannedRows;
  int mFirstScannedRow;
  
  // read options defining the S-curve
  float mScanSigma;
  float mScanThreshold;
  int mState = 0;
  std::unique_ptr<TFile> mFile = nullptr;
  std::string mFileName = "digits.root";  // output file name
  std::string mTreeNameDig = "o2sim";     // tree name for digits
  std::unique_ptr<TFile> mOutFile;
};

/// create a processor spec
/// ...
o2::framework::DataProcessorSpec getQCThrScanSimSpec();

inline float QCThrScanSim::scanFunction(float q, float q_sig, float q_thr, float fmax) const
{
  // definition of S-curve
  constexpr double sqrt_two = 1.4142136;
  return (0.5 * fmax * (1.0 + TMath::Erf((q - q_thr) / (q_sig * sqrt_two))));
}

} // namespace mft
} // namespace o2

#endif /* O2_MFT_QCTHRSCANSIM */
