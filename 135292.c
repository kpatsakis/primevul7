void MirrorJob::Fg()
{
   Job::Fg();
   source_session->SetPriority(1);
   target_session->SetPriority(1);
}