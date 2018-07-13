#include <iostream>
#include <fstream> 
#include <stdint.h>
#include <cmath>
#include <bitset>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iomanip>
#include <stdlib.h>
#include "structRunTimer.h"


int convertStringParam(std::string IN, bool* OUT);
ConfigParam ReadConfig(std::string cname);
int chToNum(char ch);
double readNumber(char* buffer, int iStart, int N0, int N1);
float pulseToFloat(unsigned int pulse, float tau);
int test(int argc, char *argv[]);

class SignalContainer
{
 public:
  SignalContainer();
  SignalContainer(std::vector<float> timeValues);
  SignalContainer(float, float, float);
  SignalContainer(std::vector<float> timeValues, std::vector<float> signalValues);
  SignalContainer(std::vector<float> timeValues, std::vector<float> signalValues, std::vector<float> errors);
  ~SignalContainer();

  float GetSignalMean(int t0, int t1);
  float GetSignalMedian(int t0, int t1);
  float GetSignalVariance(int t0, int t1);

  float GetTime(int iBin);
  float GetSignal(int iBin);
  
  int SetSignal(int iBin, float value);
  
  float GetSignalError(int iBin);
  int SetSignalError(int iBin, float value);

  int GetNbins() {return fNTimeBins;}
  
 private:
  int fNTimeBins;
  float fTimeStep;
  // float
  std::vector<float> fTimeVector;
  std::vector<float> fSignalVector;
  std::vector<float> fErrorVector;
  //  float getVectorMean();
  //  float getVectorVariance();
};

class BaseRun
{
  
 public:
  BaseRun();
  ~BaseRun();
  
  int ReadRAWData(std::string runID, std::string rawdata_dir, std::string output_dir);

  std::string GetRunID() {return fRunID;}
  
  //getters:
  std::string GetTelcode() {return fTelcode;}
  std::string GetObscode() {return fObscode;}
  std::string GetRtype() {return fRtype;}
  std::string GetPsrname() {return fPsrname;}
  std::string GetDatatype() {return fDatatype;}
  int GetNpol() {return fNpol;}
  
  int GetSumchan() {return fSumchan;}
  
  int GetUtcDay() {return fUtcday;}
  int GetUtcMonth() {return fUtcmonth;}
  int GetUtcYear() {return fUtcyear;}
  int GetUtcHour() {return fUtchour;}
  int GetUtcMinute() {return fUtcmin;}
  double GetUtcSecond() {return fUtcsec;}
  int GetDay() {return fDay;}
  int GetMonth() {return fMonth;}
  int GetYear() {return fYear;}
  int GetHour() {return fHour;}
  int GetMinute() {return fMinute;}
  double GetSecond() {return fSec;}
  
  double GetPeriod() {return fPeriod;}
  int GetNumpuls() {return fNumpuls;}
  int GetNumpointwin() {return fNumpointwin;}
  int GetNChannels() {return fNChannels;}
  float GetFreqFirst() {return fFreqFirst;}
  float GetFreqLast() {return fFreqLast;}
  float GetWLFirst() {return fWLFirst;}
  float GetWLLast() {return fWLLast;}
  float GetTau() {return fTau;}
  float GetDM() {return fDM;}

  int GetNPoints() {return fNPoints;}
  float GetDuration() {return fDuration;}

  SignalContainer* GetChannelSignal(int iband) {return &fPerChannelSignal[iband];}
  float GetFreqResponse(int iband) {return fFreqResponse[iband];} 
  
 private:
  std::string fTelcode;
  std::string fObscode;
  std::string fRtype;
  std::string fPsrname;
  std::string fDatatype;
  int fNpol;
  int fSumchan;

  int fUtcday;
  int fUtcmonth;
  int fUtcyear;
  int fUtchour;
  int fUtcmin;
  //  int fUtcsec;
  //  int fUtcnsec;
  long double fUtcsec;

  long double fPeriod;   //период пульсара
  int fNumpuls;          //число наблюденных импульсов
  int fNumpointwin;      //число измерений сигнала на импульс
  ////////////////
  
  int fNChannels;       //число каналов
  float fFreqFirst;  //низкая частота MHz
  float fFreqLast;   //высокая частота MHz
  float fWLFirst;    //большая длина волны m
  float fWLLast;     //маленькая длина волны m
  float fTau;        //время интегрирования сигнала s
  float fNPoints;    //число измерений сигнала в сеансе
  float fDuration;   //продолжительность сеанса
  //start time: 
  int fDay; 
  int fMonth;
  int fYear;
  int fHour;
  int fMinute;
  long double fSec;
  //  int fSecond;
  //  int fNsec;
  float fDM;

  std::string fRunID;
  
  std::vector<SignalContainer> fPerChannelSignal;         // хранилище первичного сигнала
  std::vector<float> fFreqResponse;			  // АЧХ
  std::vector<float> fFreqResponseMedian;
};

//  класс для обработки сеансов
//  все параметры сеанса считываются в конструкторе
class PulseExtractor : BaseRun
{
 public:
  PulseExtractor();
  PulseExtractor(BaseRun* run); // конструктор принимает ссылку на класс сеанса
  ~PulseExtractor();
  
  void SetBaseRun(BaseRun* br) {fBaseRun=br;} // можно задать ссылку на сеанс
  void SetDM(float DM) {fDM=DM;} // можно утановить произвольную DM 
  
