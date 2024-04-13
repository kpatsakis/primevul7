void MirrorJob::HandleChdir(FileAccessRef& session, int &redirections)
{
   if(!session->IsOpen())
      return;
   int res=session->Done();
   if(res<0)
   {
      if(res==FA::FILE_MOVED)
      {
	 // cd to another url.
	 const char *loc_c=session->GetNewLocation();
	 int max_redirections=ResMgr::Query("xfer:max-redirections",0);
	 if(loc_c && max_redirections>0)
	 {
	    if(++redirections>max_redirections)
	       goto cd_err_normal;
	    eprintf(_("%s: received redirection to `%s'\n"),"mirror",loc_c);

	    char *loc=alloca_strdup(loc_c);
	    ParsedURL u(loc,true);

	    bool is_file=(last_char(loc)!='/');
	    if(!u.proto)
	    {
	       FileAccess::Path new_cwd(session->GetNewCwd());
	       new_cwd.Change(0,is_file,loc);
	       session->PathVerify(new_cwd);
	       session->Roll();
	       return;
	    }
	    session->Close(); // loc_c is no longer valid.
	    session=FA::New(&u);
	    FileAccess::Path new_cwd(u.path,is_file,url::path_ptr(loc));
	    session->PathVerify(new_cwd);
	    return;
	 }
      }
   cd_err_normal:
      if(session==target_session && (script_only || FlagSet(SCAN_ALL_FIRST)))
      {
	 char *dir=alloca_strdup(session->GetFile());
	 session->Close();
	 session->Chdir(dir,false);
	 no_target_dir=true;
	 return;
      }
      if(session==source_session && create_target_dir
      && !FlagSet(NO_EMPTY_DIRS) && !skip_noaccess && parent_mirror)
      {
	 // create target dir even if failed to cd to source dir.
	 if(script)
	    fprintf(script,"mkdir %s\n",target_session->GetFileURL(target_dir).get());
	 if(!script_only)
	 {
	    ArgV *a=new ArgV("mkdir");
	    a->Append(target_dir);
	    mkdirJob *mkj=new mkdirJob(target_session->Clone(),a);
	    a->CombineTo(mkj->cmdline);
	    JobStarted(mkj);
	 }
      }
      remove_this_source_dir=false;
      eprintf("mirror: %s\n",session->StrError(res));
      stats.error_count++;
      MirrorFinished();
      set_state(FINISHING);
      source_session->Close();
      target_session->Close();
      return;
   }
   if(res==FA::OK)
      session->Close();
}