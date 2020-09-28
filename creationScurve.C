#include <glob.h> // glob(), globfree()
#include <string.h> // memset()
#include <vector>
#include <chrono>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <cmath>
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
#include <ctime>
#include <algorithm> 
#include "TString.h"
using namespace std;
using namespace std::chrono;
bool SortFunc (int i,int j)
{
 return (i<j); 
}

void CalculateDerivative(std::vector<std::pair<uint16_t,uint16_t>> data, std::vector<std::pair<uint16_t,float>> &derivative){
    
  float derive;
  uint16_t previousX, xs;
  uint16_t previousY, ys;
  int count = 0;
    
  for(auto it=data.begin();it!=data.end();it++){ // loop over measurements at given VPULSE
    xs =it->first;
    ys =it->second;
	if(count==0) {
            previousX = xs;
            previousY = ys;
            count++;
            continue;
	  }
        if ((xs - previousX) !=0) derive = abs((ys - previousY) / (xs - previousX));
        else derive = 0.;
  
        derivative.push_back(std::make_pair((xs + previousX)/2.,derive));
        previousX = xs;
        previousY = ys;
        count++;
  }
}

void AnalysisData(std::vector<std::pair<uint16_t,float>> data, double& mean, double& sdtmean,double& noise, double& sdtnoise)
// data = données pour un seul pixel
{ // returns the weighted mean x value
  double  sum  = 0.0;
  double  norm = 0.0;
  double xs, ys;
  for(auto it=data.begin();it!=data.end();it++){ // loop over measurements at given VPULSE
    xs =it->first;
    ys =it->second;
    sum += xs * ys; // xs=value, ys=weight
    norm += ys;
  }
  if (norm == 0) { // dead pixel
    mean = -1;
    return;
  }
  if (abs(sum / norm) > 500) { // outliers occur when a pixel is received twice; return 0.
    mean = 0;
    return;
  }
  mean = sum / norm;
  sdtmean=sqrt((sum*sum/norm)- pow(mean, 2));
  sum = 0.0;

  for(auto it=data.begin();it!=data.end();it++){ // loop over measurements at given VPULSE
    xs =it->first;
    ys =it->second;
    sum += ys * (xs - mean) * (xs - mean);
  }
  if (sqrt(abs(sum / norm)) > 500) {
    noise = 0.;
    return;
  }
  noise = sqrt(abs(sum / norm));
  sdtnoise=sqrt(sqrt(abs(sum*sum/norm))- pow(noise, 2));
}
vector<string> glob(const string& pattern) { //import files function  
  glob_t glob_result;
  memset(&glob_result, 0, sizeof(glob_result));
  int return_value = glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
  if(return_value != 0) {
    globfree(&glob_result);
    stringstream ss;
    ss << "glob() failed with return_value " << return_value << endl;
    throw std::runtime_error(ss.str());
  }
  vector<string> filenames;
  for(size_t i = 0; i < glob_result.gl_pathc; ++i) filenames.push_back(string(glob_result.gl_pathv[i]));
  
  globfree(&glob_result);
  return filenames;
}
vector <int> getChipDec()
{
   fstream inputDec; //read chip_ID.txt in order to have the number of chip in raw data file
  inputDec.open("/home/o2flp/alice/output_raw/trans.txt", ios_base::in);
  vector <int> vecChipDec;
  int ChipDecod;
  int sizeVecDec=vecChipDec.size();
  while(inputDec >> ChipDecod) vecChipDec.push_back(ChipDecod);
  inputDec.close();
  return vecChipDec;
}
uint32_t GetPixelId(uint16_t transId,uint16_t col, uint16_t row)
{
  return(col+(row<<10)+(transId<<19));
}
uint16_t GetTransceiverId(uint32_t pixelId)
{
  return((pixelId>>19)& 0x1F);
}
uint16_t GetRow(uint32_t pixelId)
{
  return((pixelId>>10)& 0x1FF);
}
uint16_t GetCol(uint32_t pixelId)
{
  return(pixelId & 0x3FF);
}

