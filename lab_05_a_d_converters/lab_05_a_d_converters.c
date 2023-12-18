int miliseconds, adc, adc_l, adc_h, voltage, temperature;
bit toggle;

// display a digit and optionally a period on a given position
void display(char position, char digit, char period)
{
    PORTA &= 0b11110000; // disable digit selection lines
    PORTC &= 0b00000000; // disable segment selection lines

    if (period)
        PORTC |= 0b10000000;

    switch (digit)
    {
    case 0:
    {
        PORTC |= 0b00111111;
        break;
    }
    case 1:
    {
        PORTC |= 0b00000110;
        break;
    }
    case 2:
    {
        PORTC |= 0b01011011;
        break;
    }
    case 3:
    {
        PORTC |= 0b01001111;
        break;
    }
    case 4:
    {
        PORTC |= 0b01100110;
        break;
    }
    case 5:
    {
        PORTC |= 0b01101101;
        break;
    }
    case 6:
    {
        PORTC |= 0b01111101;
        break;
    }
    case 7:
    {
        PORTC |= 0b00000111;
        break;
    }
    case 8:
    {
        PORTC |= 0b01111111;
        break;
    }
    case 9:
    {
        PORTC |= 0b01100111;
        break;
    }
    }

    PORTA |= position > 0 && position < 5
                 ? 1 << (position - 1)
                 : 0;
}

// toggle a bit variable (to switch between displaying the temperature and the voltage)
void pd2_isr() iv IVT_ADDR_INT0 ics ICS_AUTO
{
    if (toggle == 0)
    {
        toggle = 1;
    }
    else
    {
        toggle = 0;
    }
}

// read data from a specified channel and return it
int read_adc (char channel)
{
    ADMUX &= 0b11100000;        // reset the conversion channel
    ADMUX |= channel;           // set the desired conversion channel

    ADCSRA |= 1 << 6;           // start the conversion
    while (ADCSRA & (1 << 6));  // wait for the conversion to end

    adc_l = ADCL;
    adc_h = ADCH;
    adc = (adc_h << 8) | adc_l; // the result of the conversion is stored on 2 bytes: ADCH & ADCL
    return adc;
}

// set a channel and wait for the conversion
void read_adc_with_isr (char channel)
{
    ADMUX &= 0b11100000;        // reset the conversion channel
    ADMUX |= channel;           // set the desired conversion channel
    ADCSRA |= 1 << 6;           // start the conversion
}

// compute the temperature after the conversion is finished
void adc_done_isr() iv IVT_ADDR_ADC
{
    adc_l = ADCL;
    adc_h = ADCH;
    adc = ((adc_h << 8)) | adc_l; // the result of the conversion is stored on 2 bytes: ADCH & ADCL

    temperature = (int)((((float)adc * 5) / 1024) * 1000);
}

// display the desired measured data every second
void timer0_isr() iv IVT_ADDR_TIMER0_COMP ics ICS_AUTO
{
    if (miliseconds == 999)
    {
        voltage = read_adc(6);  // read an analog signal from channel 6 (connected to a potentiometer that adjusts a voltage)
        read_adc_with_isr(7);   // read an analog signal from channel 7 (connected to a temperature sensor), which is stored in the global variable `temperature`
        miliseconds = 0;
    }
    else
    {
        miliseconds++;
    }

    if (toggle == 0)
    {
        display(1, voltage % 10, 0);
        display(2, voltage / 10 % 10, 0);
        display(3, voltage / 100 % 10, 0);
        display(4, voltage / 1000 % 10, 0);
    }
    else
    {
        display(1, temperature % 10, 0);
        display(2, temperature / 10 % 10, 1);
        display(3, temperature / 100 % 10, 0);
        display(4, temperature / 1000 % 10, 0);
    }
}

// read an alternating voltage and display it's value on the 7 segment display
void exercise_1()
{
    DDRA |= 0b00001111; // PA0..PA3 -> outputs
    DDRC |= 0b11111111; // PC0..PC7 -> outputs

    SREG = 1 << 7;      // global interrupt enable

    TCCR0 = 0b00001011; // CTC mode, 64bit prescaler
    TCNT0 = 0;          // start timer at 0
    OCR0 = 125;         // interrupt at 125 (after 1ms)
    TIMSK = 0b00000010; // output compare match interrupt enable

    miliseconds = voltage = 0;

    ADMUX = 0b01000000;     // use AVCC as reference
    ADCSRA = 0b10000111;    // ADC - on, 128bit prescaler
}

void exercise_2()
{
    DDRA |= 0b00001111; // PA0..PA3 -> outputs
    DDRC |= 0b11111111; // PC0..PC7 -> outputs

    SREG = 1 << 7;      // global interrupt enable

    TCCR0 = 0b00001011; // CTC mode, 64bit prescaler
    TCNT0 = 0;          // start timer at 0
    OCR0 = 125;         // interrupt at 125 (after 1ms)
    TIMSK = 0b00000010; // output compare match interrupt enable

    GICR |= 1 << 6;         // INT0 enternal interrupt enable (PD2 -> input)
    MCUCR |= 0b00000011;    // generate interrupts on INT0 rising edges

    miliseconds = 0;
    toggle = 0;

    ADMUX = 0b01000000;     // use AVCC as reference
    ADCSRA = 0b10000111;    // ADC - on, 128bit prescaler
    ADCSRA |= 1 << 3;       // ADC interrupt enable
    SREG |= 1 << 7;         // global interrupt enable

}

void main() {
    // exercise_1();
    exercise_2();
}
