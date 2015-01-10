#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    gyro,           sensorGyro)
#pragma config(Sensor, dgtl1,  liftLimit,      sensorTouch)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port2,           DBR,           tmotorVex393, openLoop, reversed, encoder, encoderPort, I2C_2, 1000)
#pragma config(Motor,  port3,           DFR,           tmotorVex393, openLoop, reversed, encoder, encoderPort, I2C_3, 1000)
#pragma config(Motor,  port4,           DFL,           tmotorVex393, openLoop)
#pragma config(Motor,  port5,           DBL,           tmotorVex393, openLoop, reversed, encoder, encoderPort, I2C_4, 1000)
#pragma config(Motor,  port6,           rLift1,        tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port7,           rLift2,        tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port8,           lLift1,        tmotorVex393, openLoop, encoder, encoderPort, I2C_1, 1000)
#pragma config(Motor,  port9,           lLift2,        tmotorVex393, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!


const int error = 50;	//define the error

const float DUTY = 0.8;
const int INTERVAL = 800;
const int GROUND = 0; //5D
const int SKYRISE_DISPENSER = 300; //8L
const int LOW_GOAL = 600; //8D
const int MID_GOAL = 900; //8R
const int HIGH_GOAL = 1200; //8U doesnt
const int MAX_HEIGHT = 1500; //5U
int DESCENT_SPEED = -90;
int *const DESCENT_POINTER = &DESCENT_SPEED;
const int LIFT_SPEED = 100;
const int STOP_SPEED  = 0;
const int gyro_init_time_ms = 1000; //ms
const float sf_gyro = 600.*5./4095.;  //units dps_per_cnt = 600 dps/volt*5volt/4095cnts
const int gyro_dz = 2 ;// dead zone in a/d counts (approx .67 deg_per_sec per count)
const int gyro_int_step_size = 2; // integer ms step size 500 hz nominal
//Chip Vdd = 3v  and Nominal gyro voltage output = 1.5 v -> 4095*/3.3*2/3*1.5*1.511. = 1874 cnts
int gyro_bias = 1874;  //nominally 1874 cnts
int gyro_angle = 0; //gyro angle degs  1 deg resolution
int rate_cnts=0; //unbiased gyro rate a/d counts
int gyro_rate_x10 = 0;//unbiased gyro rate deg/sec*10




int button5D = vexRT[Btn5DXmtr2];
int button8L = vexRT[Btn8LXmtr2];
int button8D = vexRT[Btn8DXmtr2];
int button8R = vexRT[Btn8RXmtr2];
int button8U = vexRT[Btn8UXmtr2];
int button5U = vexRT[Btn5UXmtr2];


///////////////////////Drive///////////////////////


void driveLeft(int a)
{
	motor[DBL] = a;
	motor[DFL] = a;
}

void driveRight(int a)
{
	motor[DBR] = a;
	motor[DFR] = a;
}

void drive_straight(float dist, int speed)
{
	while((nMotorEncoder[DBR] < 360 * dist / (4 * PI) + error) || (nMotorEncoder[DBR] < 360 * dist / (4 * PI) - error))
	{
		driveRight(speed);
		driveLeft(speed);
	}
}

///////////////////////Lift///////////////////////

//function for right lift
void rightLift(int a)
{
	motor[rLift1] = a;
	motor[rLift2] = a;
}
//function for left lift
void leftLift (int a)
{
	motor[lLift1] = a;
	motor[lLift2] = a;
}

//function for lift
void lift(int a)
{
	rightLift(a);
	leftLift(a);
}


//move lift until desired value is reached
void moveLift(int a)
{
	int debug = nMotorEncoder[lLift1];
	if(nMotorEncoder[lLift1] < a - error)
	{
		lift(LIFT_SPEED);
	}
	else if(nMotorEncoder[lLift1] > a + error)
	{
		lift(*DESCENT_POINTER);
	}
	else
	{
		lift(STOP_SPEED);
	}
}

///////////////////////Gyro///////////////////////
void gyroTurn(int degrees10)  //specify the amount of degrees to turn
{
	while((abs(SensorValue[in8]) < degrees10))
	{
		driveRight(63);
		driveLeft(-63);
	}
}

