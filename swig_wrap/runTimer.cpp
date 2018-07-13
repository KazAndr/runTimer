#include "runTimer.h"

struct SumProfile bar;
struct ConfigParam foo;
struct Tpl tpl_bar;
struct Skkf skkf_bar;
struct Itoa itoa_bar;

int convertStringParam(std::string IN, bool* OUT)
{
  int returnValue=0;
  if (IN=="yes") *OUT=true;
  else if (IN=="no") *OUT=false;
  else returnValue=1;
  return returnValue;
}

ConfigParam ReadConfig(std::string cname)
{
  ConfigParam output;
  
  std::ifstream flist;
  flist.open(cname.c_str());
  char tmp[100];
  int nFiles, nThreads;
  int startFileNumber, endFileNumber;
  std::string confParam;
  //  std::string rawdata_dir, output_dir;
  //  std::string fname;

  bool bRemoveSpikes=false;
  bool bFRfilter=false;
  bool bGetDynSpectrum=false;
  bool bGetIndImpulses=false;
  bool bGetFR=false;
  bool bKeepNorm=true;
  float utccorr;
  std::string sRemoveSpikes, sFRfilter;
  std::string sGetDynSpectrum, sGetIndImpulses, sGetFR, sKeepNorm;
  float utccor;
  float nVarFR, nVarSpike;
  flist>>confParam;
  while(confParam!="runs:"){		 
    //    std::cout<<confParam<<std::endl;
    if (confParam=="inputDir") flist>>output.rawdata_dir;
    else if (confParam=="outputDir") flist>>output.output_dir;
    else if (confParam=="channelMask") flist>>output.maskfile; // добавлено мной 17.08
    else if (confParam=="tplFile") flist>>output.tplfile; // добавлено мной 28.08
    else if (confParam=="startFileNumber") flist>>startFileNumber;
    else if (confParam=="nRuns") flist>>nFiles;
    else if (confParam=="removeSpikes") flist>>sRemoveSpikes;
    else if (confParam=="FRfilter") flist>>sFRfilter;
    else if (confParam=="getDynSpectrum") flist>>sGetDynSpectrum;
    else if (confParam=="getIndImpulses") flist>>sGetIndImpulses;
    else if (confParam=="getFR") flist>>sGetFR;
    else if (confParam=="FRcleaningCut") flist>>nVarFR;
    else if (confParam=="SpikeCleaningCut") flist>>nVarSpike;
    else if (confParam=="useTrueNormalisation") flist>>sKeepNorm;
    else if (confParam=="utcCorrection") flist>>utccorr;
    flist.getline(tmp,100,'\n');
    flist>>confParam;
  }

  output.utccorr=utccorr;
  
  int err=0;
  err+=convertStringParam(sRemoveSpikes, &bRemoveSpikes);
  err+=convertStringParam(sFRfilter, &bFRfilter);
  err+=convertStringParam(sGetDynSpectrum, &bGetDynSpectrum);
  err+=convertStringParam(sGetIndImpulses, &bGetIndImpulses);
  err+=convertStringParam(sGetFR, &bGetFR);
  err+=convertStringParam(sKeepNorm, &bKeepNorm);
  
  output.doRemoveSpikes=bRemoveSpikes;
  output.doFRfiltering=bFRfilter;
  output.getDynSpectrum=bGetDynSpectrum;
  output.getIndImpulses=bGetIndImpulses;
  output.getFR=bGetFR;
  output.nVarFR=nVarFR;
  output.nVarSpike=nVarSpike;
  output.useTrueNorm=bKeepNorm;
  
  std::string rID;
  int runCounter=0;
  while (flist>>rID){
    runCounter++;
    flist.getline(tmp,100,'\n');
    //std::cout<<runCounter<<std::endl;
    if (runCounter<startFileNumber) continue;
    if (runCounter>=startFileNumber+nFiles) break;
    output.runs.push_back(rID);
  }
  return output;
}

SignalContainer::SignalContainer()
{
  fNTimeBins=0;
  fTimeStep=0;
}

SignalContainer::~SignalContainer()
{
}


SignalContainer::SignalContainer(std::vector<float> timeValues)
{
  fNTimeBins=timeValues.size();
  fTimeStep=(float)(timeValues[timeValues.size()-1]-timeValues[0])/fNTimeBins;
  fTimeVector=timeValues;
  for (int j=0; j<fNTimeBins; j++){
    fSignalVector.push_back(0);
  }
}

SignalContainer::SignalContainer(float nbins, float time0, float time1)
{
  fNTimeBins=nbins;
  fTimeStep=(float)(time0-time1)/fNTimeBins;
  for (int j=0; j<fNTimeBins; j++){
    fTimeVector.push_back(time0+j*fTimeStep);
  }
  for (int j=0; j<fNTimeBins; j++){
    fSignalVector.push_back(0);
  }
}

SignalContainer::SignalContainer(std::vector<float> timeValues, std::vector<float> signalValues)
{
  fNTimeBins=timeValues.size();
  fTimeStep=(float)(timeValues[timeValues.size()-1]-timeValues[0])/fNTimeBins;
  fTimeVector=timeValues;
  
  //  fNTimeBins=timeValues.size();
    //  fTimeStep=(float)(timeValues[timeValues.size()-1]-timeValues[0])/fNTimeBins;
    //  fTimeVector=timeValues;
  if (timeValues.size()!=signalValues.size()){
    std::cout<<"WARNING: time vector is not of the same size as signal vector"<<std::endl;
  }
  fSignalVector=signalValues;
}

SignalContainer::SignalContainer(std::vector<float> timeValues, std::vector<float> signalValues, std::vector<float> errors)
{
  SignalContainer(timeValues, signalValues);
  fErrorVector=errors;
}

float SignalContainer::GetSignalMean(int b0, int b1)
{
  float mean=0;
  if (fSignalVector.size()==0) return mean;
  if (b1>=fSignalVector.size()) b1=fSignalVector.size()-1;
  for (int i=b0; i<=b1; i++){
    mean+=fSignalVector[i];
  }
  mean=(float)mean/(float)(b1-b0+1);
  return mean;
}

float SignalContainer::GetSignalMedian(int b0, int b1)
{
  float med=0;
  if (fSignalVector.size()==0) return med;
  if (b1>=fSignalVector.size()) b1=fSignalVector.size()-1;
  std::vector<float> sorted;
  for (int i=b0; i<=b1; i++){
    sorted.push_back(fSignalVector[i]);
  }
  std::sort(sorted.begin(), sorted.end());
  if (sorted.size()%2==0){
    med=(sorted[floor(sorted.size()/2)]+sorted[floor(sorted.size()/2)-1])/2;
  }
  else med=(sorted[floor(sorted.size()/2)]);
  
  return med;
}


float SignalContainer::GetSignalVariance(int b0, int b1)
{
  float rms=0;
  if (fSignalVector.size()==0) return rms;
  float mean=GetSignalMean(b0,b1);
  for (int i=b0; i<=b1; i++){
    rms+=pow(mean-fSignalVector[i],2);
  }
  //  std::cout<<"getvar1:   "<<mean<<"      "<<rms<<std::endl;
  rms=(float)rms/(float)(b1-b0+1);
  //  std::cout<<"getvar2:   "<<rms<<std::endl;
  return sqrt(rms);
}

float SignalContainer::GetTime(int iBin)
{
  if (fTimeVector.size()==0) return 0;
  else if (fTimeVector.size()<=iBin) return fTimeVector[fTimeVector.size()];
  else {
    return fTimeVector[iBin];
  }
}

float SignalContainer::GetSignal(int iBin)
{
  //std::cout<<"get signal: "<<fSignalVector[iBin]<<std::endl;
  if (fSignalVector.size()==0) return 0;
  else if (fSignalVector.size()<=iBin) return fSignalVector[fSignalVector.size()];
  else {
    return fSignalVector[iBin];
  }
}

int SignalContainer::SetSignal(int iBin, float value)
{
  if (fSignalVector.size()<=iBin) return 0;
  else {
    fSignalVector[iBin]=value;
    return 1;
  }
}

float SignalContainer::GetSignalError(int iBin)
{
  if (fErrorVector.size()==0) return 0;
  else if (fErrorVector.size()<=iBin) return fErrorVector[fErrorVector.size()];
  else {
    return fErrorVector[iBin];
  }
}

