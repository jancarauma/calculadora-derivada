/*
 * struct.h
 * contains commonly-used typedefs and related functions
 * 
 * Copyright (c) 2015 J. G. da Silva <carauma.com>
 *
 * MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef STRUCT_H
#define STRUCT_H

#define MAX_CHAR 2048

typedef enum {false, true} bool;
typedef enum {im_bd, ex_bd} bd_type;
typedef enum {pt_cst, pt_fnc, pt_opr, pt_par, pt_sig, pt_var} ch_type;
typedef enum {cnst, expo, hypl, loga, poly, powr, trig} fn_type;

typedef struct list {
    char *entry;
    struct list *next;
} list;

typedef struct comp {
    struct list *elem;
} comp;

typedef struct term {
    struct list *segm;
} term;

typedef struct block {
    struct list *mult;
    struct list *divi;
} block;

list *init_list();
term *init_term();
comp *init_comp();
block *init_block();

#endif
