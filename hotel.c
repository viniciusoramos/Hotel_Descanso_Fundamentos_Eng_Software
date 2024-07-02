#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#define MAX_CLIENTES 100
#define MAX_FUNCIONARIOS 50
#define MAX_QUARTOS 30
#define MAX_ESTADIAS 200

typedef struct {
    int codigo;
    char nome[50];
    char endereco[100];
    char telefone[15];
} Cliente;

typedef struct {
    int codigo;
    char nome[50];
    char telefone[15];
    char cargo[30];
    float salario;
} Funcionario;

typedef struct {
    int numero;
    int quantidade_hospedes;
    float valor_diaria;
    char status[10];  // "ocupado" ou "desocupado"
} Quarto;

typedef struct {
    int codigo_estadia;
    char data_entrada[11];
    char data_saida[11];
    int quantidade_diarias;
    int codigo_cliente;
    int numero_quarto;
} Estadia;

Cliente clientes[MAX_CLIENTES];
Funcionario funcionarios[MAX_FUNCIONARIOS];
Quarto quartos[MAX_QUARTOS];
Estadia estadias[MAX_ESTADIAS];

int total_clientes = 0;
int total_funcionarios = 0;
int total_quartos = 0;
int total_estadias = 0;

// Função strptime customizada
char* strptime(const char* s, const char* format, struct tm* tm) {
    sscanf(s, "%4d-%2d-%2d", &tm->tm_year, &tm->tm_mon, &tm->tm_mday);
    tm->tm_year -= 1900;
    tm->tm_mon -= 1;
    tm->tm_hour = 0;
    tm->tm_min = 0;
    tm->tm_sec = 0;
    tm->tm_isdst = -1;
    return (char*)(s + strlen(s));
}

// Funções de utilidade
int gerar_codigo_cliente() {
    return total_clientes + 1;
}

int gerar_codigo_funcionario() {
    return total_funcionarios + 1;
}

int gerar_codigo_estadia() {
    return total_estadias + 1;
}

int calcular_diarias(const char* data_entrada, const char* data_saida) {
    struct tm tm_entrada = {0};
    struct tm tm_saida = {0};
    strptime(data_entrada, "%Y-%m-%d", &tm_entrada);
    strptime(data_saida, "%Y-%m-%d", &tm_saida);
    time_t time_entrada = mktime(&tm_entrada);
    time_t time_saida = mktime(&tm_saida);
    double seconds = difftime(time_saida, time_entrada);
    return (int)(seconds / (60 * 60 * 24));
}

void cadastrar_cliente() {
    Cliente novo_cliente;
    novo_cliente.codigo = gerar_codigo_cliente();
    printf("Nome do Cliente: ");
    scanf(" %[^\n]", novo_cliente.nome);
    printf("Endereco: ");
    scanf(" %[^\n]", novo_cliente.endereco);
    printf("Telefone: ");
    scanf(" %[^\n]", novo_cliente.telefone);

    clientes[total_clientes] = novo_cliente;
    total_clientes++;

    printf("Cliente cadastrado com sucesso! Código: %d\n", novo_cliente.codigo);
}

void cadastrar_funcionario() {
    Funcionario novo_funcionario;
    novo_funcionario.codigo = gerar_codigo_funcionario();
    printf("Nome do Funcionario: ");
    scanf(" %[^\n]", novo_funcionario.nome);
    printf("Telefone: ");
    scanf(" %[^\n]", novo_funcionario.telefone);
    printf("Cargo: ");
    scanf(" %[^\n]", novo_funcionario.cargo);
    printf("Salario: ");
    scanf("%f", &novo_funcionario.salario);

    funcionarios[total_funcionarios] = novo_funcionario;
    total_funcionarios++;

    printf("Funcionario cadastrado com sucesso! Código: %d\n", novo_funcionario.codigo);
}