int SignalContainer::SetSignalError(int iBin, float value)
{
  if (fErrorVector.size()<=iBin) return 0;
  else {
    fErrorVector[iBin]=value;
    return 1;
  }
}

int chToNum(char ch)
{
  int num=int(ch)-int('0');
  return num;
}

double readNumber(char* buffer, int iStart, int N0, int N1)
{
  double number=0;
  double decimal=0;

  for (int i=1; i<=N0; i++){
    number+=pow(10,i-1+N1)*chToNum (buffer[iStart+N0-i]);
  }
  for (int i=1; i<=N1; i++){
    decimal+=pow(10,i-1)*chToNum(buffer[iStart+N0+N1+1-i]);
  }
  double result=number+decimal;
  double returnValue=result*pow(10,-N1);
  return returnValue;
}

long double readNumberMod(char* buffer, int iStart, int N)
{
  std::vector<int> integer;
  std::vector<int> decimal;
  bool isInt=true;
  bool isDec=false;
  for (int i=0; i<N; i++){
    if (chToNum(buffer[iStart+i])==-2) {
      isDec=true;
      isInt=false;
      continue;
    }
    if (chToNum(buffer[iStart+i])<0) continue;
    if (isInt) integer.push_back(chToNum(buffer[iStart+i]));
    if (isDec) {
      decimal.push_back(chToNum(buffer[iStart+i]));
      //      std::cout<<"testDec1: "<<chToNum(buffer[iStart+i])<<std::endl;
    }
  }
  long double number=0;
  for (int i=0; i<integer.size(); i++){
    if (integer[integer.size()-1-i]>0) number += pow(10,i)*(integer[integer.size()-1-i]);
    //   if (integer[integer.size()-1-i]==0) i++;
  }
  long double dec=0;
  for (int i=0; i<decimal.size(); i++){
    if (decimal[i]>0) dec += pow(10,-i-1)*decimal[i];
    //  if (decimal[i]==0) i++;
    //   std::cout<<"testdec2: "<<pow(10,-i-1)*decimal[i]<<std::endl;
  }
  number+=dec;
  return number;
}
/*
int convertStringParam(std::string IN, bool* OUT)
{
  int returnValue=0;
  if (IN=="ye") *OUT=true;
  else if (IN=="no") *OUT=false;
  else returnValue=1;
  return returnValue;
}
*/
/*
//OLD FUNCTION (02.08.17)
float pulseToFloat(unsigned int pulse, float tau)
{
  float exp, spectr;
  spectr=(pulse&0xFFFFFF);
  exp=int(pulse&0x7F000000) >> 24;
  exp=exp-64-24;
  float ratio=tau/0.2048;
  spectr=spectr*std::pow(2,exp)/ratio;
  return spectr;
}
*/

float pulseToFloat(unsigned int pulse, float tau)
{
  float exp,spectr_t;
  
  spectr_t =  (pulse&0xFFFFFF);
  
  exp = int(pulse&0x7F000000) >> 24;
  
  exp = exp-64-24;
  
  float ratio=tau/0.2048;
  
  spectr_t=spectr_t*pow(2,exp)/ratio;
  
  spectr_t=sqrt(spectr_t*4/2048/2048);
  
  spectr_t=spectr_t*2000/2047;
  
  spectr_t=spectr_t*sqrt(2.0)/2;
  
  spectr_t=spectr_t*spectr_t;
  
  return spectr_t;
}

BaseRun::BaseRun()
{
  fTelcode="bsa1";
  fObscode="PO";
  fRtype="DPP1";
  fDatatype="I";
  fNpol=1;
  fNChannels=512;
}

BaseRun::~BaseRun()
{
}

int BaseRun::ReadRAWData(std::string runID, std::string rawdata_dir, std::string output_dir)
{
  int retVal=1;
  
  fPerChannelSignal.clear();
  
  fRunID=runID;

  std::string fname = rawdata_dir+"/"+runID;
  std::ifstream data(fname.c_str(),std::ios::binary|std::ios::in);
  
  std::cout<<"чтение сеанса "<<runID<<" из директории: "<<fname<<std::endl;
   //read header
  int length = 40; 
  char * buffer = new char [length];
  int sizeHeader;

  std::cout<<"заголовочная часть:"<<std::endl;

  std::cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<std::endl;

  for (int k=0; k<13; k++)
    {
      // std::cout << "Reading " << length << " characters... "<<std::endl;
      // read data as a block:
      data.read(buffer,length);
      //  int size = data.tellg();
      sizeHeader = data.tellg();
      std::cout<<k<<"   "<<buffer<<std::endl;
      
      //read period (in s)
      if (k==4)	{
	fPeriod=readNumberMod(buffer,13,15);
      }
      
      if (k==9) {
	fDM=readNumberMod(buffer,13,7);
      }
      
      //covert buffer to a number
      //read nPeriods and nBinsPerPeriod
      int number;
      if (buffer[17]==' ') {
	//number=1000*chToNum(buffer[13])+100*chToNum(buffer[14])+10*chToNum(buffer[15])+chToNum(buffer[16]);
	number=readNumber(buffer,13,4,0);
      }
      if (buffer[16]==' ') {
	number=readNumber(buffer,13,3,0);
      }
      if (buffer[15]==' ') {
	number=readNumber(buffer,13,2,0);
      }

      if (k==5) fNumpuls=number;
      if (k==7) fNumpointwin=number;

      std::string sbuf;
      if (k==1) {
	for (int ibuf=13; ibuf<20; ibuf++){
	  sbuf+=buffer[ibuf];
	}
	fPsrname=sbuf;
      }
      

      /////////////////////////////
      //read sumchan
      if (k==8) {
	std::string buf;
	for (int ibuf=13; ibuf<15; ibuf++){
	  buf+=buffer[ibuf];
	}
	if (buf=="ye") fSumchan=1;
	else if (buf=="no") fSumchan=0;
	else if (buf=="ad") fSumchan=2;
	//	std::cout<<"fSumchan: "<<fSumchan<<std::endl;
      }
	/////////////////////////////
      
      //read tau (in ms)
      if (k==6) fTau=readNumber(buffer,13,1,4);
      
      //read freq 0
      if (k==10) {
	fFreqFirst=readNumber(buffer,13,3,3);
	fWLFirst=3e10*pow(fFreqFirst*1e6,-1);
      }
      
      //read freq 511
      if (k==11) {
	fFreqLast=readNumber(buffer,13,3,3);
	fWLLast=3e10*pow(fFreqLast*1e6,-1);
      }
      
      //decode time
      if (k==12){
	fUtcday=readNumber(buffer,13,2,0);
	fUtcmonth=readNumber(buffer,16,2,0);
	fUtcyear=readNumber(buffer,19,2,0);
	fUtchour=readNumber(buffer,22,2,0);
	fUtcmin=readNumber(buffer,25,2,0);
	fUtcsec=readNumber(buffer,28,2,0)+1e-7*readNumber(buffer,31,7,0);
	//	fUtcsec=readNumber(buffer,28,2,0);
	//	fUtcnsec=readNumber(buffer,31,7,0);
      }
      if (k==2){
	fDay=readNumber(buffer,13,2,0);
	fMonth=readNumber(buffer,16,2,0);
	fYear=readNumber(buffer,19,4,0);
      }
      if (k==3){
	fHour=readNumber(buffer,13,2,0);
	fMinute=readNumber(buffer,16,2,0);
	fSec=readNumber(buffer,19,2,0)+1e-7*readNumber(buffer,23,7,0);
	//	fSecond=readNumber(buffer,19,2,0);
	//	fNsec=readNumber(buffer,23,7,0);
      }
    }   
  
  fNPoints=fNumpuls*fNumpointwin;
  if (fSumchan==1) fNPoints=fNumpointwin;
  fDuration=fNPoints*fTau;
  
  //  char tmp[100];
  //  TH1F sigTimeProfile[512];
  std::vector<float> freqResponse;
  //  std::vector<SignalContainer> perChannelSignal;
  //  std::cout<<"READING DATA    numPeriods: "<<fNumpuls<<"   binsPerPeriod: "<<fNumpointwin<<"   tau: "<<fTau<<std::endl;
  //	   <<"   tau: "<<fTau<<"   period: "<<fPeriod<<"  fDay: "<<fDay<<"  fSec: "<<fSecond<<"\n"
  //	   <<"     fre0: "<<fFreq0<<"   freq511: "<<fFreq511<<std::endl;
  // int lengthData= 8*sizeof(uint32_t);
  int lengthData = sizeof(uint32_t);
  char* fileContents;
  fileContents = new char[lengthData];
  unsigned int number=0;
  int ipos;
  int iPoint=0;
  int iPointAbs=0;
  int iFreq=0;
  int iPeriod=0;

  for (int i=0; i<512; i++){
    fPerChannelSignal.push_back(SignalContainer(fNPoints,0,fDuration));
  }
  while(data.good())
    {
      data.read((char *) &number,lengthData);
      int ipos = data.tellg();
      if (!data.good()) continue;
      float ampl=pulseToFloat(number,fTau);
      int iFreq=(((ipos-sizeHeader)/lengthData-1)%512);
      if (iFreq!=513) {
	//      std::cout<<iFreq<<"    "<<iPointAbs<<"    "<<ampl<<
	fPerChannelSignal[iFreq].SetSignal(iPointAbs,ampl);
	//	std::cout<<iFreq<<"    "<<iPointAbs<<"    "<<ampl<<"   "<<number<<"    "<<fPerChannelSignal[iFreq].GetSignal(iPointAbs)<<std::endl;
      }
      if (iFreq==511){
	iPoint++;
	if (iPoint%fNumpointwin==0) {
	  iPoint=0;
	  iPeriod++;
	}	      
	iPointAbs++;
      }	
    }
  //  for (int i=0; i<fNPoints; i++){
    //  std::cout<<"test: "<<i<<"  "<<fPerChannelSignal[18].GetSignal(i)<<std::endl;
  //  }
  
  for (int i=0; i<512; i++){
    fFreqResponse.push_back(fPerChannelSignal[i].GetSignalMean(0,1000000));
    //    std::cout<<i<<"     "<<fPerChannelSignal[i].GetSignalMean(0,1000000)<<std::endl;
    fFreqResponseMedian.push_back(fPerChannelSignal[i].GetSignalMedian(0,1000000));
  }

  //  for (int i=0; i<512; i++){
    //    fFreqResponse.push_back(fPerChannelSignal[i].GetSignalMean(0,1000000));
    //    for (int j=0; j<fNPoints; j++){
      //std::cout<<i<<"    "<<j<<"     "<<fPerChannelSignal[i].GetSignal(j)<<"    "<<pow(fFreqResponse[i],-1)<<"    "<<fPerChannelSignal[i].GetSignal(j)*pow(fFreqResponse[i],-1)<<"    "<<fFreqResponseMedian[i]<<std::endl;
      //fPerChannelSignal[i].SetSignal(j,fPerChannelSignal[i].GetSignal(j)
				     //*pow(fFreqResponse[i],-1));
      //fPerChannelSignal[i].SetSignal(j,fPerChannelSignal[i].GetSignal(j)-fFreqResponseMedian[i]);
    //    }
  //  }
  
  data.close();
  delete fileContents;
  delete buffer;
  std::cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<std::endl;
  return retVal;

}
PulseExtractor::PulseExtractor()
{
} 

