off_t MirrorJob::GetBytesCount()
{
   long long bytes_count=Job::GetBytesCount();
   if(!parent_mirror) {
      // bytes_transferred is cumulative over the mirror tree,
      // add it on the top only
      bytes_count+=bytes_transferred;
   }
   return bytes_count;
}