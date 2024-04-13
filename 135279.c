void MirrorJob::Statistics::Reset()
{
   tot_files=new_files=mod_files=del_files=
   tot_symlinks=new_symlinks=mod_symlinks=del_symlinks=
   dirs=del_dirs=0;
}