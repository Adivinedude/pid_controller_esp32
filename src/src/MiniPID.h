#ifndef MINIPID_H
#define MINIPID_H

#define MINIPID_AVG_COUNT 	32 // must be 2^n
#define MINIPID_AVG_MASK	0x1F

class MiniPID{
public:
	MiniPID(float, float, float);
	MiniPID(float, float, float, float);
	void setP(float);
	void setI(float);
	void setD(float);
	void setF(float);
	void setPID(float, float, float);
	void setPID(float, float, float, float);
	void setMaxIOutput(float);
	void setOutputLimits(float);
	void setOutputLimits(float,float);
	void setDirection(bool);
	void setSetpoint(float);
	void reset();
	void setOutputRampRate(float);
	void setSetpointRange(float);
	void setOutputFilter(float);
	float getOutput(float);
	float getOutput(float, float);

//private:
	float clamp(float, float, float);
	bool bounded(float, float, float);
	void checkSigns();
	void init();
	
	float P;
	float I;
	float D;
	float F;

	float maxIOutput;
	float maxError;
	float errorSum;

	float maxOutput; 
	float minOutput;

	float setpoint;

	float lastActual[MINIPID_AVG_COUNT];
	float d_avg;
	unsigned char avg_iterator;

	bool firstRun;
	bool reversed;

	float outputRampRate;
	float lastOutput;

	float outputFilter;

	float setpointRange;
	
	float error;
	float Poutput;
	float Ioutput;
	float Doutput;
	float Foutput;
};
#endif
