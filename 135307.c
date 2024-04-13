void MirrorJob::SetOlderThan(const char *f)
{
   struct timespec ts;
   if(parse_datetime(&ts,f,0))
   {
      older_than=ts.tv_sec;
      return;
   }
   struct stat st;
   if(stat(f,&st)==-1)
   {
      perror(f);
      return;
   }
   older_than=st.st_mtime;
}