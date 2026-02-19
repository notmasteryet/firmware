#include "SensorRTC.h"

class DS1307Constants
{
protected:
    // Unique I2C device address
    const uint8_t DS1307_SLAVE_ADDRESS = 0x68;

    // Register addresses
    static constexpr uint8_t SEC_REG = 0x00;
    static constexpr uint8_t MIN_REG = 0x01;
    static constexpr uint8_t HR_REG = 0x02;
    static constexpr uint8_t DAY_REG = 0x04;
    static constexpr uint8_t WEEKDAY_REG = 0x03;
    static constexpr uint8_t MONTH_REG = 0x05;
    static constexpr uint8_t YEAR_REG = 0x06;
    static constexpr uint8_t SQW_REG = 0x07;

    // Mask values
    static constexpr uint8_t SECONDS_MASK = 0x7F;
    static constexpr uint8_t MINUTES_MASK = 0x7F;
    static constexpr uint8_t HOUR_MASK = 0x3F;
    static constexpr uint8_t WEEKDAY_MASK = 0x07;
    static constexpr uint8_t CENTURY_MASK = 0x80;
    static constexpr uint8_t DAY_MASK = 0x3F;
    static constexpr uint8_t MONTH_MASK = 0x1F;

    // Other constants
    static constexpr uint8_t CLK_ENABLE = 0x80;
};

class SensorDS1307 : public SensorRTC, public DS1307Constants {
  public:
    using SensorRTC::setDateTime;
    using SensorRTC::getDateTime;

    SensorDS1307() : comm(nullptr) {}

    ~SensorDS1307()
    {
        if (comm) {
            comm->deinit();
        }
    }
#if defined(ARDUINO)
    bool begin(TwoWire &wire, int sda = -1, int scl = -1)
    {
        comm = std::make_unique<SensorCommI2C>(wire, DS1307_SLAVE_ADDRESS, sda, scl);
        if (!comm) {
            return false;
        }
        comm->init();
        return initImpl();
    }
#else
#error "Only arduino supported"
#endif  //ARDUINO

    bool begin(SensorCommCustom::CustomCallback callback)
    {
        comm = std::make_unique<SensorCommCustom>(callback, DS1307_SLAVE_ADDRESS);
        if (!comm) {
            return false;
        }
        comm->init();
        return initImpl();
    }

    void setDateTime(RTC_DateTime datetime)
    {
        uint8_t buffer[7];
        buffer[0] = DEC2BCD(datetime.getSecond()) & 0x7F;
        buffer[1] = DEC2BCD(datetime.getMinute());
        buffer[2] = DEC2BCD(datetime.getHour());
        buffer[3] = getDayOfWeek(datetime.getDay(), datetime.getMonth(), datetime.getYear()) + 1;
        buffer[4] = DEC2BCD(datetime.getDay());
        buffer[5] = DEC2BCD(datetime.getMonth());
        buffer[6] = DEC2BCD(datetime.getYear() % 100);

        comm->writeRegister(SEC_REG, buffer, 7);
    }

    RTC_DateTime getDateTime()
    {
        uint8_t buffer[7];
        comm->readRegister(SEC_REG, buffer, 7);
        uint8_t second = BCD2DEC(buffer[0] & 0x7F);
        uint8_t minute = BCD2DEC(buffer[1] & 0x7F);
        uint8_t hour   = BCD2DEC(buffer[2] & 0x3F);
        uint8_t day    = BCD2DEC(buffer[4] & 0x3F);
        uint8_t week   = BCD2DEC(buffer[3] & 0x07);
        uint8_t month  = BCD2DEC(buffer[5] & 0x1F);
        uint16_t year   = BCD2DEC(buffer[6]) + 2000;
        return RTC_DateTime(year, month, day, hour, minute, second, week);
    }

    const char *getChipName()
    {
        return "DS1307";
    }

private:
    bool initImpl()
    {
        //Check device is online
        int ret = comm->readRegister(SEC_REG);
        if (ret < 0) {
            return false;
        }
        if (BCD2DEC(ret & 0x7F) > 59) {
            return false;
        }
        return true;
    }

protected:
    std::unique_ptr<SensorCommBase> comm;
};
