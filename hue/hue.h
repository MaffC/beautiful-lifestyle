/*
 * hue.h - header file for hue.c
 */

// includes {{{
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#include <string.h>
#include <stdbool.h>

#include <wiringPi.h>
#include <piGlow.h>
//}}}

// prototypes
// struct defs {{{
typedef struct state state;
typedef struct cmd cmd;
struct state {
	int on;
	int led[6];
};
struct cmd {
	int mode;
	int colour;
	int brightness;
	int fade;
};
//}}}
// function defs {{{
// init functions
void argparse(int,char*[]);
void setup(bool);
// translation functions
int gammaCorrect(int);
int percentage(int);
// output functions
void echo(char*);
void warn(char*);
void fatal(char*);
void usage(void);
void printstate(void);
// state functions
void process(void);
int readstate(void);
int writestate(void);
// rendering functions
void fade(void);
void refreshleds(bool);
//}}}
