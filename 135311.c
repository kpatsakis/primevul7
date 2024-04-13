xstring& MirrorJob::FormatStatus(xstring& s,int v,const char *tab)
{
   if(Done())
      goto final;

   switch(state)
   {
   case(INITIAL_STATE):
   case(FINISHING):
   case(DONE):
   case(WAITING_FOR_TRANSFER):
   case(TARGET_REMOVE_OLD):
   case(TARGET_REMOVE_OLD_FIRST):
   case(TARGET_CHMOD):
   case(TARGET_MKDIR):
   case(SOURCE_REMOVING_SAME):
   case(LAST_EXEC):
      break;

   case(MAKE_TARGET_DIR):
      s.appendf("\tmkdir `%s' [%s]\n",target_dir.get(),target_session->CurrentStatus());
      break;

   case(CHANGING_DIR_SOURCE):
   case(CHANGING_DIR_TARGET):
      if(target_session->IsOpen())
	 s.appendf("\tcd `%s' [%s]\n",target_dir.get(),target_session->CurrentStatus());
      if(source_session->IsOpen())
	 s.appendf("\tcd `%s' [%s]\n",source_dir.get(),source_session->CurrentStatus());
      break;

   case(GETTING_LIST_INFO):
      if(target_list_info)
      {
	 if(target_relative_dir)
	    s.appendf("\t%s: %s\n",target_relative_dir.get(),target_list_info->Status());
	 else
	    s.appendf("\t%s\n",target_list_info->Status());
      }
      if(source_list_info)
      {
	 if(source_relative_dir)
	    s.appendf("\t%s: %s\n",source_relative_dir.get(),source_list_info->Status());
	 else
	    s.appendf("\t%s\n",source_list_info->Status());
      }
      break;
   }
   return s;

final:
   if(stats.dirs>0)
      s.appendf(plural("%sTotal: %d director$y|ies$, %d file$|s$, %d symlink$|s$\n",
		     stats.dirs,stats.tot_files,stats.tot_symlinks),
	 tab,stats.dirs,stats.tot_files,stats.tot_symlinks);
   if(stats.new_files || stats.new_symlinks)
      s.appendf(plural("%sNew: %d file$|s$, %d symlink$|s$\n",
		     stats.new_files,stats.new_symlinks),
	 tab,stats.new_files,stats.new_symlinks);
   if(stats.mod_files || stats.mod_symlinks)
      s.appendf(plural("%sModified: %d file$|s$, %d symlink$|s$\n",
		     stats.mod_files,stats.mod_symlinks),
	 tab,stats.mod_files,stats.mod_symlinks);
   if(stats.bytes)
      s.appendf("%s%s\n",tab,CopyJob::FormatBytesTimeRate(stats.bytes,transfer_time_elapsed));
   if(stats.del_dirs || stats.del_files || stats.del_symlinks)
      s.appendf(plural(FlagSet(DELETE) ?
	       "%sRemoved: %d director$y|ies$, %d file$|s$, %d symlink$|s$\n"
	      :"%sTo be removed: %d director$y|ies$, %d file$|s$, %d symlink$|s$\n",
	      stats.del_dirs,stats.del_files,stats.del_symlinks),
	 tab,stats.del_dirs,stats.del_files,stats.del_symlinks);
   if(stats.error_count)
      s.appendf(plural("%s%d error$|s$ detected\n",stats.error_count),
	       tab,stats.error_count);
   return s;
}