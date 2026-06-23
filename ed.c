#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		return 1;
	}

	FILE* f = fopen(argv[1], "r");
	
	if (f == NULL) {
		fprintf(stderr, "could not open file \"%s\"\n", argv[1]);
		return 1;
	}

	char* line = malloc(256);
	char** lines = malloc(sizeof(char*));
	int lines_allocated = 1;
	int lines_used = 0;
	bool shouldrun = true;

	while (fgets(line, 256, f) != NULL) {
		lines_used++;

		if (lines_used > lines_allocated) {
			char** temp = realloc(lines, (lines_allocated * 2) * sizeof(char*));
			
			if (temp == NULL) {
				for (int i = 0; i < lines_used; i++) {
					free(lines[i]);
				}
				free(line);

				fputs("failed to allocate lines\n", stderr);
				return 1;
			}

			else {
				lines = temp;
				lines_allocated *= 2;
			}
		}

		lines[lines_used - 1] = strdup(line);
	}

	while (shouldrun) {
		fputs("[c] ", stdout);
		fgets(line, 256, stdin);

		if (line == NULL) {
			shouldrun = false;
			break;
		}

		int index = 0;

		while (line[index] == ' ') {
			index++;
		}

		switch (line[index]) {
			case 'q':
				shouldrun = false;
				break;
			case 'p':
				switch (line[index + 1]) {
					case '\n':
					case '\0':
					case ' ':
						for (int i = 0; i < lines_used; i++) {
							fputs(lines[i], stdout);
						}
						
						break;
					case 'n':
						for (int i = 0; i < lines_used; i++) {
							printf("%08i\u2502 %s", i, lines[i]);
						}
					
						break;
				}

				if (isdigit(line[index + 1])) {
					int userline = 0;

					sscanf(line + index + 1, "%i", &userline);

					if (userline > lines_used - 1) {
						printf("line %i is out of bounds\n", userline);
						break;
					}

					fputs(lines[userline], stdout);
				}
				
				break;
			default:
				printf("unknown operation '%c'\n", line[index]);
				break;
		}
	}

	for (int i = 0; i < lines_used; i++) {
		free(lines[i]);
	}

	free(lines);
	free(line);
	fclose(f);
}
