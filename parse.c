/*
 * parse.c
 * functions pertaining to parsing
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

int n_term(char *str);
int n_block(char *str);

/* Checks whether the character at index i is a boundary. */
bool is_boundary(char *str, int i) {
    if (i == 0) {
        return false;
    } else if (!par_paired(str, i)) {                      // exists unclosed parentheses
        return false;
    } else {                                               // all parentheses are paired
        char ch_0 = str[i], ch_1 = str[i - 1], ch_2 = str[i - 2];

        if ((ch_0 == '*') || (ch_0 == '/')) {              // explicit boundary
            return true;
        } else if (ch_0 == '^') {                          // used as part of exponentiation
            return false;
        } else if ((ch_1 == 'i') && (ch_2 == 'p')) {       // pi
            return true;
        } else if (ch_1 == 'e') {                          // e
            if ((ch_2 != 's') && (ch_0 != 'c')) {
                return true;
            } else {
                return false;
            }
        } else if ((ch_0 == '.') || (ch_1 == '.')) {       // decimal
            return false;
        } else if (ch_0 == '(') {                          // boundary unless used to enclose function arguments
            if (ch_1 == '^') {
                return false;
            } else if ((ch_1 == '+') || (ch_1 == '-')) {   // sign
                return false;
            } else if ((ch_1 == ')') || (ch_1 == '*') || (ch_1 == '/')) {
                return true;
            } else {
                char *prev_2 = (char *) malloc(sizeof(char) * 8);
                char *prev_3 = (char *) malloc(sizeof(char) * 8);
                char *prev_4 = (char *) malloc(sizeof(char) * 8);
                strncpy(prev_2, str + i - 2, 2);
                strncpy(prev_3, str + i - 3, 3);
                strncpy(prev_4, str + i - 4, 4);
                
                if (strcmp(prev_2, "ln") == 0) {
                    return false;
                } else if ((strcmp(prev_4, "sinh") == 0) || (strcmp(prev_4, "cosh") == 0) || 
                           (strcmp(prev_4, "tanh") == 0) || (strcmp(prev_4, "csch") == 0) ||
                           (strcmp(prev_4, "sech") == 0) || (strcmp(prev_4, "coth") == 0)) {
                    return false;
                } else if ((strcmp(prev_3, "sin") == 0) || (strcmp(prev_3, "cos") == 0) || 
                           (strcmp(prev_3, "tan") == 0) || (strcmp(prev_3, "csc") == 0) ||
                           (strcmp(prev_3, "sec") == 0) || (strcmp(prev_3, "cot") == 0) ||
                           (strcmp(prev_3, "log") == 0)) {
                    return false;
                } else {
                    return true;
                }
            }
        } else if (id_ch_tp(ch_0) != id_ch_tp(ch_1)) {     // change in character type
            if (ch_1 == '^') {
                return false;
            } else if ((ch_1 == '+') || (ch_1 == '-')) {   // sign
                return false;
            } else if ((ch_0 == 'e') && (ch_1 == 's')) {
                return false;
            } else {
                char *prev_2 = (char *) malloc(sizeof(char) * 8);
                char *prev_3 = (char *) malloc(sizeof(char) * 8);
                char *prev_4 = (char *) malloc(sizeof(char) * 8);
                strncpy(prev_2, str + i - 2, 2);
                strncpy(prev_3, str + i - 3, 3);
                strncpy(prev_4, str + i - 4, 4);

                if (strcmp(prev_2, "ln") == 0) {
                    return false;
                } else if ((strcmp(prev_4, "sinh") == 0) || (strcmp(prev_4, "cosh") == 0) || 
                           (strcmp(prev_4, "tanh") == 0) || (strcmp(prev_4, "csch") == 0) ||
                           (strcmp(prev_4, "sech") == 0) || (strcmp(prev_4, "coth") == 0)) {
                    return false;
                } else if ((strcmp(prev_3, "sin") == 0) || (strcmp(prev_3, "cos") == 0) || 
                           (strcmp(prev_3, "tan") == 0) || (strcmp(prev_3, "csc") == 0) ||
                           (strcmp(prev_3, "sec") == 0) || (strcmp(prev_3, "cot") == 0) ||
                           (strcmp(prev_3, "log") == 0)) {
                    return false;
                } else {
                    return true;
                }
            }
        } else if (id_ch_tp(ch_0) == pt_var) {             // id_ch_tp(str[i]) = id_ch_tp(str[i - 1]) = pt_var
            return true;
        } else {
            return false;
        }
    }
}

