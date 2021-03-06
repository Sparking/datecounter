#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"

#define INI_LINE_MAXSIZE    2048

struct ini_tag {
    char *key;
    char *value;
    struct list_head tag_node;
};

struct ini_section {
    char *section;
    struct list_head section_node;
    struct list_head tag_node;
};

enum {
    INI_CONFIG_SECTION = 0,
    INI_CONFIG_KEY_VALUE,
    INI_CONFIG_EMPTY,
};

union ini_parse_block {
    char *section;
    struct {
        char *key;
        char *value;
    } kv;
};

static int ini_parse_line(char *const line, union ini_parse_block *ipb)
{
    size_t i, j;
    int line_type;

    for (i = 0; line[i] != '\0'; ++i)
        if (!isspace(line[i]))
            break;
    switch (line[i]) {
    case '\0':
    case ';':
        line_type = INI_CONFIG_EMPTY;
        break;
    case '[':
        line_type = INI_CONFIG_SECTION;
        for (j = i + 1; line[j] != '\0' && line[j] != '\r' && line[j] != '\n';
            ++j) {
            if (line[j] == ']')
                break;
        }
        if (line[j] == ']') {
            line[j] = '\0';
            ipb->section = line + i + 1;
        } else {
            line_type = INI_CONFIG_EMPTY;
        }
        break;
    default:
        line_type = INI_CONFIG_KEY_VALUE;
        for (j = i + 1; line[j] != '\0' && line[j] != '\r' && line[j] != '\n';
            ++j) {
            if (line[j] == '=')
                break;
        }
        if (line[j] == '=') {
            ipb->kv.key = line + i;
            ipb->kv.value = line + j + 1;
            while (j-- > i) {
                if (!isspace(line[j]))
                    break;
            }
            line[++j] = '\0';
            for (i = 0; ipb->kv.value[i] != '\0'
                    && ipb->kv.value[i] != '\r' && ipb->kv.value[i] != '\n';
                ++i) {
                if (!isspace(ipb->kv.value[i])) {
                    ipb->kv.value = ipb->kv.value + i;
                    break;
                }
            }
            for (i = 0; ipb->kv.value[i] != '\0'
                    && ipb->kv.value[i] != '\r' && ipb->kv.value[i] != '\n';
                ++i) {
                continue;
            }
            while (i-- > 0)
                if (!isspace(ipb->kv.value[i]))
                    break;
            ipb->kv.value[++i] = '\0';
        } else {
            line_type = INI_CONFIG_EMPTY;
        }
        break;
    }

    return line_type;
}

static struct ini_section *ini_config_find_section(INI_CONFIG *config,
    const char *name)
{
    struct ini_section *section;

    list_for_each_entry(section, &config->section_node, section_node) {
        if ((name == NULL && section->section == NULL)
            || (name != NULL && section->section != NULL
                && strcmp(name, section->section) == 0))
            return section;
    }

    return NULL;
}

static struct ini_section *ini_config_add_section(INI_CONFIG *config,
    const char *name)
{
    struct ini_section *section;

    if (config == NULL || (name != NULL && *name == '\0'))
        return NULL;

    section = ini_config_find_section(config, name);
    if (section != NULL)
        return section;

    section = (struct ini_section *)malloc(sizeof(struct ini_section));
    if (section == NULL)
        return NULL;

    INIT_LIST_HEAD(section->tag_node);
    if (name != NULL) {
        section->section = strdup(name);
        if (section->section == NULL) {
            free(section);
            return NULL;
        }
    } else {
        section->section = NULL;
    }
    if (name == NULL) {
        /* 无名节是配置中的第一节 */
        list_add(&section->section_node, &config->section_node);
    } else {
        list_add_tail(&section->section_node, &config->section_node);
    }

    return section;
}

static struct ini_tag *ini_config_find_tag(struct ini_section *section,
    const char *key)
{
    struct ini_tag *tag;

    list_for_each_entry(tag, &section->tag_node, tag_node) {
        if (strcmp(tag->key, key) == 0)
            return tag;
    }

    return NULL;
}

static struct ini_tag *ini_config_create_new_tag(const char *key,
    const char *value)
{
    struct ini_tag *tag;

    tag = (struct ini_tag *)malloc(sizeof(struct ini_tag));
    if (tag == NULL)
        return NULL;

    tag->key = strdup(key);
    if (tag->key == NULL) {
        free(tag);
        return NULL;
    }

    if (value) {
        tag->value = strdup(value);
        if (tag->value == NULL) {
            free(tag->key);
            free(tag);
            return NULL;
        }
    } else {
        tag->value = NULL;
    }

    return tag;
}

static struct ini_tag *ini_config_add_tag(struct ini_section *section,
    const char *key, const char *value)
{
    struct ini_tag *tag;

    if (section == NULL || key == NULL || *key == '\0')
        return NULL;

    tag = ini_config_find_tag(section, key);
    if (tag == NULL) {
        tag = ini_config_create_new_tag(key, value);
        if (tag)
            list_add_tail(&tag->tag_node, &section->tag_node);
    } else {
        if ((tag->value == NULL && value == NULL)
            || (tag->value != NULL && value != NULL
                && strcmp(tag->value, value) == 0))
            return tag;
        if (tag->value) {
            free(tag->value);
            tag->value = NULL;
        }
        if (value) {
            tag->value = strdup(value);
            if (tag->value == NULL) {
                list_del(&tag->tag_node);
                free(tag);
                return NULL;
            }
        }
    }

    return tag;
}

