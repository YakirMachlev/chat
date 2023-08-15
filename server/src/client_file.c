#include "clients_file.h"

static pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;

bool check_name_validity(char *name)
{
    uint8_t offset;
    for (offset = 0; offset < NAME_MAX_LENGTH && *name; offset++)
    {
        if (!isalnum(*(name++)))
        {
            return false;
        }
    }
    return true;
}

void insert_client_to_file(char *name, char *password)
{
    FILE *fp;

    pthread_mutex_lock(&file_lock);
    fp = fopen("users.txt", "a");
    if (fp)
    {
        fputs(name, fp);
        fputc(',', fp);
        fputs(password, fp);
        fputc('\n', fp);

        fclose(fp);
    }
    pthread_mutex_unlock(&file_lock);
}

bool client_file_does_client_exist(char *name)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    bool exists;
    uint8_t length;

    pthread_mutex_lock(&file_lock);
    fp = fopen("users.txt", "r");
    exists = false;
    if (fp)
    {
        while (fgets(line, MAX_LINE_LENGTH, fp) && !exists)
        {
            length = strchr(line, ',') - line;
            exists = !strncmp(name, line, length);
        }
        fclose(fp);
    }
    pthread_mutex_unlock(&file_lock);

    return exists;
}

bool client_file_check_client_validity(char *name, char *password)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    uint8_t length;
    bool valid;

    pthread_mutex_lock(&file_lock);
    fp = fopen("users.txt", "r");
    valid = false;
    if (fp)
    {
        while (fgets(line, MAX_LINE_LENGTH, fp) && !valid)
        {
            length = strchr(line, ',') - line;
            valid = !strncmp(name, line, length) && !strncmp(password, line + length + 1, PASSWORD_MAX_LENGTH);
        }
        fclose(fp);
    }
    pthread_mutex_unlock(&file_lock);
    return length;
}