#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// ============================================================================
// --- Constantes Globais ---
// ============================================================================
#define QTD_TERRITORIOS 5
#define MAX_NOME 30
#define MAX_COR 15
#define QTD_MISSOES 2
#define PLAYER_COR "AZUL" // A cor fixa do jogador para este exercício

// ============================================================================
// --- Estrutura de Dados ---
// ============================================================================
struct Territorio {
    char nome[MAX_NOME];
    char cor[MAX_COR];
    int tropas;
};

// ============================================================================
// --- Protótipos das Funções ---
// ============================================================================

// Setup e Memória
struct Territorio* alocarMapa();
void inicializarTerritorios(struct Territorio* mapa);
void liberarMemoria(struct Territorio* mapa);

// Interface e Saída (Const Correctness: apenas leem os dados)
void exibirMenuPrincipal();
void exibirMapa(const struct Territorio* mapa);
void exibirMissao(int idMissao);

// Lógica do Jogo
void faseDeAtaque(struct Territorio* mapa);
void simularAtaque(struct Territorio* atacante, struct Territorio* defensor);
int sortearMissao();
int verificarVitoria(const struct Territorio* mapa, int idMissao);

// Utilitários
void limparBufferEntrada();

// ============================================================================
// --- Função Principal (main) ---
// ============================================================================
int main() {
    // 1. Configuração Inicial (Setup)
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL)); // Semente aleatória baseada no tempo

    struct Territorio* mapa = alocarMapa();
    if (mapa == NULL) {
        printf("Erro fatal: Falha na alocação de memória.\n");
        return 1;
    }

    inicializarTerritorios(mapa);
    int missaoAtual = sortearMissao();
    int opcao = -1;

    printf("Bem-vindo ao WAR Estruturado!\n");
    printf("Você é o general do exército %s.\n", PLAYER_COR);
    
    // 2. Laço Principal do Jogo (Game Loop)
    do {
        // Exibição do estado atual
        printf("\n========================================\n");
        exibirMissao(missaoAtual);
        exibirMapa(mapa);
        exibirMenuPrincipal();

        // Leitura da escolha
        if (scanf("%d", &opcao) != 1) {
            limparBufferEntrada();
            continue;
        }
        limparBufferEntrada(); // Consumir o \n

        // Lógica de controle
        switch (opcao) {
            case 1:
                faseDeAtaque(mapa);
                break;
            case 2:
                if (verificarVitoria(mapa, missaoAtual)) {
                    printf("\nPARABÉNS! MISSÃO CUMPRIDA! VICTORY!\n");
                    opcao = 0; // Força a saída do loop
                } else {
                    printf("\nAinda não. Continue lutando!\n");
                }
                break;
            case 0:
                printf("\nEncerrando o jogo...\n");
                break;
            default:
                printf("\nOpção inválida.\n");
        }

        if (opcao != 0) {
            printf("\nPressione ENTER para continuar...");
            getchar();
        }

    } while (opcao != 0);

    // 3. Limpeza
    liberarMemoria(mapa);

    return 0;
}

// ============================================================================
// --- Implementação das Funções ---
// ============================================================================

// --- Setup e Memória ---

struct Territorio* alocarMapa() {
    // Utiliza calloc para limpar a memória inicial (zera os bytes)
    struct Territorio* ptr = (struct Territorio*) calloc(QTD_TERRITORIOS, sizeof(struct Territorio));
    return ptr;
}

void inicializarTerritorios(struct Territorio* mapa) {
    // Configuração manual para cenário de teste
    // Jogador (AZUL) começa com 2 territórios, inimigos com 3
    strcpy(mapa[0].nome, "Brasil");
    strcpy(mapa[0].cor, "AZUL");
    mapa[0].tropas = 3;

    strcpy(mapa[1].nome, "Argentina");
    strcpy(mapa[1].cor, "VERMELHO");
    mapa[1].tropas = 2;

    strcpy(mapa[2].nome, "Australia");
    strcpy(mapa[2].cor, "AZUL");
    mapa[2].tropas = 5;

    strcpy(mapa[3].nome, "Egito");
    strcpy(mapa[3].cor, "AMARELO");
    mapa[3].tropas = 2;

    strcpy(mapa[4].nome, "China");
    strcpy(mapa[4].cor, "VERMELHO");
    mapa[4].tropas = 4;
}

void liberarMemoria(struct Territorio* mapa) {
    if (mapa != NULL) {
        free(mapa);
        printf("Memória liberada com sucesso.\n");
    }
}

// --- Interface e Saída ---

