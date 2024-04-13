void MirrorJob::HandleListInfoCreation(const FileAccessRef& session,SMTaskRef<ListInfo>& list_info,const char *relative_dir)
{
   if(state!=GETTING_LIST_INFO)
      return;

   if(session==target_session && no_target_dir)
   {
      target_set=new FileSet();
      return;
   }

   list_info=session->MakeListInfo();
   if(list_info==0)
   {
      eprintf(_("mirror: protocol `%s' is not suitable for mirror\n"),
	       session->GetProto());
      MirrorFinished();
      set_state(FINISHING);
      return;
   }
   list_info->UseCache(use_cache);
   int need=FileInfo::ALL_INFO;
   if(FlagSet(IGNORE_TIME))
      need&=~FileInfo::DATE;
   if(FlagSet(IGNORE_SIZE))
      need&=~FileInfo::SIZE;
   list_info->Need(need);
   if(FlagSet(RETR_SYMLINKS))
      list_info->FollowSymlinks();

   list_info->SetExclude(relative_dir,top_exclude?top_exclude:exclude);
   list_info->Roll();
}