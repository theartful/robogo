/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * This is GNU Go, a Go program. Contact gnugo@gnu.org, or see       *
 * http://www.gnu.org/software/gnugo/ for more information.          *
 *                                                                   *
 * Copyright 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,   *
 * 2008 and 2009 by the Free Software Foundation.                    *
 *                                                                   *
 * This program is free software; you can redistribute it and/or     *
 * modify it under the terms of the GNU General Public License as    *
 * published by the Free Software Foundation - version 3 or          *
 * (at your option) any later version.                               *
 *                                                                   *
 * This program is distributed in the hope that it will be useful,   *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the     *
 * GNU General Public License in file COPYING for more details.      *
 *                                                                   *
 * You should have received a copy of the GNU General Public         *
 * License along with this program; if not, write to the Free        *
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,       *
 * Boston, MA 02111, USA.                                            *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*  Parts of this code were given to us by Tommy Thorn */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "SGFinterface.h"

#define TIME_WITH_SYS_TIME 0
#define HAVE_SYS_TIME_H 0
#define HAVE_G_VSNPRINTF 0
#define HAVE__VSNPRINTF 0


#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <stdarg.h> 
#include "sgftree.h"
//#include "gg_utils.h"

#define STRICT_SGF 's'
#define LAX_SGF    'l'

/* Set this to 1 if you want warnings for missing GM and FF properties. */
#define VERBOSE_WARNINGS 0

/* ================================================================ */
/*                     Some utility functions.                      */
/* ================================================================ */

/*
 * Utility: a checking, initializing malloc
 */



#define UNUSED(x)  (void)x
void
gg_vsnprintf(char *dest, unsigned long len, const char *fmt, va_list args)
{
    
#ifdef HAVE_VSNPRINTF
  vsnprintf(dest, len, fmt, args);
#elif HAVE_G_VSNPRINTF
  g_vsnprintf(dest, len, fmt, args);
#elif HAVE__VSNPRINTF
  _vsnprintf(dest, len, fmt, args);
#else
  UNUSED(len);
  vsprintf(dest, fmt, args);
#endif


}

void
gg_snprintf(char *dest, unsigned long len, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  gg_vsnprintf(dest, len, fmt, args);
  va_end(args);
}




void *
xalloc(unsigned int size)
{
  void *pt = malloc(size);

  if (!pt) {
    fprintf(stderr, "xalloc: Out of memory!\n");
    exit(EXIT_FAILURE);
  }

  memset(pt, 0, static_cast<unsigned long> (size));
  return pt;
}

void *
xrealloc(void *pt, unsigned int size)
{
  void *ptnew = realloc(pt, size);

  if (!ptnew) {
    fprintf(stderr, "xrealloc: Out of memory!\n");
    exit(EXIT_FAILURE);
  }
  return ptnew;
}


/* ================================================================ */
/*                           SGF Nodes                              */
/* ================================================================ */


/*
 * Allocate memory for a new SGF node.
 */

SGFNode *
sgfNewNode()
{
  SGFNode *newnode;
  newnode = new SGFNode();//xalloc(sizeof(SGFNode));
  newnode->next = NULL;
  newnode->props = NULL;
  newnode->parent = NULL;
  newnode->child = NULL;
  return newnode;
}

/*
 * Recursively free an sgf node
 */

void
sgfFreeNode(SGFNode *node)
{
  if (node == NULL)
    return;
  sgfFreeNode(node->next);
  sgfFreeNode(node->child);
  sgfFreeProperty(node->props);
  free(node);
}


/*
 * Add a generic text property to an SGF node.
 */

void
sgfAddProperty(SGFNode *node, const char *name, const char *value)
{
  SGFProperty *prop = node->props;

  if (prop)
    while (prop->next)
      prop = prop->next;

  sgfMkProperty(name, value, node, prop);
}


/*
 * Add an integer property to an SGF node.
 */

void
sgfAddPropertyInt(SGFNode *node, const char *name, long val)
{
  char buffer[10];

  gg_snprintf(buffer, 10, "%ld", val);
  sgfAddProperty(node, name, buffer);
}


/*
 * Add a float property to an SGF node.
 */

void
sgfAddPropertyFloat(SGFNode *node, const char *name, float val)
{
  char buffer[10];

  gg_snprintf(buffer, 10, "%3.1f", val);
  sgfAddProperty(node, name, buffer);
}


/*
 * Read a property as int from an SGF node.
 */

int
sgfGetIntProperty(SGFNode *node, const char *name, int *value)
{
  SGFProperty *prop;
  short nam = name[0] | name[1] << 8;

  for (prop = node->props; prop; prop = prop->next)
    if (prop->name == nam) {
      *value = atoi(prop->value);
      return 1;
    }

  return 0;
}


/*
 * Read a property as float from an SGF node.
 */

int
sgfGetFloatProperty(SGFNode *node, const char *name, float *value)
{
  SGFProperty *prop;
  short nam = name[0] | name[1] << 8;

  for (prop = node->props; prop; prop = prop->next)
    if (prop->name == nam) {
      *value = static_cast<float>(atof(prop->value));
      /* MS-C warns of loss of data (double to float) */
      return 1;
    }

  return 0;
}


/*
 * Read a property as text from an SGF node.
 */

int
sgfGetCharProperty(SGFNode *node, const char *name, char **value)
{
  SGFProperty *prop;
  short nam = name[0] | name[1] << 8;

  for (prop = node->props; prop; prop = prop->next)
    if (prop->name == nam) {
      *value = prop->value;
      return 1;
    }

  return 0;
}


