#include <stdio.h>

void matte(float x[3]);
float Ci[3];

int main()
{
	float a[3];
	a[0] = 13.f;
	a[1] = 14.f;
	a[2] = 15.f;
	matte(a);
	printf("%f %f %f\n", Ci[0], Ci[1], Ci[2]);
}

