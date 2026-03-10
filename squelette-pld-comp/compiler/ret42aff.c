#include <inttypes.h>
int main()
{
    int a;
    int b;
    {
        int c;
        c = 13;
    }
    b = 12;
    a = b;

    return a;
}