void cadastrar_estadia() {
    Estadia nova_estadia;
    int codigo_cliente, quantidade_hospedes, numero_quarto_disponivel = -1;
    char data_entrada[11], data_saida[11];

    printf("Código do Cliente: ");
    scanf("%d", &codigo_cliente);
    printf("Quantidade de Hospedes: ");
    scanf("%d", &quantidade_hospedes);
    printf("Data de Entrada (YYYY-MM-DD): ");
    scanf("%s", data_entrada);
    printf("Data de Saída (YYYY-MM-DD): ");
    scanf("%s", data_saida);

    int cliente_existe = 0;
    for (int i = 0; i < total_clientes; i++) {
        if (clientes[i].codigo == codigo_cliente) {
            cliente_existe = 1;
            break;
        }
    }
    if (!cliente_existe) {
        printf("Cliente não encontrado!\n");
        return;
    }

    for (int i = 0; i < total_quartos; i++) {
        if (quartos[i].quantidade_hospedes >= quantidade_hospedes && strcmp(quartos[i].status, "desocupado") == 0) {
            numero_quarto_disponivel = quartos[i].numero;
            break;
        }
    }
    if (numero_quarto_disponivel == -1) {
        printf("Nenhum quarto disponível para a quantidade de hospedes especificada.\n");
        return;
    }

    nova_estadia.codigo_estadia = gerar_codigo_estadia();
    strcpy(nova_estadia.data_entrada, data_entrada);
    strcpy(nova_estadia.data_saida, data_saida);
    nova_estadia.quantidade_diarias = calcular_diarias(data_entrada, data_saida);
    nova_estadia.codigo_cliente = codigo_cliente;
    nova_estadia.numero_quarto = numero_quarto_disponivel;

    estadias[total_estadias] = nova_estadia;
    total_estadias++;

    for (int i = 0; i < total_quartos; i++) {
        if (quartos[i].numero == numero_quarto_disponivel) {
            strcpy(quartos[i].status, "ocupado");
            break;
        }
    }

    printf("Estadia cadastrada com sucesso! Código da Estadia: %d\n", nova_estadia.codigo_estadia);
}

void dar_baixa_estadia() {
    int codigo_estadia;
    printf("Código da Estadia: ");
    scanf("%d", &codigo_estadia);

    int estadia_encontrada = 0;
    for (int i = 0; i < total_estadias; i++) {
        if (estadias[i].codigo_estadia == codigo_estadia) {
            estadia_encontrada = 1;
            int numero_quarto = estadias[i].numero_quarto;
            float valor_total = estadias[i].quantidade_diarias * quartos[numero_quarto - 1].valor_diaria;

            printf("Valor total a ser pago: %.2f\n", valor_total);

            for (int j = 0; j < total_quartos; j++) {
                if (quartos[j].numero == numero_quarto) {
                    strcpy(quartos[j].status, "desocupado");
                    break;
                }
            }

            for (int j = i; j < total_estadias - 1; j++) {
                estadias[j] = estadias[j + 1];
            }
            total_estadias--;
            break;
        }
    }

    if (!estadia_encontrada) {
        printf("Estadia não encontrada!\n");
    }
}

void pesquisar_cliente() {
    int codigo;
    char nome[50];
    printf("Pesquisar por (1) Código ou (2) Nome? ");
    int opcao;
    scanf("%d", &opcao);
    if (opcao == 1) {
        printf("Código do Cliente: ");
        scanf("%d", &codigo);
        for (int i = 0; i < total_clientes; i++) {
            if (clientes[i].codigo == codigo) {
                printf("Cliente encontrado: %s, %s, %s\n", clientes[i].nome, clientes[i].endereco, clientes[i].telefone);
                return;
            }
        }
    } else if (opcao == 2) {
        printf("Nome do Cliente: ");
        scanf(" %[^\n]", nome);
        for (int i = 0; i < total_clientes; i++) {
            if (strcmp(clientes[i].nome, nome) == 0) {
                printf("Cliente encontrado: %d, %s, %s\n", clientes[i].codigo, clientes[i].endereco, clientes[i].telefone);
                return;
            }
        }
    }
    printf("Cliente não encontrado!\n");
}

