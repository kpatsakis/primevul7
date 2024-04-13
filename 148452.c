n_tty_receive_char_flagged(struct tty_struct *tty, unsigned char c, char flag)
{
	char buf[64];

	switch (flag) {
	case TTY_BREAK:
		n_tty_receive_break(tty);
		break;
	case TTY_PARITY:
	case TTY_FRAME:
		n_tty_receive_parity_error(tty, c);
		break;
	case TTY_OVERRUN:
		n_tty_receive_overrun(tty);
		break;
	default:
		printk(KERN_ERR "%s: unknown flag %d\n",
		       tty_name(tty, buf), flag);
		break;
	}
}