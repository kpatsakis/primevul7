void  MirrorJob::ShowRunStatus(const SMTaskRef<StatusLine>& s)
{
   int w=s->GetWidthDelayed();
   switch(state)
   {
   case(INITIAL_STATE):
      break;

   // these have a sub-job
   case(WAITING_FOR_TRANSFER):
   case(TARGET_REMOVE_OLD):
   case(TARGET_REMOVE_OLD_FIRST):
   case(TARGET_CHMOD):
   case(TARGET_MKDIR):
   case(SOURCE_REMOVING_SAME):
   case(FINISHING):
   case(DONE):
   case(LAST_EXEC):
      Job::ShowRunStatus(s);
      break;

   case(MAKE_TARGET_DIR):
      s->Show("mkdir `%s' [%s]",target_dir.get(),target_session->CurrentStatus());
      break;

   case(CHANGING_DIR_SOURCE):
   case(CHANGING_DIR_TARGET):
      if(target_session->IsOpen() && (!source_session->IsOpen() || now%4>=2))
	 s->Show("cd `%s' [%s]",target_dir.get(),target_session->CurrentStatus());
      else if(source_session->IsOpen())
	 s->Show("cd `%s' [%s]",source_dir.get(),source_session->CurrentStatus());
      break;

   case(GETTING_LIST_INFO):
      if(target_list_info && (!source_list_info || now%4>=2))
      {
	 const char *status=target_list_info->Status();
	 int status_w=mbswidth(status, 0);
	 int dw=w-status_w;
	 if(dw<20)
	    dw=20;
	 if(target_relative_dir)
	    s->Show("%s: %s",squeeze_file_name(target_relative_dir,dw),status);
	 else
	    s->Show("%s",status);
      }
      else if(source_list_info)
      {
	 const char *status=source_list_info->Status();
	 int status_w=mbswidth(status, 0);
	 int dw=w-status_w;
	 if(dw<20)
	    dw=20;
	 if(source_relative_dir)
	    s->Show("%s: %s",squeeze_file_name(source_relative_dir,dw),status);
	 else
	    s->Show("%s",status);
      }
      break;
   }
}