INI_CONFIG *ini_config_create(const char *const file)
{
    FILE *fp;
    INI_CONFIG *config;
    char line[INI_LINE_MAXSIZE];
    struct ini_section *current_section;

    if (file == NULL)
        return NULL;

    config = (INI_CONFIG *)malloc(sizeof(*config));
    if (config == NULL)
        return NULL;

    INIT_LIST_HEAD(config->section_node);
    config->config_file = strdup(file);
    /*
    if (config->config_file == NULL) {
        ini_config_release(config);
        return NULL;
    }
    */

    fp = fopen(file, "r");
    if (fp == NULL) {
        ini_config_release(config);
        return NULL;
    }

    current_section = ini_config_add_section(config, NULL);
    /*
    if (current_section == NULL) {
        ini_config_release(config);
        return NULL;
    }
    */
    while (fgets(line, sizeof(line) - 1, fp) != NULL) {
        union ini_parse_block ipb;
        switch (ini_parse_line(line, &ipb)) {
        case INI_CONFIG_SECTION:
            current_section = ini_config_add_section(config, ipb.section);
            /*
            if (current_section == NULL) {
                ini_config_release(config);
                return NULL;
            }
            */
            break;
        case INI_CONFIG_KEY_VALUE:
            ini_config_add_tag(current_section, ipb.kv.key, ipb.kv.value);
            /*
            struct ini_tag *current_tag;
            current_tag = ini_config_add_tag(current_section, ipb.kv.key,
                ipb.kv.value);
            if (cuurent_tag == NULL) {
                ini_config_release(config);
                return NULL;
            }
            */
            break;
        default:
            break;
        }
    }
    fclose(fp);

    return config;
}

int ini_config_set(INI_CONFIG *config, const char *section_name,
    const char *key, const char *value)
{
    struct ini_section *mark;
    struct ini_section *section;
    struct ini_tag *tag;

    mark = ini_config_find_section(config, section_name);
    section = ini_config_add_section(config, section_name);
    if (section == NULL)
        return -1;

    tag = ini_config_add_tag(section, key, value);
    if (tag == NULL) {
        if (mark == NULL) {
            list_del(&section->section_node);
            free(section);
        }
        return -1;
    }
    
    return 0;
}

const char *ini_config_get(INI_CONFIG *config, const char *section_name,
    const char *key, const char *default_value)
{
    struct ini_section *section;
    struct ini_tag *tag;

    if (key == NULL)
        return NULL;

    section = ini_config_find_section(config, section_name);
    if (section == NULL)
        return default_value;
    
    tag = ini_config_find_tag(section, key);
    if (tag == NULL || tag->value == NULL)
        return default_value;
    
    return tag->value;
}

int ini_config_save2filestream(INI_CONFIG *config, FILE *fp)
{
    struct list_head *section_node;
    struct list_head *tag_node;
    struct ini_section *section;
    struct ini_tag *tag;

    if (config == NULL || fp == NULL)
        return -1;

    for (section_node = config->section_node.next;
        section_node != &config->section_node;
        section_node = section_node->next) {
        section = list_entry(section_node, struct ini_section, section_node);
        if (section->section)
            fprintf(fp, "[%s]\n", section->section);
        for (tag_node = section->tag_node.next;
            tag_node != &section->tag_node;
            tag_node = tag_node->next) {
            tag = list_entry(tag_node, struct ini_tag, tag_node);
            if (tag->value) {
                fprintf(fp, "%s = %s\n", tag->key, tag->value);
            } else {
                fprintf(fp, "%s =\n", tag->key);
            }
        }
    }
    fflush(fp);

    return 0;
}

int ini_config_saveas(INI_CONFIG *config, const char *file)
{
    int ret;
    FILE *fp;

    fp = fopen(file, "w");
    if (fp == NULL)
        return -1;
    
    ret = ini_config_save2filestream(config, fp);
    fclose(fp);

    return ret;
}

void ini_config_release(INI_CONFIG *config)
{
    if (config != NULL) {
        struct list_head *section_node;
        struct list_head *tag_node;
        struct list_head *temp_ptr;
        struct ini_section *section;
        struct ini_tag *tag;

        if (config->config_file)
            free(config->config_file);

        for (section_node = config->section_node.next;
            section_node != &config->section_node;) {
            section = list_entry(section_node, struct ini_section,
                section_node);
            for (tag_node = section->tag_node.next;
                tag_node != &section->tag_node;) {
                tag = list_entry(tag_node, struct ini_tag, tag_node);
                free(tag->key);
                if (tag->value)
                    free(tag->value);
                temp_ptr = tag_node->next;
                list_del(tag_node);
                free(tag);
                tag_node = temp_ptr;
            }
            if (section->section)
                free(section->section);
            temp_ptr = section_node->next;
            list_del(section_node);
            free(section);
            section_node = temp_ptr;
        }
        free(config);
    }
}
