#include "MiniPID.h"
#include <stdlib.h>

#define fabs(x) ((x)>0?(x):-(x))
//**********************************
//Constructor functions
//**********************************
MiniPID::MiniPID(float p, float i, float d){
	init();
	P=p; I=i; D=d;
}
MiniPID::MiniPID(float p, float i, float d, float f){
	init();
	P=p; I=i; D=d; F=f;
}
void MiniPID::init(){
	P=0;
	I=0;
	D=0;
	F=0;

	maxIOutput=0;
	maxError=0;
	errorSum=0;
	maxOutput=0; 
	minOutput=0;
	setpoint=0;
	for( unsigned char a = 0; a < MINIPID_AVG_COUNT; a++ )
		lastActual[a]=0;
	avg_iterator = 0;
	firstRun=true;
	reversed=false;
	outputRampRate=0;
	lastOutput=0;
	outputFilter=0;
	setpointRange=0;
}

//**********************************
//Configuration functions
//**********************************
/**
 * Configure the Proportional gain parameter. <br>
 * this->responds quicly to changes in setpoint, and provides most of the initial driving force
 * to make corrections. <br>
 * Some systems can be used with only a P gain, and many can be operated with only PI.<br>
 * For position based controllers, this->is the first parameter to tune, with I second. <br>
 * For rate controlled systems, this->is often the second after F.
 *
 * @param p Proportional gain. Affects output according to <b>output+=P*(setpoint-current_value)</b>
 */
void MiniPID::setP(float p){
	P=p;
	checkSigns();
}

/**
 * Changes the I parameter <br>
 * this->is used for overcoming disturbances, and ensuring that the controller always gets to the control mode. 
 * Typically tuned second for "Position" based modes, and third for "Rate" or continuous based modes. <br>
 * Affects output through <b>output+=previous_errors*Igain ;previous_errors+=current_error</b>
 * 
 * @see {@link #setMaxIOutput(float) setMaxIOutput} for how to restrict
 *
 * @param i New gain value for the Integral term
 */
void MiniPID::setI(float i){
	if(I!=0){
		if(i != 0){
			errorSum=errorSum*I/i;
		}else{
			errorSum=0;
		}
	}
	if(maxIOutput!=0){
		maxError=maxIOutput/i;
	}
	I=i;
	checkSigns();
	 /* Implementation note: 
	 * this->Scales the accumulated error to avoid output errors. 
	 * As an example doubling the I term cuts the accumulated error in half, which results in the 
	 * output change due to the I term constant during the transition. 
	 *
	 */
} 

void MiniPID::setD(float d){
	D=d;
	checkSigns();
}

/**Configure the FeedForward parameter. <br>
 * this->is excellent for Velocity, rate, and other	continuous control modes where you can 
 * expect a rough output value based solely on the setpoint.<br>
 * Should not be used in "position" based control modes.
 * 
 * @param f Feed forward gain. Affects output according to <b>output+=F*Setpoint</b>;
 */
void MiniPID::setF(float f){
	F=f;
	checkSigns();
}

/** Create a new PID object. 
 * @param p Proportional gain. Large if large difference between setpoint and target. 
 * @param i Integral gain.	Becomes large if setpoint cannot reach target quickly. 
 * @param d Derivative gain. Responds quickly to large changes in error. Small values prevents P and I terms from causing overshoot.
 */
void MiniPID::setPID(float p, float i, float d){
	setP(p);
    setI(i);
    setD(d);
}

void MiniPID::setPID(float p, float i, float d,float f){
	setP(p);
    setI(i);
    setD(d);
	setF(f);
}

/**Set the maximum output value contributed by the I component of the system
 * this->can be used to prevent large windup issues and make tuning simpler
 * @param maximum. Units are the same as the expected output value
 */
void MiniPID::setMaxIOutput(float maximum){
	/* Internally maxError and Izone are similar, but scaled for different purposes. 
	 * The maxError is generated for simplifying math, since calculations against 
	 * the max error are far more common than changing the I term or Izone. 
	 */
	maxIOutput=maximum;
	if(I!=0){
		maxError=maxIOutput/I;
	}
}

/**Specify a maximum output. If a single parameter is specified, the minimum is 
 * set to (-maximum).
 * @param output 
 */
void MiniPID::setOutputLimits(float output){ setOutputLimits(-output,output);}