PulseExtractor::~PulseExtractor()
{
} 

PulseExtractor::PulseExtractor(BaseRun* run)
{
  fBaseRun=run;
  fDM=fBaseRun->GetDM();

  fillSumProfile();

  fCompensatedSignal=SignalContainer(fBaseRun->GetNPoints(),0,fBaseRun->GetNPoints());
  fCompensatedSignalSum=SignalContainer(fBaseRun->GetNumpointwin(),0,fBaseRun->GetNumpointwin());

  for (int i=0; i<fBaseRun->GetNChannels(); i++)
    fDynamicSpectrum.push_back(SignalContainer(fBaseRun->GetNumpointwin(),0,fBaseRun->GetNumpointwin()));
  
  for (int i=0; i<fBaseRun->GetNChannels(); i++) {fChannelMask.push_back(1);}
  for (int i=0; i<fBaseRun->GetNPoints(); i++) {fSpikeMask.push_back(1);}
  fIsDynSpecAvailable=false;
  fNChan=fBaseRun->GetNChannels();
  fNChanAfterMask=fBaseRun->GetNChannels();
  fDoNormToUnity=true;
}
 

int PulseExtractor::compensateDM()
{
  std::cout<<"компенсация дисперсионного запаздывания"<<std::endl;

  float dm=fDM;
  int npoints;
  if (fIsDynSpecAvailable) npoints=fBaseRun->GetNumpointwin();
  else npoints=fBaseRun->GetNPoints();

  for (int i=0; i<npoints; i++){
    float fDnu=fabs(fBaseRun->GetFreqLast()-fBaseRun->GetFreqFirst())/512;
    float fDL=fabs(fBaseRun->GetWLLast()-fBaseRun->GetWLFirst())/512;
    float bico=0;
    for (int y=0; y<fBaseRun->GetNChannels(); y++) {
      int iChan=(fBaseRun->GetNChannels()-1)-y;
      if (fChannelMask[iChan]==0) continue;
      //calculate delay wrt 511th for particular freq[511-y]
      float dT=4.6*(-pow(fBaseRun->GetWLLast(),2)+pow(fBaseRun->GetWLLast()+y*fDL,2))*fDM*0.001; //covert to ms
      //calculate residual difference to nearest positive side pulse
      float dTnearest=dT-1000*fBaseRun->GetPeriod()*floor(dT*pow(1000*fBaseRun->GetPeriod(),-1));
      //move frequency band by -dTnearest bins, add lower bins to "upper side"
      float delta=dTnearest*pow(fBaseRun->GetTau(),-1);

      //define normalisation based on channel mask
      float normFactor=fChannelMask[iChan];

      //channel summation
      float bico1, bico2;
      if (!fIsDynSpecAvailable||fBaseRun->GetSumchan()==1||fBaseRun->GetSumchan()==2){
	bico1=normFactor*fBaseRun->GetChannelSignal(iChan)->GetSignal(int(floor(i+delta))%npoints);
	bico2=normFactor*fBaseRun->GetChannelSignal(iChan)->GetSignal(int((floor(i+delta)+1))%npoints);
      }
      if (fIsDynSpecAvailable) {
	bico1=normFactor*fDynamicSpectrum[iChan].GetSignal(int(floor(i+delta))%npoints);
	bico2=normFactor*fDynamicSpectrum[iChan].GetSignal(int((floor(i+delta)+1))%npoints);
      }
      float lowerBinFrac=1-((i+delta)-floor(i+delta));
      float upperBinFrac=1-lowerBinFrac;
      //if (floor(i+delta)+1<fBaseRun->GetNPoints())
      bico+=lowerBinFrac*bico1+upperBinFrac*bico2;
    }

    fCompensatedSignal.SetSignal(i,bico/fNChanAfterMask);
      //fBaseRun->GetNChannels();
    fCompensatedSignalSum.SetSignal(i,bico/fNChanAfterMask);
      //fBaseRun->GetNChannels();

  }

  /*
  //subtract median and normalize to unity:
  float median=fCom
  
  
  
  pensatedSignal.GetSignalMedian(0,1000000);
  for (int i=0; i<fCompensatedSignal.GetNbins(); i++){
    fCompensatedSignal.SetSignal(i, fCompensatedSignal.GetSignal(i)-median);
  }
  
  if (fDoNormToUnity){
    normToUnity(&fCompensatedSignal);
  }
  */
  return 0;
}

