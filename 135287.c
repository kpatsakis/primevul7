void  MirrorJob::InitSets()
{
   if(FlagSet(TARGET_FLAT) && !parent_mirror && target_set)
      source_set->Sort(FileSet::BYNAME_FLAT);

   source_set->Count(NULL,&stats.tot_files,&stats.tot_symlinks,&stats.tot_files);

   to_rm=new FileSet(target_set);
   to_rm->SubtractAny(source_set);

   if(FlagSet(DELETE_EXCLUDED) && target_set_excluded)
      to_rm->Merge(target_set_excluded);

   to_transfer=new FileSet(source_set);

   if(!FlagSet(TRANSFER_ALL)) {
      same=new FileSet(source_set);

      int ignore=0;
      if(FlagSet(ONLY_NEWER))
	 ignore|=FileInfo::IGNORE_SIZE_IF_OLDER|FileInfo::IGNORE_DATE_IF_OLDER;
      if(!FlagSet(UPLOAD_OLDER) && strcmp(target_session->GetProto(),"file"))
	 ignore|=FileInfo::IGNORE_DATE_IF_OLDER;
      if(FlagSet(IGNORE_TIME))
	 ignore|=FileInfo::DATE;
      if(FlagSet(IGNORE_SIZE))
	 ignore|=FileInfo::SIZE;
      to_transfer->SubtractSame(target_set,ignore);

      same->SubtractAny(to_transfer);
   }

   if(newer_than!=NO_DATE)
      to_transfer->SubtractNotNewerThan(newer_than);
   if(older_than!=NO_DATE)
      to_transfer->SubtractNotOlderThan(older_than);
   if(size_range)
      to_transfer->SubtractSizeOutside(size_range);

   if(FlagSet(SCAN_ALL_FIRST)) {
      to_mkdir=new FileSet(to_transfer);
      to_mkdir->SubtractNotDirs();
      to_mkdir->SubtractAny(target_set);
   }

   switch(recursion_mode) {
   case RECURSION_NEVER:
      to_transfer->SubtractDirs();
      break;
   case RECURSION_MISSING:
      to_transfer->SubtractDirs(target_set);
      break;
   case RECURSION_NEWER:
      to_transfer->SubtractNotOlderDirs(target_set);
      break;
   case RECURSION_ALWAYS:
      break;
   }

   if(skip_noaccess)
      to_transfer->ExcludeUnaccessible(source_session->GetUser());

   new_files_set=new FileSet(to_transfer);
   new_files_set->SubtractAny(target_set);
   old_files_set=new FileSet(target_set);
   old_files_set->SubtractNotIn(to_transfer);

   to_rm_mismatched=new FileSet(old_files_set);
   to_rm_mismatched->SubtractSameType(to_transfer);
   to_rm_mismatched->SubtractNotDirs();

   if(!FlagSet(DELETE))
      to_transfer->SubtractAny(to_rm_mismatched);

   if(FlagSet(TARGET_FLAT) && !parent_mirror && target_set) {
      source_set->Unsort();
      to_transfer->UnsortFlat();
      to_transfer->SubtractDirs();
      same->UnsortFlat();
      to_mkdir->Empty();
      new_files_set->UnsortFlat();
   }

   const char *sort_by=ResMgr::Query("mirror:sort-by",0);
   bool desc=strstr(sort_by,"-desc");
   if(!strncmp(sort_by,"name",4))
      to_transfer->SortByPatternList(ResMgr::Query("mirror:order",0));
   else if(!strncmp(sort_by,"date",4))
      to_transfer->Sort(FileSet::BYDATE);
   else if(!strncmp(sort_by,"size",4))
      to_transfer->Sort(FileSet::BYSIZE,false,true);
   if(desc)
      to_transfer->ReverseSort();

   int dir_count=0;
   if(to_mkdir) {
      to_mkdir->Count(&dir_count,NULL,NULL,NULL);
      only_dirs = (dir_count==to_mkdir->count());
   } else {
      to_transfer->Count(&dir_count,NULL,NULL,NULL);
      only_dirs = (dir_count==to_transfer->count());
   }
}