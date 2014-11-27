
#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#ifndef __ALIASES_H__
#include <aliases.h>
#endif

#ifdef __80196__
#pragma model(196)    /* tell the compiler we have an 80196 */
#endif

#define Bit(x)		(1<<(x))
#define BitSet(v,b)	((v) |= Bit(b))
#define BitClear(v,b)	((v) &= ~Bit(b))
#define BitTest(v,b)	((v) & Bit(b))

/* physical constants */

#define MHZ 			8


#define TOV_INT     0
#define ADC_INT     1
#define HSI_INT     2
#define HSO_INT     3
#define HSI_0_INT   4
#define TIMER_INT   5
#define SIO_INT     6
#define EXT_INT     7
#define TRAP_INT    8
#define UNIMP_INT   9
#define TI_INT     24
#define RI_INT     25
#define HSI_4_INT  26
#define T2_CAP_INT 27
#define T2_OV_INT  28
#define EXT_1_INT  29
#define FIFO_INT   30
#define NMI_INT    31


/* SFR bit enumerations */

	/* imask/ipend bits */
#define INT_TOV		0	/* timer overflow */
#define INT_ADC		1	/* adc done */
#define INT_HSI		2	/* hsi data available */
#define INT_HSO		3	/* hso pin */
#define INT_HSI_0	4	/* hsi.0 pin */
#define INT_TIMER	5	/* software timer */
#define INT_SIO		6	/* serial i/o */
#define INT_EXT		7	/* ext int */

	/* imask1/ipend1 bits */
#define INT_TI		0	/* serial xmit */
#define INT_RI		1	/* serial rcv */
#define INT_HSI_4	2	/* >= 4 entries in hsi fifo */
#define INT_T2_CAP	3	/* t2 capture */
#define INT_T2_OV	4	/* t2 overflow */
#define INT_EXT_1	5	/* extint pin */
#define INT_FIFO	6	/* hsio fifo full */
#define INT_NMI		7	/* nmi -- cannot really be masked */

	/* ioc0 bits */
#define IOC0_HSI_0		0	/* hsi.0 enable */
#define IOC0_T2_R_EACH_WRITE	1	/* t2 reset each write */
#define IOC0_HSI_1	  	2	/* hsi.1 enable */
#define IOC0_T2_R_EXT	   	3	/* t2 external reset enable */
#define IOC0_HSI_2		4	/* hsi.2 enable */
#define IOC0_T2_R_HSI	   	5	/* t2 reset source is hsi.0 / t2rst' */
#define IOC0_HSI_3	  	6	/* hsi.3 enable */
#define IOC0_T2_SRC	  	7	/* t2 clock source is hsi.1 / t2clk' */

	/* ioc1 bits */
#define IOC1_PWM		0	/* select pwm / p2.5' */
#define IOC1_EXTI_ACH7		1	/* extint is ach7 / extint' */
#define IOC1_T1_OV		2	/* t1 overflow int enable */
#define IOC1_T2_OV		3	/* t2 overflow int enable */
#define IOC1_HSO_4		4	/* hso.4 output enable */
#define IOC1_TXD		5	/* select txd / p2.0' */
#define IOC1_HSO_5		6	/* hso.5 enable */
#define IOC1_HSI_FIFO		7	/* hsi int on fifo full / hold reg loaded; */

	/* ioc2 bits */
#define IOC2_T2_FAST			0	/* incr t2 fast enable (1 / state time) */
#define IOC2_T2_UP_DOWN_ENABLE		1	/* enable t2 as up/down */
#define IOC2_PWM_SLOW			2	/* enable divide by 2 on pwm */
#define IOC2_NO_SH		     	3	/* disable sample and hold */
#define IOC2_AD_CLK_PRESCALE_DISABLE	4	/* disable clock prescaler (for slow xtal) */
#define IOC2_T2_ALT_INT			5	/* enable t2 int at $8000 */
#define IOC2_CAM_LOCK_ENABLE		6	/* enable lockable cam feature */
#define IOC2_CAM_CLEAR			7	/* clear entire cam */

	/* ios0 bits */
