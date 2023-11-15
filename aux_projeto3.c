#include "bt.h"

reg_index init_index(){
    reg_index init_me;
    
    memset(init_me.chave.chave,'\0',SIZE_CHAVE_PRIMA);
    init_me.offset = 0;
    
    return init_me;
}



chave_prima init_chave_prima(){
    chave_prima init_me;
    
    memset(init_me.chave,'d',SIZE_CHAVE_PRIMA);
    
    return init_me;
}



void busca_menu(chave_prima *array_chaves, int qt_chaves){
    printf("\nAs chaves em busca_p.bin são:\n\n");
    for(int i = 0; i < qt_chaves; i++){
        printf("%d. ", i+1);
        printf("%s ", array_chaves[i].chave);
        printf("\n");
    }
    printf("\nQual você deseja inserir arquivo de dados? Digite -1 para voltar\n");
    printf("Digite a opção: ");
}



void insere_menu(registro *array_ins, int qt_registros){
    printf("\nOs registros em insere.bin são:\n\n");
    for(int i = 0; i < qt_registros; i++){
        printf("%d. ", i+1);
        printf("%s ", array_ins[i].cliente_code);
        printf("%s ", array_ins[i].veiculo_code);
        printf("\n");
    }
    printf("\nQual você deseja inserir arquivo de dados? Digite -1 para voltar\n");
    printf("Digite a opção: ");
}



bool chave_prima_igual(chave_prima chave1, chave_prima chave2){
    // se as duas chaves forem iguais retorna true, e se forem diferentes retorna false
    if(!strcmp(chave1.chave, chave2.chave))
        return true;
    return false;
}



void show_main_menu(){
    printf("\nSelecione a opção desejada: \n\n");
    printf("1. Inserir um registro\n");
    printf("2. Pesquisa por chave primária\n");
    printf("3. Sair\n\n");

    printf("Digite a opção desejada: ");
}



registro *load_insere(FILE *f, int *qt_registros){
    bool resized_flag = false;

    // i -> para contar chars dentro do buffer
    // count_registros -> para contar registros
    int i = 0, count_registros = 1;

    // cria uma array de 4 registros
    // se precisarmos de mais, o realloc será feito no while.
    registro *array_de_registros = (registro *) malloc(sizeof(registro) * 20);

    // leitura dos registros e armazenagem numa array de structs
    while (fread(array_de_registros[i].cliente_code, sizeof(char), SIZE_CLIENTE_CODE + 1, f)){
        char int_buffer[4];

        fread(array_de_registros[i].veiculo_code, sizeof(char), SIZE_VEICULO_CODE + 1, f);

        fread(array_de_registros[i].cliente_nome, sizeof(char), SIZE_CLIENTE_NOME, f);
        fread(array_de_registros[i].veiculo_nome, sizeof(char), SIZE_VEICULO_NOME, f);
        fread(int_buffer, sizeof(int), 1, f);
        array_de_registros[i].no_dias = atoi(int_buffer);

        count_registros++;
        i++;

        if (count_registros > 4){
            resized_flag = true;
            registro *temp_array = (registro *)realloc(array_de_registros, sizeof(registro) * count_registros);
            array_de_registros = temp_array;
        }
    }

    count_registros--;

    // caso o espaço não tenha sido realocado, diminuir tamanho do espaco
    // por exemplo, se existem apenas 3 registros, mudar o tamanho da array de 4 para 3 registros
    if (!resized_flag){
        registro *temp_array = (registro *)realloc(array_de_registros, sizeof(registro) * count_registros);
        array_de_registros = temp_array;
    }

    *qt_registros = count_registros;

    calculate_size(array_de_registros, count_registros);

    return array_de_registros;
}



chave_prima *load_busca(FILE *f, int *qt_chaves){
    rewind(f);
    // i -> para contar chars dentro do buffer
    // count_registros -> para contar registros
    int i = 0, count_registros = 0;

    // cria uma array de 50 chaves
    chave_prima *array_de_chaves = (chave_prima *) malloc(sizeof(chave_prima) * 20);
    char veiculo_buffer[8];

    memset(array_de_chaves[i].chave,0,19);
    // leitura dos registros e armazenagem numa array de structs
    while(fread(array_de_chaves[i].chave, sizeof(char), 11, f)){
        array_de_chaves[i].chave[12] = '\0';
        fgetc(f);

        memset(veiculo_buffer,0,8);

        fread(&veiculo_buffer, sizeof(char), 7, f);
        veiculo_buffer[8] = '\0';
        fgetc(f);

        strcat(array_de_chaves[i].chave, veiculo_buffer);

        i++;
        count_registros++;

        memset(array_de_chaves[i].chave,0,19);
    }

    *qt_chaves = count_registros;

    return array_de_chaves;
}