/*
 * Is there a property of this type in the node?
 */

static int
sgfHasProperty(SGFNode *node, const char *name)
{
  SGFProperty *prop;
  short nam = name[0] | name[1] << 8;

  for (prop = node->props; prop; prop = prop->next)
    if (prop->name == nam)
      return 1;

  return 0;
}


/*
 * Overwrite a property from an SGF node with text or create a new
 * one if it does not exist.
 */

void
sgfOverwriteProperty(SGFNode *node, const char *name, const char *text)
{
  SGFProperty *prop;
  short nam = name[0] | name[1] << 8;

  for (prop = node->props; prop; prop = prop->next)
    if (prop->name == nam) {
      prop->value = new char[strlen(text)+1];
      strcpy(prop->value, text);
      return;
    }

  sgfAddProperty(node, name, text);
}


/*
 * Overwrite an int property in an SGF node with val or create a new 
 * one if it does not exist.
 */

void
sgfOverwritePropertyInt(SGFNode *node, const char *name, int val)
{
  SGFProperty *prop;
  short nam = name[0] | name[1] << 8;

  for (prop = node->props; prop; prop = prop->next)
    if (prop->name == nam) {
      prop->value = new char(12);//xrealloc(prop->value, 12);
      gg_snprintf(prop->value, 12, "%d", val);
      return;
   }

  sgfAddPropertyInt(node, name, val);
}


/* 
 * Overwrite a float property in the gametree with val or create
 * a new one if it does not exist.
 */

void
sgfOverwritePropertyFloat(SGFNode *node, const char *name, float val)
{
  SGFProperty *prop;
  short nam = name[0] | name[1] << 8;

  for (prop = node->props; prop; prop = prop->next)
    if (prop->name == nam) {
      prop->value = new char(15);//xrealloc(prop->value, 15);
      gg_snprintf(prop->value, 15, "%3.1f", val);
      return;
    }

  sgfAddPropertyFloat(node, name, val);
}


/*
 * Goto previous node.
 */

SGFNode *
sgfPrev(SGFNode *node)
{
  SGFNode *q;
  SGFNode *prev;

  if (!node->parent)
    return NULL;

  q = node->parent->child;
  prev = NULL;
  while (q && q != node) {
    prev = q;
    q = q->next;
  }

  return prev;
}


/*
 * Goto root node.
 */

SGFNode *
sgfRoot(SGFNode *node)
{
  while (node->parent)
    node = node->parent;

  return node;
}


/* ================================================================ */
/*                         SGF Properties                           */
/* ================================================================ */


/*
 * Make an SGF property.
 */
static SGFProperty *
do_sgf_make_property(short sgf_name,  const char *value,
		     SGFNode *node, SGFProperty *last)
{
  SGFProperty *prop;

  prop = static_cast<SGFProperty *> (xalloc(sizeof(SGFProperty)));
  prop->name = sgf_name;
  prop->value = new char(strlen(value) + 1);//xalloc(strlen(value) + 1);
  strcpy(prop->value, value);
  prop->next = NULL;

  if (last == NULL)
    node->props = prop;
  else
    last->next = prop;

  return prop;
}


/* Make an SGF property.  In case of a property with a range it
 * expands it and makes several properties instead.
 */
SGFProperty *
sgfMkProperty(const char *name, const  char *value,
	      SGFNode *node, SGFProperty *last)
{
  static const short properties_allowing_ranges[12] = {
    /* Board setup properties. */
    SGFAB, SGFAW, SGFAE,

    /* Markup properties. */
    SGFCR, SGFMA, SGFSQ, SGFTR, SGFDD, SGFSL,

    /* Miscellaneous properties. */
    SGFVW,

    /* Go-specific properties. */
    SGFTB, SGFTW
  };

  int k;
  short sgf_name;

  if (strlen(name) == 1)
    sgf_name = name[0] | static_cast<short> (' ' << 8);
  else
    sgf_name = name[0] | name[1] << 8;

  for (k = 0; k < 12; k++) {
    if (properties_allowing_ranges[k] == sgf_name)
      break;
  }

  if (k < 12
      && strlen(value) == 5
      && value[2] == ':') {
    char x1 = value[0];
    char y1 = value[1];
    char x2 = value[3];
    char y2 = value[4];
    char new_value[] = "xy";

    if (x1 <= x2 && y1 <= y2) {
      for (new_value[0] = x1; new_value[0] <= x2; new_value[0]++) {
	for (new_value[1] = y1; new_value[1] <= y2; new_value[1]++)
	  last = do_sgf_make_property(sgf_name, new_value, node, last);
      }

      return last;
    }
  }

  /* Not a range property. */
  return do_sgf_make_property(sgf_name, value, node, last);
}


/*
 * Recursively free an SGF property.
 *
 */

void
sgfFreeProperty(SGFProperty *prop)
{
  if (prop == NULL)
    return;
  sgfFreeProperty(prop->next);
  free(prop->value);
  free(prop);
}


/* ================================================================ */
/*                        High level functions                      */
/* ================================================================ */


/*
 * Add a stone to the current or the given node.
 * Return the node where the stone was added.
 */

SGFNode *
sgfAddStone(SGFNode *node, int color, int movex, int movey)
{
  char move[3];

  sprintf(move, "%c%c", movey + 'a', movex + 'a');
  sgfAddProperty(node, (color == 2) ? "AB" : "AW", move);

  return node;
}


/*
 * Add a move to the gametree.
 */

