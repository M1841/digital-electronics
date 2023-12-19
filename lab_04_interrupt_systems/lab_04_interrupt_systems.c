int miliseconds, seconds, minutes;
bit paused, ascending;
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

void pd2_isr() iv IVT_ADDR_INT0 ics ICS_AUTO
{
    switch (exercise)
    {
    case 3:
    {
        if (paused == 0)
        {
            paused = 1;
        }
        else
        {
            paused = 0;
        }
        break;
    }
    case 4:
    {
        seconds++;
        break;
    }
    case 5:
        if (ascending == 0)
        {
            ascending = 1;
        }
        else
        {
            ascending = 0;
        }
        break;
    }
}

void pd3_isr() iv IVT_ADDR_INT1 ics ICS_AUTO
{
    minutes++;
}

void timer0_isr() iv IVT_ADDR_TIMER0_COMP ics ICS_AUTO
{
    if (miliseconds == 999)
    {
        seconds++;
        miliseconds = 0;

        if (seconds == 60)
        {
            minutes++;
            seconds = 0;
        }
    }
    else
    {
        miliseconds++;
    }
    
    display(1, seconds % 10, 0);
    display(2, seconds / 10 % 10, 0);
    display(3, minutes % 10, 1);
    display(4, minutes / 10 % 10, 0);
}

void timer2_isr() iv IVT_ADDR_TIMER2_COMP ics ICS_AUTO
{
    if (paused == 0)
    {
        if (ascending == 1)
        {
            if (miliseconds == 999)
            {
                seconds++;
                miliseconds = 0;

                if (seconds == 60)
                {
                    minutes++;
                    seconds = 0;
                }
            }
            else
            {
                miliseconds++;
            }
        }
        else
        {
            if (miliseconds == 1)
            {
                seconds--;
                miliseconds = 1000;

                if (seconds == -1)
                {
                    minutes--;
                    seconds = 59;
                }
            }
            else
            {
                miliseconds--;
            }
        }
    }
    
    display(1, seconds % 10, 0);
    display(2, seconds / 10 % 10, 0);
    display(3, minutes % 10, 1);
    display(4, minutes / 10 % 10, 0);
}

// display the minutes and seconds passed since the program started, using Timer 0
void exercise_1()
{
    DDRA |= 0b00001111; // PA0..PA3 -> outputs
    DDRC |= 0b11111111; // PC0..PC7 -> outputs

    SREG = 1 << 7; // global interrupt enable

    TCCR0 = 0b00001011; // CTC mode, 64bit prescaler
    TCNT0 = 0;          // start timer at 0
    OCR0 = 125;         // interrupt at 125 (after 1ms)
    TIMSK = 0b00000010; // output compare match interrupt enable

    miliseconds = seconds = minutes = 0;
}

// display the minutes and seconds passed since the program started, using Timer 2
void exercise_2()
{
    DDRA |= 0b00001111; // PA0..PA3 -> outputs
    DDRC |= 0b11111111; // PC0..PC7 -> outputs

    SREG = 1 << 7; // global interrupt enable

    TCCR2 = 0b00001011; // CTC mode, 32bit prescaler
    TCNT2 = 0;          // start timer at 0
    OCR2 = 250;         // interrupt at 250 (after 1ms)
    TIMSK = 0b10000000; // output compare match interrupt enable

    miliseconds = seconds = minutes = paused = 0;
    ascending = 1;
}

// display the minutes and seconds passed since the program started, using Timer 2
// pause / resume the timer when PD2 is pressed
void exercise_3()
{
    exercise = 3;

    DDRA |= 0b00001111; // PA0..PA3 -> outputs
    DDRC |= 0b11111111; // PC0..PC7 -> outputs

    SREG = 1 << 7; // global interrupt enable

    TCCR2 = 0b00001011; // CTC mode, 32bit prescaler
    TCNT2 = 0;          // start timer at 0
    OCR2 = 250;         // interrupt at 250 (after 1ms)
    TIMSK = 0b10000000; // output compare match interrupt enable

    GICR |= 1 << 6;      // INT0 external interrupt enable (PD2 -> input)
    MCUCR |= 0b00000011; // generate interrupts on INT0 rising edges

    miliseconds = seconds = minutes = paused = 0;
    ascending = 1;
}

// display the minutes and seconds passed since the program started, using Timer 2
// increment the seconds when PD2 is pressed
// increment the minutes when PD3 is pressed
void exercise_4()
{
    exercise = 4;

    DDRA |= 0b00001111; // PA0..PA3 -> outputs
    DDRC |= 0b11111111; // PC0..PC7 -> outputs

    SREG = 1 << 7; // global interrupt enable

    TCCR2 = 0b00001011; // CTC mode, 32bit prescaler
    TCNT2 = 0;          // start timer at 0
    OCR2 = 250;         // interrupt at 250 (after 1ms)
    TIMSK = 0b10000000; // output compare match interrupt enable

    GICR |= 0b11000000;  // INT0 & INT1 external interrupt enable (PD2, PD3 -> inputs)
    MCUCR |= 0b00001111; // generate interrupts on INT0 & INT1 rising edges

    miliseconds = seconds = minutes = paused = 0;
    ascending = 1;
}

// display the minutes and seconds passed since the program started, using Timer 2
// switch the counting direction when PD2 is pressed
void exercise_5()
{
    exercise = 5;

    DDRA |= 0b00001111; // PA0..PA3 -> outputs
    DDRC |= 0b11111111; // PC0..PC7 -> outputs

    SREG = 1 << 7; // global interrupt enable

    TCCR2 = 0b00001011; // CTC mode, 32bit prescaler
    TCNT2 = 0;          // start timer at 0
    OCR2 = 250;         // interrupt at 250 (after 1ms)
    TIMSK = 0b10000000; // output compare match interrupt enable

    GICR |= 1 << 6;      // INT0 external interrupt enable (PD2 -> input)
    MCUCR |= 0b00000011; // generate interrupts on INT0 rising edges

    miliseconds = paused = 0;
    ascending = 1;
    minutes = 15;
    seconds = 30;
}

void main()
{
    // exercise_1();
    // exercise_2();
    // exercise_3();
    // exercise_4();
    exercise_5();
}