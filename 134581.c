static float _CanonConvert2EV(short in)
        {
	float frac1;
	short val = in, sign = 1, frac;
	if (val < 0) { val = -val; sign = -1; }
	frac = (val & 0x1f);
	val -= frac;
	if (frac == 0x0c) frac1 = 32.0f / 3.0f;
	else if (frac == 0x14) frac1 = 64.0f / 3.0f;
	else frac1 = (float)frac;
	return (float)sign * ((float)val + frac1) / 32.0f;
    }