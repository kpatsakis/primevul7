void PingStats::init() {
	boost::array<double, 3> probs = {{0.75, 0.80, 0.95 }};

	asQuantile = new asQuantileType(boost::accumulators::tag::extended_p_square::probabilities = probs);
	dPing = 0.0;
	uiPing = 0;
	uiPingSort = 0;
	uiUsers = 0;
	uiMaxUsers = 0;
	uiBandwidth = 0;
	uiSent = 0;
	uiRecv = 0;
	uiVersion = 0;
}