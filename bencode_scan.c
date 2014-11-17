#ifdef PRETTIEST
#define PRETTIER
#endif
#ifdef PRETTIER
#define PRETTY
#endif

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>

int curr;
int ret;

static int be_scan();

int main(int argc, char *argv[]) {
  #if _WIN32
  _setmode(_fileno(stdin), _O_BINARY);
  _setmode(_fileno(stdout), _O_BINARY);
  //_setmode(_fileno(stderr), _O_BINARY);
  #endif
  return be_scan();
}

#define check_eof() _check_eof(__LINE__)
static int _check_eof(int line) {
  if (feof(stdin)) {
    fprintf(stderr, "line %i: unexpected EOF\n", line);
    return 1;
  }
  return 0;
}

#define internal_error() _internal_error(__LINE__)
static int _internal_error(int line) {
  fprintf(stderr, "line %i: internal error\n", line);
  return 2;
}

#define bad_char() _bad_char(__LINE__)
static int _bad_char(int line) {
  fprintf(stderr, "line %i: unexpected character %c\n", line, curr);
  return 1;
}

#define NEWLINE
#define INDENT_MORE
#define INDENT_LESS

#ifdef PRETTIER
#undef NEWLINE
#define INDENT

#ifdef PRETTIEST

#undef INDENT_MORE
#undef INDENT_LESS
#undef INDENT
#define INDENT_MORE indent_more();
#define INDENT_LESS indent_less();
#define INDENT indent();

static indent_level = 0;
static void indent_more() {
  indent_level++;
}
static void indent_less() {
  if (indent_level > 0) indent_level--;
  else internal_error();
}
static void indent() {
  int cnt;
  for (cnt = indent_level; cnt > 0; cnt--) fputc(' ', stdout);
}
#endif // PRETTIEST

#define NEWLINE new_line();
static char at_start = 1;
static void new_line() {
  if (!at_start) fputc('\n', stdout);
  at_start = 0;
  INDENT
}

#endif // PRETTIER

#define INDENT_SKIP indent_skip();
static void indent_skip() {
  #ifdef UNPRETTY
  while (!feof(stdin) && ( curr == ' ' || curr == '\n')) {
    curr = fgetc(stdin);
  }
  #endif
}

static void get_next_char() {
  curr = fgetc(stdin);
}

static int be_scan_element();

static int be_scan() {
  get_next_char();
  INDENT_SKIP
  while (!feof(stdin)) {
    //fputs("<be_scan>", stdout);
    if ((ret = be_scan_element())) return ret;
  };
  return 0;
}

static int be_scan_list();
static int be_scan_dict();
static int be_scan_int();
static int be_scan_str();

static int be_scan_element() {
  if (feof(stdin)) return 0;
  switch (curr) {
    case 'l':
      if ((ret = be_scan_list())) return ret;
      break;
    case 'd':
      if ((ret = be_scan_dict())) return ret;
      break;
    case 'i': 
      if ((ret = be_scan_int())) return ret;
      break;
    #ifdef UNPRETTY
    case '"':
    #endif
    #ifdef PRETTY
    case '0'...'9':
    #endif
      if ((ret = be_scan_str())) return ret;
      break;
    default:
      return bad_char();
  };
  return 0;
}

static void put_get_next_char() {
  fputc(curr, stdout);
  get_next_char();
}

static int be_scan_list() {
  NEWLINE
  INDENT_MORE
  put_get_next_char();
  INDENT_SKIP
  while (!feof(stdin) && curr != 'e') {
    //fprintf(stdout, "<be_scan_list %c>", curr);
    if ((ret = be_scan_element())) return ret;
    INDENT_SKIP
  };
  if ((ret = check_eof())) return ret;
  INDENT_LESS
  NEWLINE
  put_get_next_char();
  INDENT_SKIP
  return 0;
}

static int be_scan_dict() {
  NEWLINE
  INDENT_MORE
  put_get_next_char();
  INDENT_SKIP
  while (!feof(stdin) && curr != 'e') {
    if ((ret = be_scan_str())) return ret;
    INDENT_MORE
    if (((ret = be_scan_element()))) return ret;
    INDENT_LESS
    INDENT_SKIP
  };
  if ((ret = check_eof())) return ret;
  INDENT_LESS
  NEWLINE
  put_get_next_char();
  INDENT_SKIP
  return 0;
}

