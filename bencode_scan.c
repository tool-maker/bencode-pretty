#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>

int curr;
int ret;

static int be_scan();
static void get_next_char();

int main(int argc, char *argv[]) {
  #if _WIN32
  _setmode(_fileno(stdin), _O_BINARY);
  _setmode(_fileno(stdout), _O_BINARY);
  //_setmode(_fileno(stderr), _O_BINARY);
  #endif
  get_next_char();
  return be_scan();
}

static void get_next_char() {
  curr = fgetc(stdin);
  #ifdef UNPRETTY
  while (!feof(stdin) && curr == '\n') {
    curr = fgetc(stdin);
  }
  #endif
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

static int be_scan_element();
static int be_scan_list();
static int be_scan_dict();
static int be_scan_int();
static int be_scan_str();

static int be_scan() {
  while (!feof(stdin)) {
    //fputs("<be_scan>", stdout);
    if ((ret = be_scan_element())) return ret;
  };
  return 0;
}

static int be_scan_element() {
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

static void put_get_next() {
  fputc(curr, stdout);
  get_next_char();
}

static int be_scan_list() {
  put_get_next();
  #ifdef PRETTIER
  fputc('\n', stdout);
  #endif
  while (!feof(stdin) && curr != 'e') {
    //fprintf(stdout, "<be_scan_list %c>", curr);
    if ((ret = be_scan_element())) return ret;
  };
  if ((ret = check_eof())) return ret;
  put_get_next();
  #ifdef PRETTIER
  fputc('\n', stdout);
  #endif
  return 0;
}

static int be_scan_dict() {
  put_get_next();
  #ifdef PRETTIER
  fputc('\n', stdout);
  #endif
  while (!feof(stdin) && curr != 'e') {
    if ((ret = be_scan_str())) return ret;
    if (((ret = be_scan_element()))) return ret;
  };
  if ((ret = check_eof())) return ret;
  put_get_next();
  #ifdef PRETTIER
  fputc('\n', stdout);
  #endif
  return 0;
}

static int be_scan_int() {
  put_get_next();
  while (!feof(stdin) && curr != 'e') {
    put_get_next();
  };
  if ((ret = check_eof())) return ret;
  put_get_next();
  #ifdef PRETTIER
  fputc('\n', stdout);
  #endif
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
      return internal_error();
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
    if(isprint(curr)) put_get_next();
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
  #ifdef PRETTIER
  fputc('\n', stdout);
  #endif
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
  fprintf(stdout, "%lli:", *len);
  while (head) {
    buff_out(head);
    head = head->next;
  };
  return 0;
}

#endif