int PulseExtractor::fillSumProfile()
{
  fSumProfile.telcode = fBaseRun->GetTelcode();
  fSumProfile.obscode = fBaseRun->GetObscode();
  fSumProfile.rtype = fBaseRun->GetRtype();
  fSumProfile.psrname = fBaseRun->GetPsrname();
  fSumProfile.datatype = fBaseRun->GetDatatype();
  fSumProfile.npol = fBaseRun->GetNpol();

  fSumProfile.sumchan = fBaseRun->GetSumchan();

  fSumProfile.nChan=fNChan;
  fSumProfile.nChanAfterMask=fNChanAfterMask;
  
  fSumProfile.year = fBaseRun->GetYear();
  fSumProfile.month = fBaseRun->GetMonth();
  fSumProfile.day = fBaseRun->GetDay();
  fSumProfile.hour = fBaseRun->GetHour();
  fSumProfile.min = fBaseRun->GetMinute();
  fSumProfile.sec = fBaseRun->GetSecond();
  fSumProfile.utcyear = fBaseRun->GetUtcYear();
  fSumProfile.utcmonth = fBaseRun->GetUtcMonth();
  fSumProfile.utcday = fBaseRun->GetUtcDay();
  fSumProfile.utchour = fBaseRun->GetUtcHour();
  fSumProfile.utcmin = fBaseRun->GetUtcMinute();
  fSumProfile.utcsec = fBaseRun->GetUtcSecond();

  fSumProfile.period = fBaseRun->GetPeriod();
  fSumProfile.numpuls = fBaseRun->GetNumpuls();
  fSumProfile.tau = fBaseRun->GetTau();
  fSumProfile.numpointwin = fBaseRun->GetNumpointwin();

  fSumProfile.freq = fBaseRun->GetFreqLast();

  for (int i=0; i<fSumProfile.numpointwin; i++) fSumProfile.prfdata.push_back(0);
  fSumProfile.nChan=fNChan;
    
  return 1;
}

int PulseExtractor::ReadMask(std::string fname)
{
  std::cout<<"чтение маски из файла: "<<fname<<std::endl;
  std::ifstream fmask;
  fmask.open(fname.c_str());
  char tmp[100];
  int iband;
  float value;
  int nActive=0;
  for (int i=0; i<512; i++){
    fmask>>iband>>value;
    if (value!=0) nActive++;
    fChannelMask[iband-1]=value;
  }
  fNChanAfterMask=nActive;
  fSumProfile.nChanAfterMask=fNChanAfterMask;
  return 1;
}

int PulseExtractor::sumPeriods()
{
  std::cout<<"суммирование индивидуальных импульсов"<<std::endl;

  if (!fIsDynSpecAvailable&&fBaseRun->GetSumchan()==0){
    for (int i=0; i<fBaseRun->GetNumpuls(); i++){
      for (int j=0; j<fBaseRun->GetNumpointwin(); j++){
	fSumProfile.prfdata[j]+=fCompensatedSignal.GetSignal(i*fBaseRun->GetNumpointwin()+j)/fBaseRun->GetNumpuls();
	fCompensatedSignalSum.SetSignal(j,fSumProfile.prfdata[j]);
      }
    }
  }
  else if (fIsDynSpecAvailable&&fBaseRun->GetSumchan()==0){
    for (int j=0; j<fBaseRun->GetNumpointwin(); j++){
      fSumProfile.prfdata[j]+=fCompensatedSignalSum.GetSignal(j);
    }
  }
  else if (fBaseRun->GetSumchan()==1||fBaseRun->GetSumchan()==2) {
    for (int j=0; j<fBaseRun->GetNumpointwin(); j++){
      fSumProfile.prfdata[j]=fCompensatedSignal.GetSignal(j);
    }
  }
  
  //subtract mean over sum profile (or median):
  float median=fCompensatedSignalSum.GetSignalMedian(0,1000000);
  for (int i=0; i<fSumProfile.prfdata.size(); i++){
    fSumProfile.prfdata[i]=fSumProfile.prfdata[i]-median;
  }
  
  //normalize to unity:
  if (fDoNormToUnity) {
    float max=-100;
    for (int i=0; i<fSumProfile.prfdata.size(); i++){
      if (fSumProfile.prfdata[i]>max) max=fSumProfile.prfdata[i];
    }
    for (int i=0; i<fSumProfile.prfdata.size(); i++){
      fSumProfile.prfdata[i]=fSumProfile.prfdata[i]/max;
    }
  }
  
  return 1;
}

int PulseExtractor::sumPerChannelPeriods()
{
  std::cout<<"суммирование периодов в каждом канале"<<std::endl;
  for (int k=0; k<fBaseRun->GetNChannels(); k++){
    std::vector<float> sums;

    for (int j=0; j<fBaseRun->GetNumpointwin(); j++) sums.push_back(0);
    
    for (int i=0; i<fBaseRun->GetNumpuls(); i++){
      for (int j=0; j<fBaseRun->GetNumpointwin(); j++){
	sums[j]+=fBaseRun->GetChannelSignal(k)->GetSignal(i*fBaseRun->GetNumpointwin()+j)/fBaseRun->GetNumpuls();
	fDynamicSpectrum[k].SetSignal(j,sums[j]);
      }
    }
  }
  
  //subtract zero from dynamic spectrum:
  for (int k=0; k<fBaseRun->GetNChannels(); k++){
    float median=fDynamicSpectrum[k].GetSignalMedian(0,1000000);
    for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
      float zsub=fDynamicSpectrum[k].GetSignal(i)-median;
      fDynamicSpectrum[k].SetSignal(i,zsub);
    }
    //normalize to unity:
    if (fDoNormToUnity) normToUnity(&fDynamicSpectrum[k]);
  }
  
  return 1;
}

int PulseExtractor::PrintSumProfile(std::string dirname)
{
  std::ofstream sumProfileStream;
  char tmp[100];
  sprintf(tmp,"%s/%s.prf",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream.open(tmp);
  printHeader(&sumProfileStream);

 
  sumProfileStream<<"### COMPENSATED SUM PROFILE "<<"\n";
  sumProfileStream<<"time        signal"<<"\n";
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    float time=fBaseRun->GetTau()*i;
    sumProfileStream<<i<<std::setw(20)<<fSumProfile.prfdata[i]<<"\n";
  }
  std::cout<<"   суммарный профиль записан в файл: "<<tmp<<std::endl;
  return 1;
}

int PulseExtractor::PrintFrequencyResponse(std::string dirname)
{
  std::ofstream sumProfileStream;
  char tmp[100];
  sprintf(tmp,"%s/%s.fr",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream.open(tmp);
  printHeader(&sumProfileStream);

  sumProfileStream<<"### FREQUENCY RESPONSE "<<"\n";
  sumProfileStream<<"frequency         mean signal"<<"\n";
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    double freq=fBaseRun->GetFreqFirst()+i*(float)(fBaseRun->GetFreqLast()-fBaseRun->GetFreqFirst())/(float)fBaseRun->GetNChannels();
    freq+=0.00001;
    sumProfileStream<<std::setprecision(7)<<std::left<<std::setw(10)<<freq<<std::setw(20)<<fBaseRun->GetFreqResponse(i)<<"\n";
  }
  std::cout<<"   АЧХ записана в файл: "<<tmp<<std::endl;

  std::ofstream sumProfileStream1;
  sprintf(tmp,"%s/masked_%s.fr",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream1.open(tmp);
  printHeader(&sumProfileStream1);

  sumProfileStream1<<"### MASKED FREQUENCY RESPONSE "<<"\n";
  sumProfileStream1<<"frequency         mean signal"<<"\n";
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    double freq=fBaseRun->GetFreqFirst()+i*(float)(fBaseRun->GetFreqLast()-fBaseRun->GetFreqFirst())/(float)fBaseRun->GetNChannels();
    freq+=0.00001;
    if (fChannelMask[i]==0) sumProfileStream1<<std::setprecision(7)<<std::left<<std::setw(10)<<freq<<std::setw(20)<<0<<"\n";
    else sumProfileStream1<<std::setprecision(7)<<std::left<<std::setw(10)<<freq<<std::setw(20)<<fBaseRun->GetFreqResponse(i)<<"\n";
    
  }
  std::cout<<"   АЧХ после маски записана в файл: "<<tmp<<std::endl;
  return 1;
}

