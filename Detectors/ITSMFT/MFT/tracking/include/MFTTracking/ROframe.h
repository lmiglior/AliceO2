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
/// \file ROframe.h
/// \brief
///

#ifndef O2_MFT_ROFRAME_H_
#define O2_MFT_ROFRAME_H_

namespace o2
{
namespace MFT
{

class ROframe final
{
 public:
  ROframe(int ROframeId);
  int getROFrameId() const;
  void clear();

 private:
  const int mROframeId;
};

inline int ROframe::getROFrameId() const { return mROframeId; }

inline void ROframe::clear()
{
}

} // namespace MFT
} // namespace o2

#endif /* O2_MFT_ROFRAME_H_ */
