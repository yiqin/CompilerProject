#ifndef _COMPILER_LIB
#define _COMPILER_LIB


/* display 's' stdout */
extern int printf( string s );

/* display the integer 'a' sur stdout : */
extern int printd( int a );

/* return nth character of 's' : */
extern int get_char_at( string s, int i );

/* replace nth char of s by c and return c */
extern int put_char_at( string s, int i, int c );

/* return the length of s */
extern int strlen( string s );

#endif /* _COMPILER_LIB */
