void MirrorJob::TransferFinished(Job *j)
{
   long long bytes_count=j->GetBytesCount();
   AddBytesTransferred(bytes_count);
   stats.bytes+=bytes_count;
   stats.time +=j->GetTimeSpent();
   if(j->ExitCode()==0 && verbose_report>=2) {
      xstring finished;
      const xstring& cmd=j->GetCmdLine();
      if(cmd[0]=='\\')
	 finished.append(cmd+1,cmd.length()-1);
      else
	 finished.append(cmd);
      const xstring& rate=Speedometer::GetStrProper(j->GetTransferRate());
      if(rate.length()>0)
	 finished.append(" (").append(rate).append(')');
      if(!(FlagSet(SCAN_ALL_FIRST) && finished.begins_with("mirror")))
	 Report(_("Finished %s"),finished.get());
   }
   JobFinished(j);
   if(transfer_count==0)
      root_mirror->transfer_time_elapsed += now-root_mirror->transfer_start_ts;
}