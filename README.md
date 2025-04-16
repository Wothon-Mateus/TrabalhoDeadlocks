[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/zYPTTDEk)
# Algoritmo do Banqueiro

Nesse projeto, você escreverá um programa multithreaded que implemente o algoritmo do banqueiro discutido na Seção 7.5.3 do livro-texto.

Vários processos (processos) solicitam e liberam recursos do banco.
O banqueiro atenderá uma solicitação somente se ela deixar o sistema em um estado seguro. 
Uma solicitação que deixe o sistema em um estado inseguro será negada. 
Essa tarefa de programação combina três tópicos diferentes: 
1. criar múltiplos threads;
2. prevenir condições de corrida; e 
3. evitar deadlocks.

## O Banqueiro

O banqueiro considerará solicitações de _p_ processos por _r_ tipos de recursos, como descrito na Seção 7.5.3. 
Os métodos definidos para o banqueiro estão definidos no arquivo de cabeçalho `bank.h`.
O arquivo `bank.c` corresponde à implementação do código do banqueiro e contém o detalhamento das estruturas de dados utilizadas pelo banqueiro para controlar o acesso de processos a recursos.

```c
typedef struct bank {
  int p;                  // total de processos
  int r;                  // total de recursos
  int *available;         // quantidade de recursos disponível
  int **maximum;          // alocação máquina por recurso para cada processo
  int **allocation;       // alocação atual de recursos a cada processo
  int **need;             // necessidade de recurso por cada processo
  pthread_mutex_t mut;    // mutex para controle de concorrência
} bank;
```

## Os Processos

O total de processos é definido no arquivo de cabeçalho `process.h`, assim como os métodos para criação, destruição e a função que executa a thread de cada processo.

A função principal deve criar `PROCESS_COUNT` processos, cada um representado por uma thread.
Os processos estarão em um loop contínuo, solicitando e depois liberando instâncias aleatórias de recursos. 
As solicitações dos processos por recursos serão limitadas por seus respectivos valores no array `max`.
Para facilitar o desenvolvimento, cada processo apenas faz uma solicitação por vez, ou seja, para fazer uma nova solicitação ele libera todos os recursos da solicitação atual.

O banqueiro atenderá uma solicitação se ela satisfizer ao algoritmo de segurança descrito na Seção 7.5.3.1 do livro-texto.
Se uma solicitação não deixa o sistema em um estado seguro, o banqueiro a negará e o processo tentará novamente em outro momento.
Os protótipos das funções para a solicitação e liberação de recursos são os seguintes:

```c
bool bank_request_resources(struct bank *b, int pid, int *request);
void bank_release_resources(struct bank *b, int pid, int *release);
```
A primeira função deve retornar `true` caso os recursos solicitados sejam alocados e `false`, caso contrário.
Múltiplos threads (processos) acessarão concorrentemente dados compartilhados por meio dessas duas funções. 
Portanto, o acesso deve ser controlado por meio de locks `mutex` para prevenir condições de corrida. 
A API Pthreads fornece locks `mutex`, conforme visto na Seção 5.9.4 do livro-texto.

## Atividades de Implementação

1. Implemente a criação dos processos, inicialização das threads e adição do processo ao banco no arquivo `main.c`.
Veja os cabeçalhos `process.h` e `bank.h` para entender as funções disponíveis.
2. Implemente a verificação de estado seguro para a requisição do processo no arquivo `bank.c`.
3. Implemente a liberação dos recursos utilizados pelo processo no arquivo `bank.c`.

## Invocação

Você deve invocar seu programa passando o número de recursos de cada tipo na linha de comando e o arquivo com o máximo que cada processo pode solicitar por recurso (veja o arquivo `maximum.txt` no repositório).
Por exemplo, se houver três tipos de recursos, com dez instâncias do primeiro tipo, cinco do segundo tipo e sete do terceiro tipo, você invocaria seu programa assim:


```bash
./bin/main 10 5 7 maximum.txt
```

O array `available` seria inicializado com esses valores. 
A matriz `maximum` seria inicializada com os valores no arquivo `maximum.txt`.

### Compilação

O repositório conta com um arquivo `Makefile` para auxiliar na compilação.

Execute o comando `make` para compilar o código..

A pasta `src` contém o código fonte e o executável é armazenado na pasta `bin`.

O comando `make clean` remove as pastas dos binários.

O comando `make run` executa o programa com os parâmetros informados no exemplo acima.
