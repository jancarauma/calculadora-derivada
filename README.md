# Calculadora de Derivadas

O programa recebe uma função diferenciável de variável única em `x` e realiza diferenciação explícita para produzir a derivada da função como saída. O código fonte pode ser compilado com o comando `make` e limpo com `make clean`. O programa pode ser executado através do arquivo executável `derivative` com `./derivative`.

### Propósito

A criação do programa surgiu da motivação de implementar uma calculadora de derivadas matemáticas que permitisse entradas flexíveis do usuário, e foi apresentada na disciplina de programação estruturada durante a faculdade. Como resultado, a implementação do programa suporta uma ampla gama de estilos de entrada flexíveis. Por exemplo, o programa interpreta as seguintes expressões como equivalentes, gerando uma saída consistente:

- `6x`
- `6 x`
- `(6)(x)`
- `(6) (x)`
- `6*x`
- `6 * x`

Outro objetivo principal da implementação foi permitir entradas do usuário o mais genéricas possível. Dessa forma, o programa pode processar funções de comprimento e profundidade arbitrários. Para suportar essa funcionalidade, o programa utiliza listas ligadas para armazenar informações da função, decompondo-a recursivamente em subcomponentes, aplicando regras de diferenciação apropriadas (incluindo as Regras da Cadeia, Divisão e Produto) e finalmente mesclando as conversões em uma saída final.

O programa foi projetado para suportar uma ampla gama de funções e constantes matemáticas, como `e` e `pi`. Seguem os tipos de funções nas quais o programa pode realizar diferenciações:

- **Função Constante**
- **Função Exponencial**
- **Função Hiperbólica**: `sinh(x)`, `cosh(x)`, `tanh(x)`, `csch(x)`, `sech(x)`, `coth(x)`
- **Função Logarítmica**: `log(x)`, `ln(x)`
- **Função Polinomial**
- **Função Potência**
- **Função Racional**
- **Função Trigonométrica**: `sin(x)`, `cos(x)`, `tan(x)`, `csc(x)`, `sec(x)`, `cot(x)`

### Mecanismo

Para explicar o algoritmo implementado pelo programa, as seguintes terminologias devem ser introduzidas:

**Delimitador**: Consiste em símbolos como `+` e `-` que quebram uma expressão em termos. Seu uso deve ser diferenciado do mesmo conjunto de símbolos usados para denotar o sinal de um valor (positivo e negativo). Na função `2x + -sinh(x)`, o `+` entre `2x` e `-sinh(x)` é um delimitador, mas o `-` na frente de `sinh(x)` não é.

**Termo**: Consiste em expressões matemáticas separadas por delimitadores. Como descrito no exemplo anterior, `2x` e `-sinh(x)` são classificados como termos.

**Fronteira**: Marca onde mais de uma expressão matemática é multiplicada ou dividida, podendo ser categorizada em dois tipos: explícita e implícita. Fronteiras explícitas são denotadas com símbolos `*` e `/`, enquanto as implícitas não são necessariamente representadas por símbolos, mas ainda implicam multiplicações ou divisões (parênteses ou adjacência de expressões matemáticas de tipos distintos). Uma fronteira pode ser usada para dividir um único termo em mais de um bloco, permitindo a aplicação das Regras de Divisão e Produto.

- **Função com fronteiras explícitas**: `2 * sinh(x)`, `x * cos(x) * 3 * sin(x)`
- **Função com fronteiras implícitas**: `(2) (sinh(x))`, `x cos(x) (3 sin(x))`

**Bloco**: Consiste em expressões matemáticas dentro de um único termo separadas por fronteiras. Por exemplo, a função `x cos(x) (3 sin(x))` consiste em três blocos: `x`, `cos(x)` e `(3 sin(x))`.

**Composta**: Refere-se a uma função matemática cujo parâmetro contém outras funções matemáticas. Sua diferenciação envolve a aplicação da Regra da Cadeia. `sin(cos(x + 3))` é um exemplo.

**Componente**: Consiste em expressões que devem ser diferenciadas individualmente para aplicar a Regra da Cadeia. Cada componente pode representar uma camada de uma função composta. Por exemplo, a função `sin(cos(x + 3))` contém três componentes: `sin(cos(x + 3))`, `cos(x + 3)` e `(x + 3)`.

A função matemática de entrada passa por uma série de recursões e é decomposta em múltiplos termos e subsequentemente em blocos. Este procedimento é executado repetitivamente até que todos os componentes sejam apenas um único termo e também um único bloco. Os componentes são então decompostos em componentes menores, ponto em que começam as operações matemáticas para diferenciar a função de entrada. À medida que os componentes, agora em suas formas convertidas, emergem das chamadas recursivas, são reunidos com outros componentes aos quais estavam inicialmente associados. Após o programa retornar com sucesso a derivada matemática da função de entrada, ele termina o processo atual e aguarda uma próxima entrada até o comando `exit`.

