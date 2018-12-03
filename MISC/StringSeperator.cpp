static int separatorstr(const char* src, char separator, int index, string &dest)
{
	if (src == NULL)
	{
		LOG(INFO) << "separatorstr IS NULL , ERROR";
		return -1;
	}
	const char *head, *tail;
	head = src;
	tail = src;
	int i = 0;

	while (tail != '\0')
	{
		if (*tail == separator){
			if (i == index){
				tail--;
				int len = (int)(tail - head) + 1;
				dest = std::string(head, len);
				return 0;
			}
			else{
				head = ++tail;
				i++;
				continue;
			}
		}
		tail++;
	}
	return -1;
}
