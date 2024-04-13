const char *MirrorJob::SetScriptFile(const char *n)
{
   script_name.set(n);
   if(strcmp(n,"-"))
   {
      script=fopen(n,"w");
      if(!script)
	 return xstring::format("%s: %s",n,strerror(errno));
      setvbuf(script,NULL,_IOLBF,0);
      script_needs_closing=true;
   }
   else
   {
      script=stdout;
      script_needs_closing=false;
   }
   return 0;
}