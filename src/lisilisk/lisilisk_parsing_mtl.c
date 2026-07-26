
#include "lisilisk_internals.h"

#include <stdio.h>
#include <ustd/parsing.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void lisilisk_parse_mtl_material_create(struct lisilisk_parse_mtl_material *mtl);
static void lisilisk_parse_mtl_material_destroy(struct lisilisk_parse_mtl_material *mtl);
static void lisilisk_parse_mtl_material_dump(struct lisilisk_parse_mtl_material *mtl,
        FILE *file);
static void lisilisk_parse_mtl_material_to(struct lisilisk_parse_mtl_material *parsed_material,
        struct material *material);
static void lisilisk_parse_mtl_textures_to(struct lisilisk_parse_mtl_material *parsed_material,
        struct material *material, PATH local_path,
        struct lisilisk_store_texture *texture_store, struct resource_manager *res_manager);

// -----------------------------------------------------------------------------

static i32 parse_material(struct parser_state *state, struct lisilisk_parse_mtl *mtl);

// -----------------------------------------------------------------------------

static i32 parse_comment(struct parser_state *state);

static i32 parse_optical_property(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl);
static i32 parse_optical_property_specular_strength(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl);
static i32 parse_optical_property_optical_density(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl);

static i32 parse_reflection_factor(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl);
static i32 parse_reflection_factor_ambient(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl);
static i32 parse_reflection_factor_diffuse(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl);
static i32 parse_reflection_factor_specular(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl);
static i32 parse_reflection_factor_emissive(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl);

static i32 parse_illumination_mode(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl);
static i32 parse_dissolve_factor(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl);


static i32 parse_texture(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl);
static i32 parse_texture_to_sampler(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl, enum material_base_sampler sampler);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static const char *MATERIAL_BASE_SAMPLERS_NAMES[MATERIAL_BASE_SAMPLERS_NUMBER] = {
        [MATERIAL_BASE_SAMPLER_AMBIENT_MASK]   = "map_Ka",
        [MATERIAL_BASE_SAMPLER_SPECULAR_MASK]  = "map_Ks",
        [MATERIAL_BASE_SAMPLER_DIFFUSE_MASK]   = "map_Kd",
        [MATERIAL_BASE_SAMPLER_EMISSIVE_MASK]  = "map_Ke",
        [MATERIAL_BASE_SAMPLER_TEXTURE]        = "# custom_texture",
        [MATERIAL_BASE_SAMPLER_SHININESS_MASK] = "map_Ns",
};

static void(*MATERIAL_SETTERS[MATERIAL_BASE_SAMPLERS_NUMBER])(struct material *, struct texture *) = {
        [MATERIAL_BASE_SAMPLER_AMBIENT_MASK]   = &material_ambient_mask,
        [MATERIAL_BASE_SAMPLER_SPECULAR_MASK]  = &material_diffuse_mask,
        [MATERIAL_BASE_SAMPLER_DIFFUSE_MASK]   = &material_specular_mask,
        [MATERIAL_BASE_SAMPLER_EMISSIVE_MASK]  = &material_emissive_mask,
        [MATERIAL_BASE_SAMPLER_TEXTURE]        = nullptr,
        [MATERIAL_BASE_SAMPLER_SHININESS_MASK] = &material_shininess_mask,
};

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
        } else if (parse_comment(&state)) {
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
}

/**
 * @brief
 *
 * @param mtl
 * @param local_path
 * @param material_store
 */
void lisilisk_parse_mtl_to(const struct lisilisk_parse_mtl *mtl,
        PATH local_path,
        struct material *default_material,
        HASHMAP(struct material *) *materials,
        struct lisilisk_store_texture *texture_store,
        struct resource_manager *res_manager)
{
    struct allocator alloc = make_system_allocator();

    struct material *new_material = nullptr;

    if (!mtl || !materials || !*materials) {
        return;
    }

    hashmap_ensure_capacity(make_system_allocator(), (HASHMAP_ANY *) materials, array_length(mtl->materials));

    for (size_t i = 0 ; i < array_length(mtl->materials) ; i++) {
        new_material = alloc.malloc(alloc, sizeof(*new_material));
        material_create(new_material, default_material);
        lisilisk_parse_mtl_material_to(mtl->materials + i, new_material);
        lisilisk_parse_mtl_textures_to(mtl->materials + i, new_material, local_path,
                texture_store, res_manager);
        hashmap_set(*materials, mtl->materials[i].name, &new_material);
    }
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
    };
    for (size_t i = 0 ; i < MATERIAL_BASE_SAMPLERS_NUMBER ; i++) {
        material->maps[i] = array_create(make_system_allocator(), sizeof(*material->maps[i]), 32);
    }
}

/**
 * @brief
 *
 * @param material
 */