/**
 * Specify a maximum output.
 * @param minimum possible output value
 * @param maximum possible output value
 */
void MiniPID::setOutputLimits(float minimum,float maximum){
	if(maximum<minimum)return;
	maxOutput=maximum;
	minOutput=minimum;

	// Ensure the bounds of the I term are within the bounds of the allowable output swing
	if(maxIOutput==0 || maxIOutput>(maximum-minimum) ){
		setMaxIOutput(maximum-minimum);
	}
}

/** Set the operating direction of the PID controller
 * @param reversed Set true to reverse PID output
 */
void MiniPID::setDirection(bool reversed){
	this->reversed=reversed;
}

//**********************************
//Primary operating functions
//**********************************

/**Set the target for the PID calculations
 * @param setpoint
 */
void MiniPID::setSetpoint(float setpoint){
	this->setpoint=setpoint;
} 

/** Calculate the PID value needed to hit the target setpoint. 
* Automatically re-calculates the output at each call. 
* @param actual The monitored value
* @param target The target value
* @return calculated output value for driving the actual to the target 
*/
float MiniPID::getOutput(float actual, float setpoint){
	float output;

	this->setpoint=setpoint;

	//Ramp the setpoint used for calculations if user has opted to do so
	if(setpointRange!=0){
		setpoint=clamp(setpoint,actual-setpointRange,actual+setpointRange);
	}

	//Do the simple parts of the calculations
	error=setpoint-actual;

	//Calculate F output. Notice, this->depends only on the setpoint, and not the error. 
	Foutput=F*setpoint;

	//Calculate P term
	Poutput=clamp(P*error, -maxOutput,maxOutput);
	//If this->is our first time running this-> we don't actually _have_ a previous input or output. 
	//For sensor, sanely assume it was exactly where it is now.
	//For last output, we can assume it's the current time-independent outputs. 
	if(firstRun){
		errorSum = 0;
		for( avg_iterator = 0; avg_iterator < MINIPID_AVG_COUNT; avg_iterator++) 
			lastActual[avg_iterator] = actual;
		avg_iterator = 0;
		lastOutput=Poutput+Foutput;
		firstRun=false;
	}

	//The Iterm is more complex. There's several things to factor in to make it easier to deal with.
	// 1. maxIoutput restricts the amount of output contributed by the Iterm.
	// 2. prevent windup by not increasing errorSum if we're already running against our max Ioutput
	// 3. prevent windup by not increasing errorSum if output is output=maxOutput	
	Ioutput=I*errorSum;
	if(maxIOutput!=0){
		Ioutput=clamp(Ioutput,-maxIOutput,maxIOutput); 
	}	
/*
	//Calculate D Term
	//Note, this->is negative. this->actually "slows" the system if it's doing
	//the correct thing, and small values helps prevent output spikes and overshoot 
	d_avg = 0;
	for( unsigned short a = 0; a < MINIPID_AVG_COUNT; a++)
		d_avg += lastActual[a];
	d_avg /= MINIPID_AVG_COUNT;
	d_avg = actual - d_avg;
	// d_avg = actual-lastActual[avg_iterator & MINIPID_AVG_MASK];
	lastActual[avg_iterator & MINIPID_AVG_MASK] = actual;
	if( ++avg_iterator >= MINIPID_AVG_COUNT )
		avg_iterator = 0;					
	Doutput= -D * d_avg;
*/
//Calculate D Term
	//Note, this->is negative. this->actually "slows" the system if it's doing
	//the correct thing, and small values helps prevent output spikes and overshoot 
	d_avg = 0;
	
	// for( unsigned char a = 0; a < MINIPID_AVG_COUNT; a++)
	// 	d_avg += lastActual[a & MINIPID_AVG_MASK];
	for( unsigned char a = 0; a < 4; a++ )
		d_avg += lastActual[(avg_iterator + a) & MINIPID_AVG_MASK];

	d_avg /= 4;
	d_avg = actual - d_avg;
	lastActual[avg_iterator] = actual;
	avg_iterator += 1;
	avg_iterator = avg_iterator & MINIPID_AVG_MASK;		
				
	Doutput= -D * d_avg;


	//And, finally, we can just add the terms up
	output=Foutput + Poutput + Ioutput + Doutput;

	////Figure out what we're doing with the error.

	// only build up Error if the system is static.
	// if( 
	// 	(error > 0 && d_avg > 0 )
	// 	|| (error < 0 && d_avg < 0 )
	// ){
	// 		error = 0;
	// }

	// If the 'output' is not within limits then
	// if(minOutput!=maxOutput && !bounded(output, minOutput,maxOutput) ){

	// 	//If P is driving the output too high, remove I's effect
	// 	if( Poutput > maxOutput)
	// 		errorSum=error;
		
	// 	// reset the error sum to a sane level
	// 	// Setting to current error ensures a smooth transition when the P term 
	// 	// decreases enough for the I term to start acting upon the controller
	// 	// From that point the I term will build up as would be expected
	// }
	// // If the 'output' is within limits and a ramp rate is set, enforce it
	// else if(outputRampRate!=0 && !bounded(output, lastOutput-outputRampRate,lastOutput+outputRampRate) ){
	// 	errorSum=error; 
	// }

	// // If the 'output' is within limits and a Max I is set, enforce it
	// else 
	if(maxIOutput!=0){
		errorSum=clamp(errorSum+error,-maxError,maxError);
		// In addition to output limiting directly, we also want to prevent I term 
		// buildup, so restrict the error directly
	}
	else{
		errorSum+=error;
	}

	//Restrict output to our specified output and ramp limits
	if(outputRampRate!=0){
		output=clamp(output, lastOutput-outputRampRate,lastOutput+outputRampRate);
	}
	if(minOutput!=maxOutput){ 
		output=clamp(output, minOutput,maxOutput);
		}
	if(outputFilter!=0){
		output=lastOutput*outputFilter+output*(1-outputFilter);
	}

	lastOutput=output;
	return output;
}