  int DoCompensation(); // функция проводит компенсацию запаздывания, время всегда приводится к самой высокой частоте 112.084, если ранее было проведено суммирование периодов для отдельных частот, исполуются свернутые профили, иначе полные профили для каждой частоты
  int SumPeriods();   // функция суммирует периоды в компенсированных данных, если суммирование было проведено ранее для каждой из частот, функция ничего не меняет, только заполняет финальный объект SumProfile
  int SumPerChannelPeriods(); // функция суммирует периоды для каждой из частот
  
  float GetDM() {return fDM;}
  int PrintFrequencyResponse(std::string outdir); // записать АЧХ (средний сигнал на частоте) в файл с именем outdir/<номер сеанса>.fr
  int PrintSumProfile(std::string outdir);   // записать суммарный профиль в файл с именем outdir/<номер сеанса>.prf
  int PrintChannelSumProfile(std::string outdir); // записать суммарный профиль для каждого частотного канала в файл с именем outdir/bands_<номер сеанса>.prf
  int PrintCompensatedImpulses(std::string outdir);  //записать в файл набор компенсированных импульсов до fBasRun->GetNumpuls() 

  SumProfile GetSumProfile() {return fSumProfile;} // получить структуру, описанную в начале этог файла

  std::vector<float> GetSumPeriodsVec(); // получить суммарный профиль в виде вектора

  std::vector<float> GetChannelSumProfile(int iChan); //получить суммарный профиль для частотного канала iChan 
  
  SignalContainer GetCompensatedImpulse(int i); // получить i-й компенсированный импульс
  std::vector<float> GetCompensatedImpulseVec(int i); // получить i-й компенсированный импульс как вектор

  int FillMaskFRweights();  // заполняются веса частотных каналов в маске. Определяется среднее значение сигнала по всем частотам, M, вес для канала определен так: w=pow(m(f)/M, -1)
  
  int ReadMask(std::string fname); // считать маску частот
  void SetChannelMask(std::vector<float> mask) {fChannelMask=mask;} // задать маску
  std::vector<float> GetChannelMask() {return fChannelMask;} // получить маску в виде вектора
  
  int RemoveSpikes(float nVar);   // удалить шумовые импульсы, частотные каналы складываются с dm=0, выбросы > nVar*sigma заменяются на медианное значение подложки
  int CleanFrequencyResponse(float nVar);  //удалить зашемленные частоты, выбросы на АЧХ по модулю > nVar*sigma добавляются в маску с весом 0
  void NormaliseToUnity(bool fDo) {fDoNormToUnity=fDo;}
  

 private:
  BaseRun* fBaseRun;
  float fDM;
  SignalContainer fCompensatedSignal;
  SignalContainer fCompensatedSignalSum;
  std::vector<SignalContainer> fDynamicSpectrum;
  SumProfile fSumProfile;
  void normToUnity(SignalContainer* h);
  int compensateDM();
  int sumPeriods();
  int sumPerChannelPeriods();
  int fillSumProfile();
  int printHeader(std::ofstream* str);

  int removeSpikes(float);
  int frequencyFilter(float);
  
  std::vector<float> fChannelMask;

  bool fIsDynSpecAvailable;
  bool fDoNormToUnity;
  
  std::vector<int> fSpikeMask;

  int fNChan;
  int fNChanAfterMask;
};

class Cor { 

public:
Cor(); // конструктор
~Cor();

// private:

//int DateToMjd (int Year, int Month, int Day) {};

public:

// кросс-корреляция данных, основная функция, вызывает остальные, возращает МПИ в долях MJD

void ccf(SumProfile finPulse, std::string rawdata_dir, std::string output_dir, std::string runs, std::string tplfile, float utcloc); 

Tpl Tplread(std::string rawdata_dir); // чтение шаблона из файла tpl в директории rawdata_dir

Tpl ScaleTpl(Tpl intpl, float tau); // приведение шаблона к новому дискрету дискрет tau - дискрет к которому надо привести


std::vector<float> dccf(Tpl tpl, SumProfile prf); // расчет нормированной на 1 диксретной ККФ кросс-корреляцией шаблона и профиля

// расчет MJD по времени старта + максимум ККФ в микросекундах

long double utc2mjd(SumProfile finPulse, float utcloc, float dtkkf);

// поиск максимума ККФ вписыванием полинома в 5 точек в окрестностях максимума возвращает положение максимума в долях отсчета от-но точки дисерктного максимума
float ApproxMax(float, float, float, float, float); 

float CalcErrorW50(SumProfile finPulse, float snr); // расчет ошибки вписывания шаблона

float SNR(SumProfile finPulse); // расчет отношения сигнал/шум в профиле

float CofM(std::vector<float> data, int datalength, float dt); // расчет центра масс вектора данных

int TOAWrite(std::string output_dir, Itoa sitoa); // запись МПИ в файл toa в формате itoa

int KKFWrite(std::string output_dir, Skkf kkf);   // запись данных по кросс-корреляции в файлы kkfdat - график и kkf - результат вписывания

int KKFdatWrite(std::string output_dir, Skkf kkf, std::string runs);   // запись данных по кросс-корреляции в файлы kkfdat - график и kkf - результат вписывания

std::string utc2mjds(SumProfile finPulse, float utcloc, float dtkkf); // возвращает MJD как строку

}; //end class Cor
