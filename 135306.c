void MirrorJob::TransferStarted(CopyJob *cp)
{
   if(transfer_count==0)
      root_mirror->transfer_start_ts=now;
   JobStarted(cp);
}