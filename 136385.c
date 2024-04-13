static int sunkbd_event(struct input_dev *dev,
			unsigned int type, unsigned int code, int value)
{
	struct sunkbd *sunkbd = input_get_drvdata(dev);

	switch (type) {

	case EV_LED:

		serio_write(sunkbd->serio, SUNKBD_CMD_SETLED);
		serio_write(sunkbd->serio,
			(!!test_bit(LED_CAPSL,   dev->led) << 3) |
			(!!test_bit(LED_SCROLLL, dev->led) << 2) |
			(!!test_bit(LED_COMPOSE, dev->led) << 1) |
			 !!test_bit(LED_NUML,    dev->led));
		return 0;

	case EV_SND:

		switch (code) {

		case SND_CLICK:
			serio_write(sunkbd->serio, SUNKBD_CMD_NOCLICK - value);
			return 0;

		case SND_BELL:
			serio_write(sunkbd->serio, SUNKBD_CMD_BELLOFF - value);
			return 0;
		}

		break;
	}

	return -1;
}