int presets(bool * const preset, int* const target)
{
	int debug = *preset;
	int debug2 = *target;

	int button5D = vexRT[Btn5DXmtr2];
	int button8L = vexRT[Btn8LXmtr2];
	int button8D = vexRT[Btn8DXmtr2];
	int button8R = vexRT[Btn8RXmtr2];
	int button8U = vexRT[Btn8UXmtr2];
	int button5U = vexRT[Btn5UXmtr2];
	int button6U = vexRT[Btn6UXmtr2];
	int button6D = vexRT[Btn6DXmtr2];


	if((button8U== 1) || (button8D== 1) || (button8L == 1)|| (button8R== 1)
		|| (button5U == 1) || (button5D == 1)&& ((button6U == 0) && (button6D== 0)))
	{
		*preset = true; //enabling the auto control
	}
	else if((button8U== 0) && (button8D== 0) && (button8L== 0)&& (button8R == 0)
		&& (button5U == 0) && (button5D== 0) && ((button6U== 1) || (button6D== 1)))
	{
		*preset = false;	//enabling manual control
	}

	if(*preset == true)
	{

		if((button8U == 0) && (button8D == 1) && (button8R == 0) && (button8L == 0)
			&& (button5U == 0) && (button5D == 0))
		{
			*target = LOW_GOAL;
		}
		else if((button8U == 0) && (button8D == 0) && (button8R == 1) && (button8L == 0)
			&& (button5U == 0) && (button5D == 0))
		{
			*target = MID_GOAL;
		}

		else if((button8U == 1) && (button8D == 0) && (button8R == 0) && (button8L == 0)
			&& (button5U == 0) && (button5D == 0))
		{
			*target = HIGH_GOAL;
		}
		else if((button8U == 0) && (button8D == 0) && (button8R == 0) && (button8L == 0)
			&& (button5U == 0) && (button5D == 1))
		{
			*target = GROUND;
		}
		else if((button8U == 0) && (button8D == 0) && (button8R == 0) && (button8L == 0)
			&& (button5U == 1) && (button5D == 0))
		{
			*target = MAX_HEIGHT;
		}
		else if((button8U == 0) && (button8D == 0) && (button8R == 0) && (button8L == 1)
			&& (button5U == 0) && (button5D == 0))
		{
			*target = SKYRISE_DISPENSER;
		}
		else
		{
		}
		debug = *target;
		moveLift(*target);
	}

	//manual control for lift
	else if(*preset == false)
	{
		if((button6U == 1) && (button6D == 0))
		{
			lift(LIFT_SPEED);
		}
		else if((button6D == 1) && (button6U == 0))
		{
			if(SensorValue(liftLimit) == 0)
			{
				lift(*DESCENT_POINTER);
			}
			else if(SensorValue(liftLimit) == 1)
			{
				lift(STOP_SPEED);
			}
		}
		else
		{
			lift(STOP_SPEED);
		}
	}
	return *target;
}




void pre_auton()
{
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
	// Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;

	// All activities that occur before the competition starts
	// Example: clearing encoders, setting servo positions, ...
}


task Process_gyro()
{
	float cum = 0;
	//. initialize gyro .
	time1[T1]= 0;
	int count = 0;
	while(time1[T1]< gyro_init_time_ms)
	{
		count++ ;
		wait1Msec(1);// this gives about 1 count per ms to average
		cum = cum + (float)(SensorValue[gyro]);

	}
	gyro_bias =  cum/count;
	cum = 0;

	//. integrate gyro.
	while(true)
	{
		int dt = time1[T1];
		rate_cnts = SensorValue[gyro]-gyro_bias;
		gyro_rate_x10 =10*sf_gyro*(float)rate_cnts;

		if(dt >=gyro_int_step_size)
		{
			float error = SensorValue[gyro]-gyro_bias;

			if(error>gyro_dz || error< -gyro_dz)
			{
				cum = cum + dt*error*sf_gyro/1000.;  //integrate if |error| > dz
			}
			ClearTimer(T1); //reset timer after use rate pulse
		}
		gyro_angle= cum;//scale to 1 deg/sec resolution
	}
}

task autonomous()
{
		SensorType[in1] = sensorNone;
	wait1Msec(1000);
	//Reconfigure Analog Port 8 as a Gyro sensor and allow time for ROBOTC to calibrate it
	SensorType[in1] = sensorGyro;
	wait1Msec(2000);
  nMotorEncoder[lLift1]=0;
	StartTask (Process_gyro);
	//drive_straight(5,100);
	//wait1Msec(1000);
	//gyroTurn(450);
	//wait1Msec(1000);
	while(!moveLift(1000)){}
	while(!moveLift(100)){}
}

//////////////////////////////////////////////////

