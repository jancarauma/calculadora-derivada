/*
 * utility.c
 * utility functions used in other parts of the program
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
#include <string.h>
#include "struct.h"
#include "utility.h"

/* Determines whether the constant parameter has trailing decimals. */
bool has_dec(char *str) {
    if ((strpbrk(str, ".") != NULL) || (strpbrk(str, "/") != NULL) ||
        (strpbrk(str, "*") != NULL) || (strpbrk(str, "+") != NULL) ||
        (strpbrk(str, "-") != NULL) || (has_func(str))) {
        return true;
    } else {
        return false;
    }   
}

/* Determines whether str contains a function. */
bool has_func(char *str) {
    if ((strstr(str, "^") != NULL) || (strstr(str, "ln") != NULL) ||
        (strstr(str, "log") != NULL) || (strstr(str, "sin") != NULL) || 
        (strstr(str, "cos") != NULL) || (strstr(str, "tan") != NULL) || 
        (strstr(str, "csc") != NULL) || (strstr(str, "sec") != NULL) || 
        (strstr(str, "cot") != NULL) || (strstr(str, "sinh") != NULL) || 
        (strstr(str, "cosh") != NULL) || (strstr(str, "tanh") != NULL) || 
        (strstr(str, "csch") != NULL) || (strstr(str, "sech") != NULL) || 
        (strstr(str, "coth") != NULL)) {
        return true;
    } else {
        return false;
    }
}

/* Determines whether any node in a list contains the variable x. */
bool has_var(list *ls) {
    list *ls_curr = ls;                          // not to modify the original list
    while (ls_curr != NULL) {
        if (strpbrk(ls_curr->entry, "x") != NULL) {
            return true;
        }
        ls_curr = ls_curr->next;
    }
    return false;
}

/* Identifies the boundary type: explicit or implicit. */
bd_type id_bd_tp(char ch) {
    if ((ch == '*') || (ch == '/')) {
        return ex_bd;
    } else {
        return im_bd;
    }
}

/* Identifies the character type. */
ch_type id_ch_tp(char ch) {
    if ((ch >= '0') && (ch <= '9') || (ch == 'e')) {
        return pt_cst;
    } else if ((ch == '*') || (ch == '/')) {
        return pt_opr;
    } else if ((ch == '(') || (ch == ')')) {
        return pt_par;
    } else if ((ch == '+') || (ch == '-')) {
        return pt_sig;
    } else if (ch == 'x') {
        return pt_var;
    } else {
        return pt_fnc;
    }
}

/* Identifies the type of the outer-most function. */
fn_type id_fn_tp(char *str) {
    char *str_cpy = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
    strcpy(str_cpy, str);                                  // to avoid modifying the original str

    while (par_enclosed(str_cpy)) {
        str_cpy = rm_par(str_cpy);
    }

    char *pt = strpbrk(str_cpy, "x");
    if (pt == NULL) {
        return cnst;
    }

    pt = strpbrk(str_cpy, "^");
    while (pt != NULL) {
        if (!par_paired(str_cpy, pt - str_cpy)) {
            pt = strpbrk(pt + 1, "^");
        } else {
            break;
        }
    }

    if (pt != NULL) {                                      // has '^'
        if (strpbrk(pt, "x") != NULL) {                    // x appears after the '^'
            return expo;
        } else {                                           // x appears before the '^'
            *pt = 0;

            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                str_cpy++;
            }
            while (par_enclosed(str_cpy)) {                // str truncated
                str_cpy = rm_par(str_cpy);
            }

            if (strpbrk(str_cpy, "x") == NULL) {           // no x found
                return cnst;
            } else if (((strcmp(str_cpy, "x") == 0) || (strcmp(str_cpy, "-x") == 0)) && 
                       ((strstr(pt + 1, "pi") == NULL) && (strpbrk(pt + 1, "e") == NULL)) &&
                       (strpbrk(pt + 1, ".*/+-") == NULL)) {
                return poly;
            } else {
                return powr;
            }
        }
    } else {                                               // no '^'
        while (par_enclosed(str_cpy)) {
            str_cpy = rm_par(str_cpy);
        }

        pt = strpbrk(str_cpy, "lsct");
        while (!par_paired(str_cpy, pt - str_cpy)) {
            pt = strpbrk(pt + 1, "lsct");
        }

        if (pt != NULL) {
            if ((strncmp(pt, "ln", 2) == 0) || (strncmp(pt, "log", 3) == 0)) {
                return loga;
            } else if ((strncmp(pt, "sinh", 4) == 0) || (strncmp(pt, "cosh", 4) == 0) ||
                       (strncmp(pt, "tanh", 4) == 0) || (strncmp(pt, "csch", 4) == 0) ||
                       (strncmp(pt, "sech", 4) == 0) || (strncmp(pt, "coth", 4) == 0)) {
                return hypl;
            } else if ((strncmp(pt, "sin", 3) == 0) || (strncmp(pt, "cos", 3) == 0) ||
                       (strncmp(pt, "tan", 3) == 0) || (strncmp(pt, "csc", 3) == 0) ||
                       (strncmp(pt, "sec", 3) == 0) || (strncmp(pt, "cot", 3) == 0)) {
                return trig;
            }
        } else {
            return poly;
        }
    }
}

