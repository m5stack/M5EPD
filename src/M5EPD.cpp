#include "M5EPD.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "soc/adc_channel.h"

#define BAT_ADC_CHANNEL ADC1_GPIO35_CHANNEL
#define BASE_VOLATAGE 3600
#define SCALE 0.5//0.78571429
#define ADC_FILTER_SAMPLE 8

M5EPD::M5EPD() : _is_adc_start(false), _isInited(false), _adc_chars(nullptr)
{
}

/** @brief Initialize the power supply, screen and other peripherals
  */
void M5EPD::begin(bool touchEnable, bool SDEnable, bool SerialEnable, bool BatteryADCEnable, bool I2CEnable)
{
    if (_isInited)
    {
        return;
    }
    _isInited = true;

    pinMode(M5EPD_MAIN_PWR_PIN, OUTPUT);
    enableMainPower();

    if (SerialEnable == true)
    {
        Serial.begin(115200);
        Serial.flush();
        delay(50);
        Serial.print("M5EPD initializing...");
    }

    pinMode(M5EPD_EXT_PWR_EN_PIN, OUTPUT);
    pinMode(M5EPD_EPD_PWR_EN_PIN, OUTPUT);
    pinMode(M5EPD_KEY_RIGHT_PIN, INPUT);
    pinMode(M5EPD_KEY_PUSH_PIN, INPUT);
    pinMode(M5EPD_KEY_LEFT_PIN, INPUT);
    delay(100);

    enableEXTPower();
    enableEPDPower();
    delay(1000);

    EPD.begin(M5EPD_SCK_PIN, M5EPD_MOSI_PIN, M5EPD_MISO_PIN, M5EPD_CS_PIN, M5EPD_BUSY_PIN);

    if (SDEnable == true)
    {
        SPI.begin(14, 13, 12, 4);
        SD.begin(4, SPI, 20000000);
    }

    if (touchEnable == true)
    {
        if(TP.begin(21, 22, 36) != ESP_OK)
        {
            log_e("Touch pad initialization failed.");
        }
    }
    else if (I2CEnable == true)
    {
        Wire.begin(21, 22, (uint32_t) 400000U);
    }

    if(BatteryADCEnable == true)
    {
        BatteryADCBegin();
    }

    if (SerialEnable == true)
    {
        Serial.println("OK");
    }
}

/** @brief Initialize ADC to read voltage, will use ADC_UNIT_1
  */
void M5EPD::BatteryADCBegin()
{
    if(_is_adc_start)
    {
        return;
    }
    _is_adc_start = true;
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(BAT_ADC_CHANNEL, ADC_ATTEN_DB_11);
    _adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, BASE_VOLATAGE, _adc_chars);
}

/** @brief Read raw data of ADC
  * @retval ADC Raw data
  */
uint32_t M5EPD::getBatteryRaw()
{
    return adc1_get_raw(BAT_ADC_CHANNEL);
}

/** @brief Read battery voltage
  * @retval voltage in mV
  */
uint32_t M5EPD::getBatteryVoltage()
{
    uint32_t adc_raw_value = 0;
    for (uint16_t i = 0; i < ADC_FILTER_SAMPLE; i++)
    {
        adc_raw_value += adc1_get_raw(BAT_ADC_CHANNEL);
    }

    adc_raw_value = adc_raw_value / ADC_FILTER_SAMPLE;
    uint32_t voltage = (uint32_t)(esp_adc_cal_raw_to_voltage(adc_raw_value, _adc_chars) / SCALE);
    return voltage;
}

/** @brief Update button status
  */
void M5EPD::update()
{
    //Button update
    BtnL.read();
    BtnP.read();
    BtnR.read();
}

void M5EPD::shutdown()
{
    this->disableMainPower();
}
int M5EPD::shutdown(int seconds)
{
    RTC.clearIRQ();
    RTC.setAlarmIRQ(seconds);
    delay(10);
    this->disableMainPower();
    return 0;
}
int M5EPD::shutdown(const rtc_time_t &RTC_TimeStruct)
{
    RTC.clearIRQ();
    RTC.setAlarmIRQ(RTC_TimeStruct);
    delay(10);
    this->disableMainPower();
    return 0;
}

int M5EPD::shutdown(const rtc_date_t &RTC_DateStruct, const rtc_time_t &RTC_TimeStruct)
{
    RTC.clearIRQ();
    RTC.setAlarmIRQ(RTC_DateStruct,RTC_TimeStruct);
    delay(10);
    this->disableMainPower();
    return 0;
}


M5EPD M5;
