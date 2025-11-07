
#include "lisilisk_internals.h"

#include <stdio.h>
#include <ustd/parsing.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void lisilisk_parse_mtl_material_create(struct lisilisk_parse_mtl_material *material);
static void lisilisk_parse_mtl_material_destroy(struct lisilisk_parse_mtl_material *material);
static void lisilisk_parse_mtl_material_dump(struct lisilisk_parse_mtl_material *material,
        FILE *file);

// -----------------------------------------------------------------------------

static i32 parse_material(struct parser_state *state, struct lisilisk_parse_mtl *mtl);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param mtl
 */
void lisilisk_parse_mtl_create(struct lisilisk_parse_mtl *mtl)
{
    *mtl = (struct lisilisk_parse_mtl) {
            .materials = array_create(make_system_allocator(),
                    sizeof(*mtl->materials), 32),
    };
}

/**
 * @brief
 *
 * @param mtl
 */
void lisilisk_parse_mtl_destroy(struct lisilisk_parse_mtl *mtl)
{
    for (size_t i = 0 ; i < array_length(mtl->materials) ; i++) {
        lisilisk_parse_mtl_material_destroy(mtl->materials + i);
    }

    array_destroy(make_system_allocator(), (ARRAY_ANY *) &mtl->materials);

    *mtl = (struct lisilisk_parse_mtl) { 0 };
}

/**
 * @brief
 *
 * @param mtl
 */
void lisilisk_parse_mtl_parse(struct lisilisk_parse_mtl *mtl,
        const ARRAY(byte) buffer)
{
    for (size_t i = 0 ; i < array_length(mtl->materials) ; i++) {
        lisilisk_parse_mtl_material_destroy(mtl->materials + i);
    }
    array_clear(mtl->materials);

    struct parser_state state = { .buffer_array = buffer, 0 };

    while (!parser_parse_end_of_file(&state)) {
        parser_skip_whitespace(&state);

        if (parser_parse_end_line(&state)) {
            // NOP
        } else if (parse_material(&state, mtl)) {
            // NOP
        } else {
            fprintf(stderr, "at line %d:%d ; parsing error. The "
                    "material library may not contain all materials.\n", state.line+1,
                    state.column+1);
            break;
        }
    }
    lisilisk_parse_mtl_dump(mtl, stdout);
}

/**
 * @brief
 *
 * @param mtl
 * @param file
 */
void lisilisk_parse_mtl_dump(struct lisilisk_parse_mtl *mtl, FILE *file)
{
    if (!mtl || !file) {
        return;
    }

    for (size_t i = 0 ; i < array_length(mtl->materials) ; i++) {
        lisilisk_parse_mtl_material_dump(mtl->materials + i, file);
    }
    printf("\n");
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param material
 */
static void lisilisk_parse_mtl_material_create(struct lisilisk_parse_mtl_material *material)
{
    *material = (struct lisilisk_parse_mtl_material) {
            .name = array_create(make_system_allocator(), sizeof(*material->name), 32),

            .Ka = { 0 },
            .Ks = { 0 },
            .Kd = { 0 },
            .Ke = { 0 },

            .Ns = 0.,

            .map_Ka = array_create(make_system_allocator(), sizeof(*material->map_Ka), 32),
            .map_Ks = array_create(make_system_allocator(), sizeof(*material->map_Ks), 32),
            .map_Kd = array_create(make_system_allocator(), sizeof(*material->map_Kd), 32),
            .map_Ke = array_create(make_system_allocator(), sizeof(*material->map_Ke), 32),
    };
}

/**
 * @brief
 *
 * @param material
 */
static void lisilisk_parse_mtl_material_destroy(struct lisilisk_parse_mtl_material *material)
{
    array_destroy(make_system_allocator(), (ARRAY_ANY *) &material->name);
    array_destroy(make_system_allocator(), (ARRAY_ANY *) &material->map_Ka);
    array_destroy(make_system_allocator(), (ARRAY_ANY *) &material->map_Ks);
    array_destroy(make_system_allocator(), (ARRAY_ANY *) &material->map_Kd);
    array_destroy(make_system_allocator(), (ARRAY_ANY *) &material->map_Ke);

    *material = (struct lisilisk_parse_mtl_material) { 0 };
}

/**
 * @brief
 *
 * @param material
 * @param file
 */
static void lisilisk_parse_mtl_material_dump(struct lisilisk_parse_mtl_material *material, FILE *file)
{
    if (array_length(material->name)) {
        fprintf(file, "newmtl %s\n", material->name);
    }

    fprintf(file, "Ka %f %f %f\n", material->Ka[0], material->Ka[1], material->Ka[2]);
    fprintf(file, "Ks %f %f %f\n", material->Ka[0], material->Ka[1], material->Ks[2]);
    fprintf(file, "Kd %f %f %f\n", material->Ka[0], material->Ka[1], material->Kd[2]);
    fprintf(file, "Ke %f %f %f\n", material->Ka[0], material->Ka[1], material->Ke[2]);
    fprintf(file, "Ns %f\n", material->Ns);


    if (array_length(material->map_Ka)) {
        fprintf(file, "map_Ka %s\n", material->map_Ka);
    }
    if (array_length(material->map_Ks)) {
        fprintf(file, "map_Ks %s\n", material->map_Ks);
    }
    if (array_length(material->map_Ks)) {
        fprintf(file, "map_Ks %s\n", material->map_Ks);
    }
    if (array_length(material->map_Ke)) {
        fprintf(file, "map_Ke %s\n", material->map_Ke);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param state
 * @param mtl
 * @return i32
 */
static i32 parse_material(struct parser_state *state, struct lisilisk_parse_mtl *mtl)
{
    struct lisilisk_parse_mtl_material *new_material = nullptr;

    parser_skip_whitespace(state);

    if (!parser_accept(state, (char []) { 'n' }, 1, NULL)) {
        return 0;
    }

    parser_expect(state, (char []) { 'e' }, 1, NULL);
    parser_expect(state, (char []) { 'w' }, 1, NULL);
    parser_expect(state, (char []) { 'm' }, 1, NULL);
    parser_expect(state, (char []) { 't' }, 1, NULL);
    parser_expect(state, (char []) { 'l' }, 1, NULL);

    parser_skip_whitespace(state);

    // make some room for the material
    array_ensure_capacity(make_system_allocator(), (ARRAY_ANY *) &mtl->materials, 1);
    if (array_push(mtl->materials, &(struct lisilisk_parse_mtl_material) { 0 })) {
        new_material = mtl->materials + array_length(mtl->materials) - 1;
    } else {
        return 0;
    }

    // read the material name
    while (!parser_lookup(state, (char []) { '\n' }, 1, NULL)) {
        array_ensure_capacity(make_system_allocator(), (ARRAY_ANY *) &new_material->name, 1);
        array_push(new_material->name, state->buffer_array + state->buffer_idx);
        parser_state_advance(state);
    }
    array_ensure_capacity(make_system_allocator(), (ARRAY_ANY *) &new_material->name, 1);
    array_push(new_material->name, &(char) { '\0' });

    // read the material fields

    while (!parser_parse_end_of_file(state)) {
        parser_skip_whitespace(state);

        if (0) {

        } else {
            break;
        }
    }

    return 1;
}