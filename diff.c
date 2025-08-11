/*
 * diff.c
 * function pertaining to differentiation
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
#include "diff.h"
#include "parse.h"
#include "struct.h"
#include "utility.h"

char *differentiate(char *str, int mode) {                 // mode determines whether to recurse
    /* to preserve the original str */
    char *str_cpy = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
    strcpy(str_cpy, str);

    /* counteracts a parentheses-enclosed entity is not composite */
    while (par_enclosed(str_cpy)) {
        str_cpy = rm_par(str_cpy);
    }

    int num_tm = n_term(str_cpy), num_bl = n_block(str_cpy);
    char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR);

    if ((num_tm == 1) && (num_bl == 1)) {
        if ((mode == 1) && (is_composite(str_cpy))) {
            /* into elements */
            comp *cp = into_comp(str_cpy);

            strcpy(rt_str, "");
            while (cp->elem != NULL) {
                while (par_enclosed(cp->elem->entry)) {
                    cp->elem->entry = rm_par(cp->elem->entry);
                }
                strcat(rt_str, "(");
                strcat(rt_str, differentiate(cp->elem->entry, 0));
                strcat(rt_str, ")");
                cp->elem = cp->elem->next;
            }

            return rt_str;
        } else {
            /* convert into derivative */
            return fn_diff(str_cpy);
        }
    } else if (num_tm != 1) {
        term *tm = into_term(str_cpy);                     // into terms
        bool op = false;

        strcpy(rt_str, "(");
        while (tm->segm != NULL) {
            if (!op) {
                /* either break it down further or recurse */
                while (par_enclosed(tm->segm->entry)) {
                    tm->segm->entry = rm_par(tm->segm->entry);
                }
                strcat(rt_str, differentiate(tm->segm->entry, 1));
                op = true;
            } else {
                strcat(rt_str, tm->segm->entry);           // operator stored
                op = false;
            }
            tm->segm = tm->segm->next;
        }
        strcat(rt_str, ")");

        return rt_str;
    } else if (num_bl != 1) {
        block *bl = into_block(str_cpy);
        list *mult_head = bl->mult;                        // to apply product rule
        list *mult_curr = mult_head;
        list *divi_head = bl->divi;                        // to apply division rule
        list *divi_curr = divi_head;

        int n_divi, n_mult = n_list(mult_curr);
        if (strlen(divi_curr->entry) == 0) {
            n_divi = 0;
            mult_curr = mult_head;
        } else {
            n_divi = n_list(divi_curr);
            mult_curr = mult_head;
            divi_curr = divi_head;
        }

        /* remove all enclosing parentheses */
        while (mult_curr != NULL) {
            while (par_enclosed(mult_curr->entry)) {
                mult_curr->entry = rm_par(mult_curr->entry);
            }
            mult_curr = mult_curr->next;
        }
        mult_curr = mult_head;
        while (divi_curr != NULL) {
            while (par_enclosed(divi_curr->entry)) {
                divi_curr->entry = rm_par(divi_curr->entry);
            }
            divi_curr = divi_curr->next;
        }
        divi_curr = divi_head;

        if (n_divi == 0) {                                 // without division rule
            strcpy(rt_str, "(");
            int targ_ind, curr_ind;
            char *df_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);

            for (targ_ind = 0; targ_ind < n_mult; targ_ind++) {
                mult_curr = mult_head;

                for (curr_ind = 0; curr_ind < n_mult; curr_ind++) {
                    if (targ_ind == curr_ind) {
                        strcpy(df_str, differentiate(mult_curr->entry, 1));
                        while (par_enclosed(df_str)) {
                            df_str = rm_par(df_str);
                        }
                        strcat(rt_str, "(");
                        strcat(rt_str, df_str);
                        strcat(rt_str, ")");
                    } else {
                        strcat(rt_str, "(");
                        strcat(rt_str, mult_curr->entry);
                        strcat(rt_str, ")");
                    }

                    if (curr_ind != (n_mult - 1)) {
                        mult_curr = mult_curr->next;
                    }
                }
                
                if (targ_ind != (n_mult - 1)) {
                    strcat(rt_str, "+");
                }
            }
            strcat(rt_str, ")");

            return rt_str;
        } else {
            if (has_var(divi_curr)) {
                /* with division rule */
                int targ_ind, curr_ind;
                char *df_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                char *df_hi_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                char *df_lo_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);

                for (targ_ind = 0; targ_ind < n_mult; targ_ind++) {
                    mult_curr = mult_head;

                    for (curr_ind = 0; curr_ind < n_mult; curr_ind++) {
                        if (targ_ind == curr_ind) {
                            strcpy(df_str, differentiate(mult_curr->entry, 1));
                            while (par_enclosed(df_str)) {
                                df_str = rm_par(df_str);
                            } 
                            strcat(df_hi_str, "(");
                            strcat(df_hi_str, df_str);
                            strcat(df_hi_str, ")");
                        } else {
                            strcat(df_hi_str, "(");
                            strcat(df_hi_str, mult_curr->entry);
                            strcat(df_hi_str, ")");
                        }

                        if (curr_ind != (n_mult - 1)) {
                            mult_curr = mult_curr->next;
                        }
                    }
                
                    if (targ_ind != (n_mult - 1)) {
                        strcat(df_hi_str, "+");
                    }
                }
                mult_curr = mult_head;

                for (targ_ind = 0; targ_ind < n_divi; targ_ind++) {
                    divi_curr = divi_head;

                    for (curr_ind = 0; curr_ind < n_divi; curr_ind++) {
                        if (targ_ind == curr_ind) {
                            strcpy(df_str, differentiate(divi_curr->entry, 1));
                            while (par_enclosed(df_str)) {
                                df_str = rm_par(df_str);
                            } 
                            strcat(df_lo_str, "(");
                            strcat(df_lo_str, df_str);
                            strcat(df_lo_str, ")");
                        } else {
                            strcat(df_lo_str, "(");
                            strcat(df_lo_str, divi_curr->entry);
                            strcat(df_lo_str, ")");
                        }

                        if (curr_ind != (n_divi - 1)) {
                            divi_curr = divi_curr->next;
                        }
                    }
                
                    if (targ_ind != (n_divi - 1)) {
                        strcat(df_lo_str, "+");
                    }
                }
                divi_curr = divi_head;

                while (par_enclosed(df_hi_str)) {
                    df_hi_str = rm_par(df_hi_str);
                }
                while (par_enclosed(df_lo_str)) {
                    df_lo_str = rm_par(df_lo_str);
                }

                /* derivative of the top */
                strcpy(rt_str, "((");
           
                strcat(rt_str, "(");
                strcat(rt_str, df_hi_str);
                strcat(rt_str, ")");

                while (divi_curr != NULL) {
                    strcat(rt_str, "(");
                    strcat(rt_str, divi_curr->entry);
                    strcat(rt_str, ")");

                    divi_curr = divi_curr->next;
                }
                divi_curr = divi_head;
                strcat(rt_str, "-");

                /* derivative of the bottom */
                while (mult_curr != NULL) {
                    strcat(rt_str, "(");
                    strcat(rt_str, mult_curr->entry);
                    strcat(rt_str, ")");

                    mult_curr = mult_curr->next;
                }
                mult_curr = mult_head;
                
                strcat(rt_str, "(");
                strcat(rt_str, df_lo_str);
                strcat(rt_str, ")");

                if (n_divi == 1) {
                    if (n_block(divi_curr->entry) == 1) {
                        strcat(rt_str, ")/(");
                        while (divi_curr != NULL) {
                            strcat(rt_str, divi_curr->entry);
                        
                            divi_curr = divi_curr->next;
                        }
                        divi_curr = divi_head;
                        strcat(rt_str, "^2))");
                    } else {
                        strcat(rt_str, ")/((");
                        while (divi_curr != NULL) {
                            strcat(rt_str, divi_curr->entry);
                            
                            divi_curr = divi_curr->next;
                        }
                        divi_curr = divi_head;
                        strcat(rt_str, ")^2))");
                    }
                } else {
                    strcat(rt_str, ")/((");
                    while (divi_curr != NULL) {
                        strcat(rt_str, "(");
                        strcat(rt_str, divi_curr->entry);
                        strcat(rt_str, ")");

                        divi_curr = divi_curr->next;
                    }
                    divi_curr = divi_head;
                    strcat(rt_str, ")^2))");
                }

                return rt_str;
            } else {
                /* simply divide */
                strcpy(rt_str, "(");
                int targ_ind, curr_ind;
                char *df_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);

                for (targ_ind = 0; targ_ind < n_mult; targ_ind++) {
                    mult_curr = mult_head;

                    for (curr_ind = 0; curr_ind < n_mult; curr_ind++) {
                        if (targ_ind == curr_ind) {
                            strcpy(df_str, differentiate(mult_curr->entry, 1));

                            while (par_enclosed(df_str)) {
                                df_str = rm_par(df_str);
                            }
                            strcat(rt_str, "(");
                            strcat(rt_str, df_str);
                            strcat(rt_str, ")");
                        } else {
                            strcat(rt_str, "(");
                            strcat(rt_str, mult_curr->entry);
                            strcat(rt_str, ")");
                        }

                        if (curr_ind != (n_mult - 1)) {
                            mult_curr = mult_curr->next;
                        }
                    }
                
                    if (targ_ind != (n_mult - 1)) {
                        strcat(rt_str, "+");
                    }
                }

                bool dec = false;
                while (divi_curr != NULL) {
                    while (par_enclosed(divi_curr->entry)) {
                        divi_curr->entry = rm_par(divi_curr->entry);
                    }

                    if (has_dec(divi_curr->entry)) {
                        dec = true;
                    }
                    divi_curr = divi_curr->next;
                }
                divi_curr = divi_head;
                
                if (!dec) {
                    int prod = 1;
                    while (divi_curr != NULL) {
                        prod *= str_int(divi_curr->entry);
                        divi_curr = divi_curr->next;
                    }
                    strcat(rt_str, "/(");
                    strcat(rt_str, int_str(prod));
                    strcat(rt_str, ")");
                } else if (n_divi == 1) {
                    strcat(rt_str, "/");
                    while (divi_curr != NULL) {
                        strcat(rt_str, "(");
                        strcat(rt_str, divi_curr->entry);
                        strcat(rt_str, ")");
                        divi_curr = divi_curr->next;
                    }
                    strcat(rt_str, ")");
                } else {
                    strcat(rt_str, "/(");
                    while (divi_curr != NULL) {
                        strcat(rt_str, "(");
                        strcat(rt_str, divi_curr->entry);
                        strcat(rt_str, ")");
                        divi_curr = divi_curr->next;
                    }
                    strcat(rt_str, "))");
                }
                strcat(rt_str, ")");

                return rt_str;
            }
        }
    }
}

