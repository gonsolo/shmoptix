surface matte ( float Kd = 1, color Cs = 1)
//surface matte (color Cs = 1)
//surface matte (float Cs)
{
	//Ci = Kd * Cs * diffuse (N);
	Ci = Kd * Cs;
	//Ci = Kd;
}

