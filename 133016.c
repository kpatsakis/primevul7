sja1105pqrs_reset_cmd_pack(void *buf, const struct sja1105_reset_cmd *reset)
{
	const int size = SJA1105_SIZE_RESET_CMD;

	memset(buf, 0, size);

	sja1105_pack(buf, &reset->switch_rst, 8, 8, size);
	sja1105_pack(buf, &reset->cfg_rst,    7, 7, size);
	sja1105_pack(buf, &reset->car_rst,    5, 5, size);
	sja1105_pack(buf, &reset->otp_rst,    4, 4, size);
	sja1105_pack(buf, &reset->warm_rst,   3, 3, size);
	sja1105_pack(buf, &reset->cold_rst,   2, 2, size);
	sja1105_pack(buf, &reset->por_rst,    1, 1, size);
}