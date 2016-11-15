surface matte ( float Kd = 1, color Cs = 1)
{
	//Ci = Kd * Cs * diffuse (N);
	//Ci = Kd * Cs;
	//Ci = diffuse(N);
	//Ci = diffuse(N);
	Ci = Cs;
}