SGFNode *
sgfAddPlay(SGFNode *node, int who, int movex, int movey)
{
  char move[3];
  SGFNode *new_node;
  
  /* a pass move? */
  if (movex == -1 && movey == -1)
    move[0] = 0;
  else
    sprintf(move, "%c%c", movey + 'a', movex + 'a');

  if (node->child)
    new_node = sgfStartVariantFirst(node->child);
  else {
    new_node = sgfNewNode();
    node->child = new_node;
    new_node->parent = node;
  }
  
  sgfAddProperty(new_node, (who == 2) ? "B" : "W", move);

  return new_node;
}


/*
 * Add a move to the gametree. New variations are added after the old
 * ones rather than before.
 */

SGFNode *
sgfAddPlayLast(SGFNode *node, int who, int movex, int movey)
{
  char move[3];
  SGFNode *new_node;

  /* a pass move? */
  if (movex == -1 && movey == -1)
    move[0] = 0;
  else
    sprintf(move, "%c%c", movey + 'a', movex + 'a');

  new_node = sgfAddChild(node);
  sgfAddProperty(new_node, (who == 2) ? "B" : "W", move);

  return new_node;
}


SGFNode *
sgfCreateHeaderNode(int boardsize, float komi, int handicap)
{
    SGFNode *root = sgfNewNode();

    sgfAddPropertyInt(root, "SZ", boardsize);
    sgfAddPropertyFloat(root, "KM", komi);
    sgfAddPropertyInt(root, "HA", handicap);

    return root;
}


/*
 * Add a comment to an SGF node.
 */

SGFNode *
sgfAddComment(SGFNode *node, const char *comment)
{
  sgfAddProperty(node, "C ", comment);

  return node;
}


/*
 * Place text on the board at position (i, j).
 */

SGFNode *
sgfBoardText(SGFNode *node, int i, int j, const char *text)
{
  char *str = new char[strlen(text) + 3]();

  sprintf(str, "%c%c:%s", j+'a', i+'a', text);
  sgfAddProperty(node, "LB", str);
  free(str);

  return node;
}


/*
 * Place a character on the board at position (i, j).
 */

SGFNode *
sgfBoardChar(SGFNode *node, int i, int j, char c)
{
  char text[2] = "";

  text[0] = c;
  text[1] = 0;

  return sgfBoardText(node, i, j, text);
}


/*
 * Place a number on the board at position (i, j).
 */

SGFNode *
sgfBoardNumber(SGFNode *node, int i, int j, int number)
{
  char text[10];

  gg_snprintf(text, 10, "%c%c:%i", j+'a', i+'a', number);
  sgfAddProperty(node, "LB", text);

  return node;
}


/*
 * Place a triangle mark on the board at position (i, j).
 */

SGFNode *
sgfTriangle(SGFNode *node, int i, int j)
{
  char text[3];

  gg_snprintf(text, 3, "%c%c", j+'a', i+'a');
  sgfAddProperty(node, "TR", text);

  return node;
}


/*
 * Place a label on the board at position (i, j).
 */

SGFNode *
sgfLabel(SGFNode *node, const char *label, int i, int j)
{
  /* allows 12 chars labels - more than enough */
  char text[16];

  gg_snprintf(text, 16, "%c%c:%s", j+'a', i+'a', label);
  sgfAddProperty(node, "LB", text);

  return node;
}


/*
 * Place a numeric label on the board at position (i, j).
 */

SGFNode *
sgfLabelInt(SGFNode *node, int num, int i, int j)
{
  char text[16];

  gg_snprintf(text, 16, "%c%c:%d", j+'a', i+'a', num);
  sgfAddProperty(node, "LB", text);

  return node;
}


/*
 * Place a circle mark on the board at position (i, j).
 */

SGFNode *
sgfCircle(SGFNode *node, int i, int j)
{
  char text[3];

  gg_snprintf(text, 3, "%c%c", j+'a', i+'a');
  sgfAddProperty(node, "CR", text);

  return node;
}


/*
 * Place a square mark on the board at position (i, j).
 */

SGFNode *
sgfSquare(SGFNode *node, int i, int j)
{
  return sgfMark(node, i, j);   /* cgoban 1.9.5 does not understand SQ */
}


/*
 * Place a (square) mark on the board at position (i, j).
 */

SGFNode *
sgfMark(SGFNode *node, int i, int j)
{
  char text[3];

  gg_snprintf(text, 3, "%c%c", j+'a', i+'a');
  sgfAddProperty(node, "MA", text);

  return node;
}


/*
 * Start a new variant. Returns a pointer to the new node.
 */

SGFNode *
sgfStartVariant(SGFNode *node)
{
  assert(node);
  assert(node->parent);

  while (node->next)
    node = node->next;
  node->next = sgfNewNode();
  node->next->parent = node->parent;

  return node->next;
}


/*
 * Start a new variant as first child. Returns a pointer to the new node.
 */

SGFNode *
sgfStartVariantFirst(SGFNode *node)
{
  SGFNode *old_first_child = node;
  SGFNode *new_first_child = sgfNewNode();

  assert(node);
  assert(node->parent);

  new_first_child->next = old_first_child;
  new_first_child->parent = old_first_child->parent;


  new_first_child->parent->child = new_first_child;

  return new_first_child;
}

/*
 * If no child exists, add one. Otherwise add a sibling to the
 * existing children. Returns a pointer to the new node.
 */

