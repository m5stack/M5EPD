/*
	Arduino library for Sensirion temperature and humidity sensors SHT30, SHT31 & SHT35.
	the heavy version.
	Check for /examples for examples of different use cases.
	
	The datasheet I followed is:
	https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf
	For more simple version check the SimpleSHT3x library.
	
	The constructor structure:
	SHT3x(	int Address = 0x44, //I2C device address, 0x44 or 0x45
				ValueIfError Value = Zero, //What to return in case of errors. Zero or PrevValue
				uint8_t HardResetPin = 255, //Number of pin RESET connected to (input from 100 to 255 if not used)
				SHT3xSensor SensorType = SHT30, //Sensor type, SHT30, SHT31 or SHT35.
				SHT3xMode Mode=Single_HighRep_ClockStretch //Operation mode , look for "enum SHT3xMode"
				); 
	
	Supports:
		Temperature data at Celsius, Kelvin and Fahrenheit scales.
		Relative humidity data.
		Absolute humidity data at Torr, mm Hg, Pa, bar, technical and standard atmosphere, psi scales.
		Data integrity (by CRC8 algorithm) (datasheet/section 4.12).
		Temperature, relative and absolute humidity tolerances (in dependence from measured values)
		Calibration (linear) of temperature and humidity data by factors or by reverse sensor values (2 points)
		Heater On/Off (integrated to SHT3x sensor) (datasheet/section 4.10)
		Different sensor actions modes (datasheet/section 4.3)
		Reset: soft (I2C) and hard (by corresponding pin) (datasheet/section 4.9)
		
	Do not supports:
		Action in periodic mode (datasheet/section 4.5)
		Interrupts (datasheet/section 3.5)
		
		
		
	Note 1: by default, the data from sensor updates not faster, than 2 times a second.
	For faster update use SetUpdateInterval(uint32_t UpdateIntervalMillisec); but do not exceed the datasheet values (10 measurments per second (100 ms)) because of sensor self-heating (datasheet/section 4.5, at the end of Table 9)
	
	Note 2: The sensor type affects the tolerance values only. 
	
	
	Created by Risele for everyone's use (profit and non-profit).

	ALL THESE WOR_DS
	ARE YOURS EXCEPT
	RISELE
	ATTEMPT NO
	namechangING THERE
	USE THEM TOGETHER
	USE THEM IN PEACE
	
*/

#ifndef SHT3x_h
#define SHT3x_h

//Arduino standart libraries
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
//Arduino I2C/TWI library
#include <Wire.h>
//For calculating the tolerance of absolute humidity
#include <math.h>

class SHT3x
{
public:
    enum ValueIfError //Define, what to return in case of error: Zero or previous value
    {
        Zero,
        PrevValue
    };
    enum SHT3xMode
    {
        Single_HighRep_ClockStretch,
        Single_MediumRep_ClockStretch,
        Single_LowRep_ClockStretch,
        Single_HighRep_NoClockStretch,
        Single_MediumRep_NoClockStretch,
        Single_LowRep_NoClockStretch
    };
    enum SHT3xSensor
    {
        SHT30,
        SHT31,
        SHT35
    };
    enum TemperatureScale
    {
        Cel,
        Far,
        Kel
    };
    enum AbsHumidityScale
    {
        mmHg,
        Torr, //same as mm Hg
        Pa,
        Bar,
        At,  //Techical atmosphere
        Atm, //Standart atmosphere
        mH2O,
        psi,
    };
    struct CalibrationPoints
    {
        float First;
        float Second;
    };
    struct CalibrationFactors
    {
        CalibrationFactors() : Factor(1.), Shift(0.) {}
        float Factor;
        float Shift;
    };

    SHT3x(int Address = 0x44,
          ValueIfError Value = Zero,
          uint8_t HardResetPin = 255,
          SHT3xSensor SensorType = SHT30,
          SHT3xMode Mode = Single_HighRep_ClockStretch);

    void Begin();
    void UpdateData();

    float GetTemperature(TemperatureScale Degree = Cel);
    float GetRelHumidity();
    float GetAbsHumidity(AbsHumidityScale Scale = Torr);
    float GetTempTolerance(TemperatureScale Degree = Cel, SHT3xSensor SensorType = SHT30);
    float GetRelHumTolerance(SHT3xSensor SensorType = SHT30);
    float GetAbsHumTolerance(AbsHumidityScale Scale = Torr, SHT3xSensor SensorType = SHT30);

    uint8_t GetError();

    void SetMode(SHT3xMode Mode = Single_HighRep_ClockStretch);
    void SetTemperatureCalibrationFactors(CalibrationFactors TemperatureCalibration);
    void SetRelHumidityCalibrationFactors(CalibrationFactors RelHumidityCalibration);
    void SetTemperatureCalibrationPoints(CalibrationPoints SensorValues, CalibrationPoints Reference);
    void SetRelHumidityCalibrationPoints(CalibrationPoints SensorValues, CalibrationPoints Reference);

    void SoftReset();
    void HardReset();

    void HeaterOn();
    void HeaterOff();

    void SetAddress(uint8_t NewAddress);
    void SetUpdateInterval(uint32_t UpdateIntervalMillisec);
    void SetTimeout(uint32_t TimeoutMillisec);

private:
    float _TemperatureCeil;
    float _RelHumidity;
    bool _OperationEnabled = false;
    uint8_t _HardResetPin;
    ValueIfError _ValueIfError;
    uint8_t _MeasLSB; //Data read command, Most Significant Byte
    uint8_t _MeasMSB; //Data read command, Most Significant Byte
    uint8_t _Address;
    SHT3xSensor _SensorType;
    uint32_t _UpdateIntervalMillisec = 500;
    uint32_t _LastUpdateMillisec = 0;
    uint32_t _TimeoutMillisec = 100;
    void SendCommand(uint8_t MSB, uint8_t LSB);
    bool CRC8(uint8_t MSB, uint8_t LSB, uint8_t CRC);
    float ReturnValueIfError(float Value);
    void ToReturnIfError(ValueIfError Value);
    CalibrationFactors _TemperatureCalibration;
    CalibrationFactors _RelHumidityCalibration;

    /* 
			* 	Factors for poly for calculating absolute humidity (in Torr):
			*	P = (RelativeHumidity /100%) * sum(_AbsHumPoly[i]*T^i)
			*	where P is absolute humidity (Torr/mm Hg),
			*	T is Temperature(Kelvins degree) / 1000,
			* 	^ means power.
			*	For more data, check the NIST chemistry webbook:
			*	http://webbook.nist.gov/cgi/cbook.cgi?ID=C7732185&Units=SI&Mask=4&Type=ANTOINE&Plot=on#ANTOINE
		*/
    float _AbsHumPoly[6] = {-157.004, 3158.0474, -25482.532, 103180.197, -209805.497, 171539.883};

    enum Errors
    {
        noError = 0,
        Timeout = 1,
        DataCorrupted = 2,
        WrongAddress = 3,
        //I2C errors
        TooMuchData = 4,
        AddressNACK = 5,
        DataNACK = 6,
        OtherI2CError = 7,
        UnexpectedError = 8
    } _Error;
    void I2CError(uint8_t I2Canswer);
};
#endif //SHT3x_h
