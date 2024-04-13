void CountOriginType(const std::set<GURL>& origins,
                     SpecialStoragePolicy* policy,
                     size_t* protected_origins,
                     size_t* unlimited_origins) {
  DCHECK(protected_origins);
  DCHECK(unlimited_origins);
  *protected_origins = 0;
  *unlimited_origins = 0;
  if (!policy)
    return;
  for (std::set<GURL>::const_iterator itr = origins.begin();
       itr != origins.end();
       ++itr) {
    if (policy->IsStorageProtected(*itr))
      ++*protected_origins;
    if (policy->IsStorageUnlimited(*itr))
      ++*unlimited_origins;
  }
}
