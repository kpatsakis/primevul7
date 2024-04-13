void MirrorJob::Statistics::Add(const Statistics &s)
{
   tot_files   +=s.tot_files;
   new_files   +=s.new_files;
   mod_files   +=s.mod_files;
   del_files   +=s.del_files;
   tot_symlinks+=s.tot_symlinks;
   new_symlinks+=s.new_symlinks;
   mod_symlinks+=s.mod_symlinks;
   del_symlinks+=s.del_symlinks;
   dirs        +=s.dirs;
   del_dirs    +=s.del_dirs;
   error_count +=s.error_count;
   bytes       +=s.bytes;
   time	       +=s.time;
}