### Entrada

**Multiplicação**: As multiplicações são geralmente representadas pelo símbolo `*`, mas também podem ser representadas implicitamente como expressões adjacentes parentetizadas ou não parentetizadas.
- `4x` = `4 * x`
- `x sin(x)` = `x * sin(x)`

**Divisão**: As divisões são representadas pelo símbolo `/`. Um termo (ou um bloco se consistir de um termo mas tendo múltiplos blocos) que imediatamente segue `/` é identificado como divisor. Sempre que uma entrada requer divisão por mais de um termo ou bloco, o divisor deve ser encapsulado em parênteses ou `/` deve preceder cada divisor.
- `e / x 234` = `(234 * e) / x` ≠ `e / (234 x)`

**Exponenciação**: As exponenciações são representadas pelo símbolo `^`. A mesma regra da divisão ainda se aplica, onde apenas um termo (ou um bloco) imediatamente seguindo `^` é considerado um expoente.
- `lnx ^ 234 x` = `(lnx ^ 234) x` ≠ `lnx ^ (234 x)`

**Parâmetro de Função**: Normalmente um parâmetro de função é encapsulado dentro de um par de parênteses que seguem o protótipo da função. Por exemplo, `x + 234` é o parâmetro de `sin(x + 234)`. É também possível escrever uma função sem os parênteses para encapsular explicitamente seu parâmetro - esta convenção apenas se aplica à função cujo parâmetro consiste de um único termo (ou um único bloco).
- `sinx + 234` = `sin(x) + 234` ≠ `sin(x + 234)`
- `lnx + sinx` = `ln(x) + sin(x)` ≠ `ln(x + sin(x))`

**Precedência**: A ordem regular de operações aritméticas PEMDAS se aplica. Algumas funções como `sin 234 ^ 3`, no entanto, são indefinidas por esta regra: `sin(234) ^ 3` vs `sin(234 ^ 3)`. As expressões são interpretadas da esquerda para a direita e, portanto, `sin()` tem precedência. É equivalente a `sin(234) ^ 3`.

### Exemplo de Execução

O programa deve ser compilado com o comando `make`, que gera um arquivo executável chamado `derivative`. O executável pode rodar com `./derivative`:

```bash
$ make
$ ./derivative

Input: x + 234
Output: 1

Input: sin(cos(x + e)) + cosh(e) + tan(ln(e))
Output: cos(cos(x + e))(-sin(x + e))

Input: tan(pi) + log(xln(sin(x)))
Output: (1 / ((x ln(sin(x))) ln(10))) (ln(sin(x)) + x ((1 / sin(x)) cos(x)))
```

`Input:` e `Output:` fazem parte dos prompts do programa. Note que a saída real do programa não inclui espaços em branco, portanto a saída do segundo exemplo apareceria no terminal como: `cos(cos(x+e))(-sin(x+e))`. Os espaços em branco foram incluídos na documentação para focar mais na precisão da saída do programa do que em seu formato.

## LIMITAÇÕES

### Computação Numérica

Embora muito esforço seja feito para analisar a string de entrada e dividi-la em componentes diferenciáveis menores, o programa tem uma limitação na realização de computações aritméticas. Por exemplo, se o programa receber uma entrada de `(2 - 2) x`, ele não interpreta o coeficiente de `x` como sendo `0`, resultando na expressão inteira igual a `0`. Em vez disso, decompõe-a no produto de dois blocos `(2 - 2)` e `x` para aplicar a Regra do Produto. A saída da função seria `(2 - 2)`, que é equivalente a `0`.

O programa é capaz de realizar algumas aritméticas simples ao diferenciar funções polinomiais e de potência - pode analisar o expoente como um valor inteiro e subtrair por 1. Mas isso ainda tem suas próprias limitações, pois não pode realizar esta operação se o valor contém valores decimais (`.`), multiplicação (`*`) ou divisão (`/`) - pode apenas analisar um inteiro. O programa compromete concatenando `- 1` ao final da string do expoente e ainda consegue produzir uma solução correta.

- `x ^ (2.3)` produz `2.3 x ^ (2.3 - 1)`

### Função

O programa falha ao diferenciar uma função que contém a variável 'x' tanto como base quanto como expoente, pois envolve diferenciação implícita. Além disso, o mecanismo implementado para decompor uma função em componentes menores, com a natureza da exponenciação (no caso de funções compostas, as funções mais externas estão localizadas à direita, enquanto outras funções típicas são lidas da esquerda para a direita) requer que a composição de múltiplas funções de exponenciação seja explicitamente separada por parênteses.

**Funções de entrada válidas**:
- `x ^ sin(e)`
- `(x ^ 2) ^ 3`

**Funções de entrada inválidas**:
- `x ^ sin(x)`
- `x ^ 2 ^ 3`

---

## LICENÇA

MIT License

Copyright (c) 2025 J. Caraumã <carauma.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