static void lisilisk_parse_mtl_material_destroy(struct lisilisk_parse_mtl_material *material)
{
    for (size_t i = 0 ; i < MATERIAL_BASE_SAMPLERS_NUMBER ; i++) {
        array_destroy(make_system_allocator(), (ARRAY_ANY *) material->maps + i);
    }
    array_destroy(make_system_allocator(), (ARRAY_ANY *) &material->name);

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
    fprintf(file, "Ks %f %f %f\n", material->Ks[0], material->Ks[1], material->Ks[2]);
    fprintf(file, "Kd %f %f %f\n", material->Kd[0], material->Kd[1], material->Kd[2]);
    fprintf(file, "Ke %f %f %f\n", material->Ke[0], material->Ke[1], material->Ke[2]);
    fprintf(file, "Ns %f\n", material->Ns);

    for (size_t i = 0 ; i < MATERIAL_BASE_SAMPLERS_NUMBER ; i++) {
        if (array_length(material->maps[i])) {
            fprintf(file, "%s %s\n", MATERIAL_BASE_SAMPLERS_NAMES[i], material->maps[i]);
        }
    }
}

/**
 * @brief
 *
 * @param mtl
 * @param material
 */
static void lisilisk_parse_mtl_material_to(struct lisilisk_parse_mtl_material *parsed_material,
        struct material *material)
{
    material->properties = (struct material_properties) {
            .ambient  = { parsed_material->Ka[0], parsed_material->Ka[1], parsed_material->Ka[2] },
            .diffuse  = { parsed_material->Kd[0], parsed_material->Kd[1], parsed_material->Kd[2] },
            .specular = { parsed_material->Ks[0], parsed_material->Ks[1], parsed_material->Ks[2] },
            .emissive = { parsed_material->Ke[0], parsed_material->Ke[1], parsed_material->Ke[2] },

            .ambient_strength = 1,
            .diffuse_strength = 1,
            .specular_strength = 1,
            .emissive_strength = 0,

            .shininess = parsed_material->Ns,
    };
}

/** */
static void lisilisk_parse_mtl_textures_to(struct lisilisk_parse_mtl_material *parsed_material,
        struct material *material, PATH local_path,
        struct lisilisk_store_texture *texture_store, struct resource_manager *res_manager)
{
    struct allocator alloc = make_system_allocator();

    u32 texture_hash = 0;
    PATH work_path = nullptr;

    if (!parsed_material || ! material || !texture_store || !res_manager || !local_path) {
        return;
    }

    work_path = path_from_cstring(alloc, local_path, '/', 2048);

    for (size_t i = 0 ; i < MATERIAL_BASE_SAMPLERS_NUMBER ; i++) {
        if (!MATERIAL_SETTERS[i] || array_length(parsed_material->maps[i]) == 0) {
            continue;
        }

        path_ensure_capacity(alloc, &work_path, array_length(parsed_material->maps[i]));
        path_append(work_path, parsed_material->maps[i]);

        lisilisk_store_texture_register(texture_store, res_manager,
                work_path, &texture_hash);
        MATERIAL_SETTERS[i](material, lisilisk_store_texture_retrieve(texture_store, texture_hash));
        path_up(work_path);

    }

