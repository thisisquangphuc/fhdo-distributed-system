#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>

/* strtok_r() won't remove the whole ${ part, only the $ */
#define remove_bracket(name) name + 1

#define remove_space(value) value + 1

static char *concat(char *buffer, char *string)
{
    if (!buffer)
    {
        return strdup(string);
    }
    if (string)
    {
        size_t length = strlen(buffer) + strlen(string) + 1;
        char *new = realloc(buffer, length);

        return strcat(new, string);
    }

    return buffer;
}

static bool is_nested(char *value)
{
    return strstr(value, "${") && strstr(value, "}");
}

/**
 * @example With TEST_DIR=${BASE_DIR}/.test the first strtok_r call will return
 * BASE_DIR}/.test instead of NULL, or an empty string
 */
static char *prepare_value(char *value)
{
    char *new = malloc(strlen(value) + 2);
    sprintf(new, " %s", value);

    return new;
}

static char *parse_value(char *value)
{
    value = prepare_value(value);

    char *search = value, *parsed = NULL, *tok_ptr;
    char *name;

    if (value && is_nested(value))
    {
        while (1)
        {
            parsed = concat(parsed, strtok_r(search, "${", &tok_ptr));
            name = strtok_r(NULL, "}", &tok_ptr);

            if (!name)
            {
                break;
            }
            parsed = concat(parsed, getenv(remove_bracket(name)));
            search = NULL;
        }
        free(value);

        return parsed;
    }
    return value;
}

static bool is_commented(char *line)
{
    if ('#' == line[0])
    {
        return true;
    }

    int i = 0;
    while (' ' == line[i])
    {
        if ('#' == line[++i])
        {
            return true;
        }
    }

    return false;
}

static void replace_cr(char *line)
{
    int i = 0;
    while (line[i])
    {
        if ('\r' == line[i])
        {
            line[i] = '\n';
        }
        i++;
    }
}

static void set_variable(char *name, char *original, bool overwrite)
{
    char *parsed;

    if (original)
    {
        parsed = parse_value(original);
        setenv(name, remove_space(parsed), overwrite);

        free(parsed);
    }
}

static void parse(FILE *file, bool overwrite)
{
    char *name, *original, *line = NULL, *tok_ptr;
    size_t len = 0;

    while (-1 != getline(&line, &len, file))
    {
        if (!is_commented(line))
        {
            replace_cr(line);
            name = strtok_r(line, "=", &tok_ptr);
            original = strtok_r(NULL, "\n", &tok_ptr);

            set_variable(name, original, overwrite);
        }
    }
    free(line);
}

static FILE *open_default(const char *base_path)
{
    FILE *fp = NULL;
    char path[strlen(base_path) + strlen(".env") + 1];
    sprintf(path, "%s/.env", base_path);
    fp = fopen(path, "rb");
    if (fp)
        printf("env: %s\n", path);
    return fp;
}

static FILE *open_cwd()
{
    FILE *fp = NULL;
    char path[PATH_MAX];
    if (getcwd(path, sizeof(path)) != NULL)
    {
        strncat(path, "/.env", strlen(path));
        fp = fopen(path, "rb");
    }
    if (fp)
        printf("env: %s\n", path);
    return fp;
}

static FILE *open_path(const char *path)
{
    FILE *fp = NULL;
    fp = fopen(path, "rb");
    if (fp)
        printf("env: %s\n", path);
    return fp;
}

int env_load(const char *path, bool overwrite)
{
    if (!path || strcmp(path, ".") == 0)
        path = ".env";

    FILE *fp = open_path(path);

    if (!fp)
        fp = open_default(path);

    if (!fp)
        fp = open_cwd();

    if (!fp)
        return -1;

    parse(fp, overwrite);
    fclose(fp);

    return 0;
}
