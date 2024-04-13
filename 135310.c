MirrorJob::~MirrorJob()
{
   if(script && script_needs_closing)
      fclose(script);
}