SGFNode *
sgfAddChild(SGFNode *node)
{
  SGFNode *new_node = sgfNewNode();
  assert(node);

  new_node->parent = node;
  
  if (!node->child)
    node->child = new_node;
  else {
    node = node->child;
    while (node->next)
      node = node->next;
    node->next = new_node;
  }

  return new_node;
}


/*
 * Write result of the game to the game tree.
 */

void
sgfWriteResult(SGFNode *node, float score, int overwrite)
{
  char text[8];
  char winner;
  float s;
  int dummy;

  /* If not writing to the SGF file, skip everything and return now. */
  if (!node)
    return;

  /* If not overwriting and there already is a result property, return. */
  if (!overwrite)
    if (sgfGetIntProperty(node, "RE", &dummy))
      return;

  if (score > 0.0) {
    winner = 'W';
    s = score;
  }
  else if (score < 0.0) {
    winner = 'B';
    s = -score;
  }
  else {
    winner = '0';
    s = 0;
  }

  if (winner == '0')
    gg_snprintf(text, 8, "0");
  else if (score < 1000.0 && score > -1000.0)
    gg_snprintf(text, 8, "%c+%3.1f", winner, s);
  else
    gg_snprintf(text, 8, "%c+%c", winner, 'R');
  sgfOverwriteProperty(node, "RE", text);
}


static void
sgf_write_header_reduced(SGFNode *root, int overwrite)
{
  time_t curtime = time(NULL);
  struct tm *loctime = localtime(&curtime);
  char str[128];
  int dummy;

  gg_snprintf(str, 128, "%4.4i-%2.2i-%2.2i",
	      loctime->tm_year+1900, loctime->tm_mon+1, loctime->tm_mday);
  if (overwrite || !sgfGetIntProperty(root, "DT", &dummy))
    sgfOverwriteProperty(root, "DT", str);
  if (overwrite || !sgfGetIntProperty(root, "AP", &dummy))
    sgfOverwriteProperty(root, "AP", "GNU Go:" __VERSION__);
  sgfOverwriteProperty(root, "FF", "4");
}


void
sgf_write_header(SGFNode *root, int overwrite, int seed, float komi,
		 int handicap, int level, int rules)
{
  char str[128];
  int dummy;

  gg_snprintf(str, 128, "GNU Go %s Random Seed %d level %d",  __VERSION__, seed, level);
  if (overwrite || !sgfGetIntProperty(root, "GN", &dummy))
    sgfOverwriteProperty(root, "GN", str);
  if (overwrite || !sgfGetIntProperty(root, "RU", &dummy))
    sgfOverwriteProperty(root, "RU", rules ? "Chinese" : "Japanese");
  sgfOverwritePropertyFloat(root, "KM", komi);
  sgfOverwritePropertyInt(root, "HA", handicap);

  sgf_write_header_reduced(root, overwrite);
}


/* ================================================================ */
/*                          Read SGF tree                           */
/* ================================================================ */


#define MAX_FILE_BUFFER 200000 /* buffer for reading SGF file. */

/*
 * SGF grammar:
 *
 * Collection = GameTree { GameTree }
 * GameTree   = "(" Sequence { GameTree } ")"
 * Sequence   = Node { Node }
 * Node       = ";" { Property }
 * Property   = PropIdent PropValue { PropValue }
 * PropIdent  = UcLetter { UcLetter }
 * PropValue  = "[" CValueType "]"
 * CValueType = (ValueType | Compose)
 * ValueType  = (None | Number | Real | Double | Color | SimpleText |
 *               Text | Point  | Move | Stone)
 *
 * The above grammar has a number of simple properties which enables us
 * to write a simpler parser:
 *   1) There is never a need for backtracking
 *   2) The only recursion is on gametree.
 *   3) Tokens are only one character
 * 
 * We will use a global state to keep track of the remaining input
 * and a global char variable, `lookahead' to hold the next token.  
 * The function `nexttoken' skips whitespace and fills lookahead with 
 * the new token.
 */


static void parse_error(const char *msg, int arg);
static void nexttoken(void);
static void match(int expected);


static FILE *sgffile;


#define sgf_getch() (getc(sgffile))


static char *sgferr;
#ifdef TEST_SGFPARSER
static int sgferrarg;
#endif
static int sgferrpos;

static int lookahead;


/* ---------------------------------------------------------------- */
/*                       Parsing primitives                         */
/* ---------------------------------------------------------------- */


static void
parse_error(const char *msg, int arg)
{
  //fprintf(stderr, msg, arg);
  fprintf(stderr, "\n");
  exit(EXIT_FAILURE);
}


static void
nexttoken()
{
  do
    lookahead = sgf_getch();
  while (isspace(lookahead));
}


static void
match(int expected)
{
  if (lookahead != expected)
    parse_error("expected: %c", expected);
  else
    nexttoken();
}

/* ---------------------------------------------------------------- */
/*                        The parser proper                         */
/* ---------------------------------------------------------------- */


static void
propident(char *buffer, int size)
{
  if (lookahead == EOF || !isupper(lookahead)) 
    parse_error("Expected an upper case letter.", 0);
  
  while (lookahead != EOF && isalpha(lookahead)) {
    if (isupper(lookahead) && size > 1) {
      *buffer++ = lookahead;
      size--;
    }
    nexttoken();
  }
  *buffer = '\0';
}


