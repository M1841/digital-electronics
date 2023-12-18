int miliseconds, value, adc, adc_h, adc_l, top, frequency;
int exercise;

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

void timer0_isr() iv IVT_ADDR_TIMER0_COMP ics ICS_AUTO
{
    switch (exercise)
    {
        case 1:
        {
            if (miliseconds == 99)
            {
                value = read_adc(6) / 4;

                OCR2 = value; // send the potentiometer value to the output compare register, to be used to regulate the frequency of the outputs (how fast LED7 blinks, in this case)

                miliseconds = 0;
            }
            else
            {
                miliseconds++;
            }

            break;
        }
        case 2:
        {
            if (miliseconds == 9)
            {
                value = read_adc(6);
                
                frequency = ((float)(20000 - 20)) / 1023 * value + 20;
                top = 1000000 / frequency - 1;

                ICR1H = top >> 8; // send `top` to the input capture register for Timer 1 (used for the buzzer)
                ICR1L = top;
                
                OCR1BH = ((top + 1) / 2) >> 8; // send `(top + 1) / 2` to the output compare register B of Timer 1
                OCR1BL = (top + 1) / 2;

                value = frequency / 10;
                miliseconds = 0;
            }
            else
            {
                miliseconds++;
            }

            break;
        }
        default: return;
    }

    display(1, value % 10, 0);
    display(2, value / 10 % 10, 0);
    display(3, value / 100 % 10, 0);
    display(4, value / 1000 % 10, 0);
}

void exercise_1()
{
    DDRA |= 0b00001111; // PA0..PA3 -> outputs
    DDRC |= 0b11111111; // PC0..PC7 -> outputs

    TCCR0 = 0b00001011; // CTC mode, 64bit prescaler
    TCNT0 = 0;          // start timer at 0
    OCR0 = 125;         // interrupt at 125 (after 1ms)
    TIMSK = 0b00000010; // output compare match interrupt enable

    DDRD |= 1 << 7;     // PD7 -> output
    TCCR2 = 0b01101101; // Fast PWM, Clear OC2 on compare match (non-inverting), no prescaler (N = 1)

    ADMUX = 0b01000000;     // use AVCC as reference
    ADCSRA = 0b10000111;    // ADC - on, ADC interrupt enable, 64 division factor

    SREG = 1 << 7;      // global interrupt enable
    
    miliseconds = value = 0;

    exercise = 1;
}

void exercise_2()
{
    DDRA |= 0b00001111; // PA0..PA3 -> outputs
    DDRC |= 0b11111111; // PC0..PC7 -> outputs

    TCCR0 = 0b00001011; // CTC mode, 64bit prescaler
    TCNT0 = 0;          // start timer at 0
    OCR0 = 125;         // interrupt at 125 (after 1ms)
    TIMSK = 0b00000010; // output compare match interrupt enable

    DDRD |= 0b10010000; // PD4, PD7 -> outputs
    TCCR1A = 0b00100010; // Clear OC1B on compare match (non-inverting), Fast PWM Pt.1
    TCCR1B = 0b00011010; // Fast PWM Pt.2, 8bit prescaler

    ADMUX = 0b01000000;     // use AVCC as reference
    ADCSRA = 0b10000111;    // ADC - on, ADC interrupt enable, 64 division factor

    SREG = 1 << 7;      // global interrupt enable
    
    miliseconds = value = 0;

    exercise = 2;
}

void main() {
    // exercise_1();
    exercise_2();
}