char *fn_diff(char *str) {
    char *str_cpy = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
    strcpy(str_cpy, str);

    fn_type fn_tp = id_fn_tp(str_cpy);

    if (fn_tp == cnst) {
        return "(0)";
    } else if (fn_tp == expo) {
        while (par_enclosed(str_cpy)) {
            str_cpy = rm_par(str_cpy);
        }

        /* (d/dx)(a^x) = (a^x)ln(a) */
        char *pt = strpbrk(str_cpy, "^");
        while (!par_paired(str_cpy, pt - str_cpy)) {
           pt = strpbrk(pt + 1, "^");
        }

        char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
        char *bef_ast = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
        char *aft_ast = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
        strncpy(bef_ast, str_cpy, pt - str_cpy);
        strcpy(aft_ast, pt + 1);
        
        while (par_enclosed(bef_ast)) {
            bef_ast = rm_par(bef_ast);
        }
        while (par_enclosed(aft_ast)) {
            aft_ast = rm_par(aft_ast);
        }

        /* (d/dx)(e^x) = e^x */
        if (strcmp(bef_ast, "e") == 0) {
            return str_cpy;
        }

        strcpy(rt_str, "(");
        if ((n_term(bef_ast) != 1) || (n_block(bef_ast) != 1)) {
            strcat(rt_str, "(");
            strcat(rt_str, bef_ast);
            strcat(rt_str, ")");
        } else {
            strcat(rt_str, bef_ast);
        }
        strcat(rt_str, "^");

        if ((n_term(aft_ast) != 1) || (n_block(aft_ast) != 1)) {
            strcat(rt_str, "(");
            strcat(rt_str, aft_ast);
            strcat(rt_str, ")");
        } else {
            strcat(rt_str, aft_ast);
        }

        strcat(rt_str, ")");
        strcat(rt_str, "ln(");
        strcat(rt_str, bef_ast);
        strcat(rt_str, ")");
        
        return rt_str;
    } else if (fn_tp == hypl) {
        while (par_enclosed(str_cpy)) {
            str_cpy = rm_par(str_cpy);
        }

        char *pt = strpbrk(str_cpy, "sct");
        while (!par_paired(str_cpy, pt - str_cpy)) {
            pt = strpbrk(pt + 1, "sct");
        }

        if (strncmp(pt, "sinh", 4) == 0) {
            /* (d/dx)(sinh(x)) = cosh(x) */
            pt[0] = 'c';
            pt[1] = 'o';
            pt[2] = 's';

            if (!par_enclosed(pt + 4)) {
                char *temp_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                strcpy(temp_str, pt + 4);

                strcpy(pt + 4, "(");
                strcat(pt + 4, temp_str);
                strcat(pt + 4, ")");
            }
            while (par_enclosed(rm_par(pt + 4))) {
                strcpy(pt + 4, rm_par(pt + 4));
            }
            
            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, "(");
                    strcat(rt_str, str_cpy);
                    strcat(rt_str, ")");

                    return rt_str;
                } else if (str_cpy[0] == '+') {
                    strcpy(rt_str, ++str_cpy);

                    return rt_str;
                }
            } else {
                strcpy(rt_str, str_cpy);

                return rt_str;
            }
        } else if (strncmp(pt, "cosh", 4) == 0) {
            /* (d/dx)(cosh(x)) = sinh(x) */
            pt[0] = 's';
            pt[1] = 'i';
            pt[2] = 'n';

            if (!par_enclosed(pt + 4)) {
                char *temp_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                strcpy(temp_str, pt + 4);

                strcpy(pt + 4, "(");
                strcat(pt + 4, temp_str);
                strcat(pt + 4, ")");
            }
            while (par_enclosed(rm_par(pt + 4))) {
                strcpy(pt + 4, rm_par(pt + 4));
            }

            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, "(");
                    strcat(rt_str, str_cpy);
                    strcat(rt_str, ")");

                    return rt_str;
                } else if (str_cpy[0] == '+') {
                    strcpy(rt_str, ++str_cpy);

                    return rt_str;
                }
            } else {
                strcpy(rt_str, str_cpy);

                return rt_str;
            }
        } else if (strncmp(pt, "tanh", 4) == 0) {
            /* (d/dx)(tanh(x)) = (sech(x))^2 */
            pt[0] = 's';
            pt[1] = 'e';
            pt[2] = 'c';

            if (!par_enclosed(pt + 4)) {
                char *temp_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                strcpy(temp_str, pt + 4);

                strcpy(pt + 4, "(");
                strcat(pt + 4, temp_str);
                strcat(pt + 4, ")");
            }
            while (par_enclosed(rm_par(pt + 4))) {
                strcpy(pt + 4, rm_par(pt + 4));
            }

            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, "(");
                    strcat(rt_str, str_cpy);
                    strcat(rt_str, "^2)");

                    return rt_str;
                } else if (str_cpy[0] == '+') {
                    strcpy(rt_str, "(");
                    strcat(rt_str, ++str_cpy);
                    strcat(rt_str, "^2)");

                    return rt_str;
                }
            } else {
                strcpy(rt_str, "(");
                strcat(rt_str, str_cpy);
                strcat(rt_str, "^2)");

                return rt_str;
            }
        } else if (strncmp(pt, "csch", 4) == 0) {
            /* (d/dx)(csch(x)) = -csch(x)coth(x) */
            if (!par_enclosed(pt + 4)) {
                char *temp_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                strcpy(temp_str, pt + 4);

                strcpy(pt + 4, "(");
                strcat(pt + 4, temp_str);
                strcat(pt + 4, ")");
            }
            while (par_enclosed(rm_par(pt + 4))) {
                strcpy(pt + 4, rm_par(pt + 4));
            }
   
            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, ++str_cpy);
                } else if (str_cpy[0] == '+') {
                    strcpy(rt_str, "(-");
                    strcat(rt_str, ++str_cpy);
                }
            } else {
                strcpy(rt_str, "(-");
                strcat(rt_str, str_cpy);
            }

            pt[0] = 'c';
            pt[1] = 'o';
            pt[2] = 't';

            strcat(rt_str, str_cpy);
            if (rt_str[0] == '(') {
                strcat(rt_str, ")");
            }

            return rt_str;
        } else if (strncmp(pt, "sech", 4) == 0) {
            /* (d/dx)(sech(x)) = -sech(x)tanh(x) */
            if (!par_enclosed(pt + 4)) {
                char *temp_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                strcpy(temp_str, pt + 4);

                strcpy(pt + 4, "(");
                strcat(pt + 4, temp_str);
                strcat(pt + 4, ")");
            }
            while (par_enclosed(rm_par(pt + 4))) {
                strcpy(pt + 4, rm_par(pt + 4));
            }

            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, ++str_cpy);
                } else if (str_cpy[0] == '+') {
                    strcpy(rt_str, "(-");
                    strcat(rt_str, ++str_cpy);
                }
            } else {
                strcpy(rt_str, "(-");
                strcat(rt_str, str_cpy);
            }

            pt[0] = 't';
            pt[1] = 'a';
            pt[2] = 'n';

            strcat(rt_str, str_cpy);
            if (rt_str[0] == '(') {
                strcat(rt_str, ")");
            }

            return rt_str;
        } else if (strncmp(pt, "coth", 4) == 0) {
            /* (d/dx)(coth(x)) = -(csch(x))^2 */
            pt[0] = 'c';
            pt[1] = 's';
            pt[2] = 'c';

            if (!par_enclosed(pt + 4)) {
                char *temp_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                strcpy(temp_str, pt + 4);

                strcpy(pt + 4, "(");
                strcat(pt + 4, temp_str);
                strcat(pt + 4, ")");
            }
            while (par_enclosed(rm_par(pt + 4))) {
                strcpy(pt + 4, rm_par(pt + 4));
            }
           
            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, "(");
                    strcat(rt_str, ++str_cpy);
                    strcat(rt_str, "^2)");

                    return rt_str;
                } else if (str_cpy[0] == '+') {
                    strcpy(rt_str, "(-");
                    strcat(rt_str, ++str_cpy);
                    strcat(rt_str, "^2)");

                    return rt_str;
                }
            } else {
                strcpy(rt_str, "(-");
                strcat(rt_str, str_cpy);
                strcat(rt_str, "^2)");

                return rt_str;
            }
        }
    } else if (fn_tp == loga) {
        while (par_enclosed(str_cpy)) {
            str_cpy = rm_par(str_cpy);
        }

        char *pt = strpbrk(str_cpy, "l");
        while (!par_paired(str_cpy, pt - str_cpy)) {
            pt = strpbrk(pt + 1, "l");
        }

        if (strncmp(pt, "ln", 2) == 0) {
            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, "(-1)");
                } else {
                    strcpy(rt_str, "(1)");
                }
                str_cpy++;
            } else {
                strcpy(rt_str, "(1)");
            }

            while ((par_enclosed(pt + 2)) && (par_enclosed(rm_par(pt + 2)))) {
                strcpy(pt + 2, rm_par(pt + 2));
            }
            strcat(rt_str, "/");
            strcat(rt_str, pt + 2);

            return rt_str;
        } else if (strncmp(pt, "log", 3) == 0) {
            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, "(-1)");
                } else {
                    strcpy(rt_str, "(1)");
                }
                str_cpy++;
            } else {
                strcpy(rt_str, "(1)");
            }

            while ((par_enclosed(pt + 3)) && (par_enclosed(rm_par(pt + 3)))) {
                strcpy(pt + 3, rm_par(pt + 3));
            }
            strcat(rt_str, "/(");
            strcat(rt_str, pt + 3);
            strcat(rt_str, "ln(10))");

            return rt_str;
        }
    } else if (fn_tp == poly) {
        while (par_enclosed(str_cpy)) {
            str_cpy = rm_par(str_cpy);
        }

        char *pt = strpbrk(str_cpy, "^");
        while (!par_paired(str_cpy, pt - str_cpy)) {
            pt = strpbrk(pt + 1, "^");
        }

        if (pt == NULL) {
            pt = strpbrk(str_cpy, "x");
            *pt = 0;

            if (str_cpy[0] == '-') {
                return "(-1)";
            } else {
                return "(1)";
            }
        }

        *(pt++) = 0;
        while (par_enclosed(pt)) {
            pt = rm_par(pt);
        }

        int exp = str_int(pt);
        char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
        if (id_ch_tp(str_cpy[0]) == pt_sig) {
            if (str_cpy[0] == '-') {                       // has a '-' sign
                if (exp < 0) {
                    strcpy(rt_str, "(");
                    if ((exp * -1) != 1) {                 // no need for a coefficient of 1
                        strcat(rt_str, int_str(exp * -1));
                    }
                    strcat(rt_str, "x^");
                    strcat(rt_str, int_str(exp - 1));
                    strcat(rt_str, ")");

                    return rt_str;
                } else if (exp == 0) {
                    return "(0)";
                } else if (exp == 1) {
                    return "(-1)";
                } else if (exp == 2) {
                    strcpy(rt_str, "(-");
                    strcat(rt_str, int_str(exp));
                    strcat(rt_str, "x)");

                    return rt_str;
                } else if (exp > 0) {
                    strcpy(rt_str, "(-");
                    strcat(rt_str, int_str(exp));
                    strcat(rt_str, "x^");
                    strcat(rt_str, int_str(exp - 1));
                    strcat(rt_str, ")");
                    
                    return rt_str;
                }
            } else if (str_cpy[0] == '+') {                // has a '+' sign
                if (exp == 0) {
                    return "(0)";
                } else if (exp == 1) {
                    return "(1)";
                } else if (exp == 2) {
                    strcpy(rt_str, "(");
                    strcat(rt_str, int_str(exp));
                    strcat(rt_str, "x");
                    strcat(rt_str, ")");

                    return rt_str;
                } else if (exp > 0) {
                    strcpy(rt_str, "(");
                    strcat(rt_str, int_str(exp));
                    strcat(rt_str, "x^");
                    strcat(rt_str, int_str(exp - 1));
                    strcat(rt_str, ")");

                    return rt_str;
                } else if (exp == -1) {
                    strcpy(rt_str, "(-x^");
                    strcat(rt_str, int_str(exp - 1));
                    strcat(rt_str, ")");

                    return rt_str;
                } else if (exp < 0) {
                    strcpy(rt_str, "(");
                    strcat(rt_str, int_str(exp));
                    strcat(rt_str, "x^");
                    strcat(rt_str, int_str(exp - 1));
                    strcat(rt_str, ")");

                    return rt_str;
                }
            }
        } else {                                           // has no sign
            if (exp == 0) {
                return "(0)";
            } else if (exp == 1) {
                return "(1)";
            } else if (exp == 2) {
                strcpy(rt_str, "(");
                strcat(rt_str, int_str(exp));
                strcat(rt_str, "x");
                strcat(rt_str, ")");

                return rt_str;
            } else if (exp > 0) {
                strcpy(rt_str, "(");
                strcat(rt_str, int_str(exp));
                strcat(rt_str, "x^");
                strcat(rt_str, int_str(exp - 1));
                strcat(rt_str, ")");

                return rt_str;
            } else if (exp == -1) {
                strcpy(rt_str, "(-x^");
                strcat(rt_str, int_str(exp - 1));
                strcat(rt_str, ")");

                return rt_str;
            } else if (exp < 0) {
                strcpy(rt_str, "(");
                strcat(rt_str, int_str(exp));
                strcat(rt_str, "x^");
                strcat(rt_str, int_str(exp - 1));
                strcat(rt_str, ")");

                return rt_str;
            }
        }
    } else if (fn_tp == powr) {
        while (par_enclosed(str_cpy)) {
            str_cpy = rm_par(str_cpy);
        }

        char *pt = strpbrk(str_cpy, "^");
        while (!par_paired(str_cpy, pt - str_cpy)) {
            pt = strpbrk(pt + 1, "^");
        }

        *(pt++) = 0;
        while (par_enclosed(str_cpy)) {
            str_cpy = rm_par(str_cpy);
        }
        while (par_enclosed(pt)) {
            pt = rm_par(pt);
        }

        if (strcmp(pt, "1") == 0) {
            if (strcmp(str_cpy, "x") == 0) {               // not composite
                return "(1)";
            } else {                                       // composite
                return "";
            }
        } else if (strcmp(pt, "0") == 0) {
            return "(0)";
        } else {
            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (strcmp(pt, "2") == 0) {
                strcpy(rt_str, "(2)(");
                strcat(rt_str, str_cpy);
                strcat(rt_str, ")");

                return rt_str;
            } else if ((id_fn_tp(pt) == cnst) && (!has_dec(pt))) {
                int exp = str_int(pt);
                strcpy(rt_str, "(");
                strcat(rt_str, pt);
                strcat(rt_str, ")(");
                strcat(rt_str, str_cpy);
                strcat(rt_str, ")^(");
                strcat(rt_str, int_str(exp - 1));
                strcat(rt_str, ")");
                
                return rt_str;
            } else {
                strcpy(rt_str, "(");
                strcat(rt_str, pt);
                strcat(rt_str, ")(");
                strcat(rt_str, str_cpy);
                strcat(rt_str, ")^(");
                strcat(rt_str, pt);
                strcat(rt_str, "-1)");

                return rt_str;
            }
        }
    } else if (fn_tp == trig) {
        while (par_enclosed(str_cpy)) {
            str_cpy = rm_par(str_cpy);
        }

        char *pt = strpbrk(str_cpy, "sct");
        while (!par_paired(str_cpy, pt - str_cpy)) {
            pt = strpbrk(pt + 1, "sct");
        }

        if (strncmp(pt, "sin", 3) == 0) {
            /* (d/dx)(sin(x)) = cos(x) */
            pt[0] = 'c';
            pt[1] = 'o';
            pt[2] = 's';

            if (!par_enclosed(pt + 3)) {
                char *temp_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                strcpy(temp_str, pt + 3);

                strcpy(pt + 3, "(");
                strcat(pt + 3, temp_str);
                strcat(pt + 3, ")");
            }
            while (par_enclosed(rm_par(pt + 3))) {
                strcpy(pt + 3, rm_par(pt + 3));
            }

            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, "(");
                    strcat(rt_str, str_cpy);
                    strcat(rt_str, ")");

                    return rt_str;
                } else {
                    strcpy(rt_str, ++str_cpy);

                    return rt_str;
                }
            } else {
                strcpy(rt_str, str_cpy);

                return rt_str;
            }
        } else if (strncmp(pt, "cos", 3) == 0) {
            /* (d/dx)(cos(x)) = -sin(x) */
            pt[0] = 's';
            pt[1] = 'i';
            pt[2] = 'n';
            
            if (!par_enclosed(pt + 3)) {
                char *temp_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                strcpy(temp_str, pt + 3);

                strcpy(pt + 3, "(");
                strcat(pt + 3, temp_str);
                strcat(pt + 3, ")");
            }
            while (par_enclosed(rm_par(pt + 3))) {
                strcpy(pt + 3, rm_par(pt + 3));
            }

            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    return str_cpy + 1;
                } else if (str_cpy[0] == '+') {
                    strcpy(rt_str, "(-");
                    strcat(rt_str, ++str_cpy);
                    strcat(rt_str, ")");

                    return rt_str;
                }
            } else {
                strcpy(rt_str, "(-");
                strcat(rt_str, str_cpy);
                strcat(rt_str, ")");

                return rt_str;
            }
        } else if (strncmp(pt, "tan", 3) == 0) {
            /* (d/dx)(tan(x)) = (sec(x))^2 */
            pt[0] = 's';
            pt[1] = 'e';
            pt[2] = 'c';

            if (!par_enclosed(pt + 3)) {
                char *temp_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                strcpy(temp_str, pt + 3);

                strcpy(pt + 3, "(");
                strcat(pt + 3, temp_str);
                strcat(pt + 3, ")");
            }
            while (par_enclosed(rm_par(pt + 3))) {
                strcpy(pt + 3, rm_par(pt + 3));
            }

            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, "(");
                    strcat(rt_str, str_cpy);
                    strcat(rt_str, "^2)");

                    return rt_str;
                } else if (str_cpy[0] == '+') {
                    strcpy(rt_str, "(");
                    strcat(rt_str, ++str_cpy);
                    strcat(rt_str, "^2)");

                    return rt_str;
                }
            } else {
                strcpy(rt_str, "(");
                strcat(rt_str, str_cpy);
                strcat(rt_str, "^2)");

                return rt_str;
            }
        } else if (strncmp(pt, "csc", 3) == 0) {
            /* (d/dx)(csc(x)) = -csc(x)cot(x) */
            if (!par_enclosed(pt + 3)) {
                char *temp_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                strcpy(temp_str, pt + 3);

                strcpy(pt + 3, "(");
                strcat(pt + 3, temp_str);
                strcat(pt + 3, ")");
            }
            while (par_enclosed(rm_par(pt + 3))) {
                strcpy(pt + 3, rm_par(pt + 3));
            }

            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, ++str_cpy);
                } else if (str_cpy[0] == '+') {
                    strcpy(rt_str, "(-");
                    strcat(rt_str, ++str_cpy);
                }
            } else {
                strcpy(rt_str, "(-");
                strcat(rt_str, str_cpy);
            }

            pt[0] = 'c';
            pt[1] = 'o';
            pt[2] = 't';

            strcat(rt_str, str_cpy);
            if (rt_str[0] == '(') {
                strcat(rt_str, ")");
            }

            return rt_str;
        } else if (strncmp(pt, "sec", 3) == 0) {
            /* (d/dx)(sec(x)) = sec(x)tan(x) */
            if (!par_enclosed(pt + 3)) {
                char *temp_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                strcpy(temp_str, pt + 3);

                strcpy(pt + 3, "(");
                strcat(pt + 3, temp_str);
                strcat(pt + 3, ")");
            }
            while (par_enclosed(rm_par(pt + 3))) {
                strcpy(pt + 3, rm_par(pt + 3));
            }

            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, "(");
                    strcat(rt_str, str_cpy++);
                } else if (str_cpy[0] == '+') {
                    strcpy(rt_str, ++str_cpy);
                }
            } else {
                strcpy(rt_str, str_cpy);
            }

            pt[0] = 't';
            pt[1] = 'a';
            pt[2] = 'n';

            strcat(rt_str, str_cpy);
            if (rt_str[0] == '(') {
                strcat(rt_str, ")");
            }

            return rt_str;
        } else if (strncmp(pt, "cot", 3) == 0) {
            /* (d/dx)(cot(x)) = -(csc(x))^2 */
            pt[0] = 'c';
            pt[1] = 's';
            pt[2] = 'c';

            if (!par_enclosed(pt + 3)) {
                char *temp_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
                strcpy(temp_str, pt + 3);

                strcpy(pt + 3, "(");
                strcat(pt + 3, temp_str);
                strcat(pt + 3, ")");
            }
            while (par_enclosed(rm_par(pt + 3))) {
                strcpy(pt + 3, rm_par(pt + 3));
            }

            char *rt_str = (char *) malloc(sizeof(char) * MAX_CHAR / 8);
            if (id_ch_tp(str_cpy[0]) == pt_sig) {
                if (str_cpy[0] == '-') {
                    strcpy(rt_str, "(");
                    strcat(rt_str, ++str_cpy);
                    strcat(rt_str, "^2)");
                    
                    return rt_str;
                } else if (str[0] == '+') {
                    strcpy(rt_str, "(-");
                    strcat(rt_str, ++str_cpy);
                    strcat(rt_str, "^2)");
                    
                    return rt_str;
                }
            } else {
                strcpy(rt_str, "(-");
                strcat(rt_str, str_cpy);
                strcat(rt_str, "^2)");

                return rt_str;
            }
        }
    }
}
