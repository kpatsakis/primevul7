void  MirrorJob::HandleFile(FileInfo *file)
{
   int	 res;
   struct stat st;

   // TODO: get rid of local hacks.

   const char *dst_name=file->name;
   if(FlagSet(TARGET_FLAT))
      dst_name=basename_ptr(dst_name);

   // dir_name returns pointer to static data - need to dup it.
   const char *source_name=dir_file(source_dir,file->name);
   source_name=alloca_strdup(source_name);
   const char *target_name=dir_file(target_dir,dst_name);
   target_name=alloca_strdup(target_name);

   const char *source_name_rel=dir_file(source_relative_dir,file->name);
   source_name_rel=alloca_strdup(source_name_rel);
   const char *target_name_rel=dir_file(target_relative_dir,dst_name);
   target_name_rel=alloca_strdup(target_name_rel);

   FileInfo::type filetype=FileInfo::NORMAL;
   if(file->Has(file->TYPE))
      filetype=file->filetype;
   else
   {
      FileInfo *target=target_set->FindByName(file->name);
      if(target && target->Has(target->TYPE))
	 filetype=target->filetype;
   }

   switch(filetype)
   {
      case(FileInfo::NORMAL):
      case(FileInfo::REDIRECT):
      {
	 bool remove_target=false;
	 bool cont_this=false;
	 bool use_pget=(pget_n>1) && target_is_local;
	 if(file->Has(file->SIZE) && file->size<pget_minchunk*2)
	    use_pget=false;
	 if(target_is_local)
	 {
	    if(lstat(target_name,&st)!=-1)
	    {
	       // few safety checks.
	       FileInfo *old=new_files_set->FindByName(file->name);
	       if(old)
		  goto skip;  // file has appeared after mirror start
	       old=old_files_set->FindByName(file->name);
	       if(old && ((old->Has(old->SIZE) && old->size!=st.st_size)
			||(old->Has(old->DATE) && old->date!=st.st_mtime)))
		  goto skip;  // the file has changed after mirror start
	       if(!script_only && access(target_name,W_OK)==-1)
	       {
		  // try to enable write access.
		  chmod(target_name,st.st_mode|0200);
	       }
	    }
	 }
	 FileInfo *old=target_set->FindByName(FileCopy::TempFileName(file->name));
	 if(old)
	 {
	    if(FlagSet(CONTINUE)
	    && old->Has(file->TYPE) && old->filetype==old->NORMAL
	    && (FlagSet(IGNORE_TIME) ||
		(file->Has(file->DATE) && old->Has(old->DATE)
		&& file->date + file->date.ts_prec < old->date - old->date.ts_prec))
	    && file->Has(file->SIZE) && old->Has(old->SIZE)
	    && file->size >= old->size)
	    {
	       cont_this=true;
	       stats.mod_files++;
	    }
	    else if(!to_rm_mismatched->FindByName(file->name))
	    {
	       if(!FlagSet(OVERWRITE)) {
		  remove_target=true;
		  Report(_("Removing old file `%s'"),target_name_rel);
	       } else {
		  Report(_("Overwriting old file `%s'"),target_name_rel);
	       }
	       stats.mod_files++;
	    }
	    else
	       stats.new_files++;
	 }
	 else if(FlagSet(ONLY_EXISTING))
	 {
	    Report(_("Skipping file `%s' (only-existing)"),source_name_rel);
	    goto skip;
	 }
	 else
	    stats.new_files++;

	 Report(_("Transferring file `%s'"),source_name_rel);

	 if(script)
	 {
	    ArgV args(use_pget?"pget":"get");
	    if(use_pget)
	    {
	       args.Append("-n");
	       args.Append(pget_n);
	    }
	    if(cont_this)
	       args.Append("-c");
	    if(remove_target)
	       args.Append("-e");
	    if(FlagSet(ASCII))
	       args.Append("-a");
	    if(remove_source_files)
	       args.Append("-E");
	    args.Append("-O");
	    args.Append(target_is_local?target_dir.get()
			:target_session->GetConnectURL().get());
	    args.Append(source_session->GetFileURL(file->name));
	    xstring_ca cmd(args.CombineQuoted());
	    fprintf(script,"%s\n",cmd.get());
	    if(script_only)
	       goto skip;
	 }

	 FileCopyPeer *src_peer=0;
	 if(source_is_local)
	    src_peer=new FileCopyPeerFDStream(new FileStream(source_name,O_RDONLY),FileCopyPeer::GET);
	 else
	    src_peer=new FileCopyPeerFA(source_session->Clone(),file->name,FA::RETRIEVE);

	 FileCopyPeer *dst_peer=0;
	 if(target_is_local)
	    dst_peer=new FileCopyPeerFDStream(new FileStream(target_name,O_WRONLY|O_CREAT|(cont_this?0:O_TRUNC)),FileCopyPeer::PUT);
	 else
	    dst_peer=new FileCopyPeerFA(target_session->Clone(),dst_name,FA::STORE);

	 FileCopy *c=FileCopy::New(src_peer,dst_peer,cont_this);
	 if(remove_source_files)
	    c->RemoveSourceLater();
	 if(remove_target)
	    c->RemoveTargetFirst();
	 if(FlagSet(ASCII))
	    c->Ascii();
	 CopyJob *cp=(use_pget ? new pgetJob(c,file->name,pget_n) : new CopyJob(c,file->name,"mirror"));
	 if(file->Has(file->DATE))
	    cp->SetDate(file->date);
	 if(file->Has(file->SIZE) && !FlagSet(IGNORE_SIZE))
	    cp->SetSize(file->size);
	 TransferStarted(cp);
	 cp->cmdline.vset("\\transfer `",source_name_rel,"'",NULL);

	 set_state(WAITING_FOR_TRANSFER);
	 break;
      }
      case(FileInfo::DIRECTORY):
      {
	 if(recursion_mode==RECURSION_NEVER || FlagSet(NO_RECURSION))
	    goto skip;

	 bool create_target_subdir=true;
	 const FileInfo *old=0;

	 if(FlagSet(TARGET_FLAT)) {
	    create_target_subdir=false;
	    target_name=target_dir;
	    goto do_submirror;
	 }

	 if(target_set)
	    old=target_set->FindByName(file->name);
	 if(!old)
	 {
	    if(FlagSet(ONLY_EXISTING))
	    {
	       Report(_("Skipping directory `%s' (only-existing)"),target_name_rel);
	       goto skip;
	    }
	 }
	 else if(old->TypeIs(old->DIRECTORY))
	 {
	    create_target_subdir=false;
	 }
	 if(target_is_local && !script_only)
	 {
	    if((FlagSet(RETR_SYMLINKS)?stat:lstat)(target_name,&st)!=-1)
	    {
	       if(S_ISDIR(st.st_mode))
	       {
		  // try to enable write access
		  // only if not enabled as chmod can clear sgid flags on directories
		  if(st.st_mode!=(st.st_mode|0700))
		     chmod(target_name,st.st_mode|0700);
		  create_target_subdir=false;
	       }
	       else
	       {
		  Report(_("Removing old local file `%s'"),target_name_rel);
		  if(remove(target_name)==-1)
		  {
		     eprintf("mirror: remove(%s): %s\n",target_name,strerror(errno));
		     goto skip;
		  }
		  create_target_subdir=true;
	       }
	    }
	 }

      do_submirror:
	 // launch sub-mirror
	 MirrorJob *mj=new MirrorJob(this,
	    source_session->Clone(),target_session->Clone(),
	    source_name,target_name);
	 AddWaiting(mj);
	 mj->cmdline.vset("\\mirror `",source_name_rel,"'",NULL);

	 mj->source_relative_dir.set(source_name_rel);
	 mj->target_relative_dir.set(target_name_rel);

	 mj->create_target_dir=create_target_subdir;

	 if(verbose_report>=3) {
	    if(FlagSet(SCAN_ALL_FIRST))
	       Report(_("Scanning directory `%s'"),mj->target_relative_dir.get());
	    else
	       Report(_("Mirroring directory `%s'"),mj->target_relative_dir.get());
	 }

	 break;
      }
      case(FileInfo::SYMLINK):
      {
	 if(FlagSet(NO_SYMLINKS))
	    goto skip;

	 if(!file->symlink)
	    goto skip;

	 if(!target_is_local)
	 {
	    if(script)
	    {
	       ArgV args("ln");
	       args.Append("-s");
	       args.Append(file->symlink);
	       args.Append(target_name);
	       xstring_ca cmd(args.CombineQuoted());
	       fprintf(script,"%s\n",cmd.get());
	       if(script_only)
		  goto skip;
	    }
	    bool remove_target=false;
	    FileInfo *old=target_set->FindByName(file->name);
	    if(old && !to_rm_mismatched->FindByName(file->name))
	    {
	       Report(_("Removing old file `%s'"),target_name_rel);
	       remove_target=true;
	       stats.mod_symlinks++;
	    }
	    else
	       stats.new_symlinks++;
	    Report(_("Making symbolic link `%s' to `%s'"),target_name_rel,file->symlink.get());
	    mvJob *j=new mvJob(target_session->Clone(),file->symlink,target_name,FA::SYMLINK);
	    if(remove_target)
	       j->RemoveTargetFirst();
	    JobStarted(j);
	    RemoveSourceLater(file);
	    break;
	 }

	 if(script)
	 {
	    ArgV args("shell");
	    args.Append("ln");
	    args.Append("-sf");
	    args.Append(shell_encode(file->symlink));
	    args.Append(shell_encode(target_name));
	    xstring_ca cmd(args.CombineQuoted());
	    fprintf(script,"%s\n",cmd.get());
	    if(script_only)
	       goto skip;
	 }

	 struct stat st;
	 if(lstat(target_name,&st)!=-1)
	 {
	    Report(_("Removing old local file `%s'"),target_name_rel);
	    stats.mod_symlinks++;
	    if(remove(target_name)==-1)
	    {
	       eprintf("mirror: remove(%s): %s\n",target_name,strerror(errno));
	       goto skip;
	    }
	 }
	 else
	 {
	    if(FlagSet(ONLY_EXISTING))
	    {
	       Report(_("Skipping symlink `%s' (only-existing)"),target_name_rel);
	       goto skip;
	    }
	    stats.new_symlinks++;
	 }
	 Report(_("Making symbolic link `%s' to `%s'"),target_name_rel,file->symlink.get());
	 res=symlink(file->symlink,target_name);
	 if(res==-1)
	    eprintf("mirror: symlink(%s): %s\n",target_name,strerror(errno));
	 RemoveSourceLater(file);
	 break;
      }
   case FileInfo::UNKNOWN:
      break;
   }
skip:
   return;
}