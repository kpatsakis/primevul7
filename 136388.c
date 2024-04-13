static int sunkbd_connect(struct serio *serio, struct serio_driver *drv)
{
	struct sunkbd *sunkbd;
	struct input_dev *input_dev;
	int err = -ENOMEM;
	int i;

	sunkbd = kzalloc(sizeof(struct sunkbd), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!sunkbd || !input_dev)
		goto fail1;

	sunkbd->serio = serio;
	sunkbd->dev = input_dev;
	init_waitqueue_head(&sunkbd->wait);
	INIT_WORK(&sunkbd->tq, sunkbd_reinit);
	snprintf(sunkbd->phys, sizeof(sunkbd->phys), "%s/input0", serio->phys);

	serio_set_drvdata(serio, sunkbd);

	err = serio_open(serio, drv);
	if (err)
		goto fail2;

	if (sunkbd_initialize(sunkbd) < 0) {
		err = -ENODEV;
		goto fail3;
	}

	snprintf(sunkbd->name, sizeof(sunkbd->name),
		 "Sun Type %d keyboard", sunkbd->type);
	memcpy(sunkbd->keycode, sunkbd_keycode, sizeof(sunkbd->keycode));

	input_dev->name = sunkbd->name;
	input_dev->phys = sunkbd->phys;
	input_dev->id.bustype = BUS_RS232;
	input_dev->id.vendor  = SERIO_SUNKBD;
	input_dev->id.product = sunkbd->type;
	input_dev->id.version = 0x0100;
	input_dev->dev.parent = &serio->dev;

	input_set_drvdata(input_dev, sunkbd);

	input_dev->event = sunkbd_event;

	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_LED) |
		BIT_MASK(EV_SND) | BIT_MASK(EV_REP);
	input_dev->ledbit[0] = BIT_MASK(LED_CAPSL) | BIT_MASK(LED_COMPOSE) |
		BIT_MASK(LED_SCROLLL) | BIT_MASK(LED_NUML);
	input_dev->sndbit[0] = BIT_MASK(SND_CLICK) | BIT_MASK(SND_BELL);

	input_dev->keycode = sunkbd->keycode;
	input_dev->keycodesize = sizeof(unsigned char);
	input_dev->keycodemax = ARRAY_SIZE(sunkbd_keycode);
	for (i = 0; i < ARRAY_SIZE(sunkbd_keycode); i++)
		__set_bit(sunkbd->keycode[i], input_dev->keybit);
	__clear_bit(KEY_RESERVED, input_dev->keybit);

	sunkbd_enable(sunkbd, true);

	err = input_register_device(sunkbd->dev);
	if (err)
		goto fail4;

	return 0;

 fail4:	sunkbd_enable(sunkbd, false);
 fail3:	serio_close(serio);
 fail2:	serio_set_drvdata(serio, NULL);
 fail1:	input_free_device(input_dev);
	kfree(sunkbd);
	return err;
}