int PulseExtractor::printHeader(std::ofstream* stream)
{
  *stream<<"### HEADER "<<"\n";
  *stream<<"telcode: "<<fSumProfile.telcode<<"\n";
  *stream<<"obscode: "<<fSumProfile.obscode<<"\n";
  *stream<<"rtype: "<<fSumProfile.rtype<<"\n";
  *stream<<"psrname: "<<fSumProfile.psrname<<"\n";
  *stream<<"period: "<<std::setprecision(13)<<fSumProfile.period<<"\n";
  *stream<<"tau: "<<std::setprecision(6)<<fSumProfile.tau<<"\n";
  *stream<<"date: "<<fSumProfile.day<<"/"<<fSumProfile.month<<"/"<<fSumProfile.year<<"\n";
  *stream<<"time: "<<fSumProfile.hour<<":"<<fSumProfile.min<<":"<<std::setprecision(7)<<fSumProfile.sec<<"\n";
  *stream<<"utctime: "<<fSumProfile.utchour<<":"<<fSumProfile.utcmin<<":"<<std::setprecision(8)<<fSumProfile.utcsec<<"\n";
  *stream<<"frequency: "<<std::setprecision(7)<<fSumProfile.freq<<"\n";
  *stream<<"N used channels: "<<fSumProfile.nChanAfterMask<<"\n";
  return 1;
}

int PulseExtractor::PrintChannelSumProfile(std::string dirname)
{
  std::ofstream sumProfileStream;
  char tmp[100];
  sprintf(tmp,"%s/bands_%s.prf",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream.open(tmp);
  printHeader(&sumProfileStream);

  sumProfileStream<<"### COMPENSATED SUM PROFILE FOR EACH FREQUENCY BAND"<<"\n";
  sumProfileStream<<"time     signal1     signal2...    signal512"<<"\n";
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    float time=fBaseRun->GetTau()*i;
    sumProfileStream<<std::setw(6)<<std::left<<i;
    for (int j=0; j<512; j++){
      sumProfileStream<<std::setw(20)<<std::left<<fDynamicSpectrum[j].GetSignal(i);
    }
    sumProfileStream<<std::endl;
  }
  std::cout<<"   динамический спектр записан в файл: "<<tmp<<std::endl;
  return 1;
}

int PulseExtractor::PrintCompensatedImpulses(std::string dirname)
{
  std::ofstream sumProfileStream;
  char tmp[100];
  sprintf(tmp,"%s/compPulses_%s.prf",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream.open(tmp);
  printHeader(&sumProfileStream);

  sumProfileStream<<"### COMPENSATED PROFILE FOR EACH IMPULSE"<<"\n";
  sumProfileStream<<"time         signal1        signal2     ...      signal(fBaseRun->GetNumpuls())"<<"\n";
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    float time=fBaseRun->GetTau()*i;
    sumProfileStream<<std::setw(6)<<std::left<<i;
    for (int j=0; j<fBaseRun->GetNumpuls(); j++){
      sumProfileStream<<std::left<<std::setw(20)<<fCompensatedSignal.GetSignal(i+j*fBaseRun->GetNumpointwin());
    }
    sumProfileStream<<std::endl;
  }
  std::cout<<"   массив индивидуальных импульсов записан в файл: "<<tmp<<std::endl;
  return 1;
}

int PulseExtractor::DoCompensation()
{
  compensateDM();
  return 1;
}

int PulseExtractor::SumPeriods()
{
  sumPeriods();
  return 1;
}

int PulseExtractor::SumPerChannelPeriods()
{
  if (fIsDynSpecAvailable) return 1;
  fIsDynSpecAvailable=true;
  sumPerChannelPeriods();
  return 1;
}

std::vector<float> PulseExtractor::GetSumPeriodsVec()
{
  std::vector<float> vec;
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    vec.push_back(fCompensatedSignalSum.GetSignal(i));
  }
  return vec;
}

SignalContainer PulseExtractor::GetCompensatedImpulse(int i)
{
  if (fDoNormToUnity){
    normToUnity(&fCompensatedSignal);
  }
  
  SignalContainer returnProfile(fBaseRun->GetNumpointwin(),0,fBaseRun->GetTau()*fBaseRun->GetNumpointwin());
  for (int k=0; k<fBaseRun->GetNumpointwin(); k++){
    returnProfile.SetSignal(k,fCompensatedSignal.GetSignal(i*fBaseRun->GetNumpointwin()+k));
  }
  
  return returnProfile;
}

std::vector<float> PulseExtractor::GetCompensatedImpulseVec(int i)
{
  if (fDoNormToUnity){
    normToUnity(&fCompensatedSignal);
  }
  
  std::vector<float> returnVec;
  for (int k=0; k<fBaseRun->GetNumpointwin(); k++){
    returnVec.push_back(fCompensatedSignal.GetSignal(i*fBaseRun->GetNumpointwin()+k));
  }
  return returnVec;
}

int PulseExtractor::removeSpikes(float nVar)
{
  std::cout<<"удаление импульсных помех"<<std::endl;
  
  SignalContainer sumSigRef(fBaseRun->GetNPoints(),0,fBaseRun->GetNPoints()*fBaseRun->GetTau());

  //calculate reference signal sum with DM=0
  for (int y=0; y<fBaseRun->GetNChannels(); y++){
    if (fChannelMask[y]==0) continue;
    for (int i=0; i<fBaseRun->GetNPoints(); i++){
      sumSigRef.SetSignal(i,sumSigRef.GetSignal(i)+fBaseRun->GetChannelSignal(y)->GetSignal(i)*fChannelMask[y]);
    }
  }

  //find spikes
  for (int i=0; i<fBaseRun->GetNPoints(); i++){
    float median, variance;
    if (i>=5) {
      median=sumSigRef.GetSignalMedian(i-5, i+5);
      variance=sumSigRef.GetSignalVariance(i-5, i+5);
    }
    else {
      median=sumSigRef.GetSignalMedian(0, i+5);
      variance=sumSigRef.GetSignalVariance(0, i+5);
    }
    if (variance==0) continue;
    if (fabs(sumSigRef.GetSignal(i)-median)/variance > nVar) {
      fSpikeMask[i]=0;
      for (int y=0; y<fBaseRun->GetNChannels(); y++){
	if (fChannelMask[y]==0) continue;
	fBaseRun->GetChannelSignal(y)->SetSignal(i,pow(fChannelMask[y],-1)*median/fBaseRun->GetNChannels());
      }
    }
  }
  return 1;
}

int PulseExtractor::frequencyFilter(float nVar)
{
  std::cout<<"удаление зашумленных каналов"<<std::endl;
  SignalContainer buf(fBaseRun->GetNChannels(),0,fBaseRun->GetNChannels());
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    buf.SetSignal(i,fBaseRun->GetFreqResponse(i));
  }
  float variance=buf.GetSignalVariance(0,1000000);
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    float med=0;
    if (i>=5||i<=fBaseRun->GetNChannels()-5){
      med=buf.GetSignalMedian(i-5,i+5);
      variance=buf.GetSignalVariance(i-5,i+5);
    }
    else if (i<5) {
      med=buf.GetSignalMedian(0,i+5);
      variance=buf.GetSignalVariance(0,i+5);
    }
    else if (i>fBaseRun->GetNChannels()-5) {
      med=buf.GetSignalMedian(i-5,100000);
      variance=buf.GetSignalVariance(i-5,100000);
    }
    if (variance==0) continue;
    if (fabs(buf.GetSignal(i)-med)/variance>nVar) {
      fChannelMask[i]=0;
    }
  }
  return 1;
}

int PulseExtractor::FillMaskFRweights()
{
  std::cout<<"вычисление весов каналов по АЧХ"<<std::endl;
  float frmean=0;
  float frweight=0;
  int nchan=0;
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    if (fChannelMask[i]==0) continue;
    frmean+=fBaseRun->GetFreqResponse(i);
    nchan++;
  }
  
  if (frmean==0||nchan==0) return 0;
  frmean=frmean/nchan;
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    if (fChannelMask[i]==0) continue;
    if (fBaseRun->GetFreqResponse(i)==0) {
      fChannelMask[i]=0;
      continue;
    }
    frweight=pow(fBaseRun->GetFreqResponse(i)/frmean,-1);
    fChannelMask[i]=frweight;
  }

  int nActive=0;
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    if (fChannelMask[i]!=0) nActive++;
  }
  fNChanAfterMask=nActive;
  fSumProfile.nChanAfterMask=fNChanAfterMask;
  
  //  if (fDoNormToUnity) normToUnity();
  
  return 1;
}

int PulseExtractor::RemoveSpikes(float nVar)
{
  removeSpikes(nVar);
  return 1;
}

