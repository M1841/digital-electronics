bit PB2_state;
int count;

// light up PB7 when PB3 is held down
void exercise_1()
{
    DDRB |= (1 << 7);  // PB7 -> output
    DDRB &= ~(1 << 3); // PB3 -> input

    while (1)
    {
        if (PINB & (1 << 3)) // PB3 == on ?
        {
            PORTB |= (1 << 7); // PB7 -> on
        }
        else
        {
            PORTB &= ~(1 << 7); // PB7 -> off
        }
    }
}

// toggle PB6 when PB2 is pressed
void exercise_2()
{
    DDRB |= (1 << 6);  // PB6 -> output
    DDRB &= ~(1 << 2); // PB2 -> input

    while (1)
    {
        if (PINB & (1 << 2)) // PB2 == on ?
        {
            if (PB2_state == 0) // state == off ?
            {
                PB2_state = 1;     // state -> on
                PORTB ^= (1 << 6); // PB6 -> on
            }
        }
        else
        {
            PB2_state = 0; // state -> off
        }
    }
}

// increment a value when PB2 is pressed and display it in binary using PD0..PD7
void exercise_3()
{
    DDRD |= 0b11111111; // PD0..PD7 -> outputs
    DDRB &= ~(1 << 2);  // PB2 -> input
    count = 0;

    while (1)
    {
        if (PINB & (1 << 2)) // PB2 == on?
        {
            if (PB2_state == 0) // state == off ?
            {
                PB2_state = 1; // state -> on

                if (count < 255)
                {
                    count++;
                }
                else
                {
                    count = 0;
                }

                PORTD = count;
            }
        }
        else
        {
            PB2_state = 0; // state -> off
        }
    }
}

void main()
{
    // exercise_1();
    // exercise_2();
    exercise_3();
}