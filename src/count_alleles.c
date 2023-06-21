/*Quick calculation of MLST distance matrices*/
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>


static const char* delimiters = "\t\n\r";
static const char REPLACE_CHAR = ' ';
static size_t MAX_LINE = 1E7;

typedef struct MLSTEntry {
    uint32_t* alleles;
    char* sample;
}MLSTEntry;

typedef struct MLSTData {
    MLSTEntry** data;
    uint32_t columns;
    uint32_t rows;
}MLSTData;


static inline __attribute__((always_inline)) MLSTData* parse_mlst_file(const char* restrict file_path){

    
    FILE* in = (FILE*)fopen(file_path, "r");
    if(!in){
        fprintf(stderr, "Error: cannot open file %s", file_path);
        exit(EXIT_FAILURE);
    }

    //const size_t MAX_LINE = 1E5; // max number of rows
    char* token;
    uint32_t columns = 0;
    uint32_t rows = 0;


    MLSTEntry** mlst_data = (MLSTEntry**)malloc(MAX_LINE * sizeof(MLSTEntry*));
    char* buffer = (char*)malloc(sizeof(char) * MAX_LINE);
    getline(&buffer, &MAX_LINE, in); // Throw away first space
    //fgets(buffer, MAX_LINE, in); // Throw away first space
    
    char* save_col;
    token = strtok_r(buffer, delimiters, &save_col);
    while((token = strtok_r(NULL, delimiters, &save_col))){
        ++columns;
        
    }


    while(getline(&buffer, &MAX_LINE, in) != -1){
    //while(fgets(buffer, MAX_LINE, in) != -1){
        uint32_t vec_idx = 0;
        char* save;
        // Was using str_tok save buffer wrong..
        token = strtok_r(buffer, delimiters, &save);

        MLSTEntry* entry = (MLSTEntry*)malloc(sizeof(MLSTEntry));
        entry->sample = (char*)malloc((strlen(token) + 1) * sizeof(char));
        strcpy(entry->sample, token);
        
        entry->alleles = (uint32_t*)malloc(sizeof(uint32_t) * columns);
        //entry->alleles = (uint32_t*)calloc(sizeof(uint32_t), columns);
        //strtok seems to be using up most of the call times
        while((token = strtok_r(NULL, delimiters, &save))){
            // Tokens which are text are converted to 0
            entry->alleles[vec_idx] = abs(atoi(token));
            vec_idx++;        
        } 
        
        mlst_data[rows] = entry;
        ++rows;
    }
    
    free(buffer);
    fclose(in);
    // TODO keep seperate ID array in the future, structs seem slower...
    
    MLSTData* out_data = (MLSTData*)malloc(sizeof(MLSTData));
    out_data->data = mlst_data;
    out_data->columns = columns;
    out_data-> rows = rows;
    return out_data;
}



/**
 * @brief simple has function to has already done combos for indexing
 * 
 * From: https://stackoverflow.com/questions/7666509/hash-function-for-string
 */
static inline __attribute__((always_inline)) uint32_t djb2_hash(char* str){
    uint32_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;

}


static inline __attribute__((always_inline)) uint32_t dist_func(const uint32_t* restrict s1, const uint32_t* restrict s2, const uint32_t cols){
    uint32_t dist = 0;
    for(size_t i = 0; i < cols; i++){
        //fprintf(stderr, "out: %lu\n", i);
        //TODO need to get this to ignore 0's
        // Torsty program ignores 0'
        // Conditionally negate values without branching
        // TODO play with bit twiddling to make this branchless..
        //bool s1_neg = s1_val ^ s1;
        //bool s2_neg = s2_val;
        // print negation values
        //fprintf(stderr, "val 1: %u, %u\n", s1_val, (s1_val ^ -s2_neg) + s2_neg);
        //fprintf(stderr, "val 2: %u, %u\n", s2_val, (s2_val ^ -s1_neg) + s1_neg);
        
        //dist = dist + (~(s1[i] ^ s2[i]) & 0x01);
	if(s1[i] != 0 && s2[i] != 0){
	        dist = dist + (s1[i] != s2[i]);
	}

        // This reduce branch misprediction????
        //if(s1[i] != s2[i] && s1[i] != 0 && s2[i] != 0){
        //    ++dist;
        //}        
    }
    return dist;
}

int main(int argc, char** argv){
    //struct stat sb;
    //char* buf = malloc(sizeof(char) * sb.st_blksize); // buffer IO more efficiently
    //setvbuf(stdout, buf, _IOFBF, sizeof(char));
    
    MLSTData* mlst_data = parse_mlst_file(argv[1]);
    uint32_t rows = mlst_data->rows;
    uint32_t columns = mlst_data->columns;

    MLSTEntry** data = mlst_data->data;
    rows--;
    uint32_t start = 0;
    //for loop seems faster
    for(size_t j = rows; j; --j){
        MLSTEntry* element_end = data[j];
        for(size_t i = 0; i < rows; i++ ){
            char* t1 = data[i]->sample;
            char* t2 = data[j]->sample;
            uint32_t unique_hash = djb2_hash(t1) + djb2_hash(t2);
            fprintf(stdout, "%u\t%s|%s\t%u\n", unique_hash, t1, t2, dist_func(data[i]->alleles, data[j]->alleles, columns));
        }
        --rows;
        // these frees are likely slowing it down
        free(data[j]->sample);
        free(data[j]->alleles);
        free(data[j]);
    }
    free(data[0]->sample);
    free(data[0]->alleles);
    free(data[0]);

    free(data);
    free(mlst_data);

    return 0;
}
