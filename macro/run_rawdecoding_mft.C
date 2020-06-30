#if !defined(__CLING__) || defined(__ROOTCLING__)

#include <TTree.h>
#include <TFile.h>
#include <TStopwatch.h>
#include <FairLogger.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>


#include "ITSMFTReconstruction/ChipMappingMFT.h"
#include "ITSMFTReconstruction/GBTWord.h"
#include "ITSMFTReconstruction/PayLoadCont.h"
#include "ITSMFTReconstruction/PixelData.h"
#include "DataFormatsITSMFT/ROFRecord.h"
#include "ITSMFTReconstruction/RawPixelReader.h"
#include "CommonDataFormat/InteractionRecord.h"

// STF version
#include "ITSMFTReconstruction/RUDecodeData.h"

#endif

// example of MFT raw data decoding
// Data can be prepared from the MC digits using run_digi2raw_mft.C
// The padding parameter should be set to "true" for CRU data and to "false" for
// the data obtained by the removing the 128 bit padding from GBT words

void run_rawdecoding_mft(std::string inpName = "rawmft.bin", // input binary data file name
                         std::string outDigName = "raw2mftdigits.root",// name for optinal digits tree
			 int complete=0,
			 int ROF=200,
			 bool padding = true,                // payload in raw data comes in 128 bit CRU words
                         bool page8kb = true,                // full 8KB CRU pages are provided (no skimming applied)
                         int nTriggersToCache = 1025,        // number of triggers per link to cache (> N 8KB CRU pages per superpage)
                         int verbose = 0)
{
  
  //  inpName = "/localhome/mft/alice/data-2020_06_12__03_30_53__.raw";
  // outDigName = "/localhome/mft/alice/O2/macro/output_raw/digits-data-2020_06_12__03_30_53_STF_200ROF.root";

  o2::itsmft::RawPixelReader<o2::itsmft::ChipMappingMFT> rawReader;
  rawReader.openInput(inpName);
  rawReader.setPadding128(padding); // payload GBT words are padded to 16B
  rawReader.imposeMaxPage(page8kb); // pages are 8kB in size (no skimming)
  rawReader.setMinTriggersToCache(nTriggersToCache);
  rawReader.setVerbosity(verbose);
  
  o2::itsmft::ChipPixelData chipData;
  TStopwatch sw;
  sw.Start();
  uint32_t roFrame = 0;
  o2::InteractionRecord irHB, irTrig;
  std::vector<o2::itsmft::Digit> digits, *digitsPtr = &digits;
  std::vector<o2::itsmft::ROFRecord> rofRecVec, *rofRecVecPtr = &rofRecVec;
  std::size_t rofEntry = 0, nrofdig = 0;
  std::unique_ptr<TFile> outFileDig;
  std::unique_ptr<TTree> outTreeDig; // output tree with digits

  std::vector<int> tab_ID;

  
  if (!outDigName.empty()) { // output to digit is requested
    outFileDig = std::make_unique<TFile>(outDigName.c_str(), "recreate");
    outTreeDig = std::make_unique<TTree>("o2sim", "Digits tree");
    outTreeDig->Branch("MFTDigit", &digitsPtr);
    outTreeDig->Branch("MFTDigitROF", &rofRecVecPtr);
  }
  
  while (rawReader.getNextChipData(chipData)) {
    if (verbose >= 10) {
      chipData.print();
    }
    
    if (outTreeDig) { // >> store digits
      if (irHB != rawReader.getInteractionRecordHB() || irTrig != rawReader.getInteractionRecord()) {
        if (!irTrig.isDummy()) {
          rofRecVec.emplace_back(irHB, roFrame, rofEntry, nrofdig); // registed finished ROF
  roFrame++;
}
irHB = rawReader.getInteractionRecordHB();
irTrig = rawReader.getInteractionRecord();
rofEntry = digits.size();
nrofdig = 0;
      }
      const auto& pixdata = chipData.getData();
      for (const auto& pix : pixdata) {
//printf("BV: run... ChipID %d row %d col %d \n", chipData.getChipID(), pix.getRowDirect(), pix.getCol());
digits.emplace_back(chipData.getChipID(), pix.getRowDirect(), pix.getCol());
nrofdig++;
      }
      
      printf("BV: run ... ROF %d ch: %d IR: ", roFrame, chipData.getChipID());
      irHB.print();

    int iterChip=0;
 
    for(int n=0;n<tab_ID.size();n++)
      if(tab_ID[n]!=chipData.getChipID())
	iterChip++;

    if(iterChip==tab_ID.size())
      tab_ID.push_back(chipData.getChipID());
  } // << store digits
    //
  std::fstream output;
  output.open("/localhome/mft/alice/output_raw/Chip_ID.txt", std::ios::out);

  for(int n=0;n<tab_ID.size();n++)
    output << tab_ID[n] << std::endl;

  output.close(); 
  if(complete==0){  if (roFrame > ROF) break;}  
  }

  if (outTreeDig) {
    // register last ROF
    rofRecVec.emplace_back(irHB, roFrame, rofEntry, nrofdig); // registed finished ROF

    // fill last (and the only one?) entry
    outTreeDig->Fill();

    // and store trees
    outTreeDig->Write();
  }

  sw.Stop();

  const auto& MAP = rawReader.getMapping();
  for (int ir = 0; ir < MAP.getNRUs(); ir++) {
    for (int il = 0; il < o2::itsmft::RUDecodeData::MaxLinksPerRU; il++) {
      const auto ruStat = rawReader.getRUDecodingStatSW(ir, il);
      if (ruStat && ruStat->nPackets) {
        printf("\nStatistics for RU%3d (HWID:0x%4x) GBTLink%d\n", ir, MAP.RUSW2FEEId(ir, il), il);
        ruStat->print();
      }
    }
  }
  rawReader.getDecodingStat().print();

  sw.Print();
}
