Gerenciador de Estruturas de Dados para Registros de Alunos

Este projeto em C implementa e gerencia três estruturas de dados distintas para armazenar e manipular registros de alunos: uma Max-Heap, uma Tabela Hash com Endereçamento Aberto e uma Árvore B+. O programa opera diretamente em arquivos binários, simulando um ambiente de armazenamento em disco, e oferece uma interface de menu baseada em texto para interagir com cada estrutura.

 Funcionalidades

    Gerar registros: Cria arquivos de registros de alunos com dados aleatórios (CPF, nome, nota).

    Manipulação de Heap (Prioridade é a nota do aluno):

        Construir uma Max-Heap a partir de um arquivo de registros.

        Inserir um novo registro.

        Remover o registro de maior prioridade (maior nota).

        Visualizar o registro de maior prioridade.

        Imprimir a Heap em formato de árvore ou como um vetor.

    Manipulação de Tabela Hash:

        Construir uma Tabela Hash a partir de um arquivo de registros.

        Inserir um novo registro.

        Buscar um registro pelo CPF.

        Remover um registro pelo CPF.

        Imprimir os conteúdos da tabela.

    Manipulação de Árvore B+:

        Construir uma Árvore B+ a partir de um arquivo de registros.

        Inserir um novo registro.

        Buscar um registro (e o nó folha onde ele se encontra) pelo CPF.

        Remover um registro pelo CPF.

        Imprimir todos os registros em ordem crescente de CPF, percorrendo as folhas.

 Estruturas de Dados

    Chave de Prioridade: nota do aluno. O registro com a maior nota fica na raiz.

    Implementação: A Heap é construída sobre um arquivo binário, que é tratado como um array. As operações sobeHeap e desceHeap manipulam os registros trocando suas posições no arquivo para manter a propriedade da Max-Heap.

    Arquivo: Utiliza um único arquivo (ex: files/heapRegistros.bin) que é uma cópia do arquivo de registros original e depois é reordenado.

Tabela Hash

    Chave: Os 9 primeiros dígitos do cpf (cpf / 100).

    Função de Hash: A função hash(cpf / 100, k) gera uma sequência de índices para sondagem. Colisões são resolvidas por endereçamento aberto.

    Arquivo: A Tabela Hash é um arquivo grande (ex: files/hashRegistros.bin) inicializado com "slots" vazios (cpf = -1). Os registros são inseridos na posição calculada pela função de hash.

Árvore B+

    Chave: Os 9 primeiros dígitos do cpf (cpf / 100).

    Ordem: Definida pela constante ORDEM (valor padrão: 10).

    Implementação: Otimizada para disco, utilizando três arquivos distintos:

        Arquivo de Metadados (.txt): Armazena informações essenciais como o ponteiro para a raiz, se a raiz é uma folha e o número total de registros.

        Arquivo de Índices (.bin): Contém os nós internos da árvore.

        Arquivo de Dados (.bin): Contém os nós folha da árvore, onde os registros completos (Registro) são armazenados. Os nós folha são ligados sequencialmente para permitir a travessia ordenada.

 Estrutura dos Arquivos

Antes da execução, apenas o código-fonte e o arquivo nomes.txt são necessários. Após a execução, o programa pode criar a seguinte estrutura de diretórios e arquivos:

.
├── main.c
├── nomes.txt
└── files/
    ├── registros.bin
    ├── registros.txt
    ├── heapRegistros.bin
    ├── hashRegistros.bin
    ├── bplusRegistros.bin
    ├── arquivoDadosArvore.bin
    ├── arquivoIndicesArvore.bin
    └── arquivoMetadadosArvore.txt

    É importante colocar "files/" antes do nome do arquivo para achar o diretório

    nomes.txt: Arquivo de entrada com nomes para geração aleatória (deve estar no mesmo diretório).

    registros.bin / registros.txt: Arquivo principal de registros gerados. A versão .txt é para fácil visualização.

    Os outros arquivos são usados pelas respectivas estruturas de dados.

    As estruturas também podem escrever seus registros em um arquivo texto para melhor visualização;

    O programa vem com alguns exemplos de estruturas já montadas em arquivo, são os arquivos enumerados de 0 a 4 no diretório files/

Compilar e executar

    O arquivo nomes.txt no mesmo diretório do executável.

    Criar manualmente um diretório chamado files no mesmo local do executável.

Compilação

gcc main.c -o programaPrincipal

./programaPrincipal

Execução

    Menu Principal: Ao iniciar, o programa exibirá o menu principal.

    ==================== MENU PRINCIPAL ====================
    1. Gerar Arquivos de Registros
    2. Heap
    3. Tabela Hash
    4. Árvore B+
    5. Sair do Programa
    ========================================================

    Gerar Registros: Caso não tenha arquivos prontos ou não queira utilizar os de exemplo, primeira etapa deve ser sempre a opção 1. Isso criará os arquivos de registros (files/registros.bin, etc.) que servem de base para todas as outras estruturas. Você será solicitado a informar quantos registros deseja criar.

    Escolher uma Estrutura: Após gerar os dados, escolha uma das opções (2, 3 ou 4) para acessar o submenu da estrutura correspondente.

    Submenus:

        No menu da Heap, você pode construir a heap, inserir, remover ou buscar o elemento de maior nota.

        No menu da Tabela Hash, você pode construir a tabela, inserir, remover ou buscar um aluno pelo CPF.

        No menu da Árvore B+, a primeira ação deve ser construir a árvore (opção 'c'). Depois, você pode inserir, remover ou buscar registros. Caso esteja utilizando um arquivo preenchido com a árvore.

🔗 Dependências

    nomes.txt: O programa depende de um arquivo chamado nomes.txt para gerar nomes de alunos aleatórios. Este arquivo deve conter uma lista de nomes e sobrenomes separados por ponto e vírgula (;), com um nome/sobrenome por linha. A função criaNomeAleatorio seleciona 4 partes aleatórias deste arquivo para compor um nome completo.

    Biblioteca Matemática (math.h): Necessária para a função pow, exigindo a flag -lm na compilação.