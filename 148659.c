usage (FILE *fp, int n)
{
  fprintf (fp, _("Usage: umount -h | -V\n"
	       "       umount -a [-d] [-f] [-r] [-n] [-v] [-t vfstypes] [-O opts]\n"
	       "       umount [-d] [-f] [-r] [-n] [-v] special | node...\n"));
  exit (n);
}