/* Checks whether the str is a composition of functions. */
bool is_composite(char *str) {
    /* preserves the original str */
    char *str_cpy = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
    strcpy(str_cpy, str);

    if (id_ch_tp(str_cpy[0]) == pt_sig) {                  // sign
        str_cpy++;
    }

    if (par_enclosed(str_cpy)) {                           // enclosed in parentheses
        return false;
    } else if (strpbrk(str_cpy, "x") == NULL) {
        return false;
    } else if ((n_term(str_cpy) != 1) || (n_block(str_cpy) != 1)) {  // has more than one term or block
        return false;
    }

    ch_type ch_tp;
    if (strncmp(str_cpy, "pi", 2) == 0) {
        ch_tp = pt_cst;
    } else {
        ch_tp = id_ch_tp(str_cpy[0]);
    }

    if (ch_tp == pt_var) {                                 // ex) x ^ (2 + 2.43)
        return false;
    } else if (ch_tp == pt_fnc) {                          // ex) sin(cosh(x + 234))
        if (strncmp(str_cpy, "ln", 2) == 0) {
            str_cpy += 2;

            while (par_enclosed(str_cpy)) {
                str_cpy = rm_par(str_cpy);
            }

            if ((n_term(str_cpy) != 1) || (n_block(str_cpy) != 1)) {
                return true;
            } else if (has_func(str_cpy)) {                // already confirmed 'x' exists
                return true;
            } else {
                return false;
            }
        } else if ((strncmp(str_cpy, "sinh", 4) == 0) || (strncmp(str_cpy, "cosh", 4) == 0) ||
                   (strncmp(str_cpy, "tanh", 4) == 0) || (strncmp(str_cpy, "csch", 4) == 0) ||
                   (strncmp(str_cpy, "sech", 4) == 0) || (strncmp(str_cpy, "coth", 4) == 0)) {
            str_cpy += 4;

            while (par_enclosed(str_cpy)) {
                str_cpy = rm_par(str_cpy);
            }

            if ((n_term(str_cpy) != 1) || (n_block(str_cpy) != 1)) {
                return true;
            } else if (has_func(str_cpy)) {
                return true;
            } else {
                return false;
            }
        } else if ((strncmp(str_cpy, "sin", 3) == 0) || (strncmp(str_cpy, "cos", 3) == 0) ||
                   (strncmp(str_cpy, "tan", 3) == 0) || (strncmp(str_cpy, "csc", 3) == 0) ||
                   (strncmp(str_cpy, "sec", 3) == 0) || (strncmp(str_cpy, "cot", 3) == 0) ||
                   (strncmp(str_cpy, "log", 3) == 0)) {
            str_cpy += 3;

            while (par_enclosed(str_cpy)) {
                str_cpy = rm_par(str_cpy);
            }

            if ((n_term(str_cpy) != 1) || (n_block(str_cpy) != 1)) {
                return true;
            } else if (has_func(str_cpy)) {
                return true;
            } else {
                return false;
            }
        }
    } else if (ch_tp == pt_par) {                          // ex) (x + sinh(x)) ^ 2
        int ind = 1, len = strlen(str_cpy);
        while (ind < len) {
            if (par_paired(str_cpy, ind)) {
                break;
            } else {
                ind++;
            }
        }
        ind--;                                             // str_cpy[ind] = ')'

        char *bef_par = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
        char *aft_par = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
        strncpy(bef_par, str_cpy + 1, ind - 1);            // does not include the enclosing parentheses
        strcpy(aft_par, str_cpy + ind + 2);                // before ^ including enclosing parentheses

        while (par_enclosed(bef_par)) {                    // x + sinh(x)
            bef_par = rm_par(bef_par);
        }
        while (par_enclosed(aft_par)) {                    // 2
            aft_par = rm_par(aft_par);
        }

        if (strpbrk(bef_par, "x") != NULL) {               // x in the base
            if ((n_term(bef_par) != 1) || (n_block(bef_par) != 1)) {
                return true;
            } else if (has_func(bef_par)) {
                return true;
            } else if (strcmp(bef_par, "x") == 0) {
                return false;
            } else {
                return true;
            }
        } else {                                           // x in the exponent
            if ((n_term(aft_par) != 1) || (n_block(aft_par) != 1)) {
                return true;
            } else if (has_func(aft_par)) {
                return true;
            } else if (strcmp(aft_par, "x") == 0) {
                return false;
            } else {
                return true;
            }
        }
    } else if (ch_tp == pt_cst) {                          // ex) 234 ^ (x + sinh(x))
        char *pt = strpbrk(str_cpy, "^");

        if (pt == NULL) {
            return false;
        } else if (strpbrk(++pt, "x") == NULL) {
            return false;
        }

        while (par_enclosed(pt)) {
            pt = rm_par(pt);
        }

        if (strpbrk(pt, "x") == NULL) {
            return false;
        } else if ((n_term(pt) != 1) || (n_block(pt) != 1)) {
            return true;
        } else {
            return false;
        }
    }
}

