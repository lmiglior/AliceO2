#include <stdio.h>
#include <math.h>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TMath.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TGraph.h"
#include <sstream>
#include <vector>
#include <fstream>


using namespace o2::base;
using namespace o2::detectors;
using o2::itsmft::Digit;

vector <int> getChipDec()
{
  fstream inputDec; //read chip_ID.txt in order to have the number of chip in raw data file
  inputDec.open("/home/o2flp/alice/output_raw/Chip_ID.txt", ios_base::in);
  vector <int> vecChipDec;
  int ChipDecod;
  int sizeVecDec=vecChipDec.size();
  while(inputDec >> ChipDecod)
    {
      vecChipDec.push_back(ChipDecod);
    }
  for(int n=0;n<sizeVecDec;n++)
    cout << vecChipDec[n] << endl;
  inputDec.close();
  return vecChipDec;
}

vector<int> getTrans()
{
  fstream inputTrans; //read chip_ID.txt in order to have the number of chip in raw data file
  inputTrans.open("/home/o2flp/alice/output_raw/trans.txt", ios_base::in);
  vector <int> vecChipTrans;
  int ChipTransod;
  int sizeVecTrans=vecChipTrans.size();
  while(inputTrans >> ChipTransod)
    {
      vecChipTrans.push_back(ChipTransod);
    }
  for(int n=0;n<sizeVecTrans;n++)
    cout << vecChipTrans[n] << endl;
  inputTrans.close();
  return vecChipTrans;

}

vector <string> getMapping()
{
  fstream inputMap; //read file with mapping converted with RU parameteres
  inputMap.open("/home/o2flp/alice/output_raw/mapping.txt",ios_base::in);
  vector <string> vecMap;
  string word;
  while(inputMap >> word)
    {
      vecMap.push_back(word);
    }
inputMap.close();
  return vecMap;

}

vector <string> getHistoName()
{
  fstream inputHistoName; //read file with namehisto
  inputHistoName.open("/home/o2flp/alice/output_raw/histoname.txt",ios_base::in);
  vector <string> vecHistoName;
  string word2;
  while(inputHistoName >> word2)
    {
      vecHistoName.push_back(word2);
    }
  inputHistoName.close();
  return vecHistoName;

}


vector <string> getCanvas()
{
  fstream inputCanvas; //read file with canvas
  inputCanvas.open("/home/o2flp/alice/output_raw/canvas.txt",ios_base::in);
  vector <string> vecCanvas;
  string word5;
  while(inputCanvas >> word5)
    {
      vecCanvas.push_back(word5);
    }
  inputCanvas.close();
  return vecCanvas;
}

void RawDec(const Char_t *inFile="/home/o2flp/alice/output_raw/fd_ROF250_200707_161243.root",const Int_t vpulselow=50){

  TFile *inputFile =new TFile(inFile);
  vector<int> vecChipDec=getChipDec();
  int sizeVecDec=vecChipDec.size();
  std::cout<<"How many chips? "<<sizeVecDec<<std::endl;
  vector<string> vecMap=getMapping();
  vector<string> vecHistoName=getHistoName();
  vector<string> vecCanvas=getCanvas();
  Int_t vpulsel=vpulselow;
  vector<int> vecTrans=getTrans();
  TCanvas *c1[sizeVecDec];
  TH2F *hplot[sizeVecDec];
  char *histoname = new char[sizeVecDec];
  //double binx,biny;

  for(int i=0;i<sizeVecDec;i++){
    hplot[i] = new TH2F(histoname,histoname,1024,0,1024,512,0,512);
    TString os1=vecMap[i];
    TString os2=vecHistoName[i];
    hplot[i]->SetName(os2);
    hplot[i]->SetTitle(os1);
     hplot[i]->GetName();
     hplot[i]->GetTitle();
 }
  
  TTree *tree=(TTree*)inputFile->Get("o2sim");
  auto nentries=tree->GetEntries();
  std::vector<o2::itsmft::Digit>* digArr = nullptr;
  tree->SetBranchAddress("MFTDigit", &digArr);
  for(int i=0;i<nentries;i++){
    tree->GetEvent(i);
    Int_t nd = digArr->size();
    cout<<"Events? "<<nd<<endl;
   
    while (nd--) {
    Int_t binx,biny, bin, binmax;
      const Digit* d = &(*digArr)[nd];
      Int_t chipID = d->getChipIndex();
      for(int j=0;j<=sizeVecDec;j++){
	if (chipID==vecChipDec[j]){
	  hplot[j]->Fill(d->getColumn(), d->getRow());
	}
      }
    }
  }
  ofstream output;
  output.open ("digit_coordinate.txt");
  Int_t bin;
  for(int k=0;k<sizeVecDec;k++){ // to store the coordinate for the thr scan
    int kk = vecTrans[k];
  	for(int binx=0;binx<=hplot[k]->GetNbinsX();binx++){ 
  	  for(int biny=0;biny<=hplot[k]->GetNbinsY();biny++){
  	    bin=hplot[k]->GetBinContent(binx,biny);
  	    if(bin!=0)output<<kk<<" "<<binx-1<<" "<<biny-1<<" "<<bin-1<<" "<<vpulsel<<"\n";
	    
  	  }
  	}
      }
      output.close();

}