void exibirMenuPrincipal() {
    printf("\n--- MENU DE AÇÃO ---\n");
    printf("1. Atacar Território\n");
    printf("2. Verificar Objetivo (Vitória)\n");
    printf("0. Sair do Jogo\n");
    printf("Escolha: ");
}

// Uso de 'const' garante que esta função não altere o jogo
void exibirMapa(const struct Territorio* mapa) {
    printf("\n--- MAPA MUNDI ---\n");
    printf("| ID | %-15s | %-10s | %s |\n", "NOME", "COR", "TROPAS");
    printf("|----|-----------------|------------|--------|\n");
    for (int i = 0; i < QTD_TERRITORIOS; i++) {
        printf("| %d  | %-15s | %-10s | %-6d |\n", 
               i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("----------------------------------------------\n");
}

void exibirMissao(int idMissao) {
    printf("MISSÃO ATUAL: ");
    if (idMissao == 1) {
        printf("Conquistar 4 territórios.\n");
    } else {
        printf("Destruir totalmente o exército VERMELHO.\n");
    }
}

// --- Lógica do Jogo ---

int sortearMissao() {
    // Retorna 1 ou 2
    return (rand() % QTD_MISSOES) + 1;
}

void faseDeAtaque(struct Territorio* mapa) {
    int origem, destino;
    
    printf("\n--- FASE DE ATAQUE ---\n");
    printf("Digite o ID do seu território (Atacante): ");
    if (scanf("%d", &origem) != 1) { limparBufferEntrada(); return; }

    printf("Digite o ID do território inimigo (Defensor): ");
    if (scanf("%d", &destino) != 1) { limparBufferEntrada(); return; }
    limparBufferEntrada();

    // Validações Básicas
    if (origem < 0 || origem >= QTD_TERRITORIOS || destino < 0 || destino >= QTD_TERRITORIOS) {
        printf("IDs inválidos.\n");
        return;
    }
    
    // Passagem por referência para alterar os dados
    struct Territorio* tOrigem = &mapa[origem];
    struct Territorio* tDestino = &mapa[destino];

    // Regras de validação do WAR
    if (strcmp(tOrigem->cor, PLAYER_COR) != 0) {
        printf("Você só pode atacar com seus próprios territórios (%s)!\n", PLAYER_COR);
        return;
    }
    if (strcmp(tOrigem->cor, tDestino->cor) == 0) {
        printf("Você não pode atacar a si mesmo!\n");
        return;
    }
    if (tOrigem->tropas <= 1) {
        printf("Tropas insuficientes para ataque (Mínimo 2).\n");
        return;
    }

    // Chama a simulação passando os ponteiros dos territórios específicos
    simularAtaque(tOrigem, tDestino);
}

void simularAtaque(struct Territorio* atacante, struct Territorio* defensor) {
    printf("\nBatalha iniciada: %s vs %s\n", atacante->nome, defensor->nome);
    
    // Simulação simples de dados (1 dado cada)
    int dadoAtaque = (rand() % 6) + 1;
    int dadoDefesa = (rand() % 6) + 1;

    printf("Dados: Ataque [%d] vs Defesa [%d]\n", dadoAtaque, dadoDefesa);

    if (dadoAtaque > dadoDefesa) {
        printf("Vitória do ataque! Defensor perde 1 tropa.\n");
        defensor->tropas--;

        // Verifica conquista
        if (defensor->tropas == 0) {
            printf("TERRITÓRIO CONQUISTADO!\n");
            strcpy(defensor->cor, atacante->cor); // Troca a cor
            defensor->tropas = 1; // Move 1 tropa
            atacante->tropas--;   // Sai 1 tropa da origem
        }
    } else {
        printf("Defesa segurou! Atacante perde 1 tropa.\n");
        atacante->tropas--;
    }
}

int verificarVitoria(const struct Territorio* mapa, int idMissao) {
    int territoriosDominados = 0;
    int vermelhosVivos = 0;

    // Varredura única no mapa (eficiência)
    for (int i = 0; i < QTD_TERRITORIOS; i++) {
        if (strcmp(mapa[i].cor, PLAYER_COR) == 0) {
            territoriosDominados++;
        }
        if (strcmp(mapa[i].cor, "VERMELHO") == 0) {
            vermelhosVivos++;
        }
    }

    // Lógica da Missão 1: Conquistar 4 territórios
    if (idMissao == 1) {
        return (territoriosDominados >= 4);
    }
    // Lógica da Missão 2: Destruir exército VERMELHO
    else if (idMissao == 2) {
        return (vermelhosVivos == 0);
    }

    return 0;
}

// --- Utilitários ---

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}