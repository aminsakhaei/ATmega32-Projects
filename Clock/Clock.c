
#include <mega32.h>
#include <delay.h>

unsigned char seg[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
int key=255,sec,count=0;
unsigned int t=0,TM;

int get_key (void){

PORTC = 0b00001100;                 //key 1,4,7,*
if(PIND.0==0){
while(!PIND.0);
return 1;
}
if(PIND.1==0){
while(!PIND.1);
return 4;
}
if(PIND.2==0){
while(!PIND.2);
return 7;
}
if(PIND.3==0){
while(!PIND.3);
return 200; 
}

PORTC = 0b00001010;                 //key 2,5,8,0
if(PIND.0==0){
while(!PIND.0);
return 2;
}
if(PIND.1==0){
while(!PIND.1);
return 5;
}
if(PIND.2==0){
while(!PIND.2);
return 8;
}
if(PIND.3==0){
while(!PIND.3);
return 0;
}

PORTC = 0b00000110;                 //key 3,6,9,#
if(PIND.0==0){
while(!PIND.0);
return 3;
}
if(PIND.1==0){
while(!PIND.1);
return 6;
}
if(PIND.2==0){
while(!PIND.2);
return 9;
}
if(PIND.3==0){
while(!PIND.3);
return 100; 
}

PORTD = 0x0F;
return 255;
}

void set_time (void){
while(key!=200){                    //œ—Ì«›  ⁄œœ «“ òÌ Åœ  « “„«‰ÌòÂ * ›‘—œÂ ‰‘œÂ
    key = get_key();
    if(key<10){
    t = (t*10) + key;
    t = t % 10000;
    }
    
    PORTB = 0b00000001;             //‰„«Ì‘
    PORTA = 0xFF;
    PORTA = seg[(t/100)/10];
    delay_us(5);

    PORTB = 0b00000010;
    PORTA = 0xFF;
    PORTA = seg[(t/100)%10];
    delay_us(5);

    PORTB = 0b00000100;
    PORTA = 0xFF;
    PORTA = seg[(t%100)/10];
    delay_us(5);

    PORTB = 0b00001000;
    PORTA = 0xFF;
    PORTA = seg[(t%100)%10];
    delay_us(5);

}
}

interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
PORTC.0 = ~(PORTC.0);
count++;
if(count==2){                       //Å” «“ ‘„—œ‰ œÊ ‰Ì„ À«‰ÌÂ
sec++;
if(sec>59){
sec=0;
TM++;
if(TM%100>59){
TM=(TM/100)*100;
TM+=100;
if(TM/100>23)
TM=0;
}
}
count=0;
}
TCNT1H = 0xC2;
TCNT1L = 0xF7;
}

void main(void)
{
//input & output ports
DDRA = 0xFF;                        
DDRB = 0x0F;
DDRC=0x0F;

PORTD=0x0F;                         //make pull-up resistors bit 0-3 of D active

set_time();
TM = t;

//timer 1
#asm("sei")
TCCR1B = (1<<CS12) ;                //clkI/O/256 (From prescaler)
TCNT1H = 0xC2;
TCNT1L = 0xF7;
TIMSK = (1<<TOIE1);

while (1)
    {
    
    PORTB = 0b00000001;            //‰„«Ì‘ ”«⁄ 
    PORTA = 0xFF;
    PORTA = seg[(TM/100)/10];
    delay_us(100);

    PORTB = 0b00000010;
    PORTA = 0xFF;
    PORTA = seg[(TM/100)%10];
    delay_us(100);

    PORTB = 0b00000100;
    PORTA = 0xFF;
    PORTA = seg[(TM%100)/10];
    delay_us(100);

    PORTB = 0b00001000;
    PORTA = 0xFF;
    PORTA = seg[(TM%100)%10];
    delay_us(100);
    }
}