/* Checks whether the character at index i is a delimiter. */
bool is_delimiter(char *str, int i) {
    if (i == 0) {                                          // index 0 is never a delimiter
        return false;
    }

    while (--i >= 0) {
        if ((str[i] == '+') || (str[i] == '-') || (str[i] == '^')) {
            return false;
        } else if ((str[i] != '+') && (str[i] != '-') && (str[i] != '*') && (str[i] != '/')) {
            return true;
        }
    }
    return true;
}

/* Returns the number of blocks. */
int n_block(char *str) {
    int i = 0, block = 1, len = strlen(str);
    while (i < len) {
        if (is_boundary(str, i) && (id_bd_tp(str[i]) == im_bd)) {
            block++;
        }
        i++;
    }

    return block;
}

/* Returns the number of terms. */
int n_term(char *str) {
    int i = 0, par = 0, term = 1, len = strlen(str);
    while (i < len) {
        if (str[i] == '(') {
            par++;
        } else if (str[i] == ')') {
            par--;
        }

        if ((par == 0) && (((str[i] == '+') || (str[i] == '-')) && is_delimiter(str, i))) {
            term++;
        }
        i++;
    }
    
    return term;
}

/* Returns a block which stores information of a term. */
block *into_block(char *str) {
    block *bl = init_block();
    list *mult_head = bl->mult;
    list *divi_head = bl->divi;

    bool divi = false, mult_1 = true, divi_1 = true;
    int old_ind = 0, new_ind = 0, len = strlen(str);

    while (new_ind <= len) {
        if (is_boundary(str, new_ind)) {         // when boundary, copy strings and update indices
            if (id_bd_tp(str[new_ind - 1]) != ex_bd) {
                if (divi) {
                    if (divi_1) {                // the first linked-list node does not need an initialization
                        strncpy(bl->divi->entry, str + old_ind, new_ind - old_ind);
                        divi_1 = false;
                    } else {                     // the rest do
                        bl->divi->next = init_list();
                        bl->divi = bl->divi->next;
                        strncpy(bl->divi->entry, str + old_ind, new_ind - old_ind);
                    }

                    divi = false;                // turn off the flag when one block has been saved
                } else {
                    if (mult_1) {                // the first linked-list node does not need an initialization
                        strncpy(bl->mult->entry, str + old_ind, new_ind - old_ind);
                        mult_1 = false;
                    } else {                     // the rest do
                        bl->mult->next = init_list();
                        bl->mult = bl->mult->next;
                        strncpy(bl->mult->entry, str + old_ind, new_ind - old_ind);
                    }
                }
            }

            if (str[new_ind] == '/') {           // if seen, the next one block is saved in bl->divi->entry
                divi = true;
            }

            if (id_bd_tp(str[new_ind]) == ex_bd) {
                old_ind = ++new_ind;
            } else {
                old_ind = new_ind++;
            }
        } else {                                 // otherwise, move on
            new_ind++;
        }
    }

    bl->mult = mult_head;
    bl->divi = divi_head;

    return bl;
}

