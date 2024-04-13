sja1105et_reset_cmd_pack(void *buf, const struct sja1105_reset_cmd *reset)
{
	const int size = SJA1105_SIZE_RESET_CMD;

	memset(buf, 0, size);

	sja1105_pack(buf, &reset->cold_rst, 3, 3, size);
	sja1105_pack(buf, &reset->warm_rst, 2, 2, size);
}