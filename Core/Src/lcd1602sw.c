#include "stdio.h"
#include "swi2c.h"
#include "gpiodef.h"
#include "lcd1602sw.h"

//bit	7	6	5	4	3	2	1	0
//def  d7  d6  d5  d4  bk  en  rw  rs
#if 0
uint8_t lcd_send_cmd(lcd1602_t *p, char cmd) {
	char data_h,data_l;
	uint8_t frame_data[6];
	if(p->d == NULL) return 255;
	data_h = (cmd&0xf0);
	data_l = ((cmd <<4)&0xf0);
	frame_data[0] = data_h | 8;
	frame_data[1] = data_h | 12;
	frame_data[2] = data_h | 8;
	swi2c_Write_0addr(p->d, p->i2c_addr, frame_data, 3);
	frame_data[3] = data_l | 8;
	frame_data[4] = data_l | 12;
	frame_data[5] = data_l | 8;
	swi2c_Write_0addr(p->d, p->i2c_addr, frame_data+3, 3);
	return 0;
}

uint8_t lcd_send_data(lcd1602_t *p, char data) {
	char data_h,data_l;
	uint8_t frame_data[6];
	if(p->d == NULL) return 255;
	data_h = (data&0xf0);
	data_l = ((data <<4)&0xf0);
	frame_data[0] = data_h | 9;
	frame_data[1] = data_h | 13;
	frame_data[2] = data_h | 9;
	swi2c_Write_0addr(p->d, p->i2c_addr, frame_data, 3);
	frame_data[3] = data_l | 9;
	frame_data[4] = data_l | 13;
	frame_data[5] = data_l | 9;
	swi2c_Write_0addr(p->d, p->i2c_addr, frame_data+3, 3);
	return 0;
}
#endif
uint8_t lcd_write(lcd1602_t *p, uint8_t data, uint8_t is_data) {
	uint8_t dh=data, dl=data;
	uint8_t buf[4];
	if(p->d == NULL) return 255;
	dh &= 240; dl <<= 4;
	buf[0] = dh | is_data | p->bklg | 4;
	buf[1] = dh | is_data | p->bklg;
	buf[2] = dl | is_data | p->bklg | 4;
	buf[3] = dl | is_data | p->bklg;
	return swi2c_Write_0addr(p->d, p->i2c_addr, buf, 4);
}

void lcd_clear(lcd1602_t *p) {
	if(p->d == NULL) return;
	lcd_write(p, 0x01, 0);
	HAL_Delay(1);
}

uint8_t lcd_init(lcd1602_t *p, struct tag_swi2c *d, uint8_t i2cadr) {
	uint8_t	res = 0;
	if(!p || !d) return 1;
	p->d = d;
	p->i2c_addr = LCD_ADDRESS;
	if(i2cadr!=0) p->i2c_addr=i2cadr;
	p->bklg = LCD_BACKLIGHT;
	//if(swi2c_Read_0addr(p->d, LCD_ADDRESS, &res, 1) != 0) { p->d = NULL; return 255; }
	swi2c_dummy_clock(p->d);

	lcd_write(p, 0x33, 0);
	swi2c_delay_ms(16);

	lcd_write(p, 0x33, 0);
	swi2c_delay_us(40);

	lcd_write(p, 0x33, 0);
	swi2c_delay_us(40);

	lcd_write(p, 0x32, 0);
	swi2c_delay_us(40);

	lcd_write(p, 0x28, 0);
	swi2c_delay_us(40);

	lcd_write(p, 0x02, 0);		//function set
	swi2c_delay_us(1600);
	//lcd_send_cmd(d, 0x08);		//Display on/off
	lcd_write(p, 0x0c, 0);		//Display on/off
	swi2c_delay_us(40);
	lcd_write(p, 0x01, 0);		//clear display
	HAL_Delay(5);
	lcd_write(p, 0x06, 0);		//Enter mode
	swi2c_delay_us(40);
	lcd_write(p, 0x0C, 0);		//Display on/off
	swi2c_delay_us(40);
	return res;
}

void lcd_send_string (lcd1602_t *p, char *str) {
	if(p->d == NULL) return;
	for(; *str; str++) { lcd_write(p, *(uint8_t*)str, 1); }
}

void lcd_put_cur(lcd1602_t *p, uint8_t row,uint8_t col) {
	if(p->d == NULL) return;
	lcd_write(p, 0x80 | (col + (0x40 * row)), 0);
}

void lcd_seti2caddr(lcd1602_t *p, uint16_t addr) { p->i2c_addr = addr; }

void lcd_set_backlight_on(lcd1602_t *p, uint8_t on) {
	if(on != 0) {
		p->bklg |= LCD_BACKLIGHT;
		lcd_write(p, 0xe, 0);
	} else {
		p->bklg &= ~LCD_BACKLIGHT;
		lcd_write(p, 0xc, 0);
	}
}
