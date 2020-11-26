#include "SHT3x.h"
SHT3x::SHT3x(int Address, ValueIfError Value, uint8_t HardResetPin, SHT3xSensor SensorType, SHT3xMode Mode)
{
    _Error = noError;
    SetAddress((uint8_t)Address);
    _ValueIfError = Value;
    SetMode(Mode);
    _SensorType = SensorType;
    _HardResetPin = HardResetPin;
}

void SHT3x::Begin()
{
    Wire.begin(21, 22);
    _OperationEnabled = true;
}

void SHT3x::UpdateData()
{
    _Error = noError;
    if ((_LastUpdateMillisec == 0) || ((millis() - _LastUpdateMillisec) >= _UpdateIntervalMillisec))
    {
        SendCommand(_MeasMSB, _MeasLSB);
        if (_Error == noError)
        {
            Wire.requestFrom(_Address, (uint8_t)6);
            uint32_t WaitingBeginTime = millis();
            while ((Wire.available() < 6) && ((millis() - WaitingBeginTime) < _TimeoutMillisec))
            {
                //Do nothing, just wait
            }
            if ((millis() - WaitingBeginTime) < _TimeoutMillisec)
            {
                _LastUpdateMillisec = WaitingBeginTime;
                uint8_t data[6];
                for (uint8_t i = 0; i < 6; i++)
                {
                    data[i] = Wire.read();
                }

                if ((CRC8(data[0], data[1], data[2])) && (CRC8(data[3], data[4], data[5])))
                {
                    uint16_t TemperatureRaw = (data[0] << 8) + (data[1] << 0);
                    uint16_t RelHumidityRaw = (data[3] << 8) + (data[4] << 0);
                    _TemperatureCeil = ((float)TemperatureRaw) * 0.00267033 - 45.;
                    _TemperatureCeil = _TemperatureCeil * _TemperatureCalibration.Factor +
                                       _TemperatureCalibration.Shift;
                    _RelHumidity = ((float)RelHumidityRaw) * 0.0015259;
                    _RelHumidity = _RelHumidity * _RelHumidityCalibration.Factor +
                                   _RelHumidityCalibration.Shift;

                    _Error = noError;
                }
                else
                {
                    _Error = DataCorrupted;
                }
            }
            else //Timeout
            {
                _Error = Timeout;
            }
        }
        else //Error after message send
        {
            // Nothing to do, measurement commands will return NULL because of Error != noError
        }
    }
    else //LastUpdate was too recently
    {
        //Nothing to do, wait for next call
    }
}

float SHT3x::GetTemperature(TemperatureScale Degree)
{
    //default scale is Celsius
    //At first, calculate in Celsius, than, if need, recalculate to Farenheit or Kelvin and than adjust according to calibration;
    float Temperature = _TemperatureCeil;
    if (Degree == Kel)
    {
        Temperature += 273.15;
    }
    else if (Degree == Far)
    {
        Temperature = Temperature * 1.8 + 32.;
    }

    return ReturnValueIfError(Temperature);
}

float SHT3x::GetRelHumidity()
{
    return ReturnValueIfError(_RelHumidity);
}

float SHT3x::GetAbsHumidity(AbsHumidityScale Scale)
{
    float millikelvins = GetTemperature(Kel) / 1000.;
    float Pressure = 0.;
    for (uint8_t i = 0; i < 6; i++)
    {
        float term = 1.;
        for (uint8_t j = 0; j < i; j++)
        {
            term *= millikelvins;
        }
        Pressure += term * _AbsHumPoly[i];
    }
    Pressure *= GetRelHumidity();
    switch (Scale)
    {
    case mmHg:
    {
        //Already in mm Hg
        break;
    }
    case Torr:
    {
        //Already in Torr
        break;
    }
    case Pa:
    {
        Pressure *= 133.322;
        break;
    }
    case Bar:
    {
        Pressure *= 0.0013332;
        break;
    }
    case At:
    {
        Pressure *= 0.0013595;
        break;
    }
    case Atm:
    {
        Pressure *= 0.0013158;
        break;
    }
    case mH2O:
    {
        Pressure *= 0.013595;
        break;
    }
    case psi:
    {
        Pressure *= 0.019337;
        break;
    }
    default:
    {
        break;
    }
    }
    return ReturnValueIfError(Pressure);
}

