
int
main(int argc, char *argv[])
{
	int fd, len;
	struct sock_opts *ptr;
	fd = Socket(AF_INET, SOCK_STREAM, 0);
	for(ptr = sock_opts; ptr->opt_str != NULL, ptr++) {
		printf("%s:", ptr->opt_str);
		if(ptr->opt_val_str == NULL)
			printf("(undefined)\n");
		else {
			len = sizeof(val);
			if(getsockopt(fd, ptr->opt_level, ptr->opt_name,
						&val, &len) == -1) {
				err_ret("getsockopt error");
			} else {
				printf("default = %s\n", (*ptr->opt_val_str)(&val, len));
			}
		}
	}
	exit(0);
}