#define IOS0_HSO_0		0	/* current state of each hso pin (0-5) */
#define IOS0_HSO_1		1
#define IOS0_HSO_2		2
#define IOS0_HSO_3		3
#define IOS0_HSO_4		4
#define IOS0_HSO_5		5
#define IOS0_CAM		6	/* cam or hold reg is full */
#define IOS0_HSO_REG		7	/* hold reg is full */

	/* ios1 bits */
#define IOS1_ST0		0	/* software timer 0 expired */
#define IOS1_ST1		1	/* software timer 1 expired */
#define IOS1_ST2		2	/* software timer 2 expired */
#define IOS1_ST3		3	/* software timer 3 expired */
#define IOS1_T2_OV		4	/* t2 overflowed */
#define IOS1_T1_OV		5	/* t1 overflowed */
#define IOS1_HSI_FIFO		6	/* hsi fifo is full */
#define IOS1_HSI_REG		7	/* hsi hold reg data is available */

	/* ios2 bits */
#define IOS2_HSO_0		0	/* hso event occured for pin  hso.0 */
#define IOS2_HSO_1		1	/* hso event occured for pin  hso.1 */
#define IOS2_HSO_2		2	/* hso event occured for pin  hso.2 */
#define IOS2_HSO_3		3	/* hso event occured for pin  hso.3 */
#define IOS2_HSO_4		4	/* hso event occured for pin  hso.4 */
#define IOS2_HSO_5		5	/* hso event occured for pin  hso.5 */
#define IOS2_HC_14		6	/* hso event cmd14 (t2 reset)  occured */
#define IOS2_T2_RST		6
#define IOS2_HC_15		7	/* hso event cmd15 (start adc) occured */
#define IOS2_ADC_STRT		7

	/* t2control bits */
#define T2C_INTERN_CLK		0	/* enable internal clocking for t2 */
#define T2C_PWM1		2	/* enable pwm1 */
#define T2C_PWM2		3	/* enable pwm2 */

	/* spcon bits */
#define SP_M0		0	/* mode */
#define SP_M1		1	/* 01 is std */
#define SP_PEN		2	/* parity enable */
#define SP_REN		3	/* enable rcv */
#define SP_TB8		4	/* set 9th bit (for modes 2,3) */

	/* spstat bits */
#define SP_OE		2	/* output overrun */
#define SP_TXE		3	/* xmitter empty */
#define SP_FE		4	/* framing error */
#define SP_TI		5	/* xmit indicator */
#define SP_RI		6	/* rcv indicator */
#define SP_RPE		7	/* rcv parity error */

	/* hso command bits */
#define HSOC_CH0		0
#define HSOC_CH1		1
#define HSOC_CH2		2
#define HSOC_CH3		3
#define HSOC_INT		4	/* cause an interupt */
#define HSOC_SET		5	/* set/clear hso line */
#define HSOC_T			6	/* base timing on t2/ t1' */
#define HSOC_CAM_LOCK		7	/* lock event in cam */
                  /* ch* is 4bit # representing channel:
                      0-5 hso 0-5
                      6   hso 0,1
                      7   hso 2,3
                      8-B software timers
                      C,D unflagged events
                      E   reset t2
                      F   start A/D
                  */

	/* ad command bits */
#define ADC_GO		3	/* start adc now */
#define ADC_RES		4	/* adc resolution 10 / 8' */
#define ADC_8BIT 	0
#define ADC_10BIT 	1

	/* ad result bits */
#define ADR_STAT	3	/* conversion in progress / idle' */


	/* The Hardware */
#define IO_BASE		0xFF0

	/* PPI stuff */
#define IO_PPI    	(IO_BASE)
#define IO_PPI_0  	((vu_char *)(IO_PPI))
#define IO_PPI_1  	((vu_char *)(IO_PPI + 2))
#define IO_PPI_2  	((vu_char *)(IO_PPI + 4))
#define IO_PPI_C  	((vu_char *)(IO_PPI + 6))

	/* eeprom bits in PPI_0 */
#define EEP_B_CS		0		/* eeprom cs bit */
#define EEP_B_SK		1		/* eeprom clk */
#define EEP_B_DI		2		/* eeprom data in */

	/* eeprom bits in PPI_2 */
#define EEP_B_DO		7		/* eeprom data out */

void initHardware(void);
u_long splx(u_long);
#define spl0()	splx(0)

extern u_char p1_image, p2_image;

#endif
