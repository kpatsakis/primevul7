const char *MirrorJob::AddPatternsFrom(Ref<PatternSet>& exclude,char opt,const char *file)
{
   FILE *f=fopen(file,"r");
   if(!f)
      return xstring::format("%s: %s",file,strerror(errno));

   xstring line;
   const char *err=0;
   int c;
   while(!feof(f)) {
      line.truncate();
      while((c=getc(f))!=EOF && c!='\n')
	 line.append(c);
      if(line.length()>0) {
	 err=AddPattern(exclude,opt,line);
	 if(err)
	    break;
      }
   }

   fclose(f);
   return err;
}