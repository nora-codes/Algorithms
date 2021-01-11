#include <stdio.h>
#include <cs50.h>

int collatz(int n, int steps);

int main(void)
{
    int input = get_int("Input number: ");
    printf("Steps Required: %i\n", collatz(input, 0));
}

int collatz(int n, int steps)
{
    if (n == 1)
        return steps;

    else if (n%2 == 0)
    {
        steps++;
        return collatz((n / 2), steps);
    }

    else
    {
        steps++;
        return collatz(((n * 3) + 1), steps);
    }
}
