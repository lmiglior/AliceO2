#include <glob.h> // glob(), globfree()
#include <string.h> // memset()
#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <cmath>

using namespace std;


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
std::vector <int> getChipDec()
{
   fstream inputDec; //read chip_ID.txt in order to have the number of chip in raw data file
  inputDec.open("/home/o2flp/alice/output_raw/Chip_ID.txt", ios_base::in);
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

// uint16_t erfunc(uint16_t x,int a,int b){
//   return a*(1+TMath::Erf((x-b)/(sqrt(2)*b)));
// }
void creationScurve(){
  
   std::vector<std::string> filenames=glob("Thr/digit_coo*.txt");
   std::cout<<"How many files? "<<filenames.size()<<std::endl;
   vector<int> vecChipDec;
   int sizeVecDec=vecChipDec.size();
   uint32_t pixelId;
   uint16_t transId, row, col;
   std::map<uint32_t,std::vector<std::pair<uint16_t,uint16_t>>> allData;
   
   for(int i=0;i<filenames.size();i++){
     ifstream file;
     file.open(filenames[i],ios::in);
     uint16_t itrans,icol,irow,ihits,ivpulse;
     while(file>>itrans>>icol>>irow>>ihits>>ivpulse)
       allData[GetPixelId(itrans,icol,irow)].push_back(std::make_pair(ivpulse,ihits));
    
     file.close();
   }    

   for(auto it =allData.begin();it !=allData.end();it++){ // loop over pixelId
     double mean, noise, sdtmean, sdtnoise;
     std::vector<std::pair<uint16_t,float>> derivative;
     CalculateDerivative(it->second, derivative);
     AnalysisData(derivative, mean, sdtmean, noise, sdtnoise);
     uint32_t pixel = it->first;
     printf("%4d %4d %4d %lf %lf %lf %lf\n",GetTransceiverId(pixel), GetRow(pixel), GetCol(pixel),mean,  sdtmean, noise, sdtnoise);

  
   }   
}
