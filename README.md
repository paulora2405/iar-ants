# Trabalho Complementar 4 de CGR
  *Paulo Roberto Albuquerque*
## Simulação de Steering Behaviours

Para visualizar melhor este README, visite o repositório do projeto: [github](https://github.com/paulora2405/udesc/tree/master/cgr/TC4-cpp-port)

### Execução

IMPORTANTE: a resolução padrão da simulação é de 1600x900, caso seu monitor não suporte esta resolução, mude-a no cabeçalho do arquivo `boids_simulation.cpp` antes de executar o programa.

Para executar faça:
```sh
$ make
$ ./main
```

### Teclas de atalho

- Para excluir todas as entidades de uma vez, use `barra de espaço`.
- Para adicionar uma entidade clique em algum lugar da janela com o `botão esquerdo` do mouse.
- Para remover a entidade mais recente, clique com o `botão direito` do mouse.
- Para aumentar e diminuir a quantidade de entidades, use `m` e `n`.
- Para sair do programa, use `s`.

### Variáveis

A classe `Boid` tem algumas variáveis estáticas importantes:
- `max_velocity`
  - A velocidade máxima que uma entidade pode alcançar.
- `min_distance`
  - A distância mínina que uma entidade tenta manter das outras.
- `close_distance`
  - A distância máxima para se considerar uma outra entidade como estando "próxima".
- `border`
  - A distância máxima que uma entidade pode chegar da borda da janela.

### Dependências

O programa tem algumas depêndencias que podem ser instaladas facilmente, no SO Linux (Debian), basta fazer:

```sh
$ sudo apt install freeglut3 freeglut3-dev libglew-dev
```
para as instalar automaticamente.
