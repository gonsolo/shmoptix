#include <stdio.h>

void matte();

float Cs = 11.f;
float Ci = 0.f;

int main()
{
	printf("Cs, Ci: %f %f\n", Cs, Ci);
	matte();
	printf("Cs, Ci: %f %f\n", Cs, Ci);
}

