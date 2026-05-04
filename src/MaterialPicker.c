#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>

#include "MaterialPicker.h"

typedef enum {
    MAT_COLOR = 0,
    MAT_NORMAL = 1,
    MAT_ROUGHNESS = 2,
} MaterialTextureType;

const char* material_extensions[] = {
    "Color",
    "NormalGL",
    "Roughness",
    NULL
};

void materials_populate_from_disk(Vec* materials, const char* directory) {
    // In the future it would be more beautiful to have XXXX.color.png and XXXX.norml.png and XXXX.rough.png
    // but we're working with ambientCG conventions 4 now


    DIR* texture_dir = opendir(directory);
    assert(texture_dir);

    struct dirent* dir_item;
    struct stat stat_result;
    char path[256];

    while ((dir_item = readdir(texture_dir))) {
        if (materials->length > 64) break;

        if (strcmp(dir_item->d_name, ".") == 0 || strcmp(dir_item->d_name, "..") == 0)
            continue;

        assert(snprintf(path, 255, "%s/%s", directory, dir_item->d_name) >= 0);
        assert(stat(path, &stat_result) == 0);

        if (S_ISDIR(stat_result.st_mode)) {
            materials_populate_from_disk(materials, path);
            continue;
        }

        assert(S_ISREG(stat_result.st_mode));

        // Get basename
        int start = strlen(path);
        while (start > 1) {
            if (path[(--start) - 1] == '/') break;
        }
        char* name = malloc(strlen(path) - start + 1);
        strcpy(name, path + start);

        // Remove extension from name and find index of last underscore
        int last_underscore = 0;
        for (int i=0; i<strlen(name); i++) {
            if (name[i] == '_') last_underscore = i;

            if (name[i] == '.') {
                name[i] = '\0';
                break;
            }
        }

        // Find the string after last underscore and see if it matches a material extension
        char* slug = malloc(strlen(name) - last_underscore + 1);
        strcpy(slug, name + last_underscore + 1);

        int mat_extension = MAT_COLOR;

        for (int i=0; material_extensions[i]; i++) {
            if (strcmp(material_extensions[i], slug) == 0) {
                mat_extension = i;
                name[last_underscore] = '\0';
                break;
            }
        }

        Matthewterial* mat = NULL;

        for (int i=0; i<materials->length; i++) {
            Matthewterial* maybe_mat = materials->data[i];
            if (strcmp(maybe_mat->name, name) == 0) {
                mat = maybe_mat;
                break;
            }
        }

        if (!mat) {
            mat = malloc(sizeof *mat);
            mat->path = malloc(strlen(directory) + 1);
            strcpy(mat->path, directory);
            mat->name = name;
            v_add(materials, mat);
        }

        if (mat_extension == MAT_COLOR) {
            mat->color = (LazyTexture) { 0 };
            mat->color.path = malloc(strlen(path) + 1);
            strcpy(mat->color.path, path);
        }
    }

    closedir(texture_dir);
}

void* materials_lazy_load_thread(void* arg) {
    Vec* materials = arg;

    while (true) {
        for (int i=0; i<materials->length; i++) {
            Matthewterial* mat = materials->data[i];
            if (mat->color.load_phase == LOAD_PHASE_UNLOADED) {
                mat->color.img = LoadImage(mat->color.path);
                mat->color.load_phase = LOAD_PHASE_IMAGE;
            }
        }
        sleep(1);
    }
}

void materials_lazy_load_online(Vec* materials) {
    for (int i=0; i<materials->length; i++) {
        Matthewterial* mat = materials->data[i];
        if (mat->color.load_phase == LOAD_PHASE_IMAGE) {
            mat->color.texture = LoadTextureFromImage(mat->color.img);
            UnloadImage(mat->color.img);
            mat->color.load_phase = LOAD_PHASE_LOADED;
        }
    }
}
