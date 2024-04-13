void MirrorJob::HandleListInfo(SMTaskRef<ListInfo>& list_info, Ref<FileSet>& set, Ref<FileSet> *fsx)
{
   if(!list_info)
      return;
   if(!list_info->Done())
      return;
   if(list_info->Error())
   {
      eprintf("mirror: %s\n",list_info->ErrorText());
      stats.error_count++;
      MirrorFinished();
      set_state(FINISHING);
      source_list_info=0;
      target_list_info=0;
      return;
   }
   set=list_info->GetResult();
   if(fsx)
      *fsx=list_info->GetExcluded();
   list_info=0;
   set->ExcludeDots(); // don't need .. and .
}