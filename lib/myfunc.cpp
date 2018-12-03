



//在指定常量字符串中，利用分隔符查找字符串
//比如 “123；456；789”，如果查找456，则separatorstr("123;456;789",';',1,dest)
:qint separatorstr(const char* src, char separator, int index, string &dest)
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
                LOG(INFO) << "FINISH , FOUND expected sector";
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