int PulseExtractor::CleanFrequencyResponse(float nVar)
{
  frequencyFilter(nVar);
  return 1;
}


std::vector<float> PulseExtractor::GetChannelSumProfile(int iChan)
{
  std::vector<float> rVec;
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    rVec.push_back(fDynamicSpectrum[iChan].GetSignal(i)*fChannelMask[iChan]);
  }
  return rVec;
}


void PulseExtractor::normToUnity(SignalContainer* scont)
{
  //  float mean=scont->GetSignalMean(0,10000000);
  float smax=-100000;
  
  for (int i=0; i<scont->GetNbins(); i++){
    if (scont->GetSignal(i)>smax) smax=scont->GetSignal(i);
  }

  for (int i=0; i<scont->GetNbins(); i++){
    if (smax!=0) scont->SetSignal(i,scont->GetSignal(i)/smax);
  }
  
  //  for (int i=0; i<fBaseRun->GetNChannels(); i++){
  //    if (fChannelMask[i]==0) continue;
  //    mean+=fBaseRun->GetChannelSignal(i)->GetSignalMean(0,10000000);
  //  }
  //  mean=mean/fNChanAfterMask;
  //  for (int i=0; i<fBaseRun->GetNChannels(); i++){
  //    if (fChannelMask[i]==0) continue;
  //    for (int j=0; j<fBaseRun->GetNPoints(); j++){
  //      float val=fBaseRun->GetChannelSignal(i)->GetSignal(j);
  //      fBaseRun->GetChannelSignal(i)->SetSignal(j,val/mean);
  //    }
  //  }
  //  return 1;
}
Cor::Cor(){}; // конструктор
Cor::~Cor(){};

// кросс-корреляция данных  
//************************

void Cor::ccf(SumProfile finPulse, std::string rawdata_dir, std::string output_dir, std::string runs, std::string tplfile, float utcloc) 
// void Cor::ccf(SumProfile finPulse, ConfigParam conf, float utcloc) 
{

//BaseRun br;

std::string sdummy;
int kkfyes,toayes,kkfdatyes; // =1 при успешной записи в файлы kkf и toa

float dt = finPulse.tau;       // дискрет меду отсчетами в профиле

long double mjd; 
Tpl tpl;
Skkf kkf;
Itoa itoa;

float dtkkf=0.;
int kkflen;
float dummy;
float maxkkf;
int maxi;
float startdelay; // задержка после времени старта указанной в протоколе в мкс, для ЦПП-DPP1=1 дискрету
float CMprf, CMtpl;             // центры масс профиля и шаблона

kkf.numpoint=finPulse.numpointwin; // присвоение длины ккф в структуре
tpl = Tplread(tplfile);            // считывкание файла шаблона из файла 'tplfile'


kkf.kkfdata=dccf(tpl,finPulse); // вычисление дискретной ККФ и запись в структуру Skkf


maxkkf=kkf.kkfdata[0]; 
maxi=0;
for (int i=0; i<finPulse.numpointwin;i++) 
	if (maxkkf<=kkf.kkfdata[i]) 
	{
	maxkkf=kkf.kkfdata[i];
        maxi=i;
	};

for (int i=0; i<5; i++) kkf.kk.push_back(kkf.kkfdata[maxi-2+i]);

//for(int i=0; i<finPulse.numpointwin;i++) std::cout << kkf.kkfdata[i] << std::endl;

if (!strcmp(finPulse.rtype.c_str(),"DPP1")) startdelay=1000.*dt; else startdelay=0.;

CMprf= CofM(finPulse.prfdata, finPulse.numpointwin, finPulse.tau);
std::cout << "Центр Масс профиля:   " << CMprf << std::endl; 
CMtpl = CofM(tpl.tpldata, tpl.numpoint, tpl.tau);
std::cout << "Центр Масс шаблона:   " << CMtpl << std::endl; 


if (CMprf >= CMtpl) kkf.maxp=startdelay+1000.*dt*(maxi+ApproxMax(kkf.kk[0], kkf.kk[1],kkf.kk[2],kkf.kk[3],kkf.kk[4]));
	else kkf.maxp=-1000.*finPulse.numpointwin*finPulse.tau+startdelay+1000.*dt*(maxi+ApproxMax(kkf.kk[0], kkf.kk[1],kkf.kk[2],kkf.kk[3],kkf.kk[4]));

itoa.TOAMJD = utc2mjd(finPulse, utcloc, kkf.maxp); // расчет MJD.MJD и запись в структуру Itoa

itoa.sMJD = utc2mjds(finPulse, utcloc, kkf.maxp);

kkf.snr = SNR(finPulse);
kkf.errmax = CalcErrorW50(finPulse, kkf.snr);
//структура kkf заполнена

itoa.psrname = finPulse.psrname;

itoa.TOAerr = kkf.errmax;
if (kkf.errmax<=999.) itoa.TOAerr = kkf.errmax; else itoa.TOAerr = 999.0; // чтобы не вылезать из формата itoa
itoa.freq = finPulse.freq;
itoa.ddm = 0.0; // поправка вносится в файл itoa только по многочастотным данным
itoa.obscode = finPulse.obscode;


std::cout << "ККФ:<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
std::cout << "Значения ККФ в окрестности максимума:" << std::endl;

printf ("%10.4f%10.4f%10.4f%10.4f%10.4f\n", kkf.kk[0],kkf.kk[1],kkf.kk[2],kkf.kk[3],kkf.kk[4]);
std::cout << "\n  Max ККФ   мкс  ||  ошибка   мкс  || отношение сигнал/шум " << std::endl;
printf ("%15.2f%15.2f%15.2f \n ", kkf.maxp, kkf.errmax, kkf.snr);

std::cout << "МПИ:<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
std::cout << "Пульсар |    МПИ(MJD)  | Err(мкс)| Частота(МГц)  | поправка DM(мкс) | Код обсерватории" << std::endl;
printf("%-9s%19.13llf%6.2f%12.5f%9.6f%4s\n",itoa.psrname.c_str(),itoa.TOAMJD,itoa.TOAerr,itoa.freq,itoa.ddm,itoa.obscode.c_str());

kkfyes = KKFWrite(output_dir, kkf);
toayes=TOAWrite(output_dir, itoa);
kkfdatyes=KKFdatWrite(output_dir, kkf, runs);


}; 
// end of function ccf
//**********************************



// функция расчета MJD.MJD из даты, возвращает mjd в формате long double
//******************************************************************************
// dtkkf - сдвиг профиля относительно шаблона (максимум ККФ) в микросекундах

long double Cor::utc2mjd(SumProfile finPulse, float utcloc, float dtkkf)
{
int yy=2000+finPulse.utcyear; // yy+2000 - т.к. формат года в dutc - двузначный
int mm=finPulse.utcmonth;
int dd=finPulse.utcday;
int hh=finPulse.utchour-utcloc;
int min=finPulse.utcmin;
long double ss=finPulse.utcsec;
int imjd;
long double fmjd;
long double mjd;


if (hh < 0) {hh+=24; dd-=dd;} // переход через сутки если по гринвичу не наступила текущая дата

// усложненная процедура расчета чтобы избежать набега ошибки из-за нехватки разрядной сетки
imjd=367*yy-7*(yy+(mm+9)/12)/4-3*((yy+(mm-9)/7)/100+1)/4 // расчет целого числа MJD
        +275*mm/9+dd+1721028-2400000;					 
fmjd=hh/24.+min/1440.+ss/86400.+dtkkf/86400./1000000.; // расчет дробной части mjd до 13 знака

mjd=imjd+fmjd;

//printf ("MJD########## из utc2mjd: %25.13llf \n ", mjd);

return mjd;
}

//******************************************************************************
// функция расчета MJD.MJD из даты, возвращает mjd в формате string