/* Returns a component which stores information of a composition of functions. */
comp *into_comp(char *str) {
    char *str_cpy = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
    strcpy(str_cpy, str);

    comp *cp = init_comp();
    list *elem_head = cp->elem;
    fn_type fn_tp;

    while (((n_term(str_cpy) == 1) && (n_block(str_cpy) == 1)) && (is_composite(str_cpy))) {
        strcpy(cp->elem->entry, str_cpy);
        cp->elem->next = init_list();
        cp->elem = cp->elem->next;

        fn_tp = id_fn_tp(str_cpy);

        if (fn_tp == loga) {
            if (id_ch_tp(str_cpy[0]) == pt_sig) {                    // sign
                str_cpy++;
            }

            if (strncmp(str_cpy, "ln", 2) == 0) {
                str_cpy += 2;
            } else if (strncmp(str_cpy, "log", 3) == 0) {
                str_cpy += 3;
            }
        } else if ((fn_tp == poly) || (fn_tp == powr)) {             // x appears before '^'
            char *pt = strpbrk(str_cpy, "^");
            while (!par_paired(str_cpy, pt - str_cpy)) {
                pt = strpbrk(pt + 1, "^");
            }
            *pt = 0;
        } else if (fn_tp == expo) {                                  // x appears after '^'
            char *pt = strpbrk(str_cpy, "^");
            while (!par_paired(str_cpy, pt - str_cpy)) {
                pt = strpbrk(pt + 1, "^");
            }
            str_cpy = pt + 1;
        } else if (fn_tp == trig) {
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                str_cpy++;
            }
            str_cpy += 3;
        } else if (fn_tp == hypl) {
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                str_cpy++;
            }
            str_cpy += 4;
        } else {
            break;
        }

        while (par_enclosed(str_cpy)) {
            str_cpy = rm_par(str_cpy);
        }
    }
    strcpy(cp->elem->entry, str_cpy);                      // inner most component

    cp->elem = elem_head;

    return cp;
}

/* Returns a linked-list which stores all the terms. */
term *into_term(char *str) {
    bool par;
    int new_ind = 0, old_ind = 0;
    term *tm = init_term();
    list *segm_head = tm->segm;

    char *sign;
    while ((sign = strpbrk(str + new_ind, "+-")) != NULL) {
        new_ind = sign - str;
        par = par_paired(str, new_ind);

        if ((par) && (is_delimiter(str, new_ind) == true)) {
            strncpy(tm->segm->entry, str + old_ind, new_ind - old_ind);

            tm->segm->next = init_list();
            tm->segm = tm->segm->next;

            strncpy(tm->segm->entry, str + new_ind, 1);

            tm->segm->next = init_list();
            tm->segm = tm->segm->next;

            new_ind++;
            old_ind = new_ind;
        } else {
            new_ind++;
        }
    }
    strcpy(tm->segm->entry, str + old_ind);
    
    tm->segm = segm_head;

    return tm;
}
