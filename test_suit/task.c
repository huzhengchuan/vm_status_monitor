#include <stdio.h>

char *fun()
{
	char *buf = (char *)malloc(100);
	strcpy(buf, "test111");
	return buf;
}
int main()
{
	char *buf = fun();
	printf("%s\r\n", buf);
	free(buf);
	buf = NULL;
}
