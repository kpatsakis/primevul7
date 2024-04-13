mode_t MirrorJob::get_mode_mask()
{
   mode_t mode_mask=0;
   if(!FlagSet(ALLOW_SUID))
      mode_mask|=S_ISUID|S_ISGID;
   if(!FlagSet(NO_UMASK))
   {
      if(target_is_local)
      {
	 mode_t u=umask(022); // get+set
	 umask(u);	      // retore
	 mode_mask|=u;
      }
      else
	 mode_mask|=022;   // sane default.
   }
   return mode_mask;
}