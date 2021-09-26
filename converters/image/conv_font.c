/* conv_font.c */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bmp.h"

struct INFO {
  const char *progname;
  char *in_filename;
  char out_filename[1024];
  char var_name[1024];
  unsigned int num_chars_x;
  unsigned int num_chars_y;
  int output_crlf;
};

struct IMAGE_READER {
  struct BMP_IMAGE *bmp;
  unsigned int w;
  unsigned int h;
  unsigned int num_chars_x;
  unsigned int num_chars_y;
};

static unsigned char *reader_get_image_line(struct IMAGE_READER *reader, int char_x, int char_y, int y)
{
  int start_x = char_x * reader->w;
  int start_y = char_y * reader->h + y;
  return &reader->bmp->data[start_y * reader->bmp->stride + start_x*(reader->bmp->bpp/8)];
}

static const char *skip_dir(const char *filename)
{
  char *slash = strrchr(filename, '/');
  if (slash) {
    return slash + 1;
  }
  return filename;
}

static void conv_font_char(struct IMAGE_READER *reader, unsigned char *out, int char_x, int char_y)
{
  int bpp = reader->bmp->bpp / 8;
  for (int y = 0; y < reader->h; y++) {
    const unsigned char *line = reader_get_image_line(reader, char_x, char_y, y);
    unsigned char line_data = 0;
    for (int x = 0; x < reader->w; x++) {
      if (*line == 0) {
        line_data |= 1 << x;  // set font pixel if input image is black
      }
      line += bpp;
    }
    *out++ = line_data;
  }
}

static int conv_file(struct INFO *info)
{
  struct IMAGE_READER reader;
  reader.bmp = bmp_read(info->in_filename, BMP_FORMAT_BGRA);
  if (! reader.bmp) {
    printf("ERROR: can't read '%s'\n", info->in_filename);
    return 1;
  }
  reader.w = reader.bmp->w / info->num_chars_x;
  reader.h = reader.bmp->h / info->num_chars_y;
  reader.num_chars_x = info->num_chars_x;
  reader.num_chars_y = info->num_chars_y;

  unsigned char out_char[32];
  if (sizeof(out_char) < reader.h) {
    printf("ERROR: font height too large!\n");
    return 1;
  }

  int first_char = 32;
  int num_chars = info->num_chars_x*info->num_chars_y;
   
  FILE *out = fopen(info->out_filename, "wb");
  if (out == NULL) {
    bmp_free(reader.bmp);
    printf("ERROR: can't open '%s'\n", info->out_filename);
    return 1;
  }

  int width = reader.w;
  if (width % 4 != 0) {
    width += 4 - width % 4;
  }

  const char *line_end = (info->output_crlf) ? "\r\n" : "\n";
  
  printf("%s -> %s (name=%s)\n", info->in_filename, info->out_filename, info->var_name);

  fprintf(out, "/* File generated automatically from %s */%s%s", skip_dir(info->in_filename), line_end, line_end);
  fprintf(out, "#define %s_width %d%s", info->var_name, reader.w, line_end);
  fprintf(out, "#define %s_height %d%s", info->var_name, reader.h, line_end);
  fprintf(out, "#define %s_first_char %d%s", info->var_name, first_char, line_end);
  fprintf(out, "#define %s_num_chars %d%s%s", info->var_name, num_chars, line_end, line_end);

  fprintf(out, "const unsigned char %s_data[] = {%s", info->var_name, line_end);
  for (int ch = 0; ch < num_chars; ch++) {
    int char_x = ch % info->num_chars_x;
    int char_y = ch / info->num_chars_x;
    conv_font_char(&reader, out_char, char_x, char_y);
    fprintf(out, "  ");
    for (int i = 0; i < reader.h; i++) {
      fprintf(out, "0x%02x,", (unsigned int) out_char[i]);
    }
    fprintf(out, "%s", line_end);
  }
  fprintf(out, "};%s", line_end);
  
  fclose(out);
  bmp_free(reader.bmp);
  return 0;
}

static void print_help(const char *progname)
{
  printf("USAGE: %s [options] INPUT.spr\n", progname);
  printf("\n");
  printf("options:\n");
  printf("   -h               show this help\n");
  printf("   -out FILE        set output file (default: based on input file)\n");
  printf("   -name NAME       set C variable name (default: based on input file)\n");
  printf("   -num-chars-x     the number of horizontal characters in the input file\n");
  printf("   -num-chars-y     the number of vertical characters in the input file\n");
  printf("   -crlf            use CR/LF line endings on output file\n");
}

static int parse_num_tiles(struct INFO *info, int coord, const char *str)
{
  char *end = NULL;
  unsigned long num_tiles = strtoul(str, &end, 0);
  if (end == NULL || end == str || *end != '\0') {
    printf("ERROR: invalid num tiles: must be a number\n");
    return 1;
  }
  if (coord == 0) {
    info->num_chars_x = (unsigned int) num_tiles;
  } else {
    info->num_chars_y = (unsigned int) num_tiles;
  }
  return 0;
}

