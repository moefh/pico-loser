/**
 * Get compilation date and time as BCD.
 *
 * These functions will be completely optimized to return compile-time
 * constants by gcc and clang.
 */

static inline unsigned int get_compilation_year(void)
{
  return (((__DATE__[ 7] - '0') << 12) |
          ((__DATE__[ 8] - '0') <<  8) |
          ((__DATE__[ 9] - '0') <<  4) |
          ((__DATE__[10] - '0') <<  0));
}

static inline unsigned int get_compilation_month(void)
{
  if (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n') return 0x01;
  if (__DATE__[0] == 'F' && __DATE__[1] == 'e' && __DATE__[2] == 'b') return 0x02;
  if (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r') return 0x03;
  if (__DATE__[0] == 'A' && __DATE__[1] == 'p' && __DATE__[2] == 'r') return 0x04;
  if (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y') return 0x05;
  if (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n') return 0x06;
  if (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l') return 0x07;
  if (__DATE__[0] == 'A' && __DATE__[1] == 'u' && __DATE__[2] == 'g') return 0x08;
  if (__DATE__[0] == 'S' && __DATE__[1] == 'e' && __DATE__[2] == 'p') return 0x09;
  if (__DATE__[0] == 'O' && __DATE__[1] == 'c' && __DATE__[2] == 't') return 0x10;
  if (__DATE__[0] == 'N' && __DATE__[1] == 'o' && __DATE__[2] == 'v') return 0x11;
  if (__DATE__[0] == 'D' && __DATE__[1] == 'e' && __DATE__[2] == 'c') return 0x12;
  return 0;
}

static inline unsigned int get_compilation_day(void)
{
  if (__DATE__[4] == ' ') return __DATE__[5] - '0';
  return ((__DATE__[4] - '0') << 4) | (__DATE__[5] - '0');
}

static inline unsigned int get_compilation_time(void)
{
  return (((__TIME__[0] - '0') << 20) |
          ((__TIME__[1] - '0') << 16) |
          ((__TIME__[3] - '0') << 12) |
          ((__TIME__[4] - '0') <<  8) |
          ((__TIME__[6] - '0') <<  4) |
          ((__TIME__[7] - '0') <<  0));
}

static inline int get_compilation_date(void)
{
  return ((get_compilation_year() << 16) |
          (get_compilation_month() << 8) |
          (get_compilation_day()   << 0));
}
