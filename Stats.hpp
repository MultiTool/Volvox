#ifndef STATS_HPP_INCLUDED
#define STATS_HPP_INCLUDED

/* ********************************************************************** */
class Stat {
public:
  double Min, Max, Avg;
  double StandardDev;

  /* ********************************************************************** */
  // copied from https://github.com/qubyte
  // A standard deviation object constructor. Running deviation (avoid growing arrays) which
  // is round-off error resistant. Based on an algorithm found in a Knuth book.
  double count, WorkData, PrevWorkData, S;
  void StandardDeviation(double FirstMeasurement) {// from https://gist.github.com/qubyte/4064710
	this->WorkData = FirstMeasurement;
	this->PrevWorkData = 0;
	this->S = 0;
	this->count = 1;
  }
  /* ********************************************************************** */
  // Add a measurement. Also calculates updates to stepwise parameters which are later used
  // to determine sigma.
  void AddMeasurement(double Measurement) {
	this->count += 1;
	this->PrevWorkData = this->WorkData;
	this->WorkData = this->WorkData + (Measurement - this->WorkData) / this->count;
	this->S = this->S + (Measurement - this->PrevWorkData) * (Measurement - this->WorkData);
  }
  /* ********************************************************************** */
  // Performs the final step needed to get the standard deviation and returns it.
  double GetStdDev() {
	return std::sqrt(this->S / (this->count - 1.0));
  }
};

/* ********************************************************************** */
class PopStats { // one of these to summarize hundreds of trials
public:
  Stat Score;// min max avg score for N trials with all the same parameters, but maybe a variety of models.
  Stat FinalGen;// generation number where we won or lost
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
