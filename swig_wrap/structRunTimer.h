struct SumProfile {
  
  std::string telcode; // код телескопа, по умолчанию bsa1 возможные опции bsa2, bsa3, Klz64
  std::string obscode;  // 2-символьный код обсерватории, по умолчанию "PO" - Пущинская обсерватория, антенна БСА
  std::string rtype; // тип приемника, по умолчанию = "DPP1" (digital pulsar processor 1)
  std::string psrname; // соотв. переменной name в файле данных БСА - не более 10 символов
  std::string datatype; //тип данных, однобуквенный код, по умолчанию = 'I' - интенсивность
  int npol; // число поляризаций, по умолчанию для БСА = 1

  int sumchan;
  
  int nChan;
  int nChanAfterMask;
  
  int year; //дата наблюдений, соответствует date в файле данных БСА
  int month;
  int day;
  int hour; //время в текущей шкале локального стандарта, соответствует time hh:mm:ss
  int min;
  long double sec;

  int utcday;
  int utcmonth;
  int utcyear;
  int utchour; //время в истиной шкале локального стандарта, dt_utc hh:mm:ss - для БСА соответствует UTC+3 часа
  int utcmin;
  long double utcsec;
  
  long double period; // период пульсара в секундах
  int numpuls; // число импульсов, сложенных в данном суммарном профиле
  
  float tau; // дискрет отсчета в миллисекундах, соотв. переменной tay в файле данных БСА
  int numpointwin; // число точек в окне <= int(period/tau)
  float freq; //  приведенная частота, МГц, если суммировали по мере дисперсии к первому (самому низкочастотному) каналу БСА = 109.584, если к последнему, 512-му = 112.084, если к другому, то вычисляется с учетом ширины канала = 2500/511 кГц
  
  std::vector<float> prfdata; // последовательность отсчетов в суммарном, после устранения меры дисперсии и приведения к нулевому уровню (вычитания нулевого уровня), профиле. 
  
};

struct ConfigParam{
  std::string rawdata_dir;
  std::string output_dir;
  std::string maskfile; // добавлено мной 17.08
  std::string tplfile; // добавлено мной 28.08
  bool doRemoveSpikes;
  bool doFRfiltering;
  bool getDynSpectrum;
  bool getIndImpulses;
  bool getFR;
  bool useTrueNorm;
  float utccorr;
  float nVarFR;
  float nVarSpike;
  std::vector<std::string> runs;
};

struct Tpl {
  std::string psrname; // имя пульсара соотв. переменной name в файле данных БСА - не более 10 символов
  std::string date;   
  //int year; //дата создания шаблона
  //int month;
  //int day;

  float freq; //  приведенная частота, МГц
  float tau; // дискрет между отсчетами шаблона в микросекунднах
  int numpoint; // длина шаблона
  
  std::vector<float> tpldata; // последовательность отсчетов профиля шаблона 
};

struct Skkf{
std::vector<float> kk; // значение 5 точек в окрестностях максимума дискретной ККФ
int numpoint; //число точек в векторе ККФ
float maxp;  // максимальная точка ККФ в микросекундах
float errmax; // ошибка вписывания в микросекундах
float snr; // отношение сигнал/шум в профиле
std::vector<float> kkfdata;
};

struct Itoa {
std::string psrname; // имя пульсара
long double TOAMJD; // МПИ в MJD и долях MJD с точностью до 12-13 знака
int iMJD;
float fMJD;
std::string sMJD;
float TOAerr; // ошибка определения МПИ в мкс
float freq; // частота приема в мегагерцах
float ddm; // поправка в меру дисперсии (для многочастотных наблюдений)
std::string obscode; // одно- или двухбуквенный код обсерватории
};

extern struct SumProfile bar;
extern struct ConfigParam foo;
extern struct Tpl tpl_bar;
extern struct Skkf skkf_bar;
extern struct Itoa itoa_bar;