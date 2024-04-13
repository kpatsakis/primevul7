void PingStats::reset() {
	delete asQuantile;
	init();
}