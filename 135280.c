void MirrorJob::MirrorFinished()
{
   if(!parent_mirror)
      return;
   assert(transfer_count>=root_transfer_count);
   transfer_count-=root_transfer_count;
}