vector <string> getMapping()
{
  fstream inputMap; //read file with mapping converted with RU parameteres
  inputMap.open("/home/o2flp/alice/output_raw/canvas.txt",ios_base::in);
  vector <string> vecMap;
  string word;
  while(inputMap >> word)
    {
      vecMap.push_back(word);
    }
  inputMap.close();
  return vecMap;
}

// uint16_t erfunc(uint16_t x,int a,int b){
//   return a*(1+TMath::Erf((x-b)/(sqrt(2)*b)));
// }
void creationScurve(const char *dataraw=" ",const char *datatime=" ",const int backbias=0){
  gROOT->SetBatch(kTRUE);
  std::time_t dttime = std::time(nullptr);
   std::vector<std::string> filenames=glob("Thr/test_lyon/digit_coo*.txt");
   std::cout<<"How many files? "<<filenames.size()<<std::endl;
   vector<int> vecChipDec=getChipDec();
   int sizeVecDec=vecChipDec.size();
   vector<string> vecMap=getMapping();
   uint32_t pixelId;
   int bbias = backbias;
   uint16_t transId, row, col;
   std::map<uint32_t,std::vector<std::pair<uint16_t,uint16_t>>> allData;
   TCanvas *c1[sizeVecDec];
   TCanvas *c2[sizeVecDec];
   TH1F *hplot[sizeVecDec];
   TH1F *hplot2[sizeVecDec];
   TH2F *hplot3[sizeVecDec];
   TH2F *hplot4[sizeVecDec];

   for(int i=0;i<filenames.size();i++){
     ifstream file;
     file.open(filenames[i],ios::in);
     uint16_t itrans,icol,irow,ihits,ivpulse;
     while(file>>itrans>>icol>>irow>>ihits>>ivpulse)
       allData[GetPixelId(itrans,icol,irow)].push_back(std::make_pair(ivpulse,ihits));    
     file.close();
   }
   std::sort (vecChipDec.begin(), vecChipDec.end(), SortFunc);
   auto start = high_resolution_clock::now();
   int scale=2;
   for(int i=0;i<sizeVecDec;i++){
     hplot[i]=new TH1F(Form("hThresh%d",vecChipDec[i]),Form("Threshold Distribution  TransID %d (%s)",vecChipDec[i],vecMap[i].c_str()),60,4.,31.);
     hplot[i]->GetXaxis()->SetTitle("Threshold (#DAC)");
     hplot[i]->GetYaxis()->SetTitle("# Pixels");
     hplot2[i]=new TH1F(Form("hNoise%d",vecChipDec[i]),Form("Noise Distribution  TransID %d (%s)",vecChipDec[i],vecMap[i].c_str()),60, 0., 3.);
     hplot2[i]->GetXaxis()->SetTitle("Noise (#DAC)");
     hplot2[i]->GetYaxis()->SetTitle("# Pixels");
     hplot3[i]=new TH2F(Form("hMap%d",vecChipDec[i]),Form("Threshold Map TransID %d (%s)",vecChipDec[i],vecMap[i].c_str()),1024/2, -0.5, 1023.5, 512/2, -0.5, 511.5);
     hplot3[i]->GetXaxis()->SetTitle("Column");
     hplot3[i]->GetYaxis()->SetTitle("Row");
     hplot3[i]->GetZaxis()->SetTitle("Threshold [#DAC]");
     hplot3[i]->SetMaximum(30);
     hplot3[i]->SetMinimum(0);
     hplot4[i]=new TH2F(Form("hMapNoise%d",vecChipDec[i]),Form("Noise Map TransID %d (%s)",vecChipDec[i],vecMap[i].c_str()),1024/2, -0.5, 1023.5, 512/2, -0.5, 511.5);
     hplot4[i]->GetXaxis()->SetTitle("Column");
     hplot4[i]->GetYaxis()->SetTitle("Row");
     hplot4[i]->GetZaxis()->SetTitle("Noise [#DAC]");
     hplot4[i]->SetMaximum(3);
     hplot4[i]->SetMinimum(0);
     
   }
   char *histoname = new char[sizeVecDec];

   for(auto it =allData.begin();it !=allData.end();it++){ // loop over pixelId
     double mean, noise, sdtmean, sdtnoise;
     std::vector<std::pair<uint16_t,float>> derivative;
     CalculateDerivative(it->second, derivative);
     AnalysisData(derivative, mean, sdtmean, noise, sdtnoise);
     uint32_t pixel = it->first;
     transId=GetTransceiverId(pixel);
     //     cout<<(int)transId<<endl;
     //          printf("%4d %4d %4d %lf %lf %lf %lf\n",GetTransceiverId(pixel), GetRow(pixel), GetCol(pixel),mean,  sdtmean, noise, sdtnoise);
     for(int j=0;j<sizeVecDec;j++){
       if((int)transId==vecChipDec[j]){
     	 hplot[j]->Fill(mean);
  	 hplot2[j]->Fill(noise);
	 hplot3[j]->Fill(GetCol(pixel),GetRow(pixel),mean);
	 hplot4[j]->Fill(GetCol(pixel),GetRow(pixel),noise);
       }
      } 
    }
   gStyle->SetPalette(55);
   for(int k=0;k<sizeVecDec;k++){   //to have the plot
     TString os3=vecMap[k];
     c1[k]= new TCanvas(histoname,histoname,1000,1000);
     c1[k]->Divide(1,2,0,0);
      c1[k]->SetName(os3);
      c1[k]->SetTitle(os3);
      c1[k]->GetName();
      c1[k]->GetTitle();
     c1[k]->cd(1);
     gPad->SetTickx(1);
     gPad->SetBottomMargin(0.1);
     hplot[k]->Draw();
     c1[k]->cd(2);
     gPad->SetTicky(1);
     hplot2[k]->Draw();
     std::string histnamesave = "Plots/test_cern/hThrs_";
     histnamesave += vecMap[k];
     histnamesave += "_trans";
     histnamesave += std::to_string(vecChipDec[k]);
     histnamesave += "_BB";
     histnamesave += std::to_string( bbias );
     histnamesave += "V_";
     //     histnamesave += std::to_string(datatime);
     histnamesave += "_raw_"; 
     histnamesave += dataraw; 
     histnamesave += "_created_";
     histnamesave += datatime;
     histnamesave += ".pdf";
     const char *finalname =  histnamesave.c_str();
     c1[k]->SaveAs(finalname);
 
     c2[k]= new TCanvas(Form("Map%d",k),Form("Map%d",k),1000,1000);
     c2[k]->SetRightMargin(0.2);
     c2[k]->Divide(1,2);
     c2[k]->cd(1);
     gStyle->SetOptStat (kFALSE);
     gStyle->SetOptTitle(kTRUE);

     //gPad->SetTickx(1);
     gPad->SetBottomMargin(0.1);
     hplot3[k]->Draw("colz PMC");
     c2[k]->cd(2);
     gStyle->SetOptStat (kFALSE);
     gStyle->SetOptTitle(kTRUE);

     hplot4[k]->Draw("colz PMC");
     std::string histnamesave2 = "Plots/test_cern/hMap_";
     histnamesave2 += vecMap[k];
     histnamesave2 += "_trans";
     histnamesave2 += std::to_string(vecChipDec[k]);
     histnamesave2 += "_BB";
     histnamesave2 += std::to_string( bbias );
     histnamesave2 += "V_";
     //     histnamesave += std::to_string(datatime);
     histnamesave2 += "_raw_";
     histnamesave2 += dataraw;
     histnamesave2 += "_created_";
     histnamesave2 += datatime;
     histnamesave2 += ".pdf";
     const char *finalname2 =  histnamesave2.c_str();
     c2[k]->SaveAs(finalname2);
   }
   auto stop = high_resolution_clock::now(); 
   auto duration = duration_cast<microseconds>(stop - start); 
   cout << "Time taken: "
	<< duration.count() << " microseconds" << endl; 
 
}
