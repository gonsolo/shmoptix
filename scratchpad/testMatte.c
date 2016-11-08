#include <stdio.h>

#if 0
void matte(float x[3]);
float Ci[3];
#endif
void matte(float f);
float Ci;

int main()
{
#if 0
	Ci[0] = 3.f;
	Ci[1] = 4.f;
	Ci[2] = 5.f;

	float a[3];
	a[0] = 13.f;
	a[1] = 14.f;
	a[2] = 15.f;
#endif
	Ci = 3.f;
	float a = 133.f;

	//printf("Ci: %f %f %f\n", Ci[0], Ci[1], Ci[2]);
	//printf("a:  %f %f %f\n",  a[0],  a[1],  a[2]);

	printf("Ci: %f\n", Ci);
	printf(" a: %f\n",  a);
	matte(a);

	printf("Ci: %f\n", Ci);

	//printf("Ci: %f %f %f\n", Ci[0], Ci[1], Ci[2]);
}

