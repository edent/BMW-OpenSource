
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         cssyyparse
#define yylex           cssyylex
#define yyerror         cssyyerror
#define yylval          cssyylval
#define yychar          cssyychar
#define yydebug         cssyydebug
#define yynerrs         cssyynerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "../css/CSSGrammar.y"


/*
 *  Copyright (C) 2002-2003 Lars Knoll (knoll@kde.org)
 *  Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 *  Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
 *  Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "config.h"

#include "CSSMediaRule.h"
#include "CSSParser.h"
#include "CSSPrimitiveValue.h"
#include "CSSPropertyNames.h"
#include "CSSRuleList.h"
#include "CSSSelector.h"
#include "CSSStyleSheet.h"
#include "Document.h"
#include "HTMLNames.h"
#include "MediaList.h"
#include "WebKitCSSKeyframeRule.h"
#include "WebKitCSSKeyframesRule.h"
#include <wtf/FastMalloc.h>
#include <stdlib.h>
#include <string.h>

using namespace WebCore;
using namespace HTMLNames;

#define YYMALLOC fastMalloc
#define YYFREE fastFree

#define YYENABLE_NLS 0
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 10000
#define YYDEBUG 0

// FIXME: Replace with %parse-param { CSSParser* parser } once we can depend on bison 2.x
#define YYPARSE_PARAM parser
#define YYLEX_PARAM parser



/* Line 189 of yacc.c  */
#line 141 "WebCore/tmp/../generated/CSSGrammar.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_EOF = 0,
     LOWEST_PREC = 258,
     UNIMPORTANT_TOK = 259,
     WHITESPACE = 260,
     SGML_CD = 261,
     INCLUDES = 262,
     DASHMATCH = 263,
     BEGINSWITH = 264,
     ENDSWITH = 265,
     CONTAINS = 266,
     STRING = 267,
     IDENT = 268,
     NTH = 269,
     HEX = 270,
     IDSEL = 271,
     IMPORT_SYM = 272,
     PAGE_SYM = 273,
     MEDIA_SYM = 274,
     FONT_FACE_SYM = 275,
     CHARSET_SYM = 276,
     NAMESPACE_SYM = 277,
     WEBKIT_RULE_SYM = 278,
     WEBKIT_DECLS_SYM = 279,
     WEBKIT_KEYFRAME_RULE_SYM = 280,
     WEBKIT_KEYFRAMES_SYM = 281,
     WEBKIT_VALUE_SYM = 282,
     WEBKIT_MEDIAQUERY_SYM = 283,
     WEBKIT_SELECTOR_SYM = 284,
     WEBKIT_VARIABLES_SYM = 285,
     WEBKIT_DEFINE_SYM = 286,
     VARIABLES_FOR = 287,
     WEBKIT_VARIABLES_DECLS_SYM = 288,
     ATKEYWORD = 289,
     IMPORTANT_SYM = 290,
     MEDIA_ONLY = 291,
     MEDIA_NOT = 292,
     MEDIA_AND = 293,
     REMS = 294,
     QEMS = 295,
     EMS = 296,
     EXS = 297,
     PXS = 298,
     CMS = 299,
     MMS = 300,
     INS = 301,
     PTS = 302,
     PCS = 303,
     DEGS = 304,
     RADS = 305,
     GRADS = 306,
     TURNS = 307,
     MSECS = 308,
     SECS = 309,
     HERZ = 310,
     KHERZ = 311,
     DIMEN = 312,
     PERCENTAGE = 313,
     FLOATTOKEN = 314,
     INTEGER = 315,
     URI = 316,
     FUNCTION = 317,
     NOTFUNCTION = 318,
     UNICODERANGE = 319,
     VARCALL = 320
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 62 "../css/CSSGrammar.y"

    bool boolean;
    char character;
    int integer;
    double number;
    CSSParserString string;

    CSSRule* rule;
    CSSRuleList* ruleList;
    CSSSelector* selector;
    Vector<CSSSelector*>* selectorList;
    CSSSelector::Relation relation;
    MediaList* mediaList;
    MediaQuery* mediaQuery;
    MediaQuery::Restrictor mediaQueryRestrictor;
    MediaQueryExp* mediaQueryExp;
    CSSParserValue value;
    CSSParserValueList* valueList;
    Vector<MediaQueryExp*>* mediaQueryExpList;
    WebKitCSSKeyframeRule* keyframeRule;
    WebKitCSSKeyframesRule* keyframesRule;
    float val;



/* Line 214 of yacc.c  */
#line 269 "WebCore/tmp/../generated/CSSGrammar.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */

/* Line 264 of yacc.c  */
#line 86 "../css/CSSGrammar.y"


static inline int cssyyerror(const char*)
{
    return 1;
}

static int cssyylex(YYSTYPE* yylval, void* parser)
{
    return static_cast<CSSParser*>(parser)->lex(yylval);
}



