
#if 0
float N[3];

void diffuse(float x[3]) {
}

void matte() {
	diffuse(N);
}

#endif

#if 1
float Ci;
void f(float Kd, float Cs[4]) {
	Ci = Kd * Cs;
}
#endif

