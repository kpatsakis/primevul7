double MirrorJob::GetTimeSpent()
{
   double t=transfer_time_elapsed;
   if(transfer_count>0)
      t+=now-root_mirror->transfer_start_ts;
   return t;
}