sound_play_event (int i)
{
	if (sound_files[i])
	{
		sound_play (sound_files[i], FALSE);
	}
}