float SHT3x::GetTempTolerance(TemperatureScale Degree, SHT3xSensor SensorType)
{
    //Temperature tolerance is similar for both SHT30 and SHT31
    //At first, calculate at Celsius (similar to Kelvins), than, if need, recalculate to Farenheit
    float Temperature = GetTemperature();
    float Tolerance = 0;
    switch (SensorType)
    {
    case SHT30:
    {
        if ((0. <= Temperature) && (Temperature <= 65.))
        {
            Tolerance = 0.2;
        }
        else if (Temperature > 65.)
        {
            //Linear from 0.2 at 65 C to 0.6 at 125 C.
            Tolerance = 0.0067 * Temperature - 0.2333;
        }
        else //if (Temperature < 0.)
        {
            //Linear from 0.6 at -40 C to 0.2 at 0 C.
            Tolerance = -0.01 * Temperature + 0.2;
        }
        break;
    }
    case SHT31:
    {
        if ((0. <= Temperature) && (Temperature <= 90.))
        {
            Tolerance = 0.2;
        }
        else if (Temperature > 65.)
        {
            //Linear from 0.2 at 90 C to 0.5 at 125 C.
            Tolerance = 0.0086 * Temperature - 0.5714;
        }
        else //if (Temperature < 0.)
        {
            //Linear from 0.3 at -40 C to 0.2 at 0 C.
            Tolerance = -0.0025 * Temperature + 0.2;
        }
        break;
    }
    case SHT35:
    {
        if (Temperature <= 0.)
        {
            Tolerance = 0.2;
        }
        else if ((0. < Temperature) && (Temperature <= 20.))
        {
            //Linear from 0.2 at 0 C to 0.1 at 20 C.
            Tolerance = -0.005 * Temperature + 0.2;
        }
        else if ((20. < Temperature) && (Temperature <= 60.))
        {
            Tolerance = 0.1;
        }
        else if ((60. < Temperature) && (Temperature <= 90.))
        {
            //Linear from 0.1 at 60 C to 0.2 at 90 C.
            Tolerance = -0.0033 * Temperature - 0.1;
        }
        else //if (90. < Temperature)
        {
            //Linear from 0.2 at 90 C to 0.4 at 125 C.
            Tolerance = 0.0057 * Temperature - 0.3143;
        }
        break;
    }
    }
    if (Degree == Far)
    {
        Tolerance *= 1.8;
    }

    return ReturnValueIfError(Tolerance);
}

float SHT3x::GetRelHumTolerance(SHT3xSensor SensorType)
{
    float RelHumidity = GetRelHumidity();
    float Tolerance = 0;
    switch (SensorType)
    {
    case SHT30:
    {
        if ((10. <= RelHumidity) && (RelHumidity <= 90.))
        {
            Tolerance = 2.;
        }
        else if (RelHumidity < 10.)
        {
            //Linear from 4 at 0% to 2 at 10%
            Tolerance = -0.2 * RelHumidity + 4.;
        }
        else
        {
            //Linear from 2 at 90% to 4 at 100%
            Tolerance = 0.2 * RelHumidity - 16.;
        }
        break;
    }
    case SHT31:
    {
        Tolerance = 2.;
        break;
    }
    case SHT35:
    {
        if (RelHumidity <= 80.)
        {
            Tolerance = 1.5;
        }
        else //if (80 < RelHumidity)
        {
            //Linear from 0.5 at 80% to 2 at 100%
            Tolerance = 0.025 * RelHumidity - 0.5;
        }
        break;
    }
    }
    return ReturnValueIfError(Tolerance);
}

float SHT3x::GetAbsHumTolerance(AbsHumidityScale Scale, SHT3xSensor SensorType)
{
    /*	Dependence of absolute humidity is similar (from 0 to 80C) to P = H*a*exp(b*T),
	*	where P is absolute humidity, H is relative humidity, T is temperature (Celsius),
	*	a ~= 0.0396, b~=0.0575. 
	*	So its relative tolerance dP/P =  square root  [ (dH/H)^2 + (b*dT)^2 ].
	*/

    float RelHumidityRelTolerance = GetRelHumTolerance(SensorType) / GetRelHumidity();
    float TemperatureRelTolerance = 0.0575 * GetTempTolerance(Cel, SensorType);
    RelHumidityRelTolerance *= RelHumidityRelTolerance;
    TemperatureRelTolerance *= TemperatureRelTolerance;
    return ReturnValueIfError(
        GetAbsHumidity(Scale) * sqrt(RelHumidityRelTolerance + TemperatureRelTolerance));
}

void SHT3x::SetMode(SHT3xMode Mode)
{
    switch (Mode)
    {
    case Single_HighRep_ClockStretch:
    {
        _MeasMSB = 0x2C;
        _MeasLSB = 0x06;
        break;
    }
    case Single_MediumRep_ClockStretch:
    {
        _MeasMSB = 0x2C;
        _MeasLSB = 0x0D;
        break;
    }
    case Single_LowRep_ClockStretch:
    {
        _MeasMSB = 0x2C;
        _MeasLSB = 0x10;
        break;
    }
    case Single_HighRep_NoClockStretch:
    {
        _MeasMSB = 0x24;
        _MeasLSB = 0x00;
        break;
    }
    case Single_MediumRep_NoClockStretch:
    {
        _MeasMSB = 0x24;
        _MeasLSB = 0x0B;
        break;
    }
    case Single_LowRep_NoClockStretch:
    {
        _MeasMSB = 0x24;
        _MeasLSB = 0x16;
        break;
    }
    default:
    {
        _MeasMSB = 0x2C;
        _MeasLSB = 0x06;
        break;
    }
    }
}

