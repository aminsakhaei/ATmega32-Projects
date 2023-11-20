
#include <mega32.h>
#include <delay.h>

float v;
unsigned char dot='.';
unsigned int a,adc_data;

interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
TCNT0 =  0x06;
}

interrupt [ADC_INT] void adc_isr(void){
adc_data = ADCW;
}

unsigned int read_adc(void){
#asm                          //noise cancelling
    in   r30,mcucr
    cbr  r30,__sm_mask
    sbr  r30,__se_bit | __sm_adc_noise_red
    out  mcucr,r30
    sleep
    cbr  r30,__se_bit
    out  mcucr,r30
#endasm
return adc_data;
}

void lcd_command (void){
PORTC = 0x00;                 //E=0
delay_ms(2);
PORTC = 0x04;                 //E=1
}

void lcd_setup (void){
PORTD = 0x38;                 //‰„«Ì‘ œ— œÊ”ÿ »« ¬—«ÌÂ 7*5
lcd_command();

PORTD = 0x0C;                 //Œ«„Ê‘ ò—œ‰ ‰‘«‰ê— Ê ‰„«Ì‘ê—
lcd_command();

PORTD = 0X01;                 //clear
lcd_command();

PORTD = 0x06;                 //„œ Ê—Êœ œ«œÂ
lcd_command();
}

void lcd_data (void){
PORTC = 0x01;                 //RS=1
delay_ms(2);
PORTC = 0x04;                 //E=1  
}

void main(void)
{
DDRC = 0xFF;                  //make port C output
PORTC = 0x04;                 //E=1(OFF)
DDRD = 0xFF;                  //make port D output  


//ADC
ACSR = (1<<ACD);                                   //Analog Comparator Disable
ADMUX = (1<<REFS0) | (0<<REFS0);                   //AVCC with external capacitor at AREF pin
ADCSRA = (1<<ADEN) | (0<<ADSC) | (1<<ADATE) | (0<<ADIF) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (0<<ADPS0);    //ADC & Auto Trigger Enable Enable,Division Factor=64
SFIOR = (1<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);      //Timer/Counter0 Overflow

//TIMER 0
TCCR0 =  (1<<CS02) | ((0<<CS01)) | ((1<<CS00));    //clkI/O/1024 (From prescaler)
TCNT0 =  0x06;
TIMSK = (1<<TOIE0);                                //Overflow Interrupt Enable
#asm("sei")

lcd_setup();

while (1)
    {
    v = read_adc();
    v = (5*v)/1023;               //Ê· «é Å«ÌÂ
    
    PORTD = 0x80;                 //‰„«Ì‘ ⁄œœ ’ÕÌÕ
    lcd_command();
    a = v;
    PORTD = 0x30 + a;
    lcd_data();

    PORTD = 0x81;                 //‰„«Ì‘ „„Ì“
    lcd_command();
    PORTD = dot;
    lcd_data();
    
    PORTD = 0x82;                 //‰„«Ì‘ «⁄‘«—
    lcd_command();
    a = (v *10);
    PORTD = 0x30 + (a%10);
    lcd_data();
}

}
