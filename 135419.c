static int snd_rawmidi_release(struct inode *inode, struct file *file)
{
	struct snd_rawmidi_file *rfile;
	struct snd_rawmidi *rmidi;
	struct module *module;

	rfile = file->private_data;
	rmidi = rfile->rmidi;
	rawmidi_release_priv(rfile);
	kfree(rfile);
	module = rmidi->card->module;
	snd_card_file_remove(rmidi->card, file);
	module_put(module);
	return 0;
}