static void
propvalue(char *buffer, int size)
{
  char *p = buffer;

  match('[');
  while (lookahead != ']' && lookahead != EOF) {
    if (lookahead == '\\') {
      lookahead = sgf_getch();
      /* Follow the FF4 definition of backslash */
      if (lookahead == '\r') {
	lookahead = sgf_getch();
	if (lookahead == '\n') 
	  lookahead = sgf_getch();
      }
      else if (lookahead == '\n') {
	lookahead = sgf_getch();
	if (lookahead == '\r') 
	  lookahead = sgf_getch();
      }
    }
    if (size > 1) {
      *p++ = lookahead;
      size--;
    }
    lookahead = sgf_getch();
  }
  match(']');
  
  /* Remove trailing whitespace. The double cast below is needed
   * because "char" may be represented as a signed char, in which case
   * characters between 128 and 255 would be negative and a direct
   * cast to int would cause a negative value to be passed to isspace,
   * possibly causing an assertion failure.
   */
  --p;
  while (p > buffer && isspace(static_cast<int> (static_cast<unsigned char> (*p))))
    --p;
  *++p = '\0';
}


static SGFProperty *
property(SGFNode *n, SGFProperty *last)
{
  char name[3];
  char buffer[4000];

  propident(name, sizeof(name));
  do {
    propvalue(buffer, sizeof(buffer));
    last = sgfMkProperty(name, buffer, n, last);
  } while (lookahead == '[');
  return last;
}


static void
node(SGFNode *n)
{
  SGFProperty *last = NULL;
  match(';');
  while (lookahead != EOF && isupper(lookahead))
    last = property(n, last);
}


static SGFNode *
sequence(SGFNode *n)
{
  node(n);
  while (lookahead == ';') {
    SGFNode *new_node = sgfNewNode();
    new_node->parent = n;
    n->child = new_node;
    n = new_node;
    node(n);
  }
  return n;
}


static void
gametree(SGFNode **p, SGFNode *parent, int mode) 
{
  if (mode == STRICT_SGF)
    match('(');
  else
    for (;;) {
      if (lookahead == EOF) {
	parse_error("Empty file?", 0);
	break;
      }
      if (lookahead == '(') {
	while (lookahead == '(')
	  nexttoken();
	if (lookahead == ';')
	  break;
      }
      nexttoken();
    }

  /* The head is parsed */
  {
    SGFNode *head = sgfNewNode();
    SGFNode *last;

    head->parent = parent;
    *p = head;

    last = sequence(head);
    p = &last->child;
    while (lookahead == '(') {
      gametree(p, last, STRICT_SGF);
      p = &((*p)->next);
    }
    if (mode == STRICT_SGF)
      match(')');
  }
}


/*
 * Fuseki readers
 * Reads an SGF file for extract_fuseki in a compact way
 */

static void
gametreefuseki(SGFNode **p, SGFNode *parent, int mode, 
	       int moves_per_game, int i)
{
  if (mode == STRICT_SGF)
    match('(');
  else
    for (;;) {
      if (lookahead == EOF) {
	parse_error("Empty file?", 0);
	break;
      }
      if (lookahead == '(') {
	while (lookahead == '(')
	  nexttoken();
	if (lookahead == ';')
	  break;
      }
      nexttoken();
    }
  
  /* The head is parsed */
  {

    SGFNode *head = sgfNewNode();
    SGFNode *last;
    head->parent = parent;
    *p = head;
    
    last = sequence(head);
    p = &last->child;
    while (lookahead == '(') {
      if (last->props 
	  && (last->props->name == SGFB || last->props->name == SGFW))
	i++;
      /* break after number_of_moves moves in SGF file */
      if (i >= moves_per_game) { 
	last->child = NULL;
	last->next = NULL;
	break;
      }
      else {
	gametreefuseki(p, last, mode, moves_per_game, i);
	p = &((*p)->next);
      }
    }
    if (mode == STRICT_SGF)
      match(')');
  }
}

SGFNode *
readsgffilefuseki(const char *filename, int moves_per_game)
{
  SGFNode *root;
  int tmpi = 0;

  if (strcmp(filename, "-") == 0)
    sgffile = stdin;
  else
    sgffile = fopen(filename, "r");

  if (!sgffile)
    return NULL;


  nexttoken();
  gametreefuseki(&root, NULL, LAX_SGF, moves_per_game, 0);

  fclose(sgffile);

  if (sgferr) {
    fprintf(stderr, "Parse error: %s at position %d\n", sgferr, sgferrpos);
    sgfFreeNode(root);
    return NULL;
  }

  /* perform some simple checks on the file */
  if (!sgfGetIntProperty(root, "GM", &tmpi)) {
    if (VERBOSE_WARNINGS)
      fprintf(stderr, "Couldn't find the game type (GM) attribute!\n");
  }
  else if (tmpi != 1) {
    fprintf(stderr, "SGF file might be for game other than go: %d\n", tmpi);
    fprintf(stderr, "Trying to load anyway.\n");
  }

  if (!sgfGetIntProperty(root, "FF", &tmpi)) {
    if (VERBOSE_WARNINGS)
      fprintf(stderr, "Can not determine SGF spec version (FF)!\n");
  }
  else if ((tmpi < 3 || tmpi > 4) && VERBOSE_WARNINGS)
    fprintf(stderr, "Unsupported SGF spec version: %d\n", tmpi);

  return root;
}





/*
 * Wrapper around readsgf which reads from a file rather than a string.
 * Returns NULL if file will not open, or some other parsing error.
 * Filename "-" means read from stdin, and leave it open when done.
 */