void print_registros(registro *array_de_registros, int qt_registros){
    printf("existem %d registros\n", qt_registros);
    for (int i = 0; i < qt_registros; i++){
        printf("Codigo Cliente: %s\n", array_de_registros[i].cliente_code);
        printf("Codigo Veiculo: %s\n", array_de_registros[i].veiculo_code);
        printf("Nome Cliente: %s\n", array_de_registros[i].cliente_nome);
        printf("Nome Veiculo: %s\n", array_de_registros[i].veiculo_nome);
        printf("Quantidade de dias: %d\n", array_de_registros[i].no_dias);
        printf("Qtd. de bytes necessarios para armazenamento: %d\n\n", array_de_registros[i].size_registro);
    }
}



void calculate_size(registro *array_de_registros, int qt_registros){
    for (int i = 0; i < qt_registros; i++){
        array_de_registros[i].size_registro =
            strlen(array_de_registros[i].cliente_code) +
            strlen(array_de_registros[i].veiculo_code) +
            strlen(array_de_registros[i].cliente_nome) +
            strlen(array_de_registros[i].veiculo_nome) +
            4 + // 04 bytes da quantidade de dias
            1 + // 01 byte que informa o tamanho do registro
            5;  // 04 bytes para os separadores de informações
    }
}



unsigned long int inserir_registro_base_de_dados(registro reg, FILE *f){
    char separador = '|';

    fseek(f, 0, SEEK_END);
    unsigned long int offset = ftell(f);
    fwrite(&(reg.size_registro), sizeof(char), 1, f);
    fwrite(reg.cliente_code, sizeof(char), 11, f);
    fwrite(&separador, sizeof(char), 1, f);
    fwrite(reg.veiculo_code, sizeof(char), 7, f);
    fwrite(&separador, sizeof(char), 1, f);
    fwrite(reg.cliente_nome, sizeof(char), strlen(reg.cliente_nome), f);
    fwrite(&separador, sizeof(char), 1, f);
    fwrite(reg.veiculo_nome, sizeof(char), strlen(reg.veiculo_nome), f);
    fwrite(&separador, sizeof(char), 1, f);
    fwrite(&(reg.no_dias), sizeof(int), 1, f);
    fwrite(&separador, sizeof(char), 1, f);

    return offset;
}



registro *get_registro_pelo_indice(reg_index item_escolhido, FILE *database_file){
    char char_buffer;
    int i = 0;

    // registro a ser retornado
    registro *reg_retornar = (registro*) malloc(sizeof(registro));

    // posiciona o cursor na posição certa para iniciar a leitura
    fseek(database_file, item_escolhido.offset, SEEK_SET);

    fread(&(reg_retornar->size_registro), sizeof(char), 1, database_file);

    while((char_buffer = fgetc(database_file)) != '|'){
        reg_retornar->cliente_code[i] = char_buffer;
        i++;
    }
    reg_retornar->cliente_code[i] = '\0';
    i = 0;

    while((char_buffer = fgetc(database_file)) != '|'){
        reg_retornar->veiculo_code[i] = char_buffer;
        i++;
    }
    reg_retornar->veiculo_code[i] = '\0';
    i = 0;
    
    while((char_buffer = fgetc(database_file)) != '|'){
        reg_retornar->cliente_nome[i] = char_buffer;
        i++;
    }
    reg_retornar->cliente_nome[i] = '\0';
    i = 0;
    
    while((char_buffer = fgetc(database_file)) != '|'){
        reg_retornar->veiculo_nome[i] = char_buffer;
        i++;
    }
    reg_retornar->veiculo_nome[i] = '\0';
    i = 0;

    fread(&(reg_retornar->no_dias), sizeof(int), 1, database_file);

    return reg_retornar;
}

void reg_index_copy(reg_index *dest, reg_index *source){
    strcpy(dest->chave.chave, source->chave.chave);
    dest->offset = source->offset;
}