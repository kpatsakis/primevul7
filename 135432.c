long snd_rawmidi_kernel_write(struct snd_rawmidi_substream *substream,
			      const unsigned char *buf, long count)
{
	return snd_rawmidi_kernel_write1(substream, NULL, buf, count);
}