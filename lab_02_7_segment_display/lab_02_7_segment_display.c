int value;
bit PB2_state;

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

// display `1234` on the 7 segment display
void exercise_1()
{
    DDRA |= 0b00001111; // PA0..PA3 -> outputs
    DDRC |= 0b11111111; // PC0..PD7 -> outputs

    while (1)
    {
        PORTA = 1 << 3;     // PA3 -> on => DIGIT 1
        PORTC = 0b00000110; // display `1`
        Delay_ms(4);

        PORTA = 1 << 2;     // PA2 -> on => DIGIT 2
        PORTC = 0b01011011; // display `2`
        Delay_ms(4);

        PORTA = 1 << 1;     // PA1 -> on => DIGIT 3
        PORTC = 0b01001111; // display `3`
        Delay_ms(4);

        PORTA = 1 << 0;     // PA0 -> on => DIGIT 4
        PORTC = 0b01100110; // display `4`
        Delay_ms(4);
    }
}

// display the last 4 digits of a given number on the 7 segment display
void exercise_2()
{
    DDRA |= 0b00001111; // PA0..PA3 -> outputs
    DDRC |= 0b11111111; // PC0..PC7 -> outputs
    value = 2023;

    while (1)
    {
        display(1, value % 10, 0);
        Delay_ms(4);

        display(2, value / 10 % 10, 0);
        Delay_ms(4);

        display(3, value / 100 % 10, 1);
        Delay_ms(4);

        display(4, value / 1000 % 10, 0);
        Delay_ms(4);
    }
}

// increment a value when PB2 is pressed and display its last 4 digits on the 7 segment display
void exercise_3()
{
    DDRB &= ~(1 << 2);  // PB2 -> input
    DDRA |= 0b00001111; // PA0..PA3 -> outputs
    DDRC |= 0b11111111; // PC0..PC7 -> outputs
    value = 0;

    while (1)
    {
        if (PINB & (1 << 2)) // PB2 == on ?
        {
            if (PB2_state == 0)
            {
                PB2_state = 1;

                if (value < 9999)
                {
                    value++;
                }
                else
                {
                    value = 0;
                }
            }
        }
        else
        {
            PB2_state = 0;
        }

        display(1, value % 10, 0);
        Delay_ms(4);

        display(2, value / 10 % 10, 0);
        Delay_ms(4);

        display(3, value / 100 % 10, 1);
        Delay_ms(4);

        display(4, value / 1000 % 10, 0);
        Delay_ms(4);
    }
}

void main()
{
    // exercise_1();
    // exercise_2();
    exercise_3();
}