SGFNode *
readsgffile(const char *filename)
{
  SGFNode *root;
  int tmpi = 0;

  if (strcmp(filename, "-") == 0)
    sgffile = stdin;
  else
    sgffile = fopen(filename, "r");

  if (!sgffile)
    return NULL;


  nexttoken();
  gametree(&root, NULL, LAX_SGF);

  if (sgffile != stdin)
    fclose(sgffile);

  if (sgferr) {
    fprintf(stderr, "Parse error: %s at position %d\n", sgferr, sgferrpos);
    sgfFreeNode(root);
    return NULL;
  }

  /* perform some simple checks on the file */
  if (!sgfGetIntProperty(root, "GM", &tmpi)) {
    if (VERBOSE_WARNINGS)
      fprintf(stderr, "Couldn't find the game type (GM) attribute!\n");
  }
  else if (tmpi != 1) {
    fprintf(stderr, "SGF file might be for game other than go: %d\n", tmpi);
    fprintf(stderr, "Trying to load anyway.\n");
  }

  if (!sgfGetIntProperty(root, "FF", &tmpi)) {
    if (VERBOSE_WARNINGS)
      fprintf(stderr, "Can not determine SGF spec version (FF)!\n");
  }
  else if ((tmpi < 3 || tmpi > 4) && VERBOSE_WARNINGS)
    fprintf(stderr, "Unsupported SGF spec version: %d\n", tmpi);

  return root;
}



/* ================================================================ */
/*                          Write SGF tree                          */
/* ================================================================ */


#define OPTION_STRICT_FF4 0

static int sgf_column = 0;

static void
sgf_putc(int c, FILE *file)
{
  if (c == '\n' && sgf_column == 0)
    return;

  fputc(c, file);

  if (c == '\n')
    sgf_column = 0;
  else
    sgf_column++;

  if (c == ']' && sgf_column > 60) {
    fputc('\n', file);
    sgf_column = 0;
  }
}

static void
sgf_puts(const char *s, FILE *file)
{
  for (; *s; s++) {
    if (*s == '[' || *s == ']' || *s == '\\') {
      fputc('\\', file);
      sgf_column++;
    }
    fputc(static_cast<int>( *s), file);
    sgf_column++;
  }
}

/* Print all properties with the given name in a node to file and mark
 * them as printed.
 *
 * If is_comment is 1, multiple properties are concatenated with a
 * newline. I.e. we write
 *
 * C[comment1
 * comment2]
 *
 * instead of
 *
 * C[comment1][comment2]
 *
 * Most other property types should be written in the latter style.
 */

static void
sgf_print_name(FILE *file, short name)
{
  sgf_putc(name & 0xff, file);
  if (name >> 8 != ' ')
    sgf_putc(name >> 8, file);
}

static void
sgf_print_property(FILE *file, SGFNode *node, short name, int is_comment)
{
  int n = 0;
  SGFProperty *prop;

  for (prop = node->props; prop; prop = prop->next) {
    if (prop->name == name) {
      prop->name |= 0x20;  /* Indicate already printed. */
      if (n == 0) {
	sgf_print_name(file, name);
	sgf_putc('[', file);
      }
      else if (is_comment)
	sgf_putc('\n', file);
      else {
	sgf_putc(']', file);
	sgf_putc('[', file);
      }
      
      sgf_puts(prop->value, file);
      n++;
    }
  }

  if (n > 0)
    sgf_putc(']', file);

  /* Add a newline after certain properties. */
  if (name == SGFAB || name == SGFAW || name == SGFAE || (is_comment && n > 1))
    sgf_putc('\n', file);
}

/*
 * Print all remaining unprinted property values at node N to file.
 */

static void
sgfPrintRemainingProperties(FILE *file, SGFNode *node)
{
  SGFProperty *prop;

  for (prop = node->props; prop; prop = prop->next)
    if (!(prop->name & 0x20))
      sgf_print_property(file, node, prop->name, 0);
}


/*
 * Print the property values of NAME at node N and mark it as printed. 
 */

static void
sgfPrintCharProperty(FILE *file, SGFNode *node, const char *name)
{
  short nam = name[0] | name[1] << 8;
  
  sgf_print_property(file, node, nam, 0);
}


/*
 * Print comments from Node node.
 *
 * NOTE: cgoban does not print "C[comment1][comment2]" and I don't know
 *       what the sgfspec says.
 */

static void
sgfPrintCommentProperty(FILE *file, SGFNode *node, const char *name)
{
  short nam = name[0] | name[1] << 8;
  
  sgf_print_property(file, node, nam, 1);
}


static void
unparse_node(FILE *file, SGFNode *node)
{
  sgf_putc(';', file);
  sgfPrintCharProperty(file, node, "B ");
  sgfPrintCharProperty(file, node, "W ");
  sgfPrintCommentProperty(file, node, "N ");
  sgfPrintCommentProperty(file, node, "C ");
  sgfPrintRemainingProperties(file, node);
}


static void
unparse_root(FILE *file, SGFNode *node)
{
  sgf_putc(';', file);
  
  if (sgfHasProperty(node, "GM"))
    sgfPrintCharProperty(file, node, "GM");
  else {
    fputs("GM[1]", file);
    sgf_column += 5;
  }
  
  sgfPrintCharProperty(file, node, "FF");
  sgf_putc('\n', file);

  sgfPrintCharProperty(file, node, "SZ");
  sgf_putc('\n', file);
  
  sgfPrintCharProperty(file, node, "GN");
  sgf_putc('\n', file);
  
  sgfPrintCharProperty(file, node, "DT");
  sgf_putc('\n', file);
  
  sgfPrintCommentProperty(file, node, "PB");
  sgfPrintCommentProperty(file, node, "BR");
  sgf_putc('\n', file);
  
  sgfPrintCommentProperty(file, node, "PW");
  sgfPrintCommentProperty(file, node, "WR");
  sgf_putc('\n', file);
  
  sgfPrintCommentProperty(file, node, "N ");
  sgfPrintCommentProperty(file, node, "C ");
  sgfPrintRemainingProperties(file, node);

  sgf_putc('\n', file);
}


