void MirrorJob::MirrorStarted()
{
   if(!parent_mirror)
      return;
   transfer_count+=root_transfer_count;
}