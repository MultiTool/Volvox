#ifndef STATS_HPP_INCLUDED
#define STATS_HPP_INCLUDED

/* ********************************************************************** */
class Stat {
public:
  double Min, Max, Avg;
  double Sum = 0, NumSamples = 0;
  double WorkData, PrevWorkData, PowerSum;// Std dev stuff
  /* ********************************************************************** */
  void Init(){// Initialize stats *before* we have a first sample value.
    this->Sum = 0.0; this->NumSamples=0; this->Avg=0.0;
    //this->Min =std::numeric_limits<double>::infinity(); this->Max =-std::numeric_limits<double>::infinity();// guaranteed to lose first comparisons.
    this->Min = INFINITY; this->Max = -INFINITY;// guaranteed to lose first comparisons.
    this->WorkData = 0;
    this->PrevWorkData = 0; this->PowerSum = 0;
  }
  /* ********************************************************************** */
  void Collect(double Measurement){
    this->NumSamples += 1;
    this->Sum += Measurement;
    this->Avg = this->Sum/this->NumSamples;
    if (this->Min>Measurement){ this->Min = Measurement; }
    if (this->Max<Measurement){ this->Max = Measurement; }
    this->UpdateStDev(Measurement);
  }
  /* ********************************************************************** */
  void FirstSample(double FirstMeasurement){
    this->NumSamples=1; this->Sum = FirstMeasurement;
    this->Avg = this->Max = this->Min = FirstMeasurement; //this->Min =std::numeric_limits<double>::infinity(); this->Max =-std::numeric_limits<double>::infinity();
    this->StartStandardDeviation(FirstMeasurement);
  }
  /* ********************************************************************** */
  void GetStats(double &Size, double &Sum, double &Minimum, double &Maximum, double &Average, double &StandardDev) {
    Minimum = Min; Maximum = Max; Average = this->Avg;
    Sum = this->Sum; Size = this->NumSamples;
    StandardDev = this->GetStdDev();
  }
  /* ********************************************************************** */
  // Copied from https://github.com/qubyte, https://gist.github.com/qubyte/4064710
  // A standard deviation object constructor. Running deviation (avoid growing arrays) which is round-off error resistant. Based on an algorithm found in a Knuth book.
  void StartStandardDeviation(double FirstMeasurement) {
    this->WorkData = FirstMeasurement;
    this->PrevWorkData = 0;
    this->PowerSum = 0;//this->NumSamples = 1;
  }
  /* ********************************************************************** */
  // Add a measurement. Also calculates updates to stepwise parameters which are later used to determine sigma.
  void UpdateStDev(double Measurement) {//AddMeasurement
    this->PrevWorkData = this->WorkData;//this->NumSamples += 1;
    this->WorkData = this->WorkData + (Measurement - this->WorkData) / this->NumSamples;
    this->PowerSum = this->PowerSum + (Measurement - this->PrevWorkData) * (Measurement - this->WorkData);
  }
  /* ********************************************************************** */
  // Performs the final step needed to get the standard deviation and returns it.
  double GetStdDev() {
    return std::sqrt(this->PowerSum / (this->NumSamples - 1.0));
  }
  /* ********************************************************************** */
  void Print_Me(){}
};

/* ********************************************************************** */
class PopStats;// forward
typedef PopStats *PopStatsPtr;
typedef std::vector<PopStatsPtr> PopStatsVec;
class PopStats { // one of these to summarize hundreds of trials
public:
  Stat Score;// min max avg score for N trials with all the same parameters, but maybe a variety of models.
  Stat FinalGen;// generation number where we won or lost
  void Init(){
    Score.Init(); FinalGen.Init();
  }
  void Print_Me(){}
};

#endif // STATS_HPP_INCLUDED


/*  // https://gist.github.com/qubyte/4064710
// A standard deviation object constructor. Running deviation (avoid growing arrays) which
// is round-off error resistant. Based on an algorithm found in a Knuth book.
function StandardDeviation(firstMeasurement) {
	this.workData = firstMeasurement;
	this.lastWorkData = null;
	this.S = 0;
	this.count = 1;
}

// Add a measurement. Also calculates updates to stepwise parameters which are later used
// to determine sigma.
StandardDeviation.prototype.addMeasurement = function (measurement) {
	this.count += 1;
	this.lastWorkData = this.workData;
	this.workData = this.workData + (measurement - this.workData) / this.count;
	this.S = this.S + (measurement - this.lastWorkData) * (measurement - this.workData);
};

// Performs the final step needed to get the standard deviation and returns it.
StandardDeviation.prototype.get = function () {
	return Math.sqrt(this.S / (this.count - 1));
};
*/
