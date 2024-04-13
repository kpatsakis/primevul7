static void sunkbd_disconnect(struct serio *serio)
{
	struct sunkbd *sunkbd = serio_get_drvdata(serio);

	sunkbd_enable(sunkbd, false);
	input_unregister_device(sunkbd->dev);
	serio_close(serio);
	serio_set_drvdata(serio, NULL);
	kfree(sunkbd);
}