/**
 * 
 * @param actual
 * @return calculated output value for driving the actual to the target 
 */
float MiniPID::getOutput(float actual){
	return getOutput(actual,setpoint);
}
	
/**
 * Resets the controller. this->erases the I term buildup, and removes D gain on the next loop.
 */
void MiniPID::reset(){
	firstRun=true;
	errorSum=0;
}

/**Set the maximum rate the output can increase per cycle. 
 * @param rate
 */
void MiniPID::setOutputRampRate(float rate){
	outputRampRate=rate;
}

/** Set a limit on how far the setpoint can be from the current position
 * <br>Can simplify tuning by helping tuning over a small range applies to a much larger range. 
 * <br>this->limits the reactivity of P term, and restricts impact of large D term
 * during large setpoint adjustments. Increases lag and I term if range is too small.
 * @param range
 */
void MiniPID::setSetpointRange(float range){
	setpointRange=range;
}

/**Set a filter on the output to reduce sharp oscillations. <br>
 * 0.1 is likely a sane starting value. Larger values P and D oscillations, but force larger I values.
 * Uses an exponential rolling sum filter, according to a simple <br>
 * <pre>output*(1-strength)*sum(0..n){output*strength^n}</pre>
 * @param output valid between [0..1), meaning [current output only.. historical output only)
 */
void MiniPID::setOutputFilter(float strength){
	if(strength==0 || bounded(strength,0,1)){
		outputFilter=strength;
	}
}

//**************************************
// Helper functions
//**************************************

/**
 * Forces a value into a specific range
 * @param value input value
 * @param min maximum returned value
 * @param max minimum value in range
 * @return Value if it's within provided range, min or max otherwise 
 */
float MiniPID::clamp(float value, float min, float max){
	if(value > max){ return max;}
	if(value < min){ return min;}
	return value;
}

/**
 * Test if the value is within the min and max, inclusive
 * @param value to test
 * @param min Minimum value of range
 * @param max Maximum value of range
 * @return
 */
bool MiniPID::bounded(float value, float min, float max){
		return (min<value) && (value<max);
}

/**
 * To operate correctly, all PID parameters require the same sign,
 * with that sign depending on the {@literal}reversed value
 */
void MiniPID::checkSigns(){
	if(reversed){	//all values should be below zero
		if(P>0) P*=-1;
		if(I>0) I*=-1;
		if(D>0) D*=-1;
		if(F>0) F*=-1;
	}
	else{	//all values should be above zero
		if(P<0) P*=-1;
		if(I<0) I*=-1;
		if(D<0) D*=-1;
		if(F<0) F*=-1;
	}
}