/* Line 264 of yacc.c  */
#line 296 "WebCore/tmp/../generated/CSSGrammar.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  24
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1275

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  90
/* YYNRULES -- Number of rules.  */
#define YYNRULES  268
/* YYNRULES -- Number of states.  */
#define YYNSTATES  516

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   320

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    84,     2,    85,     2,     2,
      74,    75,    20,    77,    76,    80,    18,    83,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    17,    73,
       2,    82,    79,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    19,     2,    81,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    71,    21,    72,    78,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,    11,    14,    17,    20,    23,    26,    29,
      32,    34,    36,    43,    50,    56,    62,    68,    74,    80,
      81,    84,    85,    88,    91,    92,    94,    96,    98,   104,
     108,   112,   113,   117,   119,   120,   124,   125,   129,   130,
     134,   136,   138,   140,   142,   144,   146,   148,   150,   152,
     153,   157,   159,   161,   163,   165,   167,   169,   171,   173,
     175,   182,   189,   193,   197,   205,   213,   214,   218,   220,
     223,   225,   229,   231,   234,   238,   243,   247,   253,   258,
     263,   270,   275,   283,   286,   292,   296,   301,   304,   311,
     315,   319,   320,   323,   325,   327,   330,   331,   336,   344,
     346,   352,   353,   357,   358,   360,   362,   364,   369,   370,
     372,   374,   379,   382,   390,   397,   400,   409,   411,   413,
     414,   418,   425,   427,   433,   435,   437,   441,   445,   453,
     457,   461,   464,   467,   470,   472,   474,   480,   482,   487,
     490,   493,   495,   497,   500,   504,   507,   509,   512,   515,
     517,   520,   522,   525,   529,   532,   534,   536,   538,   541,
     544,   546,   548,   550,   552,   554,   557,   560,   565,   574,
     580,   590,   592,   594,   596,   598,   600,   602,   604,   606,
     609,   613,   618,   623,   628,   635,   637,   640,   642,   646,
     648,   651,   654,   658,   663,   667,   673,   678,   683,   690,
     696,   699,   702,   709,   716,   719,   723,   728,   731,   734,
     737,   738,   740,   744,   747,   750,   753,   754,   756,   759,
     762,   765,   768,   772,   775,   778,   780,   783,   785,   788,
     791,   794,   797,   800,   803,   806,   809,   812,   815,   818,
     821,   824,   827,   830,   833,   836,   839,   842,   845,   848,
     851,   854,   856,   862,   866,   869,   872,   874,   877,   881,
     885,   888,   892,   894,   896,   899,   905,   909,   911
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      87,     0,    -1,    96,    98,    97,   101,   102,   103,   104,
      -1,    89,    96,    -1,    91,    96,    -1,    93,    96,    -1,
      94,    96,    -1,    95,    96,    -1,    92,    96,    -1,    90,
      96,    -1,   105,    -1,   110,    -1,    28,    71,    96,    88,
      96,    72,    -1,    30,    71,    96,   134,    96,    72,    -1,
      29,    71,    96,   156,    72,    -1,    38,    71,    96,   113,
      72,    -1,    32,    71,    96,   161,    72,    -1,    33,     5,
      96,   126,    72,    -1,    34,    71,    96,   142,    72,    -1,
      -1,    96,     5,    -1,    -1,    97,     6,    -1,    97,     5,
      -1,    -1,   100,    -1,    72,    -1,     0,    -1,    26,    96,
      12,    96,    73,    -1,    26,     1,   174,    -1,    26,     1,
      73,    -1,    -1,   101,   110,    97,    -1,   170,    -1,    -1,
     102,   111,    97,    -1,    -1,   103,   117,    97,    -1,    -1,
     104,   106,    97,    -1,   141,    -1,   129,    -1,   137,    -1,
     138,    -1,   131,    -1,   105,    -1,   173,    -1,   169,    -1,
     171,    -1,    -1,   107,   109,    97,    -1,   141,    -1,   137,
      -1,   138,    -1,   131,    -1,   108,    -1,   173,    -1,   169,
      -1,   171,    -1,   172,    -1,    22,    96,   119,    96,   127,
      73,    -1,    22,    96,   119,    96,   127,   174,    -1,    22,
       1,    73,    -1,    22,     1,   174,    -1,    35,    96,   127,
      71,    96,   113,    72,    -1,    36,    96,   112,    71,    96,
     113,    72,    -1,    -1,    37,     5,   128,    -1,   115,    -1,
     114,   115,    -1,   114,    -1,     1,   175,     1,    -1,     1,
      -1,   114,     1,    -1,   115,    73,    96,    -1,   115,   175,
      73,    96,    -1,     1,    73,    96,    -1,     1,   175,     1,
      73,    96,    -1,   114,   115,    73,    96,    -1,   114,     1,
      73,    96,    -1,   114,     1,   175,     1,    73,    96,    -1,
     116,    17,    96,   161,    -1,   116,    96,    71,    96,   156,
      72,    96,    -1,   116,     1,    -1,   116,    17,    96,     1,
     161,    -1,   116,    17,    96,    -1,   116,    17,    96,     1,
      -1,    13,    96,    -1,    27,    96,   118,   119,    96,    73,
      -1,    27,     1,   174,    -1,    27,     1,    73,    -1,    -1,
      13,     5,    -1,    12,    -1,    66,    -1,    13,    96,    -1,
      -1,    17,    96,   161,    96,    -1,    74,    96,   120,    96,
     121,    75,    96,    -1,   122,    -1,   123,    96,    43,    96,
     122,    -1,    -1,    43,    96,   123,    -1,    -1,    41,    -1,
      42,    -1,   123,    -1,   125,    96,   130,   124,    -1,    -1,
     128,    -1,   126,    -1,   128,    76,    96,   126,    -1,   128,
       1,    -1,    24,    96,   128,    71,    96,   107,   168,    -1,
      24,    96,    71,    96,   107,   168,    -1,    13,    96,    -1,
      31,    96,   132,    96,    71,    96,   133,    72,    -1,    13,
      -1,    12,    -1,    -1,   133,   134,    96,    -1,   135,    96,
      71,    96,   156,    72,    -1,   136,    -1,   135,    96,    76,
      96,   136,    -1,    63,    -1,    13,    -1,    23,     1,   174,
      -1,    23,     1,    73,    -1,    25,    96,    71,    96,   156,
      72,    96,    -1,    25,     1,   174,    -1,    25,     1,    73,
      -1,    77,    96,    -1,    78,    96,    -1,    79,    96,    -1,
      80,    -1,    77,    -1,   142,    71,    96,   156,    99,    -1,
     144,    -1,   142,    76,    96,   144,    -1,   142,     1,    -1,
     144,     5,    -1,   146,    -1,   143,    -1,   143,   146,    -1,
     144,   139,   146,    -1,   144,     1,    -1,    21,    -1,    20,
      21,    -1,    13,    21,    -1,   147,    -1,   147,   148,    -1,
     148,    -1,   145,   147,    -1,   145,   147,   148,    -1,   145,
     148,    -1,    13,    -1,    20,    -1,   149,    -1,   148,   149,
      -1,   148,     1,    -1,    16,    -1,    15,    -1,   150,    -1,
     152,    -1,   155,    -1,    18,    13,    -1,    13,    96,    -1,
      19,    96,   151,    81,    -1,    19,    96,   151,   153,    96,
     154,    96,    81,    -1,    19,    96,   145,   151,    81,    -1,
      19,    96,   145,   151,   153,    96,   154,    96,    81,    -1,
      82,    -1,     7,    -1,     8,    -1,     9,    -1,    10,    -1,
      11,    -1,    13,    -1,    12,    -1,    17,    13,    -1,    17,
      17,    13,    -1,    17,    67,    14,    75,    -1,    17,    67,
      65,    75,    -1,    17,    67,    13,    75,    -1,    17,    68,
      96,   146,    96,    75,    -1,   158,    -1,   157,   158,    -1,
     157,    -1,     1,   175,     1,    -1,     1,    -1,   157,     1,
      -1,   157,   175,    -1,   158,    73,    96,    -1,   158,   175,
      73,    96,    -1,     1,    73,    96,    -1,     1,   175,     1,
      73,    96,    -1,   157,   158,    73,    96,    -1,   157,     1,
      73,    96,    -1,   157,     1,   175,     1,    73,    96,    -1,
     159,    17,    96,   161,   160,    -1,   165,    96,    -1,   159,
       1,    -1,   159,    17,    96,     1,   161,   160,    -1,   159,
      17,    96,   161,   160,     1,    -1,    40,    96,    -1,   159,
      17,    96,    -1,   159,    17,    96,     1,    -1,   159,   174,
      -1,    13,    96,    -1,    40,    96,    -1,    -1,   163,    -1,
     161,   162,   163,    -1,   161,     1,    -1,    83,    96,    -1,
      76,    96,    -1,    -1,   164,    -1,   140,   164,    -1,    12,
      96,    -1,    13,    96,    -1,    62,    96,    -1,   140,    62,
      96,    -1,    66,    96,    -1,    69,    96,    -1,   167,    -1,
      84,    96,    -1,   166,    -1,   165,    96,    -1,    85,    96,
      -1,    65,    96,    -1,    64,    96,    -1,    63,    96,    -1,
      48,    96,    -1,    49,    96,    -1,    50,    96,    -1,    51,
      96,    -1,    52,    96,    -1,    53,    96,    -1,    54,    96,
      -1,    55,    96,    -1,    56,    96,    -1,    57,    96,    -1,
      58,    96,    -1,    59,    96,    -1,    60,    96,    -1,    61,
      96,    -1,    46,    96,    -1,    45,    96,    -1,    47,    96,
      -1,    44,    96,    -1,    70,    -1,    67,    96,   161,    75,
      96,    -1,    67,    96,     1,    -1,    15,    96,    -1,    16,
      96,    -1,    99,    -1,     1,    99,    -1,    39,     1,   174,
      -1,    39,     1,    73,    -1,   169,    97,    -1,   170,   169,
      97,    -1,   110,    -1,   129,    -1,     1,   174,    -1,    71,
       1,   175,     1,    99,    -1,    71,     1,    99,    -1,   174,
      -1,   175,     1,   174,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   271,   271,   272,   273,   274,   275,   276,   277,   278,
     282,   283,   287,   293,   299,   305,   311,   325,   332,   342,
     343,   346,   348,   349,   352,   354,   359,   360,   364,   370,
     372,   376,   378,   383,   387,   389,   396,   398,   401,   403,
     411,   412,   413,   414,   415,   419,   420,   421,   422,   426,
     427,   438,   439,   440,   441,   445,   446,   447,   448,   449,
     454,   457,   460,   463,   469,   473,   479,   483,   489,   492,
     497,   500,   503,   506,   512,   515,   518,   521,   524,   529,
     532,   538,   542,   546,   550,   554,   559,   566,   572,   577,
     578,   582,   583,   587,   588,   592,   598,   601,   607,   614,
     619,   626,   629,   635,   638,   641,   647,   652,   660,   663,
     667,   672,   677,   683,   686,   692,   698,   705,   706,   710,
     711,   719,   725,   730,   739,   740,   764,   767,   773,   777,
     780,   786,   787,   788,   792,   793,   797,   803,   812,   820,
     826,   832,   835,   839,   855,   875,   881,   882,   883,   887,
     892,   899,   905,   915,   927,   940,   948,   956,   959,   972,
     978,   986,   998,   999,  1000,  1004,  1015,  1026,  1031,  1037,
    1045,  1057,  1060,  1063,  1066,  1069,  1072,  1078,  1079,  1083,
    1113,  1133,  1151,  1169,  1188,  1203,  1206,  1211,  1214,  1217,
    1220,  1223,  1229,  1232,  1235,  1238,  1241,  1246,  1249,  1255,
    1269,  1281,  1285,  1292,  1297,  1302,  1307,  1312,  1319,  1325,
    1326,  1330,  1335,  1349,  1355,  1358,  1361,  1367,  1368,  1369,
    1370,  1376,  1377,  1378,  1379,  1380,  1381,  1383,  1386,  1389,
    1395,  1396,  1397,  1398,  1399,  1400,  1401,  1402,  1403,  1404,
    1405,  1406,  1407,  1408,  1409,  1410,  1411,  1412,  1413,  1414,
    1415,  1426,  1434,  1443,  1459,  1460,  1467,  1470,  1476,  1479,
    1485,  1486,  1490,  1496,  1502,  1520,  1521,  1525,  1526
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "TOKEN_EOF", "error", "$undefined", "LOWEST_PREC", "UNIMPORTANT_TOK",
  "WHITESPACE", "SGML_CD", "INCLUDES", "DASHMATCH", "BEGINSWITH",
  "ENDSWITH", "CONTAINS", "STRING", "IDENT", "NTH", "HEX", "IDSEL", "':'",
  "'.'", "'['", "'*'", "'|'", "IMPORT_SYM", "PAGE_SYM", "MEDIA_SYM",
  "FONT_FACE_SYM", "CHARSET_SYM", "NAMESPACE_SYM", "WEBKIT_RULE_SYM",
  "WEBKIT_DECLS_SYM", "WEBKIT_KEYFRAME_RULE_SYM", "WEBKIT_KEYFRAMES_SYM",
  "WEBKIT_VALUE_SYM", "WEBKIT_MEDIAQUERY_SYM", "WEBKIT_SELECTOR_SYM",
  "WEBKIT_VARIABLES_SYM", "WEBKIT_DEFINE_SYM", "VARIABLES_FOR",
  "WEBKIT_VARIABLES_DECLS_SYM", "ATKEYWORD", "IMPORTANT_SYM", "MEDIA_ONLY",
  "MEDIA_NOT", "MEDIA_AND", "REMS", "QEMS", "EMS", "EXS", "PXS", "CMS",
  "MMS", "INS", "PTS", "PCS", "DEGS", "RADS", "GRADS", "TURNS", "MSECS",
  "SECS", "HERZ", "KHERZ", "DIMEN", "PERCENTAGE", "FLOATTOKEN", "INTEGER",
  "URI", "FUNCTION", "NOTFUNCTION", "UNICODERANGE", "VARCALL", "'{'",
  "'}'", "';'", "'('", "')'", "','", "'+'", "'~'", "'>'", "'-'", "']'",
  "'='", "'/'", "'#'", "'%'", "$accept", "stylesheet",
  "valid_rule_or_import", "webkit_rule", "webkit_keyframe_rule",
  "webkit_decls", "webkit_variables_decls", "webkit_value",
  "webkit_mediaquery", "webkit_selector", "maybe_space", "maybe_sgml",
  "maybe_charset", "closing_brace", "charset", "import_list",
  "variables_list", "namespace_list", "rule_list", "valid_rule", "rule",
  "block_rule_list", "block_valid_rule", "block_rule", "import",
  "variables_rule", "variables_media_list", "variables_declaration_list",
  "variables_decl_list", "variables_declaration", "variable_name",
  "namespace", "maybe_ns_prefix", "string_or_uri", "media_feature",
  "maybe_media_value", "media_query_exp", "media_query_exp_list",
  "maybe_and_media_query_exp_list", "maybe_media_restrictor",
  "media_query", "maybe_media_list", "media_list", "media", "medium",
  "keyframes", "keyframe_name", "keyframes_rule", "keyframe_rule",
  "key_list", "key", "page", "font_face", "combinator", "unary_operator",
  "ruleset", "selector_list", "selector_with_trailing_whitespace",
  "selector", "namespace_selector", "simple_selector", "element_name",
  "specifier_list", "specifier", "class", "attr_name", "attrib", "match",
  "ident_or_string", "pseudo", "declaration_list", "decl_list",
  "declaration", "property", "prio", "expr", "operator", "term",
  "unary_term", "variable_reference", "function", "hexcolor", "save_block",
  "invalid_at", "invalid_at_list", "invalid_import", "invalid_media",
  "invalid_rule", "invalid_block", "invalid_block_list", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,    58,    46,    91,
      42,   124,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   123,   125,    59,    40,    41,    44,    43,   126,    62,
      45,    93,    61,    47,    35,    37
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    86,    87,    87,    87,    87,    87,    87,    87,    87,
      88,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      96,    97,    97,    97,    98,    98,    99,    99,   100,   100,
     100,   101,   101,   101,   102,   102,   103,   103,   104,   104,
     105,   105,   105,   105,   105,   106,   106,   106,   106,   107,
     107,   108,   108,   108,   108,   109,   109,   109,   109,   109,
     110,   110,   110,   110,   111,   111,   112,   112,   113,   113,
     113,   113,   113,   113,   114,   114,   114,   114,   114,   114,
     114,   115,   115,   115,   115,   115,   115,   116,   117,   117,
     117,   118,   118,   119,   119,   120,   121,   121,   122,   123,
     123,   124,   124,   125,   125,   125,   126,   126,   127,   127,
     128,   128,   128,   129,   129,   130,   131,   132,   132,   133,
     133,   134,   135,   135,   136,   136,   137,   137,   138,   138,
     138,   139,   139,   139,   140,   140,   141,   142,   142,   142,
     143,   144,   144,   144,   144,   144,   145,   145,   145,   146,
     146,   146,   146,   146,   146,   147,   147,   148,   148,   148,
     149,   149,   149,   149,   149,   150,   151,   152,   152,   152,
     152,   153,   153,   153,   153,   153,   153,   154,   154,   155,
     155,   155,   155,   155,   155,   156,   156,   156,   156,   156,
     156,   156,   157,   157,   157,   157,   157,   157,   157,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   159,   160,
     160,   161,   161,   161,   162,   162,   162,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   165,   166,   166,   167,   167,   168,   168,   169,   169,
     170,   170,   171,   172,   173,   174,   174,   175,   175
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     7,     2,     2,     2,     2,     2,     2,     2,
       1,     1,     6,     6,     5,     5,     5,     5,     5,     0,
       2,     0,     2,     2,     0,     1,     1,     1,     5,     3,
       3,     0,     3,     1,     0,     3,     0,     3,     0,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       6,     6,     3,     3,     7,     7,     0,     3,     1,     2,
       1,     3,     1,     2,     3,     4,     3,     5,     4,     4,
       6,     4,     7,     2,     5,     3,     4,     2,     6,     3,
       3,     0,     2,     1,     1,     2,     0,     4,     7,     1,
       5,     0,     3,     0,     1,     1,     1,     4,     0,     1,
       1,     4,     2,     7,     6,     2,     8,     1,     1,     0,
       3,     6,     1,     5,     1,     1,     3,     3,     7,     3,
       3,     2,     2,     2,     1,     1,     5,     1,     4,     2,
       2,     1,     1,     2,     3,     2,     1,     2,     2,     1,
       2,     1,     2,     3,     2,     1,     1,     1,     2,     2,
       1,     1,     1,     1,     1,     2,     2,     4,     8,     5,
       9,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       3,     4,     4,     4,     6,     1,     2,     1,     3,     1,
       2,     2,     3,     4,     3,     5,     4,     4,     6,     5,
       2,     2,     6,     6,     2,     3,     4,     2,     2,     2,
       0,     1,     3,     2,     2,     2,     0,     1,     2,     2,
       2,     2,     3,     2,     2,     1,     2,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     5,     3,     2,     2,     1,     2,     3,     3,
       2,     3,     1,     1,     2,     5,     3,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      19,     0,     0,     0,     0,     0,     0,     0,     0,    19,
      19,    19,    19,    19,    19,    19,    24,    19,    19,    19,
      19,    19,    19,    19,     1,     3,     9,     4,     8,     5,
       6,     7,    20,     0,    21,    25,     0,     0,     0,     0,
     103,     0,     0,     0,     0,    31,   155,   161,   160,     0,
       0,    19,   156,   146,     0,     0,    19,     0,    19,    19,
      10,    11,    41,    44,    42,    43,    40,     0,   142,     0,
       0,   141,   149,     0,   157,   162,   163,   164,   189,    19,
      19,   251,     0,     0,   185,     0,    19,   125,   124,    19,
      19,   122,    19,    19,    19,    19,    19,    19,    19,    19,
      19,    19,    19,    19,    19,    19,    19,    19,    19,    19,
      19,    19,    19,    19,    19,    19,    19,    19,    19,    19,
      19,   135,   134,    19,    19,     0,     0,   211,   217,    19,
     227,   225,   104,   105,    19,    99,   106,    19,     0,     0,
      72,    19,     0,     0,    68,     0,     0,    30,    29,    19,
      23,    22,     0,    34,    21,    33,   148,   179,     0,     0,
      19,   165,     0,   147,     0,     0,     0,   103,     0,     0,
       0,     0,   139,    19,    19,   143,   145,   140,    19,    19,
      19,     0,   155,   156,   152,     0,     0,   159,   158,    19,
     267,     0,   208,   204,    14,   190,   186,     0,    19,     0,
     201,    19,   207,   200,     0,     0,   219,   220,   254,   255,
     250,   248,   247,   249,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   221,   232,
     231,   230,   223,     0,   224,   226,   229,    19,   218,   213,
      16,    19,    19,     0,   228,     0,     0,     0,    17,    18,
      19,     0,    87,    15,    73,    69,    19,     0,    83,    19,
       0,     0,     0,     0,    36,    21,   260,    21,   180,     0,
       0,     0,     0,    19,     0,     0,     0,    62,    63,    93,
      94,    19,   127,   126,    19,   110,     0,   130,   129,    19,
     118,   117,    19,    12,     0,     0,   131,   132,   133,   144,
       0,   194,   188,    19,     0,    19,     0,   192,    19,     0,
      13,    19,    19,   253,     0,   222,   215,   214,   212,    19,
      19,    19,    19,   101,    76,    71,    19,     0,    19,    74,
      19,     0,    19,    27,    26,   266,     0,    28,   259,   258,
      19,    19,    38,    21,    32,   261,   183,   181,   182,    19,
     166,    19,     0,   172,   173,   174,   175,   176,   167,   171,
      19,   108,    49,   112,    19,    19,     0,     0,     0,     0,
      19,   268,   197,     0,   196,   193,   206,     0,     0,     0,
      19,    95,    96,     0,   115,    19,   107,    19,    79,     0,
      78,    75,    86,     0,     0,     0,   103,    66,     0,     0,
      21,    35,     0,   169,    19,     0,     0,     0,     0,    49,
     103,     0,    19,   136,   195,    19,     0,    19,     0,     0,
     123,   252,    19,     0,   100,     0,    77,    19,     0,     0,
     265,     0,     0,     0,     0,    91,     0,    45,    21,   262,
      47,    48,    46,    37,   184,     0,   178,   177,    19,    60,
      61,     0,   256,    55,    21,   263,    54,    52,    53,    51,
     114,    57,    58,    59,    56,     0,   111,    19,   119,   198,
     202,   209,   203,   121,     0,    19,   102,    80,    19,    19,
     103,    19,    90,    89,     0,     0,   264,    39,    19,     0,
     257,    50,   113,   128,     0,     0,    98,    82,     0,     0,
       0,    92,    19,     0,   168,   116,    19,    97,     0,     0,
       0,   170,   120,    64,    65,    88
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     8,    59,     9,    10,    11,    12,    13,    14,    15,
     246,    45,    34,   452,    35,   153,   264,   342,   399,    60,
     438,   408,   453,   454,   439,   343,   433,   142,   143,   144,
     145,   400,   485,   281,   320,   423,   135,   136,   386,   137,
     285,   406,   407,    62,   323,    63,   292,   494,    89,    90,
      91,    64,    65,   181,   125,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,   276,    76,   360,   448,    77,
      82,    83,    84,    85,   418,   126,   243,   127,   128,   129,
     130,   131,   460,   461,   155,   462,   463,   464,   190,   191
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -240
static const yytype_int16 yypact[] =
{
     616,    -4,    91,   115,   122,    66,   194,   201,    72,  -240,
    -240,  -240,  -240,  -240,  -240,  -240,    36,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,   244,   244,   244,   244,   244,
     244,   244,  -240,   489,  -240,  -240,   844,   343,    25,  1131,
     347,   557,   333,    -3,    37,    34,   257,  -240,  -240,   310,
     291,  -240,   304,  -240,   495,   346,  -240,   220,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,  -240,  -240,    80,   640,   330,
     703,  -240,   777,   152,  -240,  -240,  -240,  -240,   143,  -240,
    -240,  -240,   278,   433,   212,   203,  -240,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,   964,   917,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,  -240,    41,  -240,   281,   163,
     313,  -240,   290,   174,   326,   243,   362,  -240,  -240,  -240,
    -240,  -240,   375,   359,  -240,   357,  -240,  -240,   387,    22,
    -240,  -240,    32,  -240,   355,   179,   374,   295,   378,   200,
     275,   183,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,
    -240,   640,  -240,  -240,   777,   191,   564,  -240,  -240,  -240,
    -240,   404,   244,   244,  -240,   440,   337,    63,  -240,     4,
    -240,  -240,  -240,   244,   185,   123,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,  1068,   244,   244,   244,  -240,  -240,  -240,
    -240,  -240,  -240,  1190,   244,   294,   233,   305,  -240,  -240,
    -240,   418,   244,  -240,   493,   350,  -240,   129,  -240,  -240,
     206,     3,   167,   513,     8,  -240,    42,  -240,  -240,   388,
     389,   394,   557,   257,   304,   464,   286,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,  -240,   124,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,   343,   557,   244,   244,   244,  -240,
     712,   244,   514,  -240,   479,  -240,   413,   244,  -240,   544,
    -240,  -240,  -240,  -240,   991,   244,   244,   244,  -240,  -240,
    -240,  -240,  -240,   446,   244,   547,  -240,   491,  -240,   244,
    -240,   766,  -240,  -240,  -240,  -240,   492,  -240,  -240,  -240,
    -240,  -240,   481,  -240,    42,    42,  -240,  -240,  -240,  -240,
     244,  -240,   384,  -240,  -240,  -240,  -240,  -240,  -240,  -240,
    -240,   398,   244,  -240,  -240,  -240,   343,   208,   154,   353,
    -240,  -240,   244,   552,   244,   244,  1190,   470,   343,    25,
    -240,   244,   180,   160,   244,  -240,  -240,  -240,   244,   626,
     244,   244,  1190,   618,   343,     3,   301,   227,   486,   921,
    -240,    42,    56,  -240,  -240,   431,   638,    90,   437,   244,
     347,   425,  -240,  -240,   244,  -240,   470,  -240,   181,   466,
    -240,   244,  -240,   473,  -240,   160,   244,  -240,   692,   496,
    -240,   441,   546,   487,   639,   311,   413,  -240,  -240,  -240,
    -240,  -240,  -240,    42,  -240,   431,  -240,  -240,  -240,  -240,
    -240,     3,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,   437,  -240,  -240,   244,   244,
    -240,   244,  -240,  -240,  1131,  -240,    41,   244,  -240,  -240,
     400,  -240,  -240,  -240,   558,    12,  -240,    42,  -240,     1,
    -240,    42,  -240,   244,    13,   840,   244,   244,   333,   146,
     333,  -240,  -240,     2,  -240,  -240,  -240,   244,   499,   540,
     178,  -240,   244,  -240,  -240,  -240
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,
       0,  -125,  -240,  -236,  -240,  -240,  -240,  -240,  -240,   168,
    -240,   213,  -240,  -240,   -20,  -240,  -240,   -84,  -240,   483,
    -240,  -240,  -240,   142,  -240,  -240,   249,   222,  -240,  -240,
     -39,   241,  -165,  -232,  -240,  -126,  -240,  -240,   144,  -240,
     272,  -110,  -106,  -240,  -240,   -91,   611,  -240,   391,   530,
     -60,   630,   -38,   -42,  -240,   421,  -240,   354,   269,  -240,
    -239,  -240,   632,  -240,   308,  -173,  -240,   482,   601,   -33,
    -240,  -240,   267,   -17,  -240,   334,  -240,   335,   -16,   -18
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -217
static const yytype_int16 yytable[] =
{
      16,   138,   286,   333,    86,   306,    32,    32,   175,    25,
      26,    27,    28,    29,    30,    31,    61,    36,    37,    38,
      39,    40,    41,    42,   279,   335,    87,   148,   154,   266,
      32,   188,   185,    44,   186,   269,   270,    32,    87,   150,
     151,    32,    32,   340,   341,   273,   -19,   150,   151,   149,
      86,   162,   274,    53,   165,   368,   167,   169,   170,   171,
     314,    32,    33,  -191,   306,   197,   199,    17,   146,   202,
     147,    21,    24,   152,   146,   334,    88,   308,   280,   192,
     193,   172,   504,   511,   -19,   505,   203,   271,    88,   204,
     205,   363,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   299,   251,   235,   236,   363,   257,   411,    32,   244,
     306,   444,   413,   265,   245,  -191,   377,   247,   267,   419,
     344,   252,   345,   188,   188,   260,   300,   363,   278,   262,
     283,   173,   288,   187,   333,   429,   174,  -151,   393,   430,
     272,  -109,    18,  -109,   172,    32,   365,    47,    48,    49,
      50,    51,    32,   294,   295,   254,   455,   304,   296,   297,
     298,  -199,   472,    32,    32,    32,    19,   141,    32,   301,
      32,   279,   187,    20,   311,   364,  -154,   422,   307,   312,
     365,   309,   330,   416,   200,    32,    47,    48,    49,    50,
      51,    32,   349,    32,   146,   490,   189,   -67,   401,   428,
     201,   168,   365,  -151,  -151,   -19,   334,  -151,  -151,  -151,
    -151,  -151,    32,   455,   134,   249,   327,   315,    32,   174,
     337,   316,   317,   336,   258,   280,   -70,   339,   -19,    32,
     324,   515,  -199,  -199,  -199,   293,   329,   310,   188,   331,
     259,    86,  -154,  -154,   432,    22,  -154,  -154,  -154,  -154,
    -154,   289,    23,   350,   146,   443,   321,   332,   156,   412,
      32,   361,   456,   146,   362,   198,   371,   290,   291,   366,
     371,   -19,   367,   353,   354,   355,   356,   357,   457,    32,
      32,   495,   458,   372,   161,   374,    32,   319,   375,   371,
      32,   378,   379,   487,   -19,   499,    32,   459,   322,   381,
     382,   383,   384,   157,   484,   163,   388,   158,   390,   491,
     391,   176,   394,    86,   140,   177,   132,   133,    32,   456,
     396,   397,   132,   133,    78,    86,   141,   166,    32,   402,
     194,   350,    32,   248,   176,   457,    79,   371,   177,   458,
     405,    86,   253,   261,   409,   410,   284,   358,   359,   134,
     414,   466,  -108,   371,   459,   134,   263,   159,   160,   371,
     421,    54,   440,    80,   146,   425,   250,   426,   132,   133,
     450,   353,   354,   355,   356,   357,   152,   146,   435,   256,
     268,  -137,  -137,    32,   445,   302,  -137,   178,   179,   180,
     305,  -103,   468,    81,   508,   469,   509,   471,   483,   325,
     486,   134,   474,   328,  -138,  -138,   146,   477,   277,  -138,
     178,   179,   180,  -187,   195,   486,    32,   333,   451,   132,
     133,   132,   133,   446,   447,   146,    79,   282,   489,   146,
      46,   287,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,   346,   347,   403,   359,   493,    58,   348,
    -210,   239,   134,    80,   134,   496,   152,   351,   497,   498,
     373,   500,  -216,  -216,   146,  -216,  -216,   434,   503,   385,
      43,   -19,   389,   395,   -19,   507,   164,   467,   -19,   -19,
     -19,   -19,   510,    81,   146,  -187,   512,   -19,   398,   334,
     417,   146,   479,   303,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,   473,  -216,
    -216,  -210,  -210,  -210,  -205,   376,   241,  -216,   475,    32,
    -216,   480,   -19,   242,  -216,  -216,    92,    93,   481,    94,
      95,   -19,    32,   501,   146,   187,   326,   437,   478,  -150,
      46,   513,    47,    48,    49,    50,    51,    52,    53,    47,
      48,    49,    50,    51,   146,   146,   338,   370,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   514,   120,    81,  -205,  -205,  -205,   146,   239,
     387,   121,   465,   146,   122,   415,   255,   502,   123,   124,
    -216,  -216,   424,  -216,  -216,  -150,  -150,   431,   506,  -150,
    -150,  -150,  -150,  -150,     1,     2,     3,   476,     4,     5,
       6,   420,   139,    46,     7,    47,    48,    49,    50,    51,
      52,    53,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,   369,  -216,  -216,   -81,
     -81,   -81,   275,   239,   241,  -216,   352,   146,  -216,   427,
     184,   242,  -216,  -216,  -216,  -216,   404,  -216,  -216,   146,
     146,   449,   482,   187,   488,   196,   182,  -153,    47,    48,
      49,    50,    51,   183,   470,   318,   238,    47,    48,    49,
      50,    51,   492,   441,   442,     0,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
       0,  -216,  -216,   -84,   -84,   -84,     0,   392,   241,  -216,
       0,    32,  -216,     0,     0,   242,  -216,  -216,    92,    93,
       0,    94,    95,  -153,  -153,     0,     0,  -153,  -153,  -153,
    -153,  -153,    47,    48,    49,    50,    51,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,     0,   120,    81,   -85,   -85,   -85,
       0,   239,     0,   121,     0,   -19,   122,     0,     0,    32,
     123,   124,  -216,  -216,     0,  -216,  -216,    46,     0,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
       0,     0,     0,     0,     0,    58,     0,     0,     0,     0,
       0,     0,     0,     0,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,     0,  -216,
    -216,     0,     0,     0,     0,   -19,   241,  -216,   239,     0,
    -216,    -2,   436,   242,  -216,  -216,     0,     0,     0,  -216,
    -216,     0,  -216,  -216,    46,     0,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,     0,     0,     0,
       0,     0,    58,     0,     0,     0,     0,     0,     0,     0,
     152,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,     0,  -216,  -216,     0,   240,
       0,     0,   239,   241,  -216,     0,     0,  -216,     0,     0,
     242,  -216,  -216,  -216,  -216,     0,  -216,  -216,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   237,   115,   116,   117,
       0,     0,     0,     0,     0,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,     0,
    -216,  -216,     0,     0,     0,     0,   380,   241,  -216,   313,
       0,  -216,     0,    32,   242,  -216,  -216,     0,     0,     0,
      92,    93,     0,    94,    95,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    32,   120,    81,     0,
       0,     0,     0,    92,    93,   121,    94,    95,   122,     0,
       0,     0,   123,   124,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,     0,
     120,    81,    92,    93,     0,    94,    95,     0,   121,     0,
       0,   122,     0,     0,     0,   123,   124,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,     0,   120,
      81,     0,     0,     0,     0,     0,     0,   121,     0,     0,
     122,     0,     0,     0,   123,   124
};

static const yytype_int16 yycheck[] =
{
       0,    40,   167,     0,    37,     1,     5,     5,    68,     9,
      10,    11,    12,    13,    14,    15,    36,    17,    18,    19,
      20,    21,    22,    23,    12,   261,    13,    43,    45,   154,
       5,    73,    70,    33,    72,    13,    14,     5,    13,     5,
       6,     5,     5,    35,    36,    13,     5,     5,     6,    12,
      83,    51,    20,    21,    54,   294,    56,    57,    58,    59,
     233,     5,    26,     0,     1,    83,    84,    71,    71,    85,
      73,     5,     0,    39,    71,    72,    63,    73,    66,    79,
      80,     1,    81,    81,    43,    72,    86,    65,    63,    89,
      90,     1,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   181,   140,   123,   124,     1,   144,   366,     5,   129,
       1,    75,   368,   153,   134,    72,   309,   137,   155,   378,
     265,   141,   267,   185,   186,   145,   184,     1,   164,   149,
     166,    71,   168,     1,     0,   394,    76,     5,   331,   395,
     160,    71,    71,    73,     1,     5,    76,    15,    16,    17,
      18,    19,     5,   173,   174,     1,   408,   195,   178,   179,
     180,     0,     1,     5,     5,     5,    71,    13,     5,   189,
       5,    12,     1,    71,    71,    71,     5,    17,   198,    76,
      76,   201,    73,   376,     1,     5,    15,    16,    17,    18,
      19,     5,   272,     5,    71,   451,    73,    71,   343,   392,
      17,     1,    76,    71,    72,     5,    72,    75,    76,    77,
      78,    79,     5,   465,    74,    72,   254,   237,     5,    76,
      73,   241,   242,   261,     1,    66,    72,   263,     5,     5,
     250,    73,    71,    72,    73,    72,   256,    72,   300,   259,
      17,   294,    71,    72,    37,    71,    75,    76,    77,    78,
      79,    71,    71,   273,    71,   400,    43,    71,    21,    71,
       5,   281,   408,    71,   284,    73,   302,    12,    13,   289,
     306,    71,   292,     7,     8,     9,    10,    11,   408,     5,
       5,   474,   408,   303,    13,   305,     5,    13,   308,   325,
       5,   311,   312,   438,    71,   480,     5,   408,    13,   319,
     320,   321,   322,    13,    13,    21,   326,    17,   328,   454,
     330,     1,   332,   366,     1,     5,    41,    42,     5,   465,
     340,   341,    41,    42,     1,   378,    13,     1,     5,   349,
      72,   351,     5,    72,     1,   465,    13,   373,     5,   465,
     360,   394,    72,     1,   364,   365,    71,    81,    82,    74,
     370,   410,    71,   389,   465,    74,     1,    67,    68,   395,
     380,    22,   399,    40,    71,   385,    73,   387,    41,    42,
     406,     7,     8,     9,    10,    11,    39,    71,   398,    73,
      13,    71,    72,     5,   404,     1,    76,    77,    78,    79,
      73,    13,   412,    70,   498,   415,   500,   417,   434,     1,
     436,    74,   422,    73,    71,    72,    71,   427,    73,    76,
      77,    78,    79,     0,     1,   451,     5,     0,     1,    41,
      42,    41,    42,    12,    13,    71,    13,    73,   448,    71,
      13,    73,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    75,    75,    81,    82,   467,    31,    75,
       0,     1,    74,    40,    74,   475,    39,    13,   478,   479,
       1,   481,    12,    13,    71,    15,    16,     1,   488,    43,
       1,     5,     1,     1,     5,   495,     1,    72,    12,    13,
       5,    12,   502,    70,    71,    72,   506,    12,    27,    72,
      40,    71,    71,    73,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    72,    69,
      70,    71,    72,    73,     0,     1,    76,    77,    75,     5,
      80,     5,    66,    83,    84,    85,    12,    13,    71,    15,
      16,    66,     5,     5,    71,     1,    73,   399,    72,     5,
      13,    72,    15,    16,    17,    18,    19,    20,    21,    15,
      16,    17,    18,    19,    71,    71,    73,    73,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    72,    69,    70,    71,    72,    73,    71,     1,
      73,    77,   409,    71,    80,    73,   143,   485,    84,    85,
      12,    13,   383,    15,    16,    71,    72,   396,   494,    75,
      76,    77,    78,    79,    28,    29,    30,   425,    32,    33,
      34,   379,    41,    13,    38,    15,    16,    17,    18,    19,
      20,    21,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,   295,    69,    70,    71,
      72,    73,   162,     1,    76,    77,   275,    71,    80,    73,
      70,    83,    84,    85,    12,    13,   352,    15,    16,    71,
      71,    73,    73,     1,   445,    83,    13,     5,    15,    16,
      17,    18,    19,    20,   416,   243,   125,    15,    16,    17,
      18,    19,   465,   399,   399,    -1,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      -1,    69,    70,    71,    72,    73,    -1,     1,    76,    77,
      -1,     5,    80,    -1,    -1,    83,    84,    85,    12,    13,
      -1,    15,    16,    71,    72,    -1,    -1,    75,    76,    77,
      78,    79,    15,    16,    17,    18,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    -1,    69,    70,    71,    72,    73,
      -1,     1,    -1,    77,    -1,     5,    80,    -1,    -1,     5,
      84,    85,    12,    13,    -1,    15,    16,    13,    -1,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    -1,    69,
      70,    -1,    -1,    -1,    -1,    75,    76,    77,     1,    -1,
      80,     0,     1,    83,    84,    85,    -1,    -1,    -1,    12,
      13,    -1,    15,    16,    13,    -1,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      39,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    -1,    69,    70,    -1,    72,
      -1,    -1,     1,    76,    77,    -1,    -1,    80,    -1,    -1,
      83,    84,    85,    12,    13,    -1,    15,    16,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    -1,
      69,    70,    -1,    -1,    -1,    -1,    75,    76,    77,     1,
      -1,    80,    -1,     5,    83,    84,    85,    -1,    -1,    -1,
      12,    13,    -1,    15,    16,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     5,    69,    70,    -1,
      -1,    -1,    -1,    12,    13,    77,    15,    16,    80,    -1,
      -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    -1,
      69,    70,    12,    13,    -1,    15,    16,    -1,    77,    -1,
      -1,    80,    -1,    -1,    -1,    84,    85,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    -1,    69,
      70,    -1,    -1,    -1,    -1,    -1,    -1,    77,    -1,    -1,
      80,    -1,    -1,    -1,    84,    85
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    28,    29,    30,    32,    33,    34,    38,    87,    89,
      90,    91,    92,    93,    94,    95,    96,    71,    71,    71,
      71,     5,    71,    71,     0,    96,    96,    96,    96,    96,
      96,    96,     5,    26,    98,   100,    96,    96,    96,    96,
      96,    96,    96,     1,    96,    97,    13,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    31,    88,
     105,   110,   129,   131,   137,   138,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   152,   155,     1,    13,
      40,    70,   156,   157,   158,   159,   165,    13,    63,   134,
     135,   136,    12,    13,    15,    16,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      69,    77,    80,    84,    85,   140,   161,   163,   164,   165,
     166,   167,    41,    42,    74,   122,   123,   125,   126,   142,
       1,    13,   113,   114,   115,   116,    71,    73,   174,    12,
       5,     6,    39,   101,   169,   170,    21,    13,    17,    67,
      68,    13,    96,    21,     1,    96,     1,    96,     1,    96,
      96,    96,     1,    71,    76,   146,     1,     5,    77,    78,
      79,   139,    13,    20,   147,   148,   148,     1,   149,    73,
     174,   175,    96,    96,    72,     1,   158,   175,    73,   175,
       1,    17,   174,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    62,   164,     1,
      72,    76,    83,   162,    96,    96,    96,    96,    72,    72,
      73,   175,    96,    72,     1,   115,    73,   175,     1,    17,
      96,     1,    96,     1,   102,   110,    97,   169,    13,    13,
      14,    65,    96,    13,    20,   145,   151,    73,   174,    12,
      66,   119,    73,   174,    71,   126,   128,    73,   174,    71,
      12,    13,   132,    72,    96,    96,    96,    96,    96,   146,
     148,    96,     1,    73,   175,    73,     1,    96,    73,    96,
      72,    71,    76,     1,   161,    96,    96,    96,   163,    13,
     120,    43,    13,   130,    96,     1,    73,   175,    73,    96,
      73,    96,    71,     0,    72,    99,   175,    73,    73,   174,
      35,    36,   103,   111,    97,    97,    75,    75,    75,   146,
      96,    13,   151,     7,     8,     9,    10,    11,    81,    82,
     153,    96,    96,     1,    71,    76,    96,    96,   156,   144,
      73,   174,    96,     1,    96,    96,     1,   161,    96,    96,
      75,    96,    96,    96,    96,    43,   124,    73,    96,     1,
      96,    96,     1,   161,    96,     1,    96,    96,    27,   104,
     117,    97,    96,    81,   153,    96,   127,   128,   107,    96,
      96,   156,    71,    99,    96,    73,   161,    40,   160,   156,
     136,    96,    17,   121,   122,    96,    96,    73,   161,   156,
      99,   127,    37,   112,     1,    96,     1,   105,   106,   110,
     169,   171,   173,    97,    75,    96,    12,    13,   154,    73,
     174,     1,    99,   108,   109,   129,   131,   137,   138,   141,
     168,   169,   171,   172,   173,   107,   126,    72,    96,    96,
     160,    96,     1,    72,    96,    75,   123,    96,    72,    71,
       5,    71,    73,   174,    13,   118,   174,    97,   154,    96,
      99,    97,   168,    96,   133,   161,    96,    96,    96,   128,
      96,     5,   119,    96,    81,    72,   134,    96,   113,   113,
      96,    81,    96,    72,    72,    73
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 12:

/* Line 1455 of yacc.c  */
#line 287 "../css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_rule = (yyvsp[(4) - (6)].rule);
    ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 293 "../css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_keyframe = (yyvsp[(4) - (6)].keyframeRule);
    ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 299 "../css/CSSGrammar.y"
    {
        /* can be empty */
    ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 305 "../css/CSSGrammar.y"
    {
        /* can be empty */
    ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 311 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyvsp[(4) - (5)].valueList)) {
            p->m_valueList = p->sinkFloatingValueList((yyvsp[(4) - (5)].valueList));
            int oldParsedProperties = p->m_numParsedProperties;
            if (!p->parseValue(p->m_id, p->m_important))
                p->rollbackLastProperties(p->m_numParsedProperties - oldParsedProperties);
            delete p->m_valueList;
            p->m_valueList = 0;
        }
    ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 325 "../css/CSSGrammar.y"
    {
         CSSParser* p = static_cast<CSSParser*>(parser);
         p->m_mediaQuery = p->sinkFloatingMediaQuery((yyvsp[(4) - (5)].mediaQuery));
     ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 332 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(4) - (5)].selectorList)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->m_selectorListForParseSelector)
                p->m_selectorListForParseSelector->adoptSelectorVector(*(yyvsp[(4) - (5)].selectorList));
        }
    ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 354 "../css/CSSGrammar.y"
    {
  ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 364 "../css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     (yyval.rule) = static_cast<CSSParser*>(parser)->createCharsetRule((yyvsp[(3) - (5)].string));
     if ((yyval.rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyval.rule));
  ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 370 "../css/CSSGrammar.y"
    {
  ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 372 "../css/CSSGrammar.y"
    {
  ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 378 "../css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyvsp[(2) - (3)].rule));
 ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 383 "../css/CSSGrammar.y"
    {
 ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 389 "../css/CSSGrammar.y"
    {
    CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyvsp[(2) - (3)].rule));
;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 403 "../css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyvsp[(2) - (3)].rule));
 ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 426 "../css/CSSGrammar.y"
    { (yyval.ruleList) = 0; ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 427 "../css/CSSGrammar.y"
    {
      (yyval.ruleList) = (yyvsp[(1) - (3)].ruleList);
      if ((yyvsp[(2) - (3)].rule)) {
          if (!(yyval.ruleList))
              (yyval.ruleList) = static_cast<CSSParser*>(parser)->createRuleList();
          (yyval.ruleList)->append((yyvsp[(2) - (3)].rule));
      }
  ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 454 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createImportRule((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].mediaList));
    ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 457 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 460 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 463 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 469 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createVariablesRule((yyvsp[(3) - (7)].mediaList), true);
    ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 473 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createVariablesRule((yyvsp[(3) - (7)].mediaList), false);
    ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 479 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = static_cast<CSSParser*>(parser)->createMediaList();
    ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 483 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(3) - (3)].mediaList);
    ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 489 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 492 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
        if ((yyvsp[(2) - (2)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (2)].boolean);
    ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 497 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 500 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 503 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 506 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 512 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 515 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 518 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 521 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 524 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
        if ((yyvsp[(2) - (4)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (4)].boolean);
    ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 529 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
    ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 532 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (6)].boolean);
    ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 538 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = static_cast<CSSParser*>(parser)->addVariable((yyvsp[(1) - (4)].string), (yyvsp[(4) - (4)].valueList));
    ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 542 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = static_cast<CSSParser*>(parser)->addVariableDeclarationBlock((yyvsp[(1) - (7)].string));
    ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 546 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 550 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 554 "../css/CSSGrammar.y"
    {
        /* @variables { varname: } Just reduce away this variable with no value. */
        (yyval.boolean) = false;
    ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 559 "../css/CSSGrammar.y"
    {
        /* if we come across rules with invalid values like this case: @variables { varname: *; }, just discard the property/value pair */
        (yyval.boolean) = false;
    ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 566 "../css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (2)].string);
    ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 572 "../css/CSSGrammar.y"
    {
    CSSParser* p = static_cast<CSSParser*>(parser);
    if (p->m_styleSheet)
        p->m_styleSheet->addNamespace(p, (yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].string));
;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 582 "../css/CSSGrammar.y"
    { (yyval.string).characters = 0; ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 583 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 592 "../css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (2)].string);
    ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 598 "../css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 601 "../css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(3) - (4)].valueList);
    ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 607 "../css/CSSGrammar.y"
    {
        (yyvsp[(3) - (7)].string).lower();
        (yyval.mediaQueryExp) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExp((yyvsp[(3) - (7)].string), (yyvsp[(5) - (7)].valueList));
    ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 614 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaQueryExpList) = p->createFloatingMediaQueryExpList();
        (yyval.mediaQueryExpList)->append(p->sinkFloatingMediaQueryExp((yyvsp[(1) - (1)].mediaQueryExp)));
    ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 619 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(1) - (5)].mediaQueryExpList);
        (yyval.mediaQueryExpList)->append(static_cast<CSSParser*>(parser)->sinkFloatingMediaQueryExp((yyvsp[(5) - (5)].mediaQueryExp)));
    ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 626 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExpList();
    ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 629 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(3) - (3)].mediaQueryExpList);
    ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 635 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::None;
    ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 638 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Only;
    ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 641 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Not;
    ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 647 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaQuery) = p->createFloatingMediaQuery(p->sinkFloatingMediaQueryExpList((yyvsp[(1) - (1)].mediaQueryExpList)));
    ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 652 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyvsp[(3) - (4)].string).lower();
        (yyval.mediaQuery) = p->createFloatingMediaQuery((yyvsp[(1) - (4)].mediaQueryRestrictor), (yyvsp[(3) - (4)].string), p->sinkFloatingMediaQueryExpList((yyvsp[(4) - (4)].mediaQueryExpList)));
    ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 660 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = static_cast<CSSParser*>(parser)->createMediaList();
     ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 667 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaList) = p->createMediaList();
        (yyval.mediaList)->appendMediaQuery(p->sinkFloatingMediaQuery((yyvsp[(1) - (1)].mediaQuery)));
    ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 672 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(1) - (4)].mediaList);
        if ((yyval.mediaList))
            (yyval.mediaList)->appendMediaQuery(static_cast<CSSParser*>(parser)->sinkFloatingMediaQuery((yyvsp[(4) - (4)].mediaQuery)));
    ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 677 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = 0;
    ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 683 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule((yyvsp[(3) - (7)].mediaList), (yyvsp[(6) - (7)].ruleList));
    ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 686 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule(0, (yyvsp[(5) - (6)].ruleList));
    ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 692 "../css/CSSGrammar.y"
    {
      (yyval.string) = (yyvsp[(1) - (2)].string);
  ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 698 "../css/CSSGrammar.y"
    {
        (yyval.rule) = (yyvsp[(7) - (8)].keyframesRule);
        (yyvsp[(7) - (8)].keyframesRule)->setNameInternal((yyvsp[(3) - (8)].string));
    ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 710 "../css/CSSGrammar.y"
    { (yyval.keyframesRule) = static_cast<CSSParser*>(parser)->createKeyframesRule(); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 711 "../css/CSSGrammar.y"
    {
        (yyval.keyframesRule) = (yyvsp[(1) - (3)].keyframesRule);
        if ((yyvsp[(2) - (3)].keyframeRule))
            (yyval.keyframesRule)->append((yyvsp[(2) - (3)].keyframeRule));
    ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 719 "../css/CSSGrammar.y"
    {
        (yyval.keyframeRule) = static_cast<CSSParser*>(parser)->createKeyframeRule((yyvsp[(1) - (6)].valueList));
    ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 725 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 730 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = (yyvsp[(1) - (5)].valueList);
        if ((yyval.valueList))
            (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(5) - (5)].value)));
    ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 739 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 740 "../css/CSSGrammar.y"
    {
        (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER;
        CSSParserString& str = (yyvsp[(1) - (1)].string);
        if (equalIgnoringCase("from", str.characters, str.length))
            (yyval.value).fValue = 0;
        else if (equalIgnoringCase("to", str.characters, str.length))
            (yyval.value).fValue = 100;
        else
            YYERROR;
    ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 764 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 767 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 774 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createFontFaceRule();
    ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 777 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 780 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 786 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::DirectAdjacent; ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 787 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::IndirectAdjacent; ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 788 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::Child; ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 792 "../css/CSSGrammar.y"
    { (yyval.integer) = -1; ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 793 "../css/CSSGrammar.y"
    { (yyval.integer) = 1; ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 797 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createStyleRule((yyvsp[(1) - (5)].selectorList));
    ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 803 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = p->reusableSelectorVector();
            deleteAllValues(*(yyval.selectorList));
            (yyval.selectorList)->shrink(0);
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(1) - (1)].selector)));
        }
    ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 812 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (4)].selectorList) && (yyvsp[(4) - (4)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = (yyvsp[(1) - (4)].selectorList);
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(4) - (4)].selector)));
        } else
            (yyval.selectorList) = 0;
    ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 820 "../css/CSSGrammar.y"
    {
        (yyval.selectorList) = 0;
    ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 826 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (2)].selector);
    ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 832 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 836 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 840 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if (!(yyvsp[(1) - (2)].selector))
            (yyval.selector) = 0;
        else if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSSelector* end = (yyval.selector);
            while (end->tagHistory())
                end = end->tagHistory();
            end->m_relation = CSSSelector::Descendant;
            end->setTagHistory(p->sinkFloatingSelector((yyvsp[(1) - (2)].selector)));
            if (Document* doc = p->document())
                doc->setUsesDescendantRules(true);
        }
    ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 855 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (3)].selector);
        if (!(yyvsp[(1) - (3)].selector))
            (yyval.selector) = 0;
        else if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSSelector* end = (yyval.selector);
            while (end->tagHistory())
                end = end->tagHistory();
            end->m_relation = (yyvsp[(2) - (3)].relation);
            end->setTagHistory(p->sinkFloatingSelector((yyvsp[(1) - (3)].selector)));
            if ((yyvsp[(2) - (3)].relation) == CSSSelector::Child) {
                if (Document* doc = p->document())
                    doc->setUsesDescendantRules(true);
            } else if ((yyvsp[(2) - (3)].relation) == CSSSelector::DirectAdjacent || (yyvsp[(2) - (3)].relation) == CSSSelector::IndirectAdjacent) {
                if (Document* doc = p->document())
                    doc->setUsesSiblingRules(true);
            }
        }
    ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 875 "../css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 881 "../css/CSSGrammar.y"
    { (yyval.string).characters = 0; (yyval.string).length = 0; ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 882 "../css/CSSGrammar.y"
    { static UChar star = '*'; (yyval.string).characters = &star; (yyval.string).length = 1; ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 883 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 887 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(1) - (1)].string), p->m_defaultNamespace);
    ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 892 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(1) - (2)].string), p->m_defaultNamespace);
        }
    ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 899 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyval.selector) && p->m_defaultNamespace != starAtom)
            (yyval.selector)->m_tag = QualifiedName(nullAtom, starAtom, p->m_defaultNamespace);
    ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 905 "../css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(1) - (2)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        if (p->m_styleSheet)
            (yyval.selector)->m_tag = QualifiedName(namespacePrefix, (yyvsp[(2) - (2)].string),
                                      p->m_styleSheet->determineNamespace(namespacePrefix));
        else // FIXME: Shouldn't this case be an error?
            (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(2) - (2)].string), p->m_defaultNamespace);
    ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 915 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (3)].selector);
        if ((yyval.selector)) {
            AtomicString namespacePrefix = (yyvsp[(1) - (3)].string);
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->m_styleSheet)
                (yyval.selector)->m_tag = QualifiedName(namespacePrefix, (yyvsp[(2) - (3)].string),
                                          p->m_styleSheet->determineNamespace(namespacePrefix));
            else // FIXME: Shouldn't this case be an error?
                (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(2) - (3)].string), p->m_defaultNamespace);
        }
    ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 927 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector)) {
            AtomicString namespacePrefix = (yyvsp[(1) - (2)].string);
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->m_styleSheet)
                (yyval.selector)->m_tag = QualifiedName(namespacePrefix, starAtom,
                                          p->m_styleSheet->determineNamespace(namespacePrefix));
        }
    ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 940 "../css/CSSGrammar.y"
    {
        CSSParserString& str = (yyvsp[(1) - (1)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->document();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 948 "../css/CSSGrammar.y"
    {
        static UChar star = '*';
        (yyval.string).characters = &star;
        (yyval.string).length = 1;
    ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 956 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 959 "../css/CSSGrammar.y"
    {
        if (!(yyvsp[(2) - (2)].selector))
            (yyval.selector) = 0;
        else if ((yyvsp[(1) - (2)].selector)) {
            (yyval.selector) = (yyvsp[(1) - (2)].selector);
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSSelector* end = (yyvsp[(1) - (2)].selector);
            while (end->tagHistory())
                end = end->tagHistory();
            end->m_relation = CSSSelector::SubSelector;
            end->setTagHistory(p->sinkFloatingSelector((yyvsp[(2) - (2)].selector)));
        }
    ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 972 "../css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 978 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::Id;
        if (!p->m_strict)
            (yyvsp[(1) - (1)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(1) - (1)].string);
    ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 986 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].string).characters[0] >= '0' && (yyvsp[(1) - (1)].string).characters[0] <= '9') {
            (yyval.selector) = 0;
        } else {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector) = p->createFloatingSelector();
            (yyval.selector)->m_match = CSSSelector::Id;
            if (!p->m_strict)
                (yyvsp[(1) - (1)].string).lower();
            (yyval.selector)->m_value = (yyvsp[(1) - (1)].string);
        }
    ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 1004 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::Class;
        if (!p->m_strict)
            (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(2) - (2)].string);
    ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 1015 "../css/CSSGrammar.y"
    {
        CSSParserString& str = (yyvsp[(1) - (2)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->document();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 1026 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (4)].string), nullAtom));
        (yyval.selector)->m_match = CSSSelector::Set;
    ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 1031 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (8)].string), nullAtom));
        (yyval.selector)->m_match = (CSSSelector::Match)(yyvsp[(4) - (8)].integer);
        (yyval.selector)->m_value = (yyvsp[(6) - (8)].string);
    ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 1037 "../css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(3) - (5)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(namespacePrefix, (yyvsp[(4) - (5)].string),
                                   p->m_styleSheet->determineNamespace(namespacePrefix)));
        (yyval.selector)->m_match = CSSSelector::Set;
    ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 1045 "../css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(3) - (9)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(namespacePrefix, (yyvsp[(4) - (9)].string),
                                   p->m_styleSheet->determineNamespace(namespacePrefix)));
        (yyval.selector)->m_match = (CSSSelector::Match)(yyvsp[(5) - (9)].integer);
        (yyval.selector)->m_value = (yyvsp[(7) - (9)].string);
    ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 1057 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Exact;
    ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 1060 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::List;
    ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 1063 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Hyphen;
    ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 1066 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Begin;
    ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 1069 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::End;
    ;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 1072 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Contain;
    ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 1083 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(2) - (2)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoEmpty ||
                 type == CSSSelector::PseudoFirstChild ||
                 type == CSSSelector::PseudoFirstOfType ||
                 type == CSSSelector::PseudoLastChild ||
                 type == CSSSelector::PseudoLastOfType ||
                 type == CSSSelector::PseudoOnlyChild ||
                 type == CSSSelector::PseudoOnlyOfType) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            Document* doc = p->document();
            if (doc)
                doc->setUsesSiblingRules(true);
        } else if (type == CSSSelector::PseudoFirstLine) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (Document* doc = p->document())
                doc->setUsesFirstLineRules(true);
        } else if (type == CSSSelector::PseudoBefore ||
                   type == CSSSelector::PseudoAfter) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (Document* doc = p->document())
                doc->setUsesBeforeAfterRules(true);
        }
    ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 1113 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoElement;
        (yyvsp[(3) - (3)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(3) - (3)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoFirstLine) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (Document* doc = p->document())
                doc->setUsesFirstLineRules(true);
        } else if (type == CSSSelector::PseudoBefore ||
                   type == CSSSelector::PseudoAfter) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (Document* doc = p->document())
                doc->setUsesBeforeAfterRules(true);
        }
    ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 1133 "../css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument((yyvsp[(3) - (4)].string));
        (yyval.selector)->m_value = (yyvsp[(2) - (4)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (p->document())
                p->document()->setUsesSiblingRules(true);
        }
    ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 1151 "../css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument(String::number((yyvsp[(3) - (4)].number)));
        (yyval.selector)->m_value = (yyvsp[(2) - (4)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (p->document())
                p->document()->setUsesSiblingRules(true);
        }
    ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 1169 "../css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument((yyvsp[(3) - (4)].string));
        (yyvsp[(2) - (4)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(2) - (4)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (p->document())
                p->document()->setUsesSiblingRules(true);
        }
    ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 1188 "../css/CSSGrammar.y"
    {
        if (!(yyvsp[(4) - (6)].selector) || (yyvsp[(4) - (6)].selector)->simpleSelector() || (yyvsp[(4) - (6)].selector)->tagHistory())
            (yyval.selector) = 0;
        else {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector) = p->createFloatingSelector();
            (yyval.selector)->m_match = CSSSelector::PseudoClass;
            (yyval.selector)->setSimpleSelector(p->sinkFloatingSelector((yyvsp[(4) - (6)].selector)));
            (yyvsp[(2) - (6)].string).lower();
            (yyval.selector)->m_value = (yyvsp[(2) - (6)].string);
        }
    ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 1203 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 1206 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
        if ( (yyvsp[(2) - (2)].boolean) )
            (yyval.boolean) = (yyvsp[(2) - (2)].boolean);
    ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 1211 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 1214 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 1217 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 1220 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 1223 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 1229 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 1232 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 1235 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 1238 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 1241 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
        if ((yyvsp[(2) - (4)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (4)].boolean);
    ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 1246 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
    ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 1249 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (6)].boolean);
    ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 1255 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyvsp[(1) - (5)].integer) && (yyvsp[(4) - (5)].valueList)) {
            p->m_valueList = p->sinkFloatingValueList((yyvsp[(4) - (5)].valueList));
            int oldParsedProperties = p->m_numParsedProperties;
            (yyval.boolean) = p->parseValue((yyvsp[(1) - (5)].integer), (yyvsp[(5) - (5)].boolean));
            if (!(yyval.boolean))
                p->rollbackLastProperties(p->m_numParsedProperties - oldParsedProperties);
            delete p->m_valueList;
            p->m_valueList = 0;
        }
    ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 1269 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->m_valueList = new CSSParserValueList;
        p->m_valueList->addValue(p->sinkFloatingValue((yyvsp[(1) - (2)].value)));
        int oldParsedProperties = p->m_numParsedProperties;
        (yyval.boolean) = p->parseValue(CSSPropertyWebkitVariableDeclarationBlock, false);
        if (!(yyval.boolean))
            p->rollbackLastProperties(p->m_numParsedProperties - oldParsedProperties);
        delete p->m_valueList;
        p->m_valueList = 0;
    ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 1281 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 1285 "../css/CSSGrammar.y"
    {
        /* The default movable type template has letter-spacing: .none;  Handle this by looking for
        error tokens at the start of an expr, recover the expr and then treat as an error, cleaning
        up and deleting the shifted expr.  */
        (yyval.boolean) = false;
    ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 1292 "../css/CSSGrammar.y"
    {
        /* When we encounter something like p {color: red !important fail;} we should drop the declaration */
        (yyval.boolean) = false;
    ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 1297 "../css/CSSGrammar.y"
    {
        /* Handle this case: div { text-align: center; !important } Just reduce away the stray !important. */
        (yyval.boolean) = false;
    ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 1302 "../css/CSSGrammar.y"
    {
        /* div { font-family: } Just reduce away this property with no value. */
        (yyval.boolean) = false;
    ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 1307 "../css/CSSGrammar.y"
    {
        /* if we come across rules with invalid values like this case: p { weight: *; }, just discard the rule */
        (yyval.boolean) = false;
    ;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 1312 "../css/CSSGrammar.y"
    {
        /* if we come across: div { color{;color:maroon} }, ignore everything within curly brackets */
        (yyval.boolean) = false;
    ;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 1319 "../css/CSSGrammar.y"
    {
        (yyval.integer) = cssPropertyID((yyvsp[(1) - (2)].string));
    ;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 1325 "../css/CSSGrammar.y"
    { (yyval.boolean) = true; ;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 1326 "../css/CSSGrammar.y"
    { (yyval.boolean) = false; ;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 1330 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    ;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 1335 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = (yyvsp[(1) - (3)].valueList);
        if ((yyval.valueList)) {
            if ((yyvsp[(2) - (3)].character)) {
                CSSParserValue v;
                v.id = 0;
                v.unit = CSSParserValue::Operator;
                v.iValue = (yyvsp[(2) - (3)].character);
                (yyval.valueList)->addValue(v);
            }
            (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(3) - (3)].value)));
        }
    ;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 1349 "../css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 1355 "../css/CSSGrammar.y"
    {
        (yyval.character) = '/';
    ;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 1358 "../css/CSSGrammar.y"
    {
        (yyval.character) = ',';
    ;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 1361 "../css/CSSGrammar.y"
    {
        (yyval.character) = 0;
  ;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 1367 "../css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(1) - (1)].value); ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 1368 "../css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(2) - (2)].value); (yyval.value).fValue *= (yyvsp[(1) - (2)].integer); ;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 1369 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_STRING; ;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 1370 "../css/CSSGrammar.y"
    {
      (yyval.value).id = cssValueKeywordID((yyvsp[(1) - (2)].string));
      (yyval.value).unit = CSSPrimitiveValue::CSS_IDENT;
      (yyval.value).string = (yyvsp[(1) - (2)].string);
  ;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 1376 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; ;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 1377 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(2) - (3)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; ;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 1378 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_URI; ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 1379 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_UNICODE_RANGE; ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 1380 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (1)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; ;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 1381 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = CSSParserString(); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; ;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 1383 "../css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (1)].value);
  ;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 1386 "../css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (2)].value);
  ;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 1389 "../css/CSSGrammar.y"
    { /* Handle width: %; */
      (yyval.value).id = 0; (yyval.value).unit = 0;
  ;}
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 1395 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = true; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 1396 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 1397 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PERCENTAGE; ;}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 1398 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PX; ;}
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 1399 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_CM; ;}
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 1400 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MM; ;}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 1401 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_IN; ;}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 1402 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PT; ;}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 1403 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PC; ;}
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 1404 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DEG; ;}
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 1405 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_RAD; ;}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 1406 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_GRAD; ;}
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 1407 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_TURN; ;}
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 1408 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MS; ;}
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 1409 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_S; ;}
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 1410 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_HZ; ;}
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 1411 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_KHZ; ;}
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 1412 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EMS; ;}
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 1413 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSParserValue::Q_EMS; ;}
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 1414 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EXS; ;}
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 1415 "../css/CSSGrammar.y"
    {
      (yyval.value).id = 0;
      (yyval.value).fValue = (yyvsp[(1) - (2)].number);
      (yyval.value).unit = CSSPrimitiveValue::CSS_REMS;
      CSSParser* p = static_cast<CSSParser*>(parser);
      if (Document* doc = p->document())
          doc->setUsesRemUnits(true);
  ;}
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 1426 "../css/CSSGrammar.y"
    {
      (yyval.value).id = 0;
      (yyval.value).string = (yyvsp[(1) - (1)].string);
      (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_VARIABLE_FUNCTION_SYNTAX;
  ;}
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 1434 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        CSSParserFunction* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (5)].string);
        f->args = p->sinkFloatingValueList((yyvsp[(3) - (5)].valueList));
        (yyval.value).id = 0;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    ;}
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 1443 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        CSSParserFunction* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (3)].string);
        f->args = 0;
        (yyval.value).id = 0;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
  ;}
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 1459 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 1460 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 1467 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 1470 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 1476 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 1479 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 1490 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 1496 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 1502 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;



/* Line 1455 of yacc.c  */
#line 4265 "WebCore/tmp/../generated/CSSGrammar.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 1529 "../css/CSSGrammar.y"