bool presets(bool preset)
{
	nMotorEncoder[rLift1] = 0;

	if((vexRT[Btn8UXmtr2]== 1) || (vexRT[Btn8DXmtr2]== 1) || (vexRT[Btn8LXmtr2] == 1)|| (vexRT[Btn8RXmtr2]== 1)
		|| (vexRT[Btn5UXmtr2] == 0) || (vexRT[Btn5DXmtr2] == 0)&& (vexRT[Btn6UXmtr2] == 0) && (vexRT[Btn6DXmtr2]== 0))
	{
		preset = true; //enabling the auto control
	}
	else if((vexRT[Btn8UXmtr2]== 0) && (vexRT[Btn8DXmtr2]== 0) && (vexRT[Btn8LXmtr2]== 0)&& (vexRT[Btn8RXmtr2] == 0)
		&& (vexRT[Btn5UXmtr2] == 1) && (vexRT[Btn5DXmtr2]== 1) || (vexRT[Btn6U]== 1) || (vexRT[Btn6D]== 1))
	{
		preset = false;	//enabling manual control
	}

	if(preset == true)
	{
		int target = 0;

		if((button8U == 0) && (button8D == 1) && (button8R == 0) && (button8L == 0)
			&& (button5U == 0) && (button5D == 0))
		{
			target = LOW_GOAL;
		}
		else if((button8U == 0) && (button8D == 0) && (button8R == 1) && (button8L == 0)
			&& (button5U == 0) && (button5D == 0))
		{
			target = MID_GOAL;
		}
		else if((button8U == 0) && (button8D == 1) && (button8R == 0) && (button8L == 0)
			&& (button5U == 0) && (button5D == 0))
		{
			target = HIGH_GOAL;
		}
		else if((button8U == 0) && (button8D == 0) && (button8R == 0) && (button8L == 0)
			&& (button5U == 0) && (button5D == 1))
		{
			target = GROUND;
		}
		else if((button8U == 0) && (button8D == 0) && (button8R == 0) && (button8L == 0)
			&& (button5U == 1) && (button5D == 0))
		{
			target = MAX_HEIGHT;
		}
		else if((button8U == 0) && (button8D == 0) && (button8R == 0) && (button8L == 1)
			&& (button5U == 0) && (button5D == 0))
		{
			target = SKYRISE_DISPENSER;
		}
		moveLift(target);
	}

	//manual control for lift
	else if(preset == false)
	{
		if((vexRT[Btn6U] == 1) && (vexRT[Btn6D] == 0))
		{
			lift(LIFT_SPEED);
		}
		else if((vexRT[Btn6D] == 1) && (vexRT[Btn6U] == 0))
		{
			if(SensorValue(liftLimit) == 0)
			{
				lift(DESCENT_SPEED);
			}
			else if(SensorValue(liftLimit) == 1)
			{
				lift(STOP_SPEED);
			}
		}
		else
		{
			lift(STOP_SPEED);
		}
	}
	return preset;
}

task usercontrol()
{
	int count = 0;
	int target = 0;
	nMotorEncoder[lLift1] = 0;

	SensorType[in8] = sensorNone;
	wait1Msec(1000);
	//Reconfigure Analog Port 8 as a Gyro sensor and allow time for ROBOTC to calibrate it
	SensorType[in8] = sensorGyro;
	wait1Msec(2000);
	int X2 = 0, Y1 = 0, X1 = 0, threshold = 15;

	bool preset = false; //initially preset is false 7to allow for manual control

	while(true)
	{
		if(count <= DUTY*INTERVAL)
		{
			*DESCENT_POINTER = -127;
		}
		else
		{
			*DESCENT_POINTER = 0;
		}

		target = presets(&preset, &target);

		//Create "deadzone" for Y1/Ch3
		if(abs(vexRT[Ch3]) > threshold)
			Y1 = vexRT[Ch3];
		else
			Y1 = 0;
		//Create "deadzone" for X1/Ch4
		if(abs(vexRT[Ch4]) > threshold)
			X1 = vexRT[Ch4];
		else
			X1 = 0;
		//Create "deadzone" for X2/Ch1
		if(abs(vexRT[Ch1]) > threshold)
			X2 = vexRT[Ch1];
		else
			X2 = 0;

		//Remote Control Commands
		motor[DFR] = Y1 - X2 - X1;
		motor[DBR] =  Y1 - X2 + X1;
		motor[DFL] = Y1 + X2 + X1;
		motor[DBL] =  Y1 + X2 - X1;


		count++;
		if(count > INTERVAL)
		{
			count = 0;
		}
	}
}




//touch2 refers to a lift limit
//touch1 refers to the back of the robot's touch sensor