std::string Cor::utc2mjds(SumProfile finPulse, float utcloc, float dtkkf)
{
int yy=2000+finPulse.utcyear; // yy+2000 - т.к. формат года в dutc - двузначный
int mm=finPulse.utcmonth;
int dd=finPulse.utcday;
int hh=finPulse.utchour-utcloc;
int min=finPulse.utcmin;
long double ss=finPulse.utcsec;
int imjd;
long double fmjd;
long double mjd;
std::string smjd;
char tmp[100];

if (hh < 0) {hh+=24; dd-=dd;} // переход через сутки если по гринвичу не наступила текущая дата

// усложненная процедура расчета чтобы избежать набега ошибки из-за нехватки разрядной сетки
imjd=367*yy-7*(yy+(mm+9)/12)/4-3*((yy+(mm-9)/7)/100+1)/4 // расчет целого числа MJD
        +275*mm/9+dd+1721028-2400000;					 
fmjd=hh/24.+min/1440.+ss/86400.+dtkkf/86400./1000000.; // расчет дробной части mjd до 13 знака

mjd=imjd+fmjd;

//smjd << mjd;
//sprintf ("%25.13llf \n ",smjd.c_str(), mjd);

sprintf(tmp,"%19.13llf",tmp,mjd);
for (int i=0; i<19; i++) smjd.push_back(tmp[i]);
smjd.push_back('\0');
return smjd;

}




//Чтение файла шаблона. Возвращает шаблон в структуре Tpl
//******************************************************************************
//Cor::Tpl Cor::Tplread(std::string rawdata_dir)
Tpl Cor::Tplread(std::string tplfile)  
{
//Cor::Tpl tpl;
Tpl tpl;
std::string tfilename=tplfile;
std::ifstream itpl(tfilename.c_str());
float dummy;
std::string sdummy;

if (!itpl) {std::cerr << "Не могу найти файл шаблона " <<  tfilename << "  аварийный выход " <<std::endl;  exit(0);}
	else {std::cout << "Шаблон считан из файла " << tfilename << std::endl;};

itpl >> tpl.psrname >> tpl.date >> tpl.freq >> tpl.tau >> tpl.numpoint >> sdummy; 
// std::cout <<  tpl.psrname <<"\n" << tpl.date << "\n" << tpl.freq << "\n" << tpl.tau << "\n" << tpl.numpoint << "\n" << sdummy << "\n"; 
for (int i=0;i<tpl.numpoint;i++) {itpl >> dummy; tpl.tpldata.push_back(dummy);};

return tpl;
};

//Cor::Tpl ScaleTpl(Cor::Tpl intpl, float tau)
Tpl Cor::ScaleTpl(Tpl intpl, float tau) {return intpl;};

int Cor::TOAWrite(std::string output_dir, Itoa itoa)
{
   std::string tfilename=output_dir+"/toa";  

   FILE * pFile;
   
   pFile = fopen (tfilename.c_str(),"a");
   fprintf(pFile,"%-9s%19.13llf%6.2f%12.5f%9.6f%4s\n","1133+16",itoa.TOAMJD,itoa.TOAerr,itoa.freq,itoa.ddm,"PO");
   fclose (pFile);
   std::cout<<"МПИ добавлены в файл: "<<tfilename<<std::endl;
   return 1;
};
// Endof Tplread
//**********************************************************************************************



//**********************************************************************************************
// Запись результат кросс-корреляции в файл output_dir/kkf
int Cor::KKFWrite(std::string output_dir, Skkf kkf)
{ 
   std::string tfilename=output_dir+"/kkf";
   
   FILE * pFile;
   
   pFile = fopen (tfilename.c_str(),"a");
   fprintf (pFile, "%10.4f%10.4f%10.4f%10.4f%10.4f%15.2f%15.2f%15.2f \n", kkf.kk[0],kkf.kk[1],kkf.kk[2],kkf.kk[3],kkf.kk[4],kkf.maxp, kkf.errmax, kkf.snr);
   fclose (pFile);
   std::cout<<"Параметры кросс-корреляции добавлены в файл: "<<tfilename<<std::endl;
   return 1;
};
//Endof KKFWrite
//**********************************************************************************************


//**********************************************************************************************
// Запись значения ККФ в файл output_dir/*.kkf
int Cor::KKFdatWrite(std::string output_dir, Skkf kkf, std::string runs)
  {
   
  std::ofstream kkfStream;
  char tmp[100];
  sprintf(tmp,"%s/%s.kkf",output_dir.c_str(),runs.c_str());
  kkfStream.open(tmp);

  for (int i=0; i<kkf.numpoint-1; i++){
    kkfStream<<i<<"     "<<kkf.kkfdata[i]<<"\n";
    }
  std::cout<<"ККФ записана в файл: "<<tmp<<std::endl;
  return 1;
  }

//*********************************************************************************************
// расчет центра масс вектора данных

float Cor::CofM(std::vector<float> data, int datalength, float dt)
{
float cm;
float mass;
cm=0;
mass=0;
float min,max;
std::vector<float> d=std::vector<float>(datalength,0);
for (int i=0; i<datalength-1; i++) d[i]=data[i];

d[datalength]=0;

min=0;
min=max;

//for (int i=0; i<datalength-1; i++)
//	{
//	if (max <= d[i]) max=d[i];
//	if (min >= d[i]) min=d[i];
//	}
//for (int i=0; i<datalength-1; i++) d[i]=(d[i]-min)/(max-min); // вычитание 0 и нормировка на 1


for (int i=0; i<datalength; i++) {mass=mass+d[i];};
for (int i=0; i<datalength; i++) {cm=cm+d[i]*dt*i*1000.;};
cm=cm/mass;
return cm;
}


//**********************************************************************************************
//расчет дискретной ККФ кросс-корреляцией шаблона и профиля и норммирование на 1

std::vector<float> Cor::dccf(Tpl tpl, SumProfile prf)

{
Cor cor;
int ntpl = tpl.numpoint;         // число точек в шаблоне
const int np = prf.numpointwin;   // число точек в профиле     
std::vector<float> d;            // дискретная ккф
const int kkflen = np;                     // длина массива ККФ по умолчанию если массив профиля > массива шаблона = длине профиля
float min,max;
std::vector<float> kkfarr=std::vector<float>(kkflen,0);            // массив для корреляции 
float swap;
std::vector<float> pr=std::vector<float>(np,0);                   // временный массив с профилем


for (int i=0; i<kkflen; i++) kkfarr[i]=0; // инициализация массивов
for (int i=0; i<np; i++) pr[i]=prf.prfdata[i]; 

pr[np-1]=pr[np-2]; // устранение вероятного '0' в последней точке профиля


for (int i=0; i<kkflen; i++)    // расчет ККФ 
	{          
	for (int j=0; j<ntpl; j++) kkfarr[i]=kkfarr[i]+tpl.tpldata[j]*pr[j]/ntpl; // расчет ккф
	swap=pr[0];        // циклический сдвиг массива влево
	for (int k=0; k<kkflen-1; k++) pr[k]=pr[k+1]; 
	pr[kkflen-1]=swap;
	};

max=kkfarr[0];
min=max;
for (int i=0; i<kkflen; i++)
	{
	if (max <= kkfarr[i]) max=kkfarr[i];
 	if (min >= kkfarr[i]) min=kkfarr[i];
	}
for (int i=0; i<kkflen; i++) kkfarr[i]=(kkfarr[i]-min)/(max-min); // вычитание 0 и нормировка на 1
for (int i=0; i<kkflen; i++) d.push_back(kkfarr[i]);              // возврат значения в вектор
return d;
}; 


//**********************************************************************************************
//Расчет максимума ККФ вписыванием полинома 4 степени 5 точек вблизи максимума дискретной ККФ, 
// возвращает положение реального максимума относительно дискретного в отсчетах

