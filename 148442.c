n_tty_receive_buf_common(struct tty_struct *tty, const unsigned char *cp,
			 char *fp, int count, int flow)
{
	struct n_tty_data *ldata = tty->disc_data;
	int room, n, rcvd = 0;

	down_read(&tty->termios_rwsem);

	while (1) {
		room = receive_room(tty);
		n = min(count, room);
		if (!n) {
			if (flow && !room)
				ldata->no_room = 1;
			break;
		}
		__receive_buf(tty, cp, fp, n);
		cp += n;
		if (fp)
			fp += n;
		count -= n;
		rcvd += n;
	}

	tty->receive_room = room;
	n_tty_check_throttle(tty);
	up_read(&tty->termios_rwsem);

	return rcvd;
}