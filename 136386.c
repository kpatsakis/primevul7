static void sunkbd_set_leds_beeps(struct sunkbd *sunkbd)
{
	serio_write(sunkbd->serio, SUNKBD_CMD_SETLED);
	serio_write(sunkbd->serio,
		(!!test_bit(LED_CAPSL,   sunkbd->dev->led) << 3) |
		(!!test_bit(LED_SCROLLL, sunkbd->dev->led) << 2) |
		(!!test_bit(LED_COMPOSE, sunkbd->dev->led) << 1) |
		 !!test_bit(LED_NUML,    sunkbd->dev->led));
	serio_write(sunkbd->serio,
		SUNKBD_CMD_NOCLICK - !!test_bit(SND_CLICK, sunkbd->dev->snd));
	serio_write(sunkbd->serio,
		SUNKBD_CMD_BELLOFF - !!test_bit(SND_BELL, sunkbd->dev->snd));
}