/*
 * p->child is the next move.
 * p->next  is the next variation
 */

static void
unparse_game(FILE *file, SGFNode *node, int root)
{
  if (!root)
    sgf_putc('\n', file);
  sgf_putc('(', file);
  if (root)
    unparse_root(file, node);
  else
    unparse_node(file, node);

  node = node->child;
  while (node != NULL && node->next == NULL) {
    unparse_node(file, node);
    node = node->child;
  } 

  while (node != NULL) {
    unparse_game(file, node, 0);
    node = node->next;
  }
  sgf_putc(')', file);
  if (root)
    sgf_putc('\n', file);
}

/* Printed properties are marked by adding the 0x20 bit to the
 * property name (changing an upper case letter to lower case). This
 * function removes this mark so that we can print the property next
 * time too. It recurses to all properties in the linked list.
 */
static void
restore_property(SGFProperty *prop)
{
  if (prop) {
    restore_property(prop->next);
    prop->name &= ~0x20;
  }
}

/* When called with the tree root, recurses to all properties in the
 * tree and removes all print marks.
 */
static void
restore_node(SGFNode *node)
{
  if (node) {
    restore_property(node->props);
    restore_node(node->child);
    restore_node(node->next);
  }
}


/*
 * Opens filename and writes the game stored in the sgf structure.
 */

int
writesgf(SGFNode *root, const char *filename)
{
  FILE *outfile;

  if (strcmp(filename, "-") == 0) 
    outfile = stdout;
  else
    outfile = fopen(filename, "w");

  if (!outfile) {
    fprintf(stderr, "Can not open %s\n", filename);
    return 0;
  }

  sgf_write_header_reduced(root, 0);

  sgf_column = 0;
  unparse_game(outfile, root, 1);
  if (outfile != stdout)
    fclose(outfile);
  
  /* Remove "printed" marks so that the tree can be written multiple
   * times.
   */
  restore_node(root);
  
  return 1;
}


short str2short(char *str)
{
  return (str[0] | str[1] << 8);
}

const char* getPropertyName(short propName)
{
  switch(propName)
  {
     /*  Add Black       setup            list of stone */
    case SGFAB: return "SGFAB";
     /*  Add Empty       setup            list of point */

    case SGFAE: return "SGFAE";
        /*  Annotation      game-info        simpletext */

    case SGFAN: return "SGFAN";
        /*  Application     root             composed simpletext ':' simpletext */

    case SGFAP: return "SGFAP";
        /*  Arrow           -                list of composed point ':' point */

    case SGFAR: return "SGFAR";
        /*  Who adds stones - (LOA)          simpletext */

    case SGFAS: return "SGFAS";
        /*  Add White       setup            list of stone */

    case SGFAW: return "SGFAW";
        /*  Black           move             move */

    case SGFB: return "SGFB";
        /*  Black time left move             real */

    case SGFBL: return "SGFBL";
        /*  Bad move        move             double */

    case SGFBM: return "SGFBM";
        /*  Black rank      game-info        simpletext */

    case SGFBR: return "SGFBR";
        /*  Black team      game-info        simpletext */

    case SGFBT: return "SGFBT";
        /*  Comment         -                text */

    case SGFC: return "SGFC";
        /*  Charset         root             simpletext */

    case SGFCA: return "SGFCA";
        /*  Copyright       game-info        simpletext */

    case SGFCP: return "SGFCP";
        /*  Circle          -                list of point */

    case SGFCR: return "SGFCR";
        /*  Dim points      - (inherit)      elist of point */

    case SGFDD: return "SGFDD";
        /*  Even position   -                double */

    case SGFDM: return "SGFDM";
        /*  Doubtful        move             none */

    case SGFDO: return "SGFDO";
        /*  Date            game-info        simpletext */

    case SGFDT: return "SGFDT";
        /*  Event           game-info        simpletext */

    case SGFEV: return "SGFEV";
        /*  Fileformat      root             number (range: */

    case SGFFF: return "SGFFF";
        /*  Figure          -                none | composed number ":" simpletext */

    case SGFFG: return "SGFFG";
        /*  Good for Black  -                double */

    case SGFGB: return "SGFGB";
        /*  Game comment    game-info        text */

    case SGFGC: return "SGFGC";
        /*  Game            root             number (range: */

    case SGFGM: return "SGFGM";
        /*  Game name       game-info        simpletext */

    case SGFGN: return "SGFGN";
        /*  Good for White  -                double */

    case SGFGW: return "SGFGW";
        /*  Handicap        game-info (Go)   number */

    case SGFHA: return "SGFHA";
        /*  Hotspot         -                double */

    case SGFHO: return "SGFHO";
        /*  Initial pos.    game-info (LOA)  simpletext */

    case SGFIP: return "SGFIP";
        /*  Interesting     move             none */

    case SGFIT: return "SGFIT";
        /*  Invert Y-axis   game-info (LOA)  simpletext */

    case SGFIY: return "SGFIY";
        /*  Komi            game-info (Go)   real */

    case SGFKM: return "SGFKM";
        /*  Ko              move             none */

    case SGFKO: return "SGFKO";
        /*  Label           -                list of composed point ':' simpletext */

    case SGFLB: return "SGFLB";
        /*  Line            -                list of composed point ':' point */

    case SGFLN: return "SGFLN";
        /*  Mark            -                list of point */

    case SGFMA: return "SGFMA";
        /*  set move number move             number */

    case SGFMN: return "SGFMN";
        /*  Nodename        -                simpletext */

    case SGFN: return "SGFN";
        /*  OtStones Black  move             number */

    case SGFOB: return "SGFOB";
        /*  Opening         game-info        text */

    case SGFON: return "SGFON";
        /*  Overtime        game-info        simpletext */

    case SGFOT: return "SGFOT";
        /*  OtStones White  move             number */

    case SGFOW: return "SGFOW";
        /*  Player Black    game-info        simpletext */

    case SGFPB: return "SGFPB";
        /*  Place           game-info        simpletext */

    case SGFPC: return "SGFPC";
        /*  Player to play  setup            color */

    case SGFPL: return "SGFPL";
        /*  Print move mode - (inherit)      number */

    case SGFPM: return "SGFPM";
        /*  Player White    game-info        simpletext */
        /*  Round           game-info        simpletext */

    case SGFPW: return "SGFPW";
        /*  Result          game-info        simpletext */

    case SGFRE: return "SGFRE";

    case SGFRO: return "SGFRO";
        /*  Rules           game-info        simpletext */

    case SGFRU: return "SGFRU";
        /*  Markup          - (LOA)          point */

    case SGFSE: return "SGFSE";
        /*  Selected        -                list of point */

    case SGFSL: return "SGFSL";
        /*  Source          game-info        simpletext */

    case SGFSO: return "SGFSO";
        /*  Square          -                list of point */

    case SGFSQ: return "SGFSQ";
        /*  Style           root             number (range: */

    case SGFST: return "SGFST";
        /*  Setup type      game-info (LOA)  simpletext */

    case SGFSU: return "SGFSU";
        /*  Size            root             (number | composed number ':' number) */

    case SGFSZ: return "SGFSZ";
        /*  Territory Black - (Go)           elist of point */

    case SGFTB: return "SGFTB";
        /*  Tesuji          move             double */

    case SGFTE: return "SGFTE";
        /*  Timelimit       game-info        real */

    case SGFTM: return "SGFTM";
        /*  Triangle        -                list of point */

    case SGFTR: return "SGFTR";
        /*  Territory White - (Go)           elist of point */

    case SGFTW: return "SGFTW";
        /*  Unclear pos     -                double */

    case SGFUC: return "SGFUC";
        /*  User            game-info        simpletext */

    case SGFUS: return "SGFUS";
        /*  Value           -                real */

    case SGFV: return "SGFV";
        /*  View            - (inherit)      elist of point */

    case SGFVW: return "SGFVW";
        /*  White           move             move */

    case SGFW: return "SGFW";
        /*  White time left move             real */

    case SGFWL: return "SGFWL";
        /*  White rank      game-info        simpletext */

    case SGFWR: return "SGFWR";
        /*  White team      game-info        simpletext */

    case SGFWT: return "SGFWT";

    default: return "not listed";
  }
}

