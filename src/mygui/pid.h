#ifndef PID_H
#define PID_H
#include <stdint.h>
#include "MiniPID.h"
#include <string>
#include "MAX31855.h"
#include "config.h"


#ifndef MIN_ON_TIME
#define MIN_ON_TIME 1
#endif

#ifndef DEFAULT_CYCLE_LENGTH
#define DEFAULT_CYCLE_LENGTH 10000
#endif

class OVEN_SIMULATOR;
class PID_CONTROLLER;
//extern OVEN_SIMULATOR oven; //oven simulator
extern PID_CONTROLLER pid;  //heat controler object

class pid_settings;

enum CONTROL_MODE{
  PID = 0,
  MANUAL
};

enum NEXT_EVENT{
  NONE = 0,
  SETPOINT,
  PEAK,
  PEAK_WAIT,
  PEAK_TEST
};

class OVEN_SIMULATOR{
public:
	uint32_t  time_current;
	double    heat_total;
	double    heat_reported_true;
	double    heat_reported;

	float     heat_wattage;

	float     oven_surface_area; //square feet
	float		  oven_r_value;
	float		  oven_exterior_temp;//Celsius
	float		  oven_material_weight; //kilograms
	float     oven_material_specific_heat;
	float     oven_equalize_rate;
            
	void      setup();
	void      loop(uint32_t elapse);
	bool      heat_active = false;

	double temp_loss, temp_gain, it, et;
};

class PID_CONTROLLER{
		public:
	//private:
		//values in KELVIN
		float  temp_target = 0; //temp we want 
		float  setpoint = 0;    //temp we want with applied rate over time
		bool  setpoint_direction = false;

		//timers
		// uint32_t time_cycle_active;
		uint32_t time_cycle_current;
		// uint32_t time_cycle_last;
		int debug_str_length=0;


		//controls
		bool active; 		//turn controler on/off 
		bool error = false; //flag for PV error. shuts down if true
		const char* error_code = 0; //error string for the thermocouple
		uint8_t _output_pin;//output pin
		uint8_t _output_channel;// pwm channel
		float   _output_frequency = 0.1;
		uint8_t mode = PID;	//mode of operation
		uint32_t time_to_next_command;
		volatile float* temp_current;  //pointer to pv (temp reading)

		//settings
		uint32_t time_current;		 //time in milliseconds
		uint32_t time_cycle_length;  //length of a heating cycle in milliseconds
		float  max_rate_of_change=0; //max rate of change per minute
		float  power = 0;			 //output of the pid algrothem
		float  peak_wait_value;
		bool   peak_wait_direction;	 //true = trending up, false = trending down
		uint32_t peak_timer;
		
		//pid controller with default values
		MiniPID pid=MiniPID(0,0,0);
		uint32_t cycle_counter;
		std::string profile; //list of commands waiting to excuite


//methods
		void setup(pid_settings* p, uint8_t pin, uint8_t pwm_channel); //Call before using this class
		void load_settings(pid_settings* p){
			if(p){
				pid.setP( p->P );
				pid.setI( p->I );
				pid.setD( p->D );
				//pid.setMaxIOutput( p->MAXI );
				max_rate_of_change = p->R;
				mode = p->mode;
				_output_frequency = p->freq;
				time_cycle_length = p->cycle;
			}
		}
		uint32_t loop(uint32_t elapse);			  //Primary class worker. call at regulare time intervals

		void set_temp(float t){
			temp_target = convert_temp_units(t, KELVIN, config_file.units);
			setpoint = *temp_current;
		};

		uint32_t GetTime(){return time_current;};

		float GetSetpoint(){
			return convert_temp_units(setpoint, config_file.units, KELVIN);
		}
		float GetTarget(){
			return convert_temp_units(temp_target, config_file.units, KELVIN);
		}
		float GetPV(){return convert_temp_units(*temp_current, config_file.units, KELVIN);}
		//bool GetOutput(){return time_cycle_active;}

		void run_next_command();
};

std::string FormatTimeOutput(PID_CONTROLLER* p);

#endif
