xstring& MirrorJob::FormatShortStatus(xstring& s)
{
   if(bytes_to_transfer>0 && (!parent_mirror || parent_mirror->bytes_to_transfer!=bytes_to_transfer)) {
      long long curr_bytes_transferred=GetBytesCount();
      if(parent_mirror)
         curr_bytes_transferred+=bytes_transferred;
      s.appendf("%s/%s (%d%%)",
	 xhuman(curr_bytes_transferred),xhuman(bytes_to_transfer),
	 percent(curr_bytes_transferred,bytes_to_transfer));
      double rate=GetTransferRate();
      if(rate>=1)
	 s.append(' ').append(Speedometer::GetStrProper(rate));
   }
   return s;
}