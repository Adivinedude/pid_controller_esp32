#include "pid.h"
#include <cmath>
#include "Arduino.h"
#include "../command_interface.h"
#include <sstream>
#include <TelnetStream.h>
#include "config.h"

extern command_interface cmd;
extern float convert_temp_units( float value, uint8_t to, uint8_t from);
extern bool volatile DEBUG_MODE;
extern bool volatile DEBUG_MODE_FIRST;
//OVEN_SIMULATOR oven; //oven simulator
PID_CONTROLLER pid;  //heat controler object

void OVEN_SIMULATOR::setup(){
  heat_wattage = 5000;

  oven_surface_area = 24; //square ft
  oven_r_value = 3;
  oven_exterior_temp = 0; //c
  oven_material_weight = 20; //kg
  oven_material_specific_heat = 0.21;
  oven_equalize_rate = 1; // 0.00-1.00
  
  heat_total = oven_exterior_temp;
  heat_reported = oven_exterior_temp;
  heat_reported_true = oven_exterior_temp;
  heat_active = false;
}

void OVEN_SIMULATOR::loop(uint32_t duration){
  //calculate heat loss per hour.
  //kw = (WT * Cp * deltaTEMP) / (3415 * hours);
  // deltaTemp = (kw * 3415 * hours) / (wt * cp);
  //float temp_loss, temp_gain, it, et;
  it = heat_total*1.8+32;
  et = oven_exterior_temp*1.8+32;
  temp_loss = (oven_surface_area * (it - et) )/oven_r_value; //btu per hour lost
  temp_loss *= 0.293071; // watts per hour lost
  temp_loss = (temp_loss/1000 * 3415) / (oven_material_weight * oven_material_specific_heat); //temp loss per hour
  temp_loss /= 3600; //temp loss per second

  if( heat_active ){
    temp_gain = (heat_wattage/1000 * 3415) / (oven_material_weight * oven_material_specific_heat);
    temp_gain /= 3600;
  }else{
    temp_gain = 0;
  }

  temp_loss = temp_loss/1.8;
  temp_gain = temp_gain/1.8;
  
  double d = (double)duration;
  heat_total += (temp_gain-temp_loss)* (d/1000);
  heat_reported_true += (heat_total-heat_reported_true) * oven_equalize_rate * (d/1000);
  heat_reported = round(heat_reported_true * 4) / 4;

}

void PID_CONTROLLER::setup(pid_settings* p, uint8_t pin, uint8_t pwm_channel){
  ledcSetup(pwm_channel, _output_frequency, 16);
  ledcAttachPin(pin, pwm_channel);
  ledcWrite(pwm_channel, 0); // Set output to 0
  _output_pin = pin;
  _output_channel = pwm_channel;
  temp_target  = convert_temp_units(0, KELVIN, CELSIUS);
  active = false;                            //is heating allowed
  time_current = 0;
  setpoint = 273.15;
  //profile code
  time_to_next_command = 0;
  time_cycle_current = 0;
  load_settings(p);
  pid.reset();
  pid.setOutputLimits(0,100);
	pid.setOutputRampRate(100);
 }

void PID_CONTROLLER::run_next_command(){
//ToDo: use correct timer usage. 
  if(time_current > time_to_next_command){
    std::stringstream p(profile);
    std::string token;
    time_to_next_command = 0;
    
    std::getline(p, token, '\r');
    TelnetStream.println("");
    TelnetStream.println(token.c_str());
    Serial.println("");
    Serial.println(token.c_str());
    cmd.sendCommandLine(token.c_str());
    profile = "";
    std::getline(p, profile, '\0');
  }  
}