static int make_default_out_filename(struct INFO *info, const char *prefix, const char *ext)
{
  size_t out_pos = 0;

  // copy directory
  const char *in_filename = info->in_filename;
  const char *last_slash = strrchr(in_filename, '/');
  if (last_slash != NULL) {
    size_t copy_len = last_slash - in_filename + 1;   // from start to last slash (including slash)
    if (out_pos + copy_len > sizeof(info->out_filename)) {
      printf("%s: input filename too long\n", info->progname);
      return 1;
    }
    memcpy(&info->out_filename[out_pos], in_filename, copy_len);
    out_pos += copy_len;
    in_filename += copy_len;
  }

  // add prefix
  size_t prefix_len = strlen(prefix);
  if (out_pos + prefix_len > sizeof(info->out_filename)) {
    printf("%s: input filename too long\n", info->progname);
    return 1;
  }
  memcpy(&info->out_filename[out_pos], prefix, prefix_len);
  out_pos += prefix_len;
  
  // copy file name without extension
  const char *last_dot = strrchr(in_filename, '.');
  size_t copy_len = (last_dot == NULL) ? strlen(in_filename) : last_dot - in_filename;
  if (out_pos + copy_len > sizeof(info->out_filename)) {
    return 1;
  }
  memcpy(&info->out_filename[out_pos], in_filename, copy_len);
  out_pos += copy_len;

  // copy new extension and trailing '\0'
  size_t ext_len = strlen(ext) + 1;
  if (out_pos + ext_len > sizeof(info->out_filename)) {
    return 1;
  }
  memcpy(&info->out_filename[out_pos], ext, ext_len);
  return 0;
}

static int make_default_var_name(struct INFO *info)
{
  const char *last_slash = strrchr(info->in_filename, '/');
  if (last_slash == NULL) {
    last_slash = info->in_filename;
  } else {
    last_slash++;  // skip slash
  }

  const char *last_dot = strrchr(last_slash, '.');
  if (last_dot == NULL) {
    last_dot = last_slash + strlen(last_slash);
  }

  size_t len = last_dot - last_slash;
  if (len + 1 > sizeof(info->var_name)) {
    return 1;
  }
  const char *in = last_slash;
  char *out = info->var_name;
  while (in != last_dot) {
    char c = *in++;
    if ((c < 'A' || c > 'Z') && (c < 'a' || c > 'z') && (c < '0' || c > '9') && (c != '_')) {
      c = '_';
    }
    *out++ = c;
  }
  *out++ = '\0';
  return 0;
}

static int argument_missing(const char *progname, const char *option)
{
  printf("%s: argument missing for option '%s'\n", progname, option);
  return 1;
}

static int parse_args(struct INFO *info, int argc, char *argv[])
{
  info->progname = argv[0];
  info->in_filename = NULL;
  info->out_filename[0] = '\0';
  info->var_name[0] = '\0';
  info->num_chars_x = 16;
  info->num_chars_y = 6;
  info->output_crlf = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (strcmp(argv[i], "-h") == 0) {
        print_help(argv[0]);
        exit(0);
      } else if (strcmp(argv[i], "-out") == 0) {
        if (i+1 >= argc) {
          return argument_missing(info->progname, argv[i]);
        }
        snprintf(info->out_filename, sizeof(info->out_filename), "%s", argv[++i]);
      } else if (strcmp(argv[i], "-name") == 0) {
        if (i+1 >= argc) {
          return argument_missing(info->progname, argv[i]);
        }
        snprintf(info->var_name, sizeof(info->var_name), "%s", argv[++i]);
      } else if (strcmp(argv[i], "-num-chars-x") == 0) {
        if (i+1 >= argc) {
          return argument_missing(info->progname, argv[i]);
        }
        if (parse_num_tiles(info, 0, argv[++i]) != 0) {
          return 1;
        }
      } else if (strcmp(argv[i], "-num-chars-y") == 0) {
        if (i+1 >= argc) {
          return argument_missing(info->progname, argv[i]);
        }
        if (parse_num_tiles(info, 1, argv[++i]) != 0) {
          return 1;
        }
      } else if (strcmp(argv[i], "-crlf") == 0) {
        info->output_crlf = 1;
      } else {
        printf("%s: unknown option: '%s'\n", info->progname, argv[i]);
        return 1;
      }
    } else {
      if (info->in_filename != NULL) {
        printf("%s: only one input file supported\n", info->progname);
        return 1;
      }
      info->in_filename = argv[i];
    }
  }

  if (! info->in_filename) {
    printf("%s: no input file!\n", argv[0]);
    return 1;
  }
  
  if (info->out_filename[0] == '\0') {
    if (make_default_out_filename(info, "spr_", ".h") != 0) {
      printf("%s: input filename too long\n", info->progname);
      return 1;
    }
  }
  if (info->var_name[0] == '\0') {
    if (make_default_var_name(info) != 0) {
      printf("%s: input filename too long\n", info->progname);
      return 1;
    }
  }
  return 0;
}

int main(int argc, char *argv[])
{
  static struct INFO info;
  if (parse_args(&info, argc, argv) != 0) {
    return 1;
  }

  return conv_file(&info);
}