static int be_scan_int() {
  NEWLINE
  put_get_next_char();
  while (!feof(stdin) && curr != 'e') {
    put_get_next_char();
  };
  if ((ret = check_eof())) return ret;
  put_get_next_char();
  INDENT_SKIP
  return 0;
}

static int be_scan_str_by_len();
static int be_scan_str_for_mark();

static int be_scan_str() {
  switch (curr) {
    #ifdef PRETTY
    case '0'...'9':
      return be_scan_str_by_len();
    #endif
    #ifdef UNPRETTY
    case '"':
      return be_scan_str_for_mark();
    #endif
    default:
      return bad_char();
  };
}  

const int ESC = ',';
const int MARK = '"';

const int xdig_to[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
int xdig_from(int curr) {
  switch (curr) {
    default: return -1;
    case '0': return 0x0;
    case '1': return 0x1;
    case '2': return 0x2;
    case '3': return 0x3;
    case '4': return 0x4;
    case '5': return 0x5;
    case '6': return 0x6;
    case '7': return 0x7;
    case '8': return 0x8;
    case '9': return 0x9;
    case 'a': return 0xa;
    case 'b': return 0xb;
    case 'c': return 0xc;
    case 'd': return 0xd;
    case 'e': return 0xe;
    case 'f': return 0xf;
  }
}

#ifdef PRETTY
static int be_scan_str_by_len() {
  long long len = 0;
  char stop = 0;
  NEWLINE
  while (!feof(stdin) && !stop) {
    switch (curr) {
      case '0'...'9':
        len = 10 * len + (curr - '0');
        get_next_char();
        break;
      default:
        stop = 1;
        break;
    };
  };
  if ((ret = check_eof())) return ret;
  if (curr != ':') return bad_char();
  //fprintf(stdout, "<be_scan_str_by_len %lli>", len);
  fputc('"', stdout);
  get_next_char();
  while (!feof(stdin) && len > 0) {
    if (curr == ESC || curr == MARK) fputc(ESC, stdout);
    if(isprint(curr)) put_get_next_char();
    else {
      fputc(ESC, stdout);
      fputc(xdig_to[curr >> 4], stdout);
      fputc(xdig_to[curr & 0xf], stdout);
      get_next_char();
    }
    len--;
  }
  if ((ret = check_eof())) return ret;
  fputc(MARK, stdout);
  return 0;
}
#endif

#ifdef UNPRETTY

#define blen 2000
typedef struct buff buff;
struct buff {
  struct buff *next;
  long long len;
  int byte[blen];
};
static int be_scan_str_for_mark_buff(long long *, struct buff *, struct buff *);

static int be_scan_str_for_mark() {
  long long len = 0;
  get_next_char();
  if ((ret = check_eof())) return ret;
  return be_scan_str_for_mark_buff(&len, NULL, NULL);
}

static void buff_out(struct buff *b) {
  int i;
  for (i = 0; i < b->len; i++) fputc(b->byte[i], stdout);
}

static int be_scan_str_for_mark_buff(long long *len, struct buff *head, struct buff *tail) {
  struct buff b;
  b.next = NULL;
  b.len = 0;
  if (!head) head = &b;
  if (tail) tail->next = &b;
  tail = &b;
  while (!feof(stdin) && curr != MARK && b.len < blen) {
    if (curr == ESC) {
      get_next_char();
      if ((ret = check_eof())) return ret;
      if (curr != ESC && curr != MARK) {
        int xdig, outc;
        xdig = xdig_from(curr);
        if (xdig < 0) return bad_char();
        outc = xdig << 4;
        get_next_char();
        if ((ret = check_eof())) return ret;
        xdig = xdig_from(curr);
        if (xdig < 0) return bad_char();
        outc |= xdig;
        curr = outc;
      }
    };
    b.byte[b.len] = curr;
    (*len)++;
    b.len++;
    get_next_char();
  };
  if ((ret = check_eof())) return ret;
  if (curr != MARK) return be_scan_str_for_mark_buff(len, head, tail);
  get_next_char();
  INDENT_SKIP
  fprintf(stdout, "%lli:", *len);
  while (head) {
    buff_out(head);
    head = head->next;
  };
  return 0;
}

#endif
