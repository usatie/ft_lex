#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
	char *src = "#include <stdio.h>\n"
		"#include <string.h>\n"
		"#include <stdlib.h>\n"
		"\n"
		"int main(void) {\n"
		"\tprintf(\"Hello, world!\\n\");\n"
		"\treturn 0;\n"
		"}\n";
	char *filename = "lex.yy.c";
	int fd = open(filename, O_CREAT | O_WRONLY, 0644);
	if (fd < 0) {
		perror("open");
		return 1;
	}
	size_t src_len = strlen(src);
	size_t written = 0;
	while (written < src_len) {
		ssize_t n = write(fd, src + written, src_len - written);
		if (n < 0) {
			perror("write");
			close(fd);
			return 1;
		}
		written += (size_t)n;
	}
	close(fd);
	return 0;
}
