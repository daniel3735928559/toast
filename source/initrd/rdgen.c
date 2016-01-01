#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RD_SIZE 64
#define FN_SIZE 16

struct rd_header
{
	
};

struct rd_entry
{
  char name[FN_SIZE];
  //unsigned int is_dir;//new
  unsigned int start;
  unsigned int size;
};

int main(char argc, char **argv)
{
  char manif[64] = "./manifest";
  if(!argc){ strcpy(manif, argv[0]); }
  FILE *setup = fopen(manif, "r");
  if(setup == NULL){
    printf("Error accessing: %s -- File not found. \n", manif);
    return 0;
  }
  char next[100];
  int n = 0;
  int i = 0;
  if(fgets(next, 64, setup) != NULL)
    sscanf(next, "%d", &n);
  else
    return -1;
  //int current_start = n * sizeof(struct rd_entry) + sizeof(int);
  int current_start = 0;
  struct rd_entry entries[n];
  while(fgets(next, FN_SIZE+20, setup) != NULL){
    char next_name[FN_SIZE];
    int next_size;
    sscanf(next, "%d,%s", &next_size, &next_name);
    strcpy(entries[i].name, next_name);
    entries[i].start = current_start;
    entries[i].size = next_size;
    current_start += next_size;
    i++;
  }
  fclose(setup);
  FILE *output = fopen("./initrd.img", "w");
  fwrite(&n, sizeof(int), 1, output);
  fwrite(entries, sizeof(struct rd_entry), n, output);
  for(i = 0; i < n; i++){
    char next_file[FN_SIZE+10] = "./files/";
    strcat(next_file, entries[i].name);
    FILE *in = fopen(next_file, "r");
    char *buffer = (unsigned char *)malloc(entries[i].size);
    fread(buffer, 1, entries[i].size, in);
    fwrite(buffer, 1, entries[i].size, output);
    fclose(in);
    free(buffer);
  }
  fclose(output);
  return 0;
}
