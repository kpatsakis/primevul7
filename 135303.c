const char *MirrorJob::AddPattern(Ref<PatternSet>& exclude,char opt,const char *optarg)
{
   PatternSet::Type type=
      (opt=='x'||opt=='X'||opt=='\0'?PatternSet::EXCLUDE:PatternSet::INCLUDE);
   PatternSet::Pattern *pattern=0;
   if(opt=='x' || opt=='i')
   {
      Ref<PatternSet::Regex> rx(new PatternSet::Regex(optarg));
      if(rx->Error())
	 return xstring::get_tmp(rx->ErrorText());
      pattern=rx.borrow();
   }
   else if(opt=='X' || opt=='I')
   {
      pattern=new PatternSet::Glob(optarg);
   }
   if(!exclude)
   {
      const char *default_exclude=ResMgr::Query("mirror:exclude-regex",0);
      const char *default_include=ResMgr::Query("mirror:include-regex",0);

      // don't create default pattern set if not needed
      if(!pattern && !(default_exclude && *default_exclude))
	 return NULL;

      exclude=new PatternSet;
      /* Make default_exclude the first pattern so that it can be
       * overridden by --include later, and do that only when first
       * explicit pattern is for exclusion - otherwise all files are
       * excluded by default and no default exclusion is needed. */
      if(type==PatternSet::EXCLUDE && default_exclude && *default_exclude)
      {
	 exclude->Add(type,new PatternSet::Regex(default_exclude));
	 if(default_include && *default_include)
	    exclude->Add(PatternSet::INCLUDE,new PatternSet::Regex(default_include));
      }
   }
   if(pattern)
      exclude->Add(type,pattern);

   return NULL; // no error
}