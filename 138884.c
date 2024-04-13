tor_version_supports_microdescriptors(const char *platform)
{
  return tor_version_as_new_as(platform, "0.2.3.1-alpha");
}