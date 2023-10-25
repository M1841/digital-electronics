int miliseconds, seconds, minutes;
bit PB0_state, PB1_state, paused;

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

// display the minutes and seconds passed since the program started, using Timer 0
void exercise_1()
{
    // initialize Timer 0
    TCCR0 = 0b00000011; // 64bit prescaler
    TCNT0 = 0;          // start counting from 0
    miliseconds = seconds = minutes = 0;

    while (1)
    {
        if (TCNT0 >= 125)
        {
            TCNT0 = 0;
            miliseconds++;

            if (miliseconds > 999)
            {
                miliseconds = 0;
                seconds++;
            }
            if (seconds == 60)
            {
                seconds = 0;
                minutes++;
            }
        }

        display(1, seconds % 10, 0);
        display(2, seconds / 10 % 10, 0);
        display(3, minutes % 10, 1);
        display(4, minutes / 10 % 10, 0);
    }
}

// display the minutes and seconds passed since the program started, using Timer 2
void exercise_2()
{
    // initialize Timer 2
    TCCR2 = 0b00000100; // 64bit prescaler
    TCNT2 = 0;          // start counting from 0
    miliseconds = seconds = minutes = 0;

    while (1)
    {
        if (TCNT2 >= 125)
        {
            TCNT2 = 0;
            miliseconds++;

            if (miliseconds > 999)
            {
                miliseconds = 0;
                seconds++;
            }
            if (seconds == 60)
            {
                seconds = 0;
                minutes++;
            }
        }

        display(1, seconds % 10, 0);
        display(2, seconds / 10 % 10, 0);
        display(3, minutes % 10, 1);
        display(4, minutes / 10 % 10, 0);
    }
}

// display the minutes and seconds passed since the program started, using Timer 2
// pause / resume the timer when PB1 is pressed
// reset the timer when PB0 is pressed
void exercise_3()
{
    DDRB &= 0b11111100; // PB0, PB1 -> inputs
    // initialize Timer 2
    TCCR2 = 0b00000100; // 64bit prescaler
    TCNT2 = 0;          // start counting from 0
    miliseconds = seconds = minutes = 0;
    paused = 0;

    while (1)
    {
        if (PINB & (1 << 0)) // PB0 == on ?
        {
            if (PB0_state == 0) // state == off ?
            {
                PB0_state = 1; // state -> on

                TCNT2 = 0; // reset timer
                miliseconds = 0;
                seconds = 0;
                minutes = 0;
            }
        }
        else
        {
            PB0_state = 0; // state -> off
        }

        if (PINB & (1 << 1)) // PB1 == on ?
        {
            if (PB1_state == 0) // state == off ?
            {
                PB1_state = 1; // state -> on

                if (paused == 1)
                {
                    paused = 0;
                }
                else
                {
                    paused = 1;
                }
            }
        }
        else
        {
            PB1_state = 0; // state -> off
        }

        if (TCNT2 >= 125)
        {
            TCNT2 = 0;
            if (paused == 0)
            {
                miliseconds++;

                if (miliseconds > 999)
                {
                    miliseconds = 0;
                    seconds++;
                }
                if (seconds == 60)
                {
                    seconds = 0;
                    minutes++;
                }
            }
        }

        display(1, seconds % 10, 0);
        display(2, seconds / 10 % 10, 0);
        display(3, minutes % 10, 1);
        display(4, minutes / 10 % 10, 0);
    }
}

void main()
{
    // exercise_1();
    // exercise_2();
    exercise_3();
}