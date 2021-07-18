
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void get_sample_name(char *dest, size_t dest_size, const char *src)
{
  // start aftr last '/' or start
  const char *start = src + strlen(src);
  while (start > src && *start != '/')
    start--;
  if (*start == '/') start++;

  char *d = dest;
  for (size_t i = 0; i < dest_size-1; i++) {
    char c = start[i];
    if (c == '.' || c == '\0') break;
    if (isalnum(c) || c == '_')
      *d++ = c;
    else
      *d++ = '_';
  }
  *d = '\0';
}

int main(int argc, char *argv[])
{
  const char *filename = argv[1];
  if (! filename) {
    printf("USAGE: %s filename\n", argv[0]);
    exit(1);
  }
  
  FILE *f = fopen(filename, "r");
  if (! f) {
    printf("%s: can't open %s\n", argv[0], filename);
    exit(1);
  }

  char sample_name[1024];
  get_sample_name(sample_name, sizeof(sample_name), filename);
  
  printf("static const unsigned char snd_%s[] = {\n", sample_name);
  int n = 0;
  while (1) {
    int c = fgetc(f);
    if (c == EOF) break;
    if (n == 0) {
      printf(" ");
    }
    printf(" %3d,", c);
    if (++n == 16) {
      n = 0;
      printf("\n");
    }
  }
  if (n) printf("\n");
  printf("};\n");
  
  fclose(f);
}
