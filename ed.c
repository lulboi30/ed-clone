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
   int current_line = 0;
   bool shouldrun = true;

   char* lformat = malloc(16); // just in case
   strcpy(lformat, "%-8i| %s");

   char* prompt = strdup("-> ");

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
      fputs(prompt, stdout);
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
         case 'q': // quit
            shouldrun = false;
            break;
         case 'p': // print
            switch (line[index + 1]) {
               case '\n': // all lines #1
               case '\0': // all lines #2
               case ' ':  // all lines #3
                  for (int i = 0; i < lines_used; i++) {
                     fputs(lines[i], stdout);
                  }
                  
                  break;
               case 'n': // all lines, numbered
                  for (int i = 0; i < lines_used; i++) {
                     // 0s looked ugly
                     // now you can customize it
                     printf(lformat, i, lines[i]);
                  }
               
                  break;
               case 'c': // current line
                  fputs(lines[current_line], stdout);
                  break;
            }

            // specific line
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
         case 's': // set option
            switch (line[index + 1]) {
               case 'e': // editor
                  switch (line[index + 2]) {
                     case 'p': // prompt
                        free(prompt);
                        prompt = strdup(line + index + 3);
                        prompt[strcspn(prompt, "\n")] = '\0';
                        break;
                  }
                  break;
               case 'l': // line
                  switch (line[index + 2]) {
                     case 'n': // number
                        int userline = 0;
                        sscanf(line + index + 3, "%i", &userline);

                        if (userline > lines_used - 1) {
                           printf("line %i is out of bounds\n", userline);
                        } else {
                           current_line = userline;
                        }
                                                
                        break;
                     case 'f': // format
                        memset(lformat, 0, 16);

                        switch (line[index + 3]) {
                           case 'l': // left aligned
                              strcat(lformat, "%-8i");
                              break;
                           case 'r': // right aligned
                              strcat(lformat, "%8i");
                              break;
                           case '0': // 0 aligned
                              strcat(lformat, "%08i");
                              break;
                        }

                        char last[5];
                        last[0] = line[index + 4];
                        last[1] = ' ';
                        last[2] = '%';
                        last[3] = 's';
                        last[4] = '\0';

                        strcat(lformat, last);
                        
                        break;
                  }
                  
                  break;
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