float Cor::ApproxMax(float y1, float y2, float y3, float y4, float y5) 
{

//float y1,y2,y3,y4,y5; //y3 - точка максимума
int npoli=4001; // число точек на единицу сетки в которыx ищется максимум путем перебора на сетке с дискретом 1/100 сетки
float x,maxpoli,smaxpoli,ndelta,maxpoint;

float a,b,c,d,e; // коэффициенты полинома 5 степени на сетке -2,-1,0,1,2

//y1=kkfarr[maxpoint-2];y2=kkfarr[maxpoint-1];y3=kkfarr[maxpoint];y4=kkfarr[maxpoint+1];y5=kkfarr[maxpoint+2];
//test
//y1=-1.; y2=0.3; y3=1.; y4=1.; y5=-1.;

a=y1/24-y2/6+y3/4-y4/6+y5/24;
b=-(y1/12)+y2/6-y4/6+y5/12;
c=-(y1/24)+(2*y2)/3-(5*y3)/4 + (2*y4)/3 - y5/24;
d=1./12*(y1-8*y2+8*y4-y5);
e=y3;

//поиск локального максимума ККФ вблизи точки дискретного максимума перебором по сетке до 1/100 ККФ


// maxpoli0=kkfarr[maxpoint-2];
maxpoli=y1;
//std::cout << "x =   " << x << "ndelta =  " << ndelta << std::endl;
for (int i=0; i<npoli; i++)
	{
	ndelta = 4./(npoli-1); 
	x=-2.+i*ndelta; // перебор по сетке от -2 до 2
	smaxpoli=a*x*x*x*x+b*x*x*x+c*x*x+d*x+e;
	if (smaxpoli>=maxpoli)  {maxpoli=smaxpoli; maxpoint=x;};
	};

// printf ("MAXIMUM CCF########## из ApproxMax: %13f\n%10f \n", maxpoli,maxpoint);
return maxpoint;
};
//**************************************************************************************************

//**************************************************************************************************
// расчет ошибки вписывания шаблона возвращает значение в мкс
// оценка ошибки вписывания по ширине и SNR элемента профиля, Труды ФИАН т.199 "Пульсары", ф.(11) стр. 153

float Cor::CalcErrorW50(SumProfile prf,float snr)
{
int n=prf.numpointwin;
int right,left;
std::vector<float> a=std::vector<float>(n,0);
float sigma;
float sum;
float max;
int imax;
float w50; // ширина импульса на 50% интенсивности

max=0; imax=0; right=0; left=0;
for (int i=0; i<n; i++) a[i]=prf.prfdata[i]; a[n-1]=a[n-2];// присвоение значений временного массива
for (int i=0; i<n; i++) if (max <= a[i]) {max=a[i]; imax=i;};

right=imax; while (a[right]>=max/2.) right++;
left=imax; while (a[left]>=max/2.) left--;
w50=prf.tau*(right-left)*1000;

// std::cout << "right=" << right << "  left=" << left << "   w50=" << w50 << std::endl;


return 0.3*w50/snr;
}; 

//**************************************************************************************************


//**************************************************************************************************
// расчет отношения сигнал/шум в профиле
float Cor::SNR(SumProfile prf)
{
int n=prf.numpointwin;int nsigmapoints;
int counter;
float min;
float max;
 std::vector<float> a=std::vector<float>(n,0);
 std::vector<float> sa=std::vector<float>(n,0);
float sigma;
float sum;
float average;
float porog=0.1; // порог отсечки для вычисления СКО
float step=0.05;

for (int i=0; i<n; i++) a[i]=prf.prfdata[i]; a[n-1]=a[n-2];// присвоение значений временного массива

min=0; max=0;
// вычитание 0 и нормировка на 1
for (int i=0; i<n; i++)
	{
	if (max <= a[i]) max=a[i];
 	if (min >= a[i]) min=a[i];
	};
for (int i=0; i<n; i++) a[i]=(a[i]-min)/(max-min); 
for (int i=2; i<n-2; i++) sa[i]=(a[i-2]+a[i-1]+a[i]+a[i+1]+a[i+2])/5; sa[0]=sa[2]; sa[1]=sa[2]; sa[n-2]=sa[n-3]; sa[n-1]=sa[n-3];// сглаживание вспом массива по 5 точкам

sum=0.; nsigmapoints=0; sigma=0; average=0;
// расчет шумовой дорожки до 10% максимума несглаженного профиля, сглаживание введно для подавления импульсных помех при малых SNR
counter=n-1;
do 
{	
while (sa[counter]<=porog) {sum=sum+a[counter]; nsigmapoints++; counter--;}
porog+=step;
} while (nsigmapoints <=10); 

//std::cout << ">> nsigmapoints, porog =   " << nsigmapoints <<" , " << porog-step<< std::endl;

average=sum/nsigmapoints;
for (int i=n-nsigmapoints; i<n; i++) sigma+=(a[i]-average)*(a[i]-average);
sigma=sqrt(sigma/nsigmapoints); // вычисление СКО

return 1./sigma; 
}; 

//**************************************************************************************************


int test(int argc, char *argv[])
{
  //read input with options
  if (argc > 3) {
    std::cout<<"too many params"<<std::endl;
    return 1;
  }
  
  std::string configName;

  if (argc==3){
    if (strcmp("-f",argv[1])==0) configName=std::string(argv[2]);
    else {
      std::cout<<argv[1]<<": wrong parameter option, only -f option is understood"<<std::endl;
      return 1;
    }
  }
  if (argc==2){
    std::cout<<argv[1]<<": do not understand this parameter"<<std::endl;
    return 1;
  }
  
  if (argc==1){
    configName="config/mainConfig.cff";
  }
  
  ConfigParam conf=ReadConfig(configName);

  //шапка программы:
  std::cout<<"#######################################################"<<std::endl;
  std::cout<<"# Программа для расчета МПИ пульсаров run_timer v 2.0 #"<<std::endl;
  std::cout<<"# ПРАО АКЦ ФИАН                                  2018 #"<<std::endl;
  std::cout<<"#######################################################"<<std::endl;

  // создаем контейнер для данных сеанса
  BaseRun br;
  //loop over files
  
  for (int iPack=0; iPack<floor(conf.runs.size()); iPack++){

    // считываем данные сеанса
    br.ReadRAWData(conf.runs[iPack], conf.rawdata_dir, conf.output_dir);

    // создаем объект класса для обработки
    PulseExtractor pulse(&br);

    //  считываем маску
    pulse.ReadMask("examples/channel_mask.dat");

    pulse.NormaliseToUnity(!conf.useTrueNorm);
    pulse.FillMaskFRweights();

    // фильтры на данный момент отсутствуют (13.07.17)
    if (conf.doFRfiltering) pulse.CleanFrequencyResponse(conf.nVarFR);
    if (conf.doRemoveSpikes) pulse.RemoveSpikes(conf.nVarSpike);
    
    if (!conf.getIndImpulses){
      pulse.SumPerChannelPeriods();
      pulse.DoCompensation();
      if (conf.getDynSpectrum) pulse.PrintChannelSumProfile(conf.output_dir.c_str());
    }
    
    if (conf.getIndImpulses){
      if (!conf.getDynSpectrum){
	pulse.DoCompensation();
	pulse.PrintCompensatedImpulses(conf.output_dir.c_str());
      }
      if (conf.getDynSpectrum) {
	pulse.DoCompensation();
	pulse.PrintCompensatedImpulses(conf.output_dir.c_str());
	pulse.SumPerChannelPeriods();
	pulse.DoCompensation();
	pulse.PrintChannelSumProfile(conf.output_dir.c_str());
      }
    }
    
    /*
    // суммируем периоды для каждой из частот
    if (conf.getDynSpectrum) {
      pulse.SumPerChannelPeriods();
      pulse.PrintPerChannelSumProfile(conf.output_dir.c_str());
      // компенсируем dm 
      pulse.DoCompensation();
    }
    */
    
    // суммируем периоды для компенсированных данных
    pulse.SumPeriods();
    
    // далее получаем данные
    // получение структуры суммарного импульса
    SumProfile finPulse=pulse.GetSumProfile();
    // распечатать суммарный импульс в файл
    pulse.PrintSumProfile(conf.output_dir.c_str());
    //  распечатать суммарные импульсы по частотам в файл
    // pulse.PrintPerChannelSumProfile(output_dir.c_str());

    //  распечатать АЧХ в файл
    if (conf.getFR) pulse.PrintFrequencyResponse(conf.output_dir.c_str());

    // получить суммарный импульс в виде вектора
    //std::vector<float> sumpuls=pulse.GetSumPeriodsVec();

    // СЮДА МОЖНО ДОБАВЛЯТЬ КОД ДЛЯ КРОСС-КОРРЕЛЯЦИИ

    Cor cor;
    Skkf skkf;
     
    cor.ccf(finPulse, conf.rawdata_dir, conf.output_dir, conf.runs[iPack], conf.tplfile, conf.utccorr);
    
  }
  std::cout<<"######################################"<<std::endl;
  std::cout<<"# работа программы успешно завершена #"<<std::endl;
  std::cout<<"######################################"<<std::endl;  
  return 0;
}
