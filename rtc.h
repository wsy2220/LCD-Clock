void rtc_init();
char rtc_read_reg(char cmd);
void rtc_write_reg(char cmd, char b);

enum{
	SEC_W = 0x80,
	SEC_R,
	MIN_W,
	MIN_R,
	HR_W,
	HR_R,
	DAT_W,
	DAT_R,
	MON_W,
	MON_R,
	DAY_W,
	DAY_R,
	YR_W,
	YR_R,
	WP_W,
	WP_R,
	TCS_W,
	TCS_R
};

#define RTC_RAM_R(i) (0xC0 + (i)*2 + 1)
#define RTC_RAM_W(i) (0xC0 + (i)*2)
