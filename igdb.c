//Name: Acacie Song
//ID: 261182381

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "database.h"
#include <stdbool.h> //bool to be more readable
#include <limits.h>
#include<errno.h>

//list
char * conversion_UNIX_to_normal_time(long unsigned int unixDate);
void list(Database *db);
//testInput
int no_illegal_element(int length, char *input);
int test_handle_followers(Database *db, char *handle, char *endptr);
int test_comments(int length, char * comments);

int add(Database *db, char * handleT, char * followersT);
int update(Database *db, char *handleT, char *followersT);

/*typedef int bool
 * const int true=1, false=2;*/

int main_loop(Database * db){
char operation[100];
bool isSave = true;
printf("Loaded %d records.\n", db->size);
while(strcmp(operation, "exit")!=0){
	//get Operation
	printf("> ");
        char *fgetsResult = fgets(operation, sizeof(operation), stdin);
	//if Ctrl D pressed
	if(fgetsResult==NULL){
		if(feof(stdin)){
			printf("Ctrl + D is pressed. Exit\n");
			break;
		}else{
		printf("hey");
		}
	}
	operation[strcspn(operation, "\n")]='\0';
	//check if too long
	if (strlen(operation) >= sizeof(operation) - 1) {
            printf("The operation is too long\n");
            // Clear buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);

            continue;
        }
	char *token;
	if(operation[0] == '\0') {
		printf("No input. Please reenter a valid operation\n");
		continue;
	}
	token = strtok(operation," ");
        //list
	if(strcmp(token, "list")==0){
		//too much argument
		token = strtok(NULL," ");
		if(token!=NULL){
			printf("Too Much Argument\n");
			continue;
		}
		list(db);
	//add
        }else if(strcmp(token, "add")==0){
		
		token = strtok(NULL, " ");
		char *handleT = token;
		token = strtok(NULL, " ");
		char * followersT = token;
		//if just 1 or 2 argument
		if(handleT==NULL||followersT==NULL){
			printf("Error: usage: add HANDLE FOLLOWERS\n");
			continue;
		}
		
		//if have more than 3
		token=strtok(NULL, " ");
		if(token!=NULL){
			printf("Error: usage: add HANDLE FOLLOWERS\n");
			continue;
		}
		
		if(add(db, handleT, followersT)==true){
			isSave = false;
		}
	//update
        }else if(strcmp(token, "update")==0){
		
		token = strtok(NULL, " ");
		char *handleT = token;
		token = strtok(NULL," ");
		char *followersT = token;
		//if just 1 or 2 argument;
		if(handleT==NULL||followersT==NULL){
			printf("Error: usage: update HANDLE FOLLOWERS\n");
			continue;
		}

		//if more than 3 arguments
		token=strtok(NULL, " ");
		if(token!=NULL){
			printf("Error: usage: update HANDLE FOLLOWERS\n");
			continue;
		}
		
		if(update(db, handleT, followersT)==true){
			isSave = false;
		}
	//save
        }else if(strcmp(token, "save")==0){
		//too much argument
		token = strtok(NULL, "");
		if(token!=NULL){
			printf("Error: too much argument\n");
			continue;
		}

                db_write_csv(db, "database.csv");
		isSave = true;
		printf("Wrote %d records.\n",db->size);
        //exit
	}else if(strcmp(token, "exit")==0){
		
		token = strtok(NULL, "");
		if(token!=NULL){
			//if it is exit fr, exit program
			if(strcmp(token, "fr")==0){ 
				break;
			}else{ //if else, too much argument
				printf("Error: too much argument except fr\n");
				continue;
			}
		}
		//warning to save change
		if(isSave == false){
                	printf("Error: you did not save your changes. Use 'exit fr' to force exiting anyway\n");
			strcpy(operation, "");
		}
        }else{
                printf("Incorrect Output: it has to be --> \nlist, add HANDLE FOLLOWERS, update HANDLE FOLLOWERS, save, exit, exit fr or Ctrl+D\n");
        }
}
db_free(db);
return 0;
}

int main()
{
    Database db = db_create();
    db_load_csv(&db, "database.csv");
    return main_loop(&db);
}

//time
char * conversion_UNIX_to_normal_time(long unsigned int unixDate){
	//Conversion UNIX timestamp to time_t type
	time_t timestamp = (time_t) unixDate;
        //separate time information
	struct tm *timeinfo;
	//memory to stored info
        char *buffer = malloc(80*sizeof(char));

	//Convert UNIX to local time and format it
        timeinfo = localtime(&timestamp);
        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
	
	return buffer;
}

