surface matte(float Kd = 1, color Cs = 1)
{
	//normal Nf = faceforward(normalize(N), I);
	//Oi = Os;
	//Ci = Os * Cs * (Ka*ambient() + Kd*diffuse(Nf));
	//Ci = Kd * Cs * diffuse(N); // works

	normal Nf;
}
