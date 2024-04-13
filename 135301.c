void MirrorJob::Bg()
{
   source_session->SetPriority(0);
   target_session->SetPriority(0);
   Job::Bg();
}