void list(Database *db){
	int rows=db->size;
	
	//header
	printf("HANDLE                          | FOLLOWERS | LAST MODIFIED       | COMMENT                           \n");
	printf("--------------------------------|-----------|---------------------|-----------------------------------\n");
	
	//show each row data
	for(int i=0; i<rows; i++){
		printf("%-32s|", db->data[i].handle);
		printf("%-11ld|", db->data[i].followers);
		char *normalTime = conversion_UNIX_to_normal_time(db->data[i].timestamp);
		printf("%-21s|", normalTime);
		printf("%s", db->data[i].comment);
		printf("\n");
		free(normalTime);
	}
}
//validate Input

int no_illegal_element(int length, char *input){
	//no commas, no \n and \0
	for(int i=0; i<length; i++){
		//to verify two element
		if(input[i]=='\\'){
			i++;
			if(input[i]=='n'||input[i]=='0'||input[i]=='t'){
				printf("Error: cannot contain \\n, \\0 or \\t\n");
				return false;
			}
		}

		if(input[i]==','){
			printf("Error: cannot contain commas.\n");
			return false;
		}
	}
	return true;
}

int test_comments(int length, char *comments){
	//Test comments too long	
	if(comments[length-1]!='\n'){
		printf("the comment is too long\n");
		//clear overflow
		int c;
		while((c=getchar())!='\n'&&c!=EOF);
		return false;
	}
	//delete \n of it.
	comments[strcspn(comments, "\n")]='\0';
	
	//no commas, no \n, no \0.
	if(no_illegal_element(length, comments)==false){
		return false;
	}
	return true;
}

int test_handle_followers(Database *db, char *handleT, char *endptr){
	//no @
	if(handleT [0] !='@'){
		printf("you don't have @\n");
		return false;
        }
	//the length of username
	int length = strlen(handleT);
        //+1 or not
        if(length+1>32){
                printf("the username is too long\n");
		//clear overflow
		for (int i=32; i<length + 1; i++){
			handleT[i]='\0';
		}
		return false;
        }
	//no commas, \n, \0
	if(no_illegal_element(length, handleT)==false){
		return false;
	}
	//if the 3nd argument followers is an integer
	if(*endptr!='\0'){
		printf("The 3rd arg followers has to be an integer\n");
		return false;
	}
	return true;
}

int update(Database *db, char * handleT, char * followersT){
	//neg
	if(followersT[0]=='-'){
                printf("follower should be positive\n");
                return false;
        }
	char *endptr;
	long unsigned int followersTInt = strtoul(followersT, &endptr, 10);
	//too much int
	if (followersTInt >= ULONG_MAX && errno == ERANGE) {
		printf("Error: Input exceeds max value of unsigned long int.\n");
        	return false;
    	}
	//validate input handle and followers
	if(test_handle_followers(db, handleT, endptr)==false){
		return false;		
	}
	//validate if the handle already exists, if yes, you can update it
	Record *r;
	r=db_lookup(db, handleT);
        if(r==NULL){
                printf("Error: no entry with handle %s\n", handleT);
                return false;
        }
	
	//comments
        char comments[64];
	printf("Comment> ");
        fgets(comments, sizeof(comments), stdin);
	
	int length = strlen(comments);
	//testing comments
	if(test_comments(length, comments)==false){
		return false;
	}
	//updating
	strncpy(r->handle, handleT, sizeof(r->handle) -1);
        r->handle[sizeof(r->handle)-1]='\0';
        strncpy(r->comment, comments, sizeof(r->comment)-1);
        r->comment[sizeof(r->comment)-1]='\0';
	r->followers = followersTInt;
	r->timestamp = (long unsigned int) time(NULL);
	return true;
}

int add(Database *db, char * handleT, char * followersT){
	//negative
	if(followersT[0]=='-'){
        	printf("follower should be positive\n");
		return false;
	}
	char *endptr;
	long unsigned int followersTInt = strtoul(followersT, &endptr, 10);
	//too much integer
	if (followersTInt == ULONG_MAX && errno == ERANGE) {
                printf("Error: Input exceeds max value of unsigned long int.\n");
                return false;
        }
	//validate input handle and Followers
	if(test_handle_followers(db, handleT, endptr)==false){
		return false;		
	}
	//validate if the handle already exists, if yes, you can't add it.
	if(db_lookup(db, handleT)!=NULL){
		printf("handle %s already exists. \n", handleT);
		return false;
	}
	//comments
	char comments[64];
	printf("Comment> ");
        fgets(comments, sizeof(comments), stdin);
	//validate comments
	int length = strlen(comments);
	if(test_comments(length, comments)==false){
		return false;
	}
	//add the new record
	Record r = (Record){
		.handle = "",
		.comment = "",
		.followers = followersTInt,
		.timestamp = (long int) time(NULL), 
	};
	strncpy(r.handle, handleT, sizeof(r.handle) -1);
	r.handle[sizeof(r.handle)-1]='\0';
	strncpy(r.comment, comments, sizeof(r.comment)-1);
	r.comment[sizeof(r.comment)-1]='\0';
		
	db_append(db, &r); 
	return true;
}
