/*
 * main.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // fork()
#include <sys/wait.h> // wait()

#include "diff.h"
#include "parse.h"
#include "simplify.h"
#include "struct.h"
#include "utility.h"

#define DEBUG 0
#define DEBUG_TERM 1
#define DEBUG_BLOCK 1
#define DEBUG_COMP 1
#define DEBUG_FN_TP 1
#define DEBUG_FN_DF 1
#define DEBUG_SMIN 1
#define DEBUG_SMOUT 1

/* Função para exibir o cabeçalho */
void print_header(void) {
    printf("===========================================\n");
    printf("     Calculadora de Derivadas 1.0 (CLI)      \n");
    printf("===========================================\n");
    printf("Digite uma função de x e receba sua derivada.\n");
    printf("Comandos especiais:\n");
    printf("  help  -> mostrar ajuda\n");
    printf("  exit  -> sai do programa\n");
    printf("-------------------------------------------\n");
}

/* Função para exibir instruções detalhadas */
void print_help(void) {
    printf("\n=== AJUDA ===\n");
    printf("Exemplos de entrada:\n");
    printf("  x^2           -> retorna 2x\n");
    printf("  sin(x)        -> retorna cos(x)\n");
    printf("  e^(3x)        -> retorna 3e^(3x)\n");
    printf("  ln(x)         -> retorna 1/x\n");
    printf("\nObservações:\n");
    printf(" - Use ^ para potência.\n");
    printf(" - Funções suportadas: polinômios, trigonométricas, exponenciais, logarítmicas.\n");
    printf(" - Espaços serão ignorados.\n");
    printf(" - Use parênteses para agrupar termos.\n");
    printf("========================\n\n");
}

int main(int argc, char *argv[]) {
    char *m_func = (char *) malloc(sizeof(char) * MAX_CHAR);

    print_header();

    /* input inicial */
    printf("Input: ");
    fgets(m_func, MAX_CHAR, stdin);
    m_func[strlen(m_func) - 1] = 0;
    m_func = wo_space(m_func);

    pid_t pid;
    while (strcmp(m_func, "exit") != 0) {

        /* Comando help */
        if (strcmp(m_func, "help") == 0) {
            print_help();
            printf("Input: ");
            fgets(m_func, MAX_CHAR, stdin);
            m_func[strlen(m_func) - 1] = 0;
            m_func = wo_space(m_func);
            continue;
        }

        if ((pid = fork()) < 0) {
            perror("fork error");
            exit(1);
        }

        if (pid == 0) { // child
            if (!par_paired(m_func, strlen(m_func))) {
                printf("uneven number of open/closed parentheses\n");
                exit(0);
            }

            #if DEBUG
                #if DEBUG_TERM
                printf("n_term: %d\n", n_term(m_func));
                term *tm = into_term(m_func);
                debug_term(tm);
                #endif

                #if DEBUG_BLOCK
                printf("n_block: %d\n", n_block(m_func));
                block *bl = into_block(m_func);
                debug_block(bl);
                #endif

                #if DEBUG_COMP
                printf("is_composite(): %d\n", is_composite(m_func));
                comp *cp = into_comp(m_func);
                debug_comp(cp);
                #endif

                #if DEBUG_FN_TP
                printf("0-cnst.\t1-expo.\t2-hypl.\t3-loga.\t4-poly.\t5-powr.\t6-trig.\n");
                printf("fn_tp(): %d\n", id_fn_tp(m_func));
                #endif

                #if DEBUG_FN_DF
                printf("fn_diff(): %s\n", fn_diff(m_func));
                #endif

                #if DEBUG_SMIN
                printf("simp_input(): %s\n", simp_input(m_func));
                #endif

                #if DEBUG_SMOUT
                printf("simp_output(): %s\n", simp_output(m_func));
                #endif
            #endif

            char *derv = differentiate(simp_input(m_func), 1);
            derv = simp_output(derv);
            while (par_enclosed(derv)) {
                derv = rm_par(derv);
            }

            printf("Output: ");
            if (strcmp(derv, "") == 0) {
                printf("0");
            } else if (*derv == '+') {
                printf("%s\n", derv + 1);
            } else {
                printf("%s\n", derv);
            }

            exit(0);
        } else { // parent
            wait(NULL);
        }

        /* próximo input */
        printf("Entrada: ");
        fgets(m_func, MAX_CHAR, stdin);
        m_func[strlen(m_func) - 1] = 0;
        m_func = wo_space(m_func);
    }

    free(m_func);
    return 0;
}

