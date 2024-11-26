//Name: Acacie Song
//ID: 261182381

#include <time.h>
#include <stdio.h>
#include "database.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void print_record(Record const *r);
void print_database(Database const *db);
Record parse_record(char const * line);

//print
void print_record(Record const *r){
	printf("Handle: %s\n", r->handle);
	printf("Comment: %s\n", r->comment);
	printf("Followers: %ld\n", r->followers);
	printf("Time: %ld\n", r->timestamp);
}

void print_database(Database const *db){
	for(int i=0; i<db->size; i++){
		printf("%d\n",i);
		print_record(&db->data[i]);
	}
	if(db->size==0){
		printf("No record\n");
	}
	printf("Size: %d\n", db->size);
	printf("Capacity: %d\n", db->capacity);
}
//loading
void db_load_csv(Database * db, char const * path){
	FILE *file = fopen(path, "r");
	//no file exists
	if(file == NULL){
		fprintf(stderr, "No File\n");
		exit(1);
	}
	//add each line in the file to a database
	char *line = NULL;
	size_t size=0;
	while(getline(&line, &size, file) != -1){
		line[strcspn(line, "\n")]='\0';
		Record r = parse_record(line);
		db_append(db,&r);
	}
	free(line);	
	fclose(file);
}

void db_write_csv(Database * db, char const * path){
	//overwrite file with the contents of the database in csv
	FILE *file = fopen(path, "wt");
	//the file doesn't exist
	if(file==NULL){ 
		fprintf(stderr, "No File\n");
		exit(1);
	}
	//print in format of csv, handle, followers, comment, timestamp
	for(int i=0; i<db->size; i++){
		fprintf(file,"%s,%ld,%s,%ld\n",db->data[i].handle, db->data[i].followers, db->data[i].comment, db->data[i].timestamp);
	}
	fclose(file);
}

Record parse_record(char const * line){
	//parses a line of csv into a record		
	char *line_c = strdup(line);
	char *token;
	Record r;
	
	//get each value in the line
	//handle
	token = strtok(line_c, ",");
	if(token != NULL){
		strcpy(r.handle, token);		
	}
	//follower
	token = strtok(NULL, ",");
	if(token != NULL){ 
		char *endptr;
		r.followers = strtoul(token, &endptr, 10);
		//as we assume that csv is right.
	}
	//comment
	token = strtok(NULL, ",");
	if(token != NULL){
		strcpy(r.comment, token);
	}
	//time
	token = strtok(NULL, ",");
	if(token !=NULL){
		char *endptr;
		r.timestamp = strtoul(token, &endptr, 10);
		//as we don't need to verify for csv the special case or space.
	}
	free(line_c);
	return r;
}

Database db_create(){
	Database db;
	//create initial database
	db.data=(Record *)malloc(4 * sizeof(Record));
	//initialize 
	db.capacity=4;
	db.size=0;
	return db;
}

void db_append(Database * db, Record const * item){
	//if not enough capacity
	if(db->size >= db->capacity){
		db->capacity *=2;
		//new capacity
		db->data = (Record *)realloc(db->data, db->capacity * sizeof(Record));
	}
	//put the record in database
	db->data[db->size] = *item;
	db->size++;
}

Record * db_index(Database *db, int index){
	//invalide index out of bound or negative
	if(index>=db->size||index<0){
		return NULL;
	}
	return &db->data[index];
}

Record * db_lookup(Database *db, char const * handle){
	//if no input
	if(db==NULL || handle == NULL){
		return NULL;
	}
	//check for if have same handle
	for(int i=0; i<db->size; i++){
		if(strcmp(db->data[i].handle, handle) == 0){
			return &db->data[i];	
		}
	}
	return NULL; //if didn't found
}
void db_free(Database * db){
	free(db->data);
	db->data=NULL;
	db->capacity=0;
	db->size=0;
}
