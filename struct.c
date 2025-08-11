/*
 * struct.c
 * struct initialization
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

#include <stdlib.h>
#include "struct.h"

list *init_list() {
    list *ls = (list *) malloc(sizeof(list));
    ls->entry = (char *) malloc(sizeof(char) * (MAX_CHAR / 8));
    ls->next = NULL;

    return ls;
}

comp *init_comp() {
    comp *cp = (comp *) malloc(sizeof(comp));
    cp->elem = init_list();
    
    return cp;
}

term *init_term() {
    term *tm = (term *) malloc(sizeof(term));
    tm->segm = init_list();

    return tm;
}

block *init_block() {
    block *bl = (block *) malloc(sizeof(block));
    bl->mult = init_list();
    bl->divi = init_list();

    return bl;
}
