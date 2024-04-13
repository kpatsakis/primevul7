CMD(mirror)
{
#define args (parent->args)
#define eprintf parent->eprintf
   enum {
      OPT_ALLOW_CHOWN,
      OPT_DELETE_FIRST,
      OPT_IGNORE_SIZE,
      OPT_IGNORE_TIME,
      OPT_LOOP,
      OPT_MAX_ERRORS,
      OPT_NO_DEREFERENCE,
      OPT_NO_SYMLINKS,
      OPT_NO_UMASK,
      OPT_OLDER_THAN,
      OPT_ONLY_MISSING,
      OPT_ONLY_EXISTING,
      OPT_PERMS,
      OPT_REMOVE_SOURCE_FILES,
      OPT_REMOVE_SOURCE_DIRS,
      OPT_SCRIPT,
      OPT_SCRIPT_ONLY,
      OPT_SIZE_RANGE,
      OPT_USE_CACHE,
      OPT_USE_PGET_N,
      OPT_SKIP_NOACCESS,
      OPT_ON_CHANGE,
      OPT_NO_EMPTY_DIRS,
      OPT_DEPTH_FIRST,
      OPT_ASCII,
      OPT_SCAN_ALL_FIRST,
      OPT_OVERWRITE,
      OPT_NO_OVERWRITE,
      OPT_RECURSION,
      OPT_UPLOAD_OLDER,
      OPT_TRANSFER_ALL,
      OPT_TARGET_FLAT,
      OPT_DELETE_EXCLUDED,
   };
   static const struct option mirror_opts[]=
   {
      {"delete",no_argument,0,'e'},
      {"allow-suid",no_argument,0,'s'},
      {"allow-chown",no_argument,0,OPT_ALLOW_CHOWN},
      {"include",required_argument,0,'i'},
      {"exclude",required_argument,0,'x'},
      {"include-glob",required_argument,0,'I'},
      {"exclude-glob",required_argument,0,'X'},
      {"include-rx-from",required_argument,0,'i'+'f'},
      {"exclude-rx-from",required_argument,0,'x'+'f'},
      {"include-glob-from",required_argument,0,'I'+'f'},
      {"exclude-glob-from",required_argument,0,'X'+'f'},
      {"only-newer",no_argument,0,'n'},
      {"no-recursion",no_argument,0,'r'},
      {"no-perms",no_argument,0,'p'},
      {"perms",no_argument,0,OPT_PERMS},
      {"no-umask",no_argument,0,OPT_NO_UMASK},
      {"continue",no_argument,0,'c'},
      {"reverse",no_argument,0,'R'},
      {"verbose",optional_argument,0,'v'},
      {"newer-than",required_argument,0,'N'},
      {"file",required_argument,0,'f'},
      {"directory",required_argument,0,'F'},
      {"older-than",required_argument,0,OPT_OLDER_THAN},
      {"size-range",required_argument,0,OPT_SIZE_RANGE},
      {"dereference",no_argument,0,'L'},
      {"no-dereference",no_argument,0,OPT_NO_DEREFERENCE},
      {"use-cache",no_argument,0,OPT_USE_CACHE},
      {"Remove-source-files",no_argument,0,OPT_REMOVE_SOURCE_FILES},
      {"Remove-source-dirs",no_argument,0,OPT_REMOVE_SOURCE_DIRS},
      {"Move",no_argument,0,OPT_REMOVE_SOURCE_DIRS},
      {"parallel",optional_argument,0,'P'},
      {"ignore-time",no_argument,0,OPT_IGNORE_TIME},
      {"ignore-size",no_argument,0,OPT_IGNORE_SIZE},
      {"only-missing",no_argument,0,OPT_ONLY_MISSING},
      {"only-existing",no_argument,0,OPT_ONLY_EXISTING},
      {"log",required_argument,0,OPT_SCRIPT},
      {"script",    required_argument,0,OPT_SCRIPT_ONLY},
      {"just-print",optional_argument,0,OPT_SCRIPT_ONLY},
      {"dry-run",   optional_argument,0,OPT_SCRIPT_ONLY},
      {"delete-first",no_argument,0,OPT_DELETE_FIRST},
      {"use-pget-n",optional_argument,0,OPT_USE_PGET_N},
      {"no-symlinks",no_argument,0,OPT_NO_SYMLINKS},
      {"loop",no_argument,0,OPT_LOOP},
      {"max-errors",required_argument,0,OPT_MAX_ERRORS},
      {"skip-noaccess",no_argument,0,OPT_SKIP_NOACCESS},
      {"on-change",required_argument,0,OPT_ON_CHANGE},
      {"no-empty-dirs",no_argument,0,OPT_NO_EMPTY_DIRS},
      {"depth-first",no_argument,0,OPT_DEPTH_FIRST},
      {"ascii",no_argument,0,OPT_ASCII},
      {"target-directory",required_argument,0,'O'},
      {"destination-directory",required_argument,0,'O'},
      {"scan-all-first",no_argument,0,OPT_SCAN_ALL_FIRST},
      {"overwrite",no_argument,0,OPT_OVERWRITE},
      {"no-overwrite",no_argument,0,OPT_NO_OVERWRITE},
      {"recursion",required_argument,0,OPT_RECURSION},
      {"upload-older",no_argument,0,OPT_UPLOAD_OLDER},
      {"transfer-all",no_argument,0,OPT_TRANSFER_ALL},
      {"flat",no_argument,0,OPT_TARGET_FLAT},
      {"delete-excluded",no_argument,0,OPT_DELETE_EXCLUDED},
      {0}
   };

   int opt;
   unsigned flags=0;
   int max_error_count=0;

   bool use_cache=false;

   FileAccessRef source_session;
   FileAccessRef target_session;

   int	 verbose=0;
   const char *newer_than=0;
   const char *older_than=0;
   Ref<Range> size_range;
   bool  remove_source_files=false;
   bool  remove_source_dirs=false;
   bool	 skip_noaccess=ResMgr::QueryBool("mirror:skip-noaccess",0);
   int	 parallel=-1;
   int	 use_pget=-1;
   bool	 reverse=false;
   bool	 script_only=false;
   bool	 no_empty_dirs=ResMgr::QueryBool("mirror:no-empty-dirs",0);
   const char *script_file=0;
   const char *on_change=0;
   const char *recursion_mode=0;
   bool single_file=false;
   bool single_dir=false;

   Ref<PatternSet> exclude;
   Ref<PatternSet> top_exclude;

   if(!ResMgr::QueryBool("mirror:set-permissions",0))
      flags|=MirrorJob::NO_PERMS;
   if(ResMgr::QueryBool("mirror:dereference",0))
      flags|=MirrorJob::RETR_SYMLINKS;
   if(ResMgr::QueryBool("mirror:overwrite",0))
      flags|=MirrorJob::OVERWRITE;

   const char *source_dir=NULL;
   const char *target_dir=NULL;

   args->rewind();
   while((opt=args->getopt_long("esi:x:I:X:nrpcRvN:LP:af:F:O:",mirror_opts,0))!=EOF)
   {
      switch(opt)
      {
      case('e'):
	 flags|=MirrorJob::DELETE;
	 break;
      case('s'):
	 flags|=MirrorJob::ALLOW_SUID;
	 break;
      case(OPT_ALLOW_CHOWN):
	 flags|=MirrorJob::ALLOW_CHOWN;
	 break;
      case('a'):
	 flags|=MirrorJob::ALLOW_SUID|MirrorJob::ALLOW_CHOWN|MirrorJob::NO_UMASK;
	 break;
      case('r'):
	 recursion_mode="never";
	 break;
      case('n'):
	 flags|=MirrorJob::ONLY_NEWER;
	 break;
      case('p'):
	 flags|=MirrorJob::NO_PERMS;
	 break;
      case(OPT_PERMS):
	 flags&=~MirrorJob::NO_PERMS;
	 break;
      case('c'):
	 flags|=MirrorJob::CONTINUE;
	 break;
      case('x'):
      case('i'):
      case('X'):
      case('I'):
      {
	 const char *err=MirrorJob::AddPattern(exclude,opt,optarg);
	 if(err)
	 {
	    eprintf("%s: %s\n",args->a0(),err);
	    goto no_job;
	 }
	 break;
      }
      case('x'+'f'):
      case('i'+'f'):
      case('X'+'f'):
      case('I'+'f'):
      {
	 const char *err=MirrorJob::AddPatternsFrom(exclude,opt-'f',optarg);
	 if(err)
	 {
	    eprintf("%s: %s\n",args->a0(),err);
	    goto no_job;
	 }
	 break;
      }
      case('R'):
	 reverse=true;
	 break;
      case('L'):
	 flags|=MirrorJob::RETR_SYMLINKS;
	 break;
      case(OPT_NO_DEREFERENCE):
	 flags&=~MirrorJob::RETR_SYMLINKS;
	 break;
      case('v'):
	 if(optarg)
	    verbose=atoi(optarg);
	 else
	    verbose++;
	 if(verbose>1)
	    flags|=MirrorJob::REPORT_NOT_DELETED;
	 break;
      case('N'):
	 newer_than=optarg;
	 break;
      case('f'): // mirror for a single file (or glob pattern).
	 single_file=true;
	 /*fallthrough*/
      case('F'): // mirror for a single directory (or glob pattern).
      {
	 xstring pattern(basename_ptr(optarg));
	 if(opt=='F') {
	    single_dir=true;
	    if(pattern.last_char()!='/')
	       pattern.append('/');
	 }
	 if(!top_exclude)
	    top_exclude=new PatternSet();
	 top_exclude->Add(PatternSet::INCLUDE,new PatternSet::Glob(pattern));
	 const char *dir=dirname(optarg);
	 if(source_dir && strcmp(source_dir,dir)) {
	    eprintf(_("%s: multiple --file or --directory options must have the same base directory\n"),args->a0());
	    goto no_job;
	 }
	 if(!source_dir)
	    source_dir=alloca_strdup(dir); // save the temp string
	 break;
      }
      case('O'):
	 target_dir=optarg;
	 break;
      case(OPT_OLDER_THAN):
	 older_than=optarg;
	 break;
      case(OPT_SIZE_RANGE):
	 size_range=new Range(optarg);
	 if(size_range->Error())
	 {
	    eprintf("%s: --size-range \"%s\": %s\n",
	       args->a0(),optarg,size_range->ErrorText());
	    goto no_job;
	 }
	 break;
      case(OPT_NO_UMASK):
	 flags|=MirrorJob::NO_UMASK;
	 break;
      case(OPT_USE_CACHE):
	 use_cache=true;
	 break;
      case(OPT_REMOVE_SOURCE_FILES):
	 remove_source_files=true;
	 break;
      case(OPT_REMOVE_SOURCE_DIRS):
	 remove_source_dirs=true;
	 break;
      case(OPT_IGNORE_TIME):
	 flags|=MirrorJob::IGNORE_TIME;
	 break;
      case(OPT_IGNORE_SIZE):
	 flags|=MirrorJob::IGNORE_SIZE;
	 break;
      case(OPT_ONLY_MISSING):
	 flags|=MirrorJob::IGNORE_TIME|MirrorJob::IGNORE_SIZE;
	 break;
      case('P'):
	 if(optarg)
	    parallel=atoi(optarg);
	 else
	    parallel=3;
	 break;
      case(OPT_USE_PGET_N):
	 if(optarg)
	    use_pget=atoi(optarg);
	 else
	    use_pget=3;
	 break;
      case(OPT_SCRIPT_ONLY):
	 script_only=true;
      case(OPT_SCRIPT):
	 script_file=optarg;
	 if(script_file==0)
	    script_file="-";
	 break;
      case(OPT_DELETE_FIRST):
	 flags|=MirrorJob::REMOVE_FIRST|MirrorJob::DELETE;
	 break;
      case(OPT_NO_SYMLINKS):
	 flags|=MirrorJob::NO_SYMLINKS;
	 break;
      case(OPT_LOOP):
	 flags|=MirrorJob::LOOP;
	 break;
      case(OPT_MAX_ERRORS):
	 max_error_count=atoi(optarg);
	 break;
      case(OPT_SKIP_NOACCESS):
	 skip_noaccess=true;
	 break;
      case(OPT_ON_CHANGE):
	 on_change=optarg;
	 break;
      case(OPT_ONLY_EXISTING):
	 flags|=MirrorJob::ONLY_EXISTING;
	 break;
      case(OPT_NO_EMPTY_DIRS):
	 no_empty_dirs=true;
	 break;
      case(OPT_DEPTH_FIRST):
	 flags|=MirrorJob::DEPTH_FIRST;
	 break;
      case(OPT_SCAN_ALL_FIRST):
	 flags|=MirrorJob::SCAN_ALL_FIRST|MirrorJob::DEPTH_FIRST;
	 break;
      case(OPT_ASCII):
	 flags|=MirrorJob::ASCII|MirrorJob::IGNORE_SIZE;
	 break;
      case(OPT_OVERWRITE):
	 flags|=MirrorJob::OVERWRITE;
	 break;
      case(OPT_NO_OVERWRITE):
	 flags&=~MirrorJob::OVERWRITE;
	 break;
      case(OPT_RECURSION):
	 recursion_mode=optarg;
	 break;
      case(OPT_UPLOAD_OLDER):
	 flags|=MirrorJob::UPLOAD_OLDER;
	 break;
      case(OPT_TRANSFER_ALL):
	 flags|=MirrorJob::TRANSFER_ALL;
	 break;
      case(OPT_TARGET_FLAT):
	 flags|=MirrorJob::TARGET_FLAT|MirrorJob::SCAN_ALL_FIRST|MirrorJob::DEPTH_FIRST;
	 break;
      case(OPT_DELETE_EXCLUDED):
	 flags|=MirrorJob::DELETE_EXCLUDED;
	 break;
      case('?'):
	 eprintf(_("Try `help %s' for more information.\n"),args->a0());
      no_job:
	 return 0;
      }
   }

   if(exclude && xstrcasecmp(recursion_mode,"never"))
   {
      /* Users usually don't want to exclude all directories when recursing */
      if(exclude->GetFirstType()==PatternSet::INCLUDE)
	 exclude->AddFirst(PatternSet::INCLUDE,new PatternSet::Regex("/$"));
   }

   /* add default exclusion if no explicit patterns were specified */
   if(!exclude)
      MirrorJob::AddPattern(exclude,'\0',0);

   args->back();

   const char *arg=args->getnext();
   if(arg)
   {
      if(source_dir)
      {
	 eprintf(_("%s: ambiguous source directory (`%s' or `%s'?)\n"),args->a0(),
	    source_dir,arg);
	 goto no_job;
      }
      source_dir=arg;
      ParsedURL source_url(source_dir);
      if(source_url.proto && source_url.path)
      {
	 source_session=FileAccess::New(&source_url);
	 if(!source_session)
	 {
	    eprintf("%s: %s%s\n",args->a0(),source_url.proto.get(),
		     _(" - not supported protocol"));
	    goto no_job;
	 }
	 source_dir=alloca_strdup(source_url.path);
      }
      arg=args->getnext();
      if(arg)
      {
	 if(target_dir)
	 {
	    eprintf(_("%s: ambiguous target directory (`%s' or `%s'?)\n"),args->a0(),
	       target_dir,arg);
	    goto no_job;
	 }
	 target_dir=arg;
	 ParsedURL target_url(target_dir);
	 if(target_url.proto && target_url.path)
	 {
	    target_session=FileAccess::New(&target_url);
	    if(!target_session)
	    {
	       eprintf("%s: %s%s\n",args->a0(),target_url.proto.get(),
			_(" - not supported protocol"));
	       goto no_job;
	    }
	    target_dir=alloca_strdup(target_url.path);
	 }
	 if(last_char(arg)=='/' && basename_ptr(arg)[0]!='/' && last_char(source_dir)!='/')
	 {
	    // user wants source dir name appended.
	    const char *base=basename_ptr(source_dir);
	    if(base[0]!='/' && strcmp(base,basename_ptr(arg))) {
	       target_dir=xstring::cat(target_dir,base,NULL);
	       target_dir=alloca_strdup(target_dir); // save the buffer
	    }
	 }
      }
      else
      {
	 target_dir=basename_ptr(source_dir);
	 if(target_dir[0]=='/')
	    target_dir=".";
	 else if(target_dir[0]=='~') {
	    target_dir=dir_file(".",target_dir);
	    target_dir=alloca_strdup(target_dir); // save the buffer
	 }
      }
   }

   if(!source_dir) {
      if(ResMgr::QueryBool("mirror:require-source",0)) {
	 eprintf(_("%s: source directory is required (mirror:require-source is set)\n"),args->a0());
	 return 0;
      }
      source_dir=".";
   }
   if(!target_dir)
      target_dir=".";

   if(!reverse)
   {
      if(!source_session)
	 source_session=parent->session->Clone();
      if(!target_session)
	 target_session=FileAccess::New("file");
   }
   else //reverse
   {
      if(!source_session)
	 source_session=FileAccess::New("file");
      if(!target_session)
	 target_session=parent->session->Clone();
   }

   if(no_empty_dirs)
      flags|=MirrorJob::NO_EMPTY_DIRS|MirrorJob::DEPTH_FIRST;

   if(recursion_mode && strcasecmp(recursion_mode,"always")
   && (flags&MirrorJob::DEPTH_FIRST)) {
      eprintf("%s: --recursion-mode=%s conflicts with other specified options\n",
	 args->a0(),recursion_mode);
      return 0;
   }

   if(parallel<0) {
      int parallel1=ResMgr::Query("mirror:parallel-transfer-count",source_session->GetHostName());
      int parallel2=ResMgr::Query("mirror:parallel-transfer-count",target_session->GetHostName());
      if(parallel1>0)
	 parallel=parallel1;
      if(parallel2>0 && (parallel<0 || parallel>parallel2))
	 parallel=parallel2;
   }
   if(use_pget<0) {
      int use_pget1=ResMgr::Query("mirror:use-pget-n",source_session->GetHostName());
      int use_pget2=ResMgr::Query("mirror:use-pget-n",target_session->GetHostName());
      if(use_pget1>0)
	 use_pget=use_pget1;
      if(use_pget2>0 && (use_pget<0 || use_pget>use_pget2))
	 use_pget=use_pget2;
   }

   JobRef<MirrorJob> j(new MirrorJob(0,source_session.borrow(),target_session.borrow(),source_dir,target_dir));
   j->SetFlags(flags,1);
   j->SetVerbose(verbose);
   j->SetExclude(exclude.borrow());
   j->SetTopExclude(top_exclude.borrow());

   if(newer_than)
      j->SetNewerThan(newer_than);
   if(older_than)
      j->SetOlderThan(older_than);
   if(size_range)
      j->SetSizeRange(size_range.borrow());
   j->UseCache(use_cache);
   if(remove_source_files)
      j->RemoveSourceFiles();
   if(remove_source_dirs)
      j->RemoveSourceDirs();
   if(skip_noaccess)
      j->SkipNoAccess();
   if(parallel<0)
      parallel=0;
   if(parallel>64)
      parallel=64;   // a (in)sane limit.
   if(parallel)
      j->SetParallel(parallel);
   if(use_pget>1 && !(flags&MirrorJob::ASCII))
      j->SetPGet(use_pget);

   if(!recursion_mode && single_file && !single_dir)
      recursion_mode="never";

   if(recursion_mode) {
      const char *err=j->SetRecursionMode(recursion_mode);
      if(err) {
	 eprintf("%s: %s\n",args->a0(),err);
	 return 0;
      }
   }
   if(script_file)
   {
      const char *err=j->SetScriptFile(script_file);
      if(err)
      {
	 eprintf("%s: %s\n",args->a0(),err);
	 return 0;
      }
   }
   if(script_only)
   {
      j->ScriptOnly();
      if(!script_file)
	 j->SetScriptFile("-");
   }
   j->SetMaxErrorCount(max_error_count);
   if(on_change)
      j->SetOnChange(on_change);

   return j.borrow();

#undef args
}