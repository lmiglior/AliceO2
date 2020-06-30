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
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include <sstream>
#include <vector>
#include <fstream>


using namespace o2::base;
using namespace o2::detectors;
using o2::itsmft::Digit;

vector <int> getChipDec()
{
  fstream inputDec; //read chip_ID.txt in order to have the number of chip in raw data file
  inputDec.open("/localhome/mft/alice/output_raw/Chip_ID.txt", ios_base::in);
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

vector <string> getMapping()
{
  fstream inputMap; //read file with mapping converted with RU parameteres
  inputMap.open("/localhome/mft/alice/output_raw/mapping.txt",ios_base::in);
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
  inputHistoName.open("/localhome/mft/macro/output_raw/histoname.txt",ios_base::in);
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
  inputCanvas.open("/localhome/mft/alice/O2/macro/output_raw/canvas.txt",ios_base::in);
  vector <string> vecCanvas;
  string word5;
  while(inputCanvas >> word5)
    {
      vecCanvas.push_back(word5);
    }
  inputCanvas.close();
  return vecCanvas;
}

void RawDec(const Char_t *inFile="/localhome/mft/alice/output_raw/digits-data-2020_06_12__03_30_53_STF_200ROF.root"){

  TFile *inputFile =new TFile(inFile);
  vector<int> vecChipDec=getChipDec();
  int sizeVecDec=vecChipDec.size();

  vector<string> vecMap=getMapping();
  vector<string> vecHistoName=getHistoName();
  vector<string> vecCanvas=getCanvas();

  TCanvas *c1[sizeVecDec];
  TH2F *hplot[sizeVecDec];
  char *histoname = new char[sizeVecDec];
  for(int i=0;i<sizeVecDec;i++){
     hplot[i] = new TH2F(histoname,histoname,128,-0.5,1023.5,64,-0.5,511.50);
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
    cout<<nd<<endl;
    while (nd--) {
      const Digit* d = &(*digArr)[nd];
      Int_t chipID = d->getChipIndex();
      for(int j=0;j<=sizeVecDec;j++){
	if (chipID==vecChipDec[j]){
	  hplot[j]->Fill(d->getColumn(), d->getRow());
	}
      }
    }
  }
  for(int k=0;k<sizeVecDec;k++){  // visualisation
    TString os3=vecCanvas[k];
    c1[k]= new TCanvas();
    c1[k]->SetName(os3);
    c1[k]->SetTitle(os3);
    c1[k]->GetName();
    c1[k]->GetTitle();
    c1[k]->cd(k);
    gStyle->SetOptStat(0);
    hplot[k]->Draw("colz");
    std::ostringstream onameh;
    onameh<<"hplot"<<k<<".txt";
    std::ofstream myFile(onameh);
    myFile << "BinNo" << "\t"<< "Start" << "\t"<< "End" << "\t"<< "No" << endl; 
    for (Int_t ii=1; ii<=hplot[k]->GetNbinsX();ii++) {
      myFile << ii << "\t" <<
	(double)(hplot[k]->GetBinCenter(ii))-(double)(myHist->GetBinWidth(ii)/2.) << "\t" <<
	(double)(hplot[k]->GetBinCenter(ii))+(double)(hplot[k]->GetBinWidth(ii)/2.) << "\t" << (double)hplot[k]->GetBinContent(ii) << endl; 
    }
    myFile.close(); 
  }
  

}