    path_destroy(alloc, &work_path);
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
static i32 parse_material(struct parser_state *state, struct lisilisk_parse_mtl *lib_mtl)
{
    struct lisilisk_parse_mtl_material *new_material = nullptr;

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
    array_ensure_capacity(make_system_allocator(), (ARRAY_ANY *) &lib_mtl->materials, 1);
    if (array_push(lib_mtl->materials, &(struct lisilisk_parse_mtl_material) { 0 })) {
        new_material = lib_mtl->materials + array_length(lib_mtl->materials) - 1;
        lisilisk_parse_mtl_material_create(new_material);
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

        if (parser_parse_end_line(state)) {
            // NOP
        } else if (parse_comment(state)) {
            // NOP
        } else if (parse_optical_property(state, new_material)) {
            // NOP
        } else if (parse_reflection_factor(state, new_material)) {
            // NOP
        } else if (parse_illumination_mode(state, new_material)) {
            // NOP
        } else if (parse_dissolve_factor(state, new_material)) {
            // NOP
        } else if (parse_texture(state, new_material)) {
            // NOP
        } else {
            break;
        }
    }

    return 1;
}

static i32 parse_comment(struct parser_state *state)
{
    // detect comment character
    if (!parser_accept(state, (char []) { '#' }, 1, NULL)) {
        return 0;
    }

    while (!parser_lookup(state, (char []) { '\n' }, 1, NULL)) {
        parser_state_advance(state);
    }

    return 1;
}

static i32 parse_optical_property(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl)
{
    char read_char = '\0';

    if (!parser_accept(state, (char []) { 'N' }, 1, NULL)) {
        return 0;
    }

    if (!parser_expect(state, (char []) { 's', 'i' }, 2, &read_char)) {
        return 0;
    }

    parser_skip_whitespace(state);

    switch (read_char) {
        case ('s'):
            return parse_optical_property_specular_strength(state, mtl);
        case ('i'):
            return parse_optical_property_optical_density(state, mtl);
        default:
            break;
    }

    return 0;
}

static i32 parse_optical_property_specular_strength(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl)
{
    return parser_parse_value_float(state, &mtl->Ns);
}

static i32 parse_optical_property_optical_density(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl)
{
    (void) mtl;

    // not supported

    while (!parser_lookup(state, (char []) { '\n' }, 1, NULL)) {
        parser_state_advance(state);
    }

    return 1;
}

static i32 parse_reflection_factor(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl)
{
    char read_char = '\0';

    if (!parser_accept(state, (char []) { 'K' }, 1, NULL)) {
        return 0;
    }

    if (!parser_expect(state, (char []) { 'a', 'd', 's', 'e'}, 4, &read_char)) {
        return 0;
    }

    parser_skip_whitespace(state);

    switch (read_char) {
        case ('a'):
            return parse_reflection_factor_ambient(state, mtl);
        case ('d'):
            return parse_reflection_factor_diffuse(state, mtl);
        case ('s'):
            return parse_reflection_factor_specular(state, mtl);
        case ('e'):
            return parse_reflection_factor_emissive(state, mtl);
        default:
            break;
    }

    return 0;
}

static i32 parse_reflection_factor_ambient(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl)
{
    return parser_parse_value_float(state, mtl->Ka)
            && parser_parse_value_float(state, mtl->Ka + 1)
            && parser_parse_value_float(state, mtl->Ka + 2);
}

static i32 parse_reflection_factor_diffuse(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl)
{
    return parser_parse_value_float(state, mtl->Kd)
            && parser_parse_value_float(state, mtl->Kd + 1)
            && parser_parse_value_float(state, mtl->Kd + 2);
}

static i32 parse_reflection_factor_specular(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl)
{
    return parser_parse_value_float(state, mtl->Ks)
            && parser_parse_value_float(state, mtl->Ks + 1)
            && parser_parse_value_float(state, mtl->Ks + 2);
}

static i32 parse_reflection_factor_emissive(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl)
{
    return parser_parse_value_float(state, mtl->Ke)
            && parser_parse_value_float(state, mtl->Ke + 1)
            && parser_parse_value_float(state, mtl->Ke + 2);
}

static i32 parse_illumination_mode(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl)
{
    (void) mtl;

    if (!parser_accept(state, (char []) { 'i' }, 1, NULL)) {
        return 0;
    }
    if (!(parser_expect(state, (char []) { 'l' }, 1, NULL)
            && parser_expect(state, (char []) { 'l' }, 1, NULL)
            && parser_expect(state, (char []) { 'u' }, 1, NULL)
            && parser_expect(state, (char []) { 'm' }, 1, NULL))) {
        return 0;
    }

    // not supported

    while (!parser_lookup(state, (char []) { '\n' }, 1, NULL)) {
        parser_state_advance(state);
    }

    return 1;
}

static i32 parse_dissolve_factor(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl)
{
    (void) mtl;

    if (!parser_accept(state, (char []) { 'd' }, 1, NULL)) {
        return 0;
    }

    // not supported

    while (!parser_lookup(state, (char []) { '\n' }, 1, NULL)) {
        parser_state_advance(state);
    }

    return 1;
}

static i32 parse_texture(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl)
{
    char read_char = '\0';

    if (!parser_accept(state, (char []) { 'm' }, 1, NULL)) {
        return 0;
    }

    if (!(parser_expect(state, (char []) { 'a' }, 1, NULL)
            && parser_expect(state, (char []) { 'p' }, 1, NULL)
            && parser_expect(state, (char []) { '_' }, 1, NULL)
            && parser_expect(state, (char []) { 'K' }, 1, NULL))) {
        return 0;
    }

    if (!parser_expect(state, (char []) { 'a', 'd', 's', 'e' }, 4, &read_char)) {
        return 0;
    }

    parser_skip_whitespace(state);

    switch (read_char) {
        case ('a'):
            return parse_texture_to_sampler(state, mtl, MATERIAL_BASE_SAMPLER_AMBIENT_MASK);
        case ('d'):
            return parse_texture_to_sampler(state, mtl, MATERIAL_BASE_SAMPLER_DIFFUSE_MASK);
        case ('s'):
            return parse_texture_to_sampler(state, mtl, MATERIAL_BASE_SAMPLER_SPECULAR_MASK);
        case ('e'):
            return parse_texture_to_sampler(state, mtl, MATERIAL_BASE_SAMPLER_EMISSIVE_MASK);
        default:
            break;
    }

    return 1;
}

static i32 parse_texture_to_sampler(struct parser_state *state,
        struct lisilisk_parse_mtl_material *mtl, enum material_base_sampler sampler)
{
    while (!parser_lookup(state, (char []) { '\n' }, 1, NULL)) {
        array_ensure_capacity(make_system_allocator(), (ARRAY_ANY *) &mtl->maps[sampler], 1);
        array_push(mtl->maps[sampler], state->buffer_array + state->buffer_idx);
        parser_state_advance(state);
    }

    array_ensure_capacity(make_system_allocator(), (ARRAY_ANY *) &mtl->maps[sampler], 1);
    array_push(mtl->maps[sampler], &(char) { '\0' });

    return 1;

}
