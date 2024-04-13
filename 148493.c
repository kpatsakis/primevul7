n_tty_receive_signal_char(struct tty_struct *tty, int signal, unsigned char c)
{
	if (!L_NOFLSH(tty)) {
		/* flushing needs exclusive termios_rwsem */
		up_read(&tty->termios_rwsem);
		n_tty_flush_buffer(tty);
		tty_driver_flush_buffer(tty);
		down_read(&tty->termios_rwsem);
	}
	if (I_IXON(tty))
		start_tty(tty);
	if (L_ECHO(tty)) {
		echo_char(c, tty);
		commit_echoes(tty);
	} else
		process_echoes(tty);
	isig(signal, tty);
	return;
}