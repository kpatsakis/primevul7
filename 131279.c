static int get_dev_to_use(struct spk_synth *synth, dev_t *dev_no)
{
	/* use ser only when dev is not specified */
	if (strcmp(synth->dev_name, SYNTH_DEFAULT_DEV) ||
	    synth->ser == SYNTH_DEFAULT_SER)
		return tty_dev_name_to_number(synth->dev_name, dev_no);

	return ser_to_dev(synth->ser, dev_no);
}