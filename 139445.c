static long convert(long val, int oldsgnd, int oldprec, int newsgnd,
  int newprec)
{
	if (newsgnd != oldsgnd) {
	}
	if (newprec != oldprec) {
		if (newprec > oldprec) {
			val <<= newprec - oldprec;
		} else if (oldprec > newprec) {
			val >>= oldprec - newprec;
		}
	}
	return val;
}