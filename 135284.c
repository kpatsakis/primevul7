bool MirrorJob::Statistics::HaveSomethingDone(unsigned flags)
{
   bool del=(flags&MirrorJob::DELETE);
   return new_files|mod_files|(del_files*del)|new_symlinks|mod_symlinks|(del_symlinks*del)|(del_dirs*del);
}