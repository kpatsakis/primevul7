static int ser_to_dev(int ser, dev_t *dev_no)
{
	if (ser < 0 || ser > (255 - 64)) {
		pr_err("speakup: Invalid ser param. Must be between 0 and 191 inclusive.\n");
		return -EINVAL;
	}

	*dev_no = MKDEV(4, (64 + ser));
	return 0;
}