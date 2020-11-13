/***
 *** Authors: Matej Otčenáš, Mário Gažo
 *** Date: 10.11.2020
 *** Filename: main.h
 ***/


/**
 * Header guard
 */
#ifndef _MAIN_H_
#define _MAIN_H_


/**
 * Include of libraries (C/C++)
 */
//#include "simlib.h"
#include <iostream>
#include <math.h>
#include <getopt.h>
#include <string> 
#include <fstream>

/**
 * Macros
 */
 #define START_TIME 0
 #define END_TIME 100


/**
 * Functions
 */
 void parseArgs(int argc, char **argv);
 void calculateSIR();
 void print_help();

 #endif //_MAIN_H_