/* Converts an integer to a str. */
char *int_str(int n) {
    bool neg;
    if (n < 0) {
        neg = true;
        n *= -1;
    } else {
        neg = false;
    }

    char *pt;
    char *str = (char *) malloc(sizeof(char) * 128);
    if (neg) {
        strcpy(str, "-");
        pt = str + 1;
    } else {
        pt = str;
    }
    
    int fct = 0, val = 1;
    while (n >= val) {
        fct++;
        val *= 10;
    }
    fct--;
    val /= 10;

    char ch;
    while (fct >= 0) {
        ch = (n / val) + 48;
        n %= val;
        *pt = ch;
        pt++;
        fct--;
        val /= 10;
    }
    *pt = 0;

    return str;
}

/* Returns the number of nodes in a linked list. */
int n_list(list* ls) {
    int n = 0;
    while (ls != NULL) {
        n++;
        ls = ls->next;
    }
    return n;
}

/* Examines whether str is redundantly enclosed by parentheses. */
bool par_enclosed(char *str) {
    if (str[0] == '(') {
        int len = strlen(str);
        if (str[len - 1] == ')') {
            int ind = 1, par = 1;
            while (ind < len) {
                if (str[ind] == '(') {
                    par++;
                } else if (str[ind] == ')') {
                    par--;
                }

                if ((par == 0) && (ind != (len - 1))) {
                    return false;
                }
                
                ind++;
            }
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

/* Examines all parentheses which appears upto index i are paired up. */
bool par_paired(char *str, int i) {
    int par = 0;

    while (--i >= 0) {
        if (str[i] == '(') {
            par++;
        } else if (str[i] == ')') {
            par--;
        }
    }
    
    if (par == 0) {
        return true;
    } else {
        return false;
    }    
}

/* Creates a new list that is the reverse of the parameter. */
list *rev_list(list *ls) {
    list *rev_curr = NULL;

    while (ls != NULL) {
        list *rev = init_list();
        strcpy(rev->entry, ls->entry);

        if (rev_curr != NULL) {
            rev->next = rev_curr;
        }
        rev_curr = rev;

        ls = ls->next;
    }

    return rev_curr;
}

/* Removes a pair of redundant parentheses enclosing str. */
char *rm_par(char *str) {
    char *str_cpy = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
    strcpy(str_cpy, str);

    str_cpy[strlen(str_cpy) - 1] = 0;
    return str_cpy + 1;
}

/* Converts a str into an int. */
int str_int(char *str) {
    char *str_cpy = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
    strcpy(str_cpy, str);

    bool neg = false;
    if (str_cpy[0] == '-') {
        neg = true;
        str_cpy++;
    }

    int ind, fct = 1, n = 0, len = strlen(str_cpy);
    for (ind = (len - 1); ind >= 0; ind--) {
        n += ((str_cpy[ind] - 48) * fct);
        fct *= 10;
    }
    
    if (neg) {
        return (-1 * n);
    } else {
        return n;
    }
}

/* Removes all blank spaces in str. */
char *wo_space(char *str) {
    int ind = 0, fill = 0, len = strlen(str);
    char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR);

    while (ind < len) {
        char ch;
        if ((ch = str[ind++]) != ' ') {
            rt_str[fill++] = ch;
        }
    }
    rt_str[fill] = 0;

    return rt_str;
}
