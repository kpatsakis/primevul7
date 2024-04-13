void MirrorJob::SetNewerThan(const char *f)
{
   struct timespec ts;
   if(parse_datetime(&ts,f,0))
   {
      newer_than=ts.tv_sec;
      return;
   }
   struct stat st;
   if(stat(f,&st)==-1)
   {
      perror(f);
      return;
   }
   newer_than=st.st_mtime;
}