void printNodeProps(SGFNode* head)
{
  //printf("enter \n");
  //printf("head is %p\n",head);
  SGFProperty * next = head->props;
  
  while(next)
  {

    printf("property name in bytes %d\n",next->name);
    printf("property name %s\n",getPropertyName(next->name));
    printf("propery value %s\n",next->value);
    printf("---------------------------------------------\n");
    
    next = next->next;
    // printf("child %d", head);
  }
  printf("---------------------this is the end of this node----------------------\n");
  return;
}

void printGameTreeRecursive(SGFNode* head, bool isVariation, int nodeNumber)
{
  SGFNode* next = head;
  int i = 0;
  while(next)
  {
    if(isVariation)
    {
      printf("this is a variation and it's from node: %d\n", nodeNumber);
      i = nodeNumber;
    }
    printf("this is the node number: %d\n", i);
    printf("is variation: %d\n", isVariation);
    printNodeProps(next);
    if(next->next)
    {
      isVariation = true;
      printf("!!!this node has a variation and it's number is: %d\n", i);
      printGameTreeRecursive(next->next, isVariation, i);
    }
    next = next->child;
    i++;
    isVariation = false;
    // printf("child %d", head);
  }
  return;
}

void printVariation(SGFNode* head, int nodeNumber)
{
  int i = nodeNumber;
  SGFNode* next = head;
  while(next)
  {
    printf("this is node number: %d and it's a variation from node number: %d\n",i,nodeNumber);
    printNodeProps(next);
    i++;
    next = next->child;
    
  }
}


void printGameTree(SGFNode* head)
{
    SGFNode* next = head;
    SGFNode* nextVariation = NULL;
    int i = 0;
    int index, player = 0;
    bool valid = false;
    while(next)
    {
      printf("this is node number: %d\n", i);
      nextVariation = next->next;
      printNodeProps(next);

      sgf_play_node(next, index, player, valid); //here just for testing
      printf("index: %d, player: %d\n",index, player);

      while(nextVariation)
      {
        printf("!!!this node has a variation and it's number is: %d\n", i);
        printVariation(next,i);
        nextVariation = nextVariation->next;
      }
        next = next->child;
        i++;
    }
    return;
}

// int main()
// {
//   char const* filename = "my_sgf.sgf"; 
//   SGFNode *treeHead = readsgffile(filename);
//   printGameTree(treeHead);
//   sgfFreeNode(treeHead);

// }
