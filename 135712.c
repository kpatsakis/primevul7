static void setup_diff_attr_check(struct git_attr_check *check)
{
	static struct git_attr *attr_diff;

	if (!attr_diff) {
		attr_diff = git_attr("diff", 4);
	}
	check[0].attr = attr_diff;
}