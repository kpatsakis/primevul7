void MirrorJob::JobStarted(Job *j)
{
   AddWaiting(j);
   transfer_count++;
}