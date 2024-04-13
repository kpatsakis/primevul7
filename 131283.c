static int spk_ttyio_receive_buf2(struct tty_struct *tty,
				  const unsigned char *cp, char *fp, int count)
{
	struct spk_ldisc_data *ldisc_data = tty->disc_data;

	if (spk_ttyio_synth->read_buff_add) {
		int i;

		for (i = 0; i < count; i++)
			spk_ttyio_synth->read_buff_add(cp[i]);

		return count;
	}

	if (!ldisc_data->buf_free)
		/* ttyio_in will tty_schedule_flip */
		return 0;

	/* Make sure the consumer has read buf before we have seen
	 * buf_free == true and overwrite buf
	 */
	mb();

	ldisc_data->buf = cp[0];
	ldisc_data->buf_free = false;
	complete(&ldisc_data->completion);

	return 1;
}