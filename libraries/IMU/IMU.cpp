#include "IMU.H"

IMU::IMU(float wGyro) {
	IMU::wGyro = wGyro;
	firstRound = 1;
	radToDeg = 180/PI;
	degToRad = PI/180;
	
	gyroscope = Gyroscope();
	accelerometer = Accelerometer();
}

void IMU::initialize() {
	gyroscope.initialize();
	accelerometer.initialize();
}

void IMU::getAttitude(float* vec) {
	unsigned long currentMillis;
	unsigned long interval;
	float tmp;
	
	currentMillis = millis();	
	// Read sensor data
	accelerometer.getData(acc);
	acc[2] *= -1; // Invert accelerometer z-axis
	gyroscope.getData(rates);
	dT = (currentMillis - lastMillis) * 0.001f;
	lastMillis = currentMillis;	
	normalize(acc);
	
	if (firstRound) {
		// Initialize previous estimate 
		attitude[0] = acc[0];
		attitude[1] = acc[1];
		attitude[2] = acc[2];
		
		firstRound = 0;
	} else {
		if (abs(attitude[2]) < 0.01) {
			// Skip gyro because of numerical instability
			rates[0] = attitude[0];
			rates[1] = attitude[1];
			rates[2] = attitude[2];
		} else {
			// Process gyro data
			Axz = atan2(attitude[0],attitude[2]);
			Axz += rates[0] * degToRad * dT;
			Ayz = atan2(attitude[1],attitude[2]);
			Ayz += rates[1] * degToRad * dT;
			char gyroZsign = (cos(Axz) >= 0) ? 1 : -1;
			
			attGyro[0] = sin(Axz) / sqrt(1 + square(cos(Axz)) + square(tan(Ayz)));
			attGyro[1] = sin(Ayz) / sqrt(1 + square(cos(Ayz)) + square(tan(Axz)));
			attGyro[2] = gyroZsign * sqrt(1 - square(attGyro[0]) - square(attGyro[1]));
					
		}
		// Combine data
		attitude[0] = (acc[0] + attGyro[0]*wGyro)/(1+wGyro);
		attitude[1] = (acc[1] + attGyro[1]*wGyro)/(1+wGyro);
		attitude[2] = (acc[2] + attGyro[2]*wGyro)/(1+wGyro);
	
		normalize(attitude);
		
		vec[0] = attitude[0];
		vec[1] = attitude[1];
		vec[2] = attitude[2];	
	}
	
	/*	
	// Process gyroscope data
	float Axz = Axz_prev + rates[0] * dT;
	float Ayz = Ayz_prev + rates[1] * dT;
	
	float cotAxz = 1 / tan(Axz);
	float secAxz = 1 / cos(Axz);
	float cotAyz = 1 / tan(Ayz);
	float secAyz = 1 / cos(Ayz);
	
	attGyro[0] = 1 / sqrt(1 + cotAxz*cotAxz + secAyz*secAyz);
	attGyro[1] = 1 / sqrt(1 + cotAyz*cotAyz + secAxz*secAxz);
	
	attGyro[2] = sqrt(1 - attGyro[0]*attGyro[0] - attGyro[1]*attGyro[1]);
	
	if (attitudePrev[2] < 0)
		attGyro[2] = -1 * attGyro[2];
		
	// Combine data
	
	attitude[0] = (attAcc[0] + attGyro[0]*wGyro)/(1+wGyro);
	attitude[1] = (attAcc[1] + attGyro[1]*wGyro)/(1+wGyro);
	attitude[2] = (attAcc[2] + attGyro[2]*wGyro)/(1+wGyro);
	
	normalize(attitude);
	
	vec[0] = attitude[0];
	vec[1] = attitude[1];
	vec[2] = attitude[2];
	
	attitudePrev[0] = attitude[0];
	attitudePrev[1] = attitude[1];
	attitudePrev[2] = attitude[2];
	
	Axz_prev = Axz;
	Ayz_prev = Ayz;*/
}

void IMU::normalize(float* vec) {
	float length = sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
	vec[0] /= length;
	vec[1] /= length;
	vec[2] /= length;
}

float IMU::square(float num) {
	return num*num;
}
	