void MirrorJob::JobFinished(Job *j)
{
   if(j->ExitCode()!=0)
      stats.error_count++;
   RemoveWaiting(j);
   Delete(j);
   assert(transfer_count>0);
   transfer_count--;
}