void SHT3x::SetTemperatureCalibrationFactors(CalibrationFactors TemperatureCalibration)
{
    _TemperatureCalibration = TemperatureCalibration;
}

void SHT3x::SetRelHumidityCalibrationFactors(CalibrationFactors RelHumidityCalibration)
{
    _RelHumidityCalibration = RelHumidityCalibration;
}

void SHT3x::SetTemperatureCalibrationPoints(CalibrationPoints SensorValues, CalibrationPoints Reference)
{
    _TemperatureCalibration.Factor = (Reference.Second - Reference.First) / (SensorValues.Second - SensorValues.First);
    _TemperatureCalibration.Shift = Reference.First - _TemperatureCalibration.Factor * SensorValues.First;
}

void SHT3x::SetRelHumidityCalibrationPoints(CalibrationPoints SensorValues, CalibrationPoints Reference)
{
    _RelHumidityCalibration.Factor = (Reference.Second - Reference.First) / (SensorValues.Second - SensorValues.First);
    _RelHumidityCalibration.Shift = Reference.First - _RelHumidityCalibration.Factor * SensorValues.First;
}

void SHT3x::SoftReset()
{
    SendCommand(0x30, 0xA2);
}

void SHT3x::HardReset()
{
    if (_HardResetPin < 100) //MEGA2560 have only 68 pins
    {
        pinMode(_HardResetPin, OUTPUT);
        digitalWrite(_HardResetPin, LOW);
        delayMicroseconds(1); //Reset pin have to be LOW at least 350ns, so 1 usec is enough. I think, there is no need for micros() for 1 usec.
        digitalWrite(_HardResetPin, HIGH);
    }
}

void SHT3x::HeaterOn()
{
    SendCommand(0x30, 0x6D);
}

void SHT3x::HeaterOff()
{
    SendCommand(0x30, 0x66);
}

void SHT3x::SetAddress(uint8_t Address)
{
    if ((Address == 0x44) || (Address == 0x45))
    {
        _Address = Address;
    }
    else
    {
        _Error = WrongAddress;
    }
}

void SHT3x::SetUpdateInterval(uint32_t UpdateIntervalMillisec)
{
    if (UpdateIntervalMillisec > 0)
    {
        _UpdateIntervalMillisec = UpdateIntervalMillisec;
    }
}

void SHT3x::SetTimeout(uint32_t TimeoutMillisec)
{
    if (TimeoutMillisec > 0)
    {
        _TimeoutMillisec = _TimeoutMillisec;
    }
}

void SHT3x::I2CError(uint8_t I2Canswer)
{
    switch (I2Canswer)
    {
    case 0:
    {
        _Error = noError;
        break;
    }
    case 1:
    {
        _Error = TooMuchData;
        break;
    }
    case 2:
    {
        _Error = AddressNACK;
        break;
    }
    case 3:
    {
        _Error = DataNACK;
        break;
    }
    case 4:
    {
        _Error = OtherI2CError;
        break;
    }
    default:
    {
        _Error = UnexpectedError;
        break;
    }
    }
}

void SHT3x::SendCommand(uint8_t MSB, uint8_t LSB)
{
    if (_OperationEnabled)
    {
        //Everything is OK, nothing to do
    }
    else
    {
        Wire.begin();
        _OperationEnabled = true;
    }
    Wire.beginTransmission(_Address);
    // Send Soft Reset command
    Wire.write(MSB);
    Wire.write(LSB);
    // Stop I2C transmission
    Wire.endTransmission();
}

bool SHT3x::CRC8(uint8_t MSB, uint8_t LSB, uint8_t CRC)
{
    /*
	*	Name  : CRC-8
	*	Poly  : 0x31	x^8 + x^5 + x^4 + 1
	*	Init  : 0xFF
	*	Revert: false
	*	XorOut: 0x00
	*	Check : for 0xBE,0xEF CRC is 0x92
	*/
    uint8_t crc = 0xFF;
    uint8_t i;
    crc ^= MSB;

    for (i = 0; i < 8; i++)
        crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;

    crc ^= LSB;
    for (i = 0; i < 8; i++)
        crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;

    if (crc == CRC)
    {
        return true;
    }
    else
    {
        return false;
    }
}

uint8_t SHT3x::GetError()
{
    return _Error;
}

float SHT3x::ReturnValueIfError(float Value)
{
    if (_Error == noError)
    {
        return Value;
    }
    else
    {
        if (_ValueIfError == PrevValue)
        {
            return Value;
        }
        else
        {
            return 0.;
        }
    }
}

void SHT3x::ToReturnIfError(ValueIfError Value)
{
    _ValueIfError = Value;
}
