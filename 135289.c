MirrorJob::MirrorJob(MirrorJob *parent,
   FileAccess *source,FileAccess *target,
   const char *new_source_dir,const char *new_target_dir)
 :
   bytes_transferred(0), bytes_to_transfer(0),
   source_dir(new_source_dir), target_dir(new_target_dir),
   transfer_time_elapsed(0), root_transfer_count(0),
   verbose_report(0),
   parent_mirror(parent), root_mirror(parent?parent->root_mirror:this)
{

   source_session=source;
   target_session=target;
   // TODO: get rid of this.
   source_is_local=!strcmp(source_session->GetProto(),"file");
   target_is_local=!strcmp(target_session->GetProto(),"file");

   create_target_dir=true;
   no_target_dir=false;
   remove_this_source_dir=false;

   flags=0;
   recursion_mode=RECURSION_ALWAYS;
   max_error_count=0;

   exclude=0;

   set_state(INITIAL_STATE);

   newer_than=NO_DATE;
   older_than=NO_DATE;
   size_range=0;

   script=0;
   script_only=false;
   script_needs_closing=false;

   use_cache=false;
   remove_source_files=false;
   remove_source_dirs=false;
   skip_noaccess=false;

   parallel=1;
   pget_n=1;
   pget_minchunk=0x10000;

   source_redirections=0;
   target_redirections=0;

   if(parent_mirror)
   {
      bool parallel_dirs=ResMgr::QueryBool("mirror:parallel-directories",0);
      // If parallel_dirs is true, allow parent mirror to continue
      // processing other directories, otherwise block it until we
      // get file sets and start transfers.
      // See also comment at MirrorJob::MirrorStarted().
      root_transfer_count=parallel_dirs?1:1024;

      // inherit flags and other things
      SetFlags(parent->flags,1);
      UseCache(parent->use_cache);

      SetExclude(parent->exclude);

      verbose_report=parent->verbose_report;
      newer_than=parent->newer_than;
      older_than=parent->older_than;
      size_range=parent->size_range;
      parallel=parent->parallel;
      pget_n=parent->pget_n;
      pget_minchunk=parent->pget_minchunk;
      remove_source_files=parent->remove_source_files;
      remove_source_dirs=parent->remove_source_dirs;
      skip_noaccess=parent->skip_noaccess;
      no_target_dir=parent->no_target_dir;
      recursion_mode=parent->recursion_mode;

      script=parent->script;
      script_needs_closing=false;
      script_name.set(parent->script_name);
      script_only=parent->script_only;

      max_error_count=parent->max_error_count;
   }
   MirrorStarted();
}