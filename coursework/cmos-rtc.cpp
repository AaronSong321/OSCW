/*
 * CMOS Real-time Clock
 * SKELETON IMPLEMENTATION -- TO BE FILLED IN FOR TASK (1)
 */

/*
 * STUDENT NUMBER: s2067807
 */
#include <infos/drivers/timer/rtc.h>
#include <infos/kernel/log.h>
#include <arch/x86/pio.h>

using namespace infos::drivers;
using namespace infos::drivers::timer;
#define P(...) infos::kernel::syslog.messagef(infos::kernel::LogLevel::INFO, __VA_ARGS__)



class CMOSRTC : public RTC {
private:
	static constexpr int CmosAddress = 0x70;
	static constexpr int CmosData = 0x71;
	static constexpr int ThisCentury = 20;
	enum Port{
		Second=0,
		Minute=0x02,
		Hour=0x04,
		Day=0x07,
		Month=0x08,
		Year=0x09,
		Century=0x32,
		BCD=0x0B,
	};

	unsigned short registerB = -1;
	void ReadRegisterB();
	bool IsUpdateInProgress();
	unsigned short GetOriginal(Port port);
	bool IsBinaryCodedDecimal();
	bool Is12Hour();
	unsigned short ConvertToBinary(unsigned short originalValue);
	unsigned short GetRtcValue(Port port);
	unsigned short GetRtcHourValue();
	unsigned short GetRtcYearValue();


public:
	static const DeviceClass CMOSRTCDeviceClass;

	const DeviceClass& device_class() const override
	{
		return CMOSRTCDeviceClass;
	}

	/**
	 * Interrogates the RTC to read the current date & time.
	 * @param tp Populates the tp structure with the current data & time, as
	 * given by the CMOS RTC device.
	 */
	void read_timepoint(RTCTimePoint& tp) override
	{
		ReadRegisterB();
		tp.seconds = GetRtcValue(Port::Second);
		tp.minutes = GetRtcValue(Port::Minute);
		tp.hours = GetRtcHourValue();
		tp.day_of_month = GetRtcValue(Port::Day);
		tp.month = GetRtcValue(Port::Month);
		tp.year = GetRtcYearValue();
	}
};

using namespace infos::arch::x86;
bool CMOSRTC::IsUpdateInProgress(){
	using namespace infos::kernel;
	__outb(CmosAddress, 0x0A);
	// int value = __inb(CmosAddress);
	// syslog.messagef(LogLevel::INFO, "update bit: %d %d", value, value&0x80);
	return (__inb(CmosAddress) & 0x80) != 0;
}
unsigned short CMOSRTC::GetOriginal(Port port){
	__outb(CmosAddress, (int)port);
	return __inb(CmosData);
}
void CMOSRTC::ReadRegisterB(){
	// while (IsUpdateInProgress());
	registerB = GetOriginal(Port::BCD);
}
bool CMOSRTC::IsBinaryCodedDecimal(){
	return (registerB & 0x04) == 0;
}
bool CMOSRTC::Is12Hour(){
	return (registerB & 0x02) == 0;
}
unsigned short CMOSRTC::ConvertToBinary(unsigned short originalValue){
	return (originalValue&0xf)+originalValue/16*10;
}
unsigned short CMOSRTC::GetRtcValue(Port port){
	auto f = GetOriginal(port);
	return IsBinaryCodedDecimal()?ConvertToBinary(f):f;
}
unsigned short CMOSRTC::GetRtcHourValue(){
	auto h = GetOriginal(Port::Hour);
	if (IsBinaryCodedDecimal())
		h = ((h&0xf)+(h&0x70)/16*10)|(h&0x80);
	if (Is12Hour() && (h&0x80)!=0){
		h = ((h & 0x7f)+12)%24;
	}
	return h;
}
unsigned short CMOSRTC::GetRtcYearValue(){
	auto year = GetRtcValue(Port::Year);
	auto century = GetRtcValue(Port::Century);
	if (!century)
		century = ThisCentury;
	year += century * 100;
	return year;
}

const DeviceClass CMOSRTC::CMOSRTCDeviceClass(RTC::RTCDeviceClass, "cmos-rtc");

RegisterDevice(CMOSRTC);
