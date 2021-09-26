
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static const char *skip_dir(const char *filename)
{
  char *slash = strrchr(filename, '/');
  if (slash) {
    return slash + 1;
  }
  return filename;
}

static void get_sample_name(char *dest, size_t dest_size, const char *src)
{
  // start after last '/' or start
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
  const char *in_filename = (argc < 2) ? NULL : argv[1];
  const char *out_filename = (argc < 3) ? NULL : argv[2];
  if (! in_filename || ! out_filename) {
    printf("USAGE: %s input.raw output.h\n", argv[0]);
    exit(1);
  }
  
  FILE *in = fopen(in_filename, "r");
  if (! in) {
    printf("%s: can't open input file %s\n", argv[0], in_filename);
    exit(1);
  }

  FILE *out = fopen(out_filename, "wb");
  if (! out) {
    printf("%s: can't open output file %s\n", argv[0], out_filename);
    fclose(in);
    exit(1);
  }

  char sample_name[1024];
  get_sample_name(sample_name, sizeof(sample_name), in_filename);

  printf("%s -> %s (%s)\n", in_filename, out_filename, sample_name);

  fprintf(out, "/* File generated automatically from %s */\n\n", skip_dir(in_filename));
  
  fprintf(out, "static const unsigned char snd_%s[] = {\n", sample_name);
  int n = 0;
  while (1) {
    int c = fgetc(in);
    if (c == EOF) break;
    if (n == 0) {
      fprintf(out, " ");
    }
    fprintf(out, " %3d,", c);
    if (++n == 16) {
      n = 0;
      fprintf(out, "\n");
    }
  }
  if (n) fprintf(out, "\n");
  fprintf(out, "};\n");
  
  fclose(out);
  fclose(in);
}