uint32_t PID_CONTROLLER::loop(uint32_t elapse){
  //keep track of the elapse time. used for all the counters
  time_current += elapse;
  //LOGF("%d", time_to_next_command)
  switch(time_to_next_command){
    case 0:
      break;

    case SETPOINT:
      
      if( (setpoint_direction == true  && *temp_current >= temp_target) ||
          (setpoint_direction == false && *temp_current <= temp_target)
      ){
        if( temp_target == setpoint );
          run_next_command();
      }
      break;

    case PEAK:
      peak_wait_value = *temp_current;
      peak_timer = cycle_counter;
      time_to_next_command = PEAK_WAIT;
      break;

    case PEAK_WAIT:
      if( abs(*temp_current-peak_wait_value) > abs(peak_wait_value*.01) ||  cycle_counter - peak_timer > 30){
        peak_wait_direction = (*temp_current > peak_wait_value)?true:false;
        time_to_next_command = PEAK_TEST;
      }
      break;

    case PEAK_TEST:
      if( peak_wait_direction ){
        if( *temp_current > peak_wait_value){
          peak_wait_value = *temp_current;
          peak_timer = cycle_counter;
        }
      }else{
        if( *temp_current < peak_wait_value){
          peak_wait_value = *temp_current;
          peak_timer = cycle_counter;
        }
      }
      if( cycle_counter - peak_timer > 10)
        run_next_command();

      break;

    default:
      run_next_command();
  }
  
  uint32_t rt = time_cycle_length; //thread sleep time

  if( time_cycle_current <= time_current ){
    time_cycle_current = 0;
  }
  
  //if the current cycle timer is expired
  if( !time_cycle_current && active && !error){
    //start a new current cycle timer
    time_cycle_current = time_current + time_cycle_length;

    //change temp based on max rate of change
    if( max_rate_of_change == 0 ){
      setpoint = temp_target;
    }else{
      float change = max_rate_of_change * ((float)time_cycle_length/1000/60);
      //LOGF("rate: %.2f Time: %u charge: %.8f\n\r", max_rate_of_change, time_cycle_length, change);
      if(setpoint < temp_target)
        setpoint += change;
      if(setpoint > temp_target)
        setpoint -= change;
      if( abs(setpoint-temp_target) <= change)
        setpoint = temp_target;
    }
    
    // calculate power output based on operating mode
    switch(mode){
      case PID: 
        power = pid.getOutput(*temp_current,setpoint);
        ++cycle_counter;
        break;
      case MANUAL:
        setpoint = temp_target;
        pid.getOutput(*temp_current,*temp_current);
        ++cycle_counter;
        power = convert_temp_units(setpoint, config_file.units, KELVIN);
        break;
    }
    // Set PWM output
    
    ledcWrite(_output_channel, (power/100 * 65535));

    if( DEBUG_MODE ){
      char* _c = new char[200];
      //Serial.print('\r');
      //TelnetStream.print('\r');
      if( DEBUG_MODE_FIRST ){
        DEBUG_MODE_FIRST = false;
        snprintf(_c, 200, "Time PV SV pw | Kp Ki Kd | Es Da Err TTNC" );
        Serial.println(_c);
        TelnetStream.println(_c);
      }
      std::string timestr = FormatTimeOutput(this);

      snprintf(_c, 200, "%5d %6.02f %6.02f %5.01f | %6.02f %6.02f %6.02f | %6.02f %6.02f %6.02f %s"
        , time_current/1000, GetPV(), GetSetpoint(), power,  
        pid.Poutput, pid.Ioutput, pid.Doutput, 
        pid.errorSum, pid.d_avg, pid.error,
        timestr.c_str());

      Serial.println(_c);
      TelnetStream.println(_c);
      delete _c;
    }
  } 
  if(!active){
    pid.errorSum = 0;
    for( unsigned short a = 0; a < MINIPID_AVG_COUNT; a++)
      pid.lastActual[a] = *temp_current;
    ledcWrite(_output_channel, 0);
  }

  return rt;
}

std::string FormatTimeOutput(PID_CONTROLLER* p){
	char buffer[20];
	uint32_t worker_value;
  if( p->error != 0 )
    return p->error_code;
	switch( p->time_to_next_command ){
		case NONE:
			worker_value = p->time_current/1000; //display current time
			break;
		case SETPOINT:
			return "Target";
		case PEAK:
      return "Peak";
    case PEAK_TEST:
			return "Peak Test";
    case PEAK_WAIT:
      return "Peak Wait";
		default:
			worker_value = (p->time_to_next_command-p->time_current) / 1000;
	}
	uint32_t seconds, minutes, hours;
	seconds = worker_value; 				   //total seconds
	minutes = seconds / 60;                               //total minutes
	seconds = seconds % 60;                    //seconds left
	hours   = minutes / 60;                               //total hours
	minutes = minutes % 60;                    //minutes left
	snprintf(buffer, 20, "%d:%02d:%02d", hours, minutes, seconds);
	return std::string(buffer);
}