void pesquisar_funcionario() {
    int codigo;
    char nome[50];
    printf("Pesquisar por (1) Código ou (2) Nome? ");
    int opcao;
    scanf("%d", &opcao);
    if (opcao == 1) {
        printf("Código do Funcionario: ");
        scanf("%d", &codigo);
        for (int i = 0; i < total_funcionarios; i++) {
            if (funcionarios[i].codigo == codigo) {
                printf("Funcionario encontrado: %s, %s, %s, %.2f\n", funcionarios[i].nome, funcionarios[i].telefone, funcionarios[i].cargo, funcionarios[i].salario);
                return;
            }
        }
    } else if (opcao == 2) {
        printf("Nome do Funcionario: ");
        scanf(" %[^\n]", nome);
        for (int i = 0; i < total_funcionarios; i++) {
            if (strcmp(funcionarios[i].nome, nome) == 0) {
                printf("Funcionario encontrado: %d, %s, %s, %.2f\n", funcionarios[i].codigo, funcionarios[i].telefone, funcionarios[i].cargo, funcionarios[i].salario);
                return;
            }
        }
    }
    printf("Funcionario não encontrado!\n");
}

void cadastrar_quarto() {
    Quarto novo_quarto;
    novo_quarto.numero = total_quartos + 1;
    printf("Quantidade de Hospedes: ");
    scanf("%d", &novo_quarto.quantidade_hospedes);
    printf("Valor da Diaria: ");
    scanf("%f", &novo_quarto.valor_diaria);
    strcpy(novo_quarto.status, "desocupado");

    quartos[total_quartos] = novo_quarto;
    total_quartos++;

    printf("Quarto cadastrado com sucesso! Número do Quarto: %d\n", novo_quarto.numero);
}

void listar_estadias() {
    printf("Listagem de Estadias:\n");
    for (int i = 0; i < total_estadias; i++) {
        printf("Código da Estadia: %d | Cliente: %d | Quarto: %d | Data Entrada: %s | Data Saída: %s | Diárias: %d\n",
            estadias[i].codigo_estadia, estadias[i].codigo_cliente, estadias[i].numero_quarto,
            estadias[i].data_entrada, estadias[i].data_saida, estadias[i].quantidade_diarias);
    }
}

void menu_principal() {
    int opcao;
    do {
        printf("\n### Sistema de Gerenciamento Hoteleiro ###\n");
        printf("1. Cadastrar Cliente\n");
        printf("2. Cadastrar Funcionario\n");
        printf("3. Cadastrar Quarto\n");
        printf("4. Cadastrar Estadia\n");
        printf("5. Dar Baixa em Estadia\n");
        printf("6. Pesquisar Cliente\n");
        printf("7. Pesquisar Funcionario\n");
        printf("8. Listar Estadias\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                cadastrar_cliente();
                break;
            case 2:
                cadastrar_funcionario();
                break;
            case 3:
                cadastrar_quarto();
                break;
            case 4:
                cadastrar_estadia();
                break;
            case 5:
                dar_baixa_estadia();
                break;
            case 6:
                pesquisar_cliente();
                break;
            case 7:
                pesquisar_funcionario();
                break;
            case 8:
                listar_estadias();
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
    } while (opcao != 0);
}

int main() {
    // Exemplo de inicialização dos quartos (pode ser modificado conforme necessário)
    for (int i = 0; i < MAX_QUARTOS; i++) {
        Quarto quarto;
        quarto.numero = i + 1;
        quarto.quantidade_hospedes = (i % 5) + 1;  // Exemplo: variando de 1 a 5 hospedes por quarto
        quarto.valor_diaria = 100.0 + (i * 10.0);  // Exemplo: aumentando o valor da diaria para cada quarto
        strcpy(quarto.status, "desocupado");
        quartos[i] = quarto;
        total_quartos++;
    }

    menu_principal();

    return 0;
}