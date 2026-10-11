#include "../runtime/source/so_util.h"
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#define NOVA3_ANIMATOR_SAMPLER_CTOR_B_EMPTY_SCENE_EXIT_OFFSET 0x00875FB4U
#define NOVA3_ANIMATOR_SAMPLER_CTOR_A_EMPTY_SCENE_EXIT_OFFSET 0x00874E00U
#define NOVA3_SKYBOX_CTOR_B_SCENE_USE_OFFSET 0x00107EBCU
#define NOVA3_SKYBOX_CTOR_A_SCENE_USE_OFFSET 0x00107BD4U
#define NOVA3_LASER_SET_BDAE_FAILURE_CLEANUP_OFFSET 0x001079D4U
#define NOVA3_LASER_SET_BDAE_MESH_NULL_TAIL_OFFSET 0x001079B0U
#define NOVA3_LASER_CTOR_B_SCENE_FAILURE_EXIT_OFFSET 0x00107904U
#define NOVA3_LASER_CTOR_B_MESH_FAILURE_EXIT_OFFSET 0x001078E0U
#define NOVA3_LASER_CTOR_A_SCENE_FAILURE_EXIT_OFFSET 0x001070DCU
#define NOVA3_LASER_CTOR_A_MESH_FAILURE_EXIT_OFFSET 0x001070B8U
#include "patch_vita_compat.h"
extern so_module so_mod;


uintptr_t g_patch_base;
uintptr_t g_patch_head;
uintptr_t g_patch_size = 0x10000;
Jit g_jit;

#define NOVA3_FILE_STREAM_MGR_LIMIT_OFFSET 0x008E296CU
#define NOVA3_POST_EFFECTS_GATE_OFFSET 0x002F0268U
#define NOVA3_POST_EFFECTS_UPDATE_OFFSET 0x005E4A9CU
#define NOVA3_POST_EFFECTS_DISABLE_ALL_OFFSET 0x005E4AF8U
#define NOVA3_POST_EFFECTS_GET_NO_ACTIVE_OFFSET 0x005E4B34U
#define NOVA3_POST_EFFECTS_INIT_HUD_OFFSET 0x005E4BB0U
#define NOVA3_POST_EFFECTS_IS_ACTIVE_OFFSET 0x005E510CU
#define NOVA3_POST_EFFECTS_LOAD_OFFSET 0x005E5630U
#define NOVA3_POST_EFFECTS_SAVE_OFFSET 0x005E56FCU
#define NOVA3_POST_EFFECTS_BUILD_GRADING_OFFSET 0x005E603CU
#define NOVA3_POST_EFFECTS_GET_EFFECT_OFFSET 0x005E6BC4U
#define NOVA3_POST_EFFECTS_DEACTIVATE_OFFSET 0x005E6D68U
#define NOVA3_POST_EFFECTS_ACTIVATE_OFFSET 0x005E6E50U
#define NOVA3_POST_EFFECTS_POST_DRAW_OFFSET 0x005E9284U
#define NOVA3_POST_EFFECTS_PRE_DRAW_OFFSET 0x005EA0ECU
#define NOVA3_COLLADA_SCENE_CONSUMER_A_OFFSET 0x0010702CU
#define NOVA3_COLLADA_SCENE_CONSUMER_B_OFFSET 0x001083F8U
#define NOVA3_COLLADA_SCENE_CONSUMER_C_OFFSET 0x00108A40U
#define NOVA3_COLLADA_SCENE_CONSUMER_D_OFFSET 0x001221D4U

#define NOVA3_NAVSPACE_POS_TO_GRID_SAFE_OFFSET    0x005E9818U
#define NOVA3_NAVSPACE_POS_TO_GRID_OFFSET         0x005E9924U
#define NOVA3_NAVSPACE_IS_POS_NOT_OUTSIDE_OFFSET  0x005E9A60U
#define NOVA3_NAVSPACE_IS_POS_VALID_DEST_OFFSET   0x005E9B64U
#define NOVA3_NAVSPACE_GET_GRID_UINT_OFFSET       0x005E5504U
#define NOVA3_NAVSPACE_GET_GRID_3I_OFFSET         0x005E556CU
#define NOVA3_NAVSPACE_GET_GRID_VEC_OFFSET        0x005E55C4U
#define NOVA3_NAVSPACE_QUERY_GET_CLOSEST_CELL_OFFSET 0x005F1C90U
#define NOVA3_NAVSPACE_QUERY_GET_CLOSEST_POS_OFFSET  0x005E9764U
#define NOVA3_NAVSPACE_QUERY_GET_CLOSEST_NAV_POS_OFFSET 0x005F23C4U
#define NOVA3_NAVSPACE_QUERY_TEST_RAY_OFFSET      0x005FCF24U
#define NOVA3_NAVSPACE_QUERY_TEST_MOVE_OFFSET     0x005FC668U

typedef void (*nova3_post_effects_update_fn)(void *);
typedef void (*nova3_post_effects_action_fn)(void *, int);
typedef void (*nova3_post_effects_init_hud_fn)(void *, void *);
typedef int (*nova3_post_effects_is_active_fn)(void *, int);
typedef void (*nova3_post_effects_file_fn)(void *, void *);
typedef void *(*nova3_post_effects_get_effect_fn)(void *, int);
typedef void (*nova3_post_effects_draw_fn)(void *, int, int);

static nova3_post_effects_update_fn nova3_post_effects_update_original;
static nova3_post_effects_action_fn nova3_post_effects_disable_all_original;
static nova3_post_effects_update_fn nova3_post_effects_get_no_active_original;
static nova3_post_effects_init_hud_fn nova3_post_effects_init_hud_original;
static nova3_post_effects_is_active_fn nova3_post_effects_is_active_original;
static nova3_post_effects_file_fn nova3_post_effects_load_original;
static nova3_post_effects_file_fn nova3_post_effects_save_original;
static nova3_post_effects_update_fn nova3_post_effects_build_grading_original;
static nova3_post_effects_get_effect_fn nova3_post_effects_get_effect_original;
static nova3_post_effects_action_fn nova3_post_effects_deactivate_original;
static nova3_post_effects_action_fn nova3_post_effects_activate_original;
static nova3_post_effects_draw_fn nova3_post_effects_post_draw_original;
static nova3_post_effects_draw_fn nova3_post_effects_pre_draw_original;

static void nova3_post_effects_update_guard(void *self) { if (self) nova3_post_effects_update_original(self); }
static void nova3_post_effects_disable_all_guard(void *self, int param) { if (self) nova3_post_effects_disable_all_original(self, param); }
static void nova3_post_effects_get_no_active_guard(void *self) { if (self) nova3_post_effects_get_no_active_original(self); }
static void nova3_post_effects_init_hud_guard(void *self, void *hud) { if (self) nova3_post_effects_init_hud_original(self, hud); }
static int nova3_post_effects_is_active_guard(void *self, int param) { return self ? nova3_post_effects_is_active_original(self, param) : 0; }
static void nova3_post_effects_load_guard(void *self, void *file) { if (self) nova3_post_effects_load_original(self, file); }
static void nova3_post_effects_save_guard(void *self, void *file) { if (self) nova3_post_effects_save_original(self, file); }
static void nova3_post_effects_build_grading_guard(void *self) { if (self) nova3_post_effects_build_grading_original(self); }
static void *nova3_post_effects_get_effect_guard(void *self, int param) { return self ? nova3_post_effects_get_effect_original(self, param) : NULL; }
static void nova3_post_effects_deactivate_guard(void *self, int param) { if (self) nova3_post_effects_deactivate_original(self, param); }
static void nova3_post_effects_activate_guard(void *self, int param) { if (self) nova3_post_effects_activate_original(self, param); }
static void nova3_post_effects_post_draw_guard(void *self, int p1, int p2) { if (self) nova3_post_effects_post_draw_original(self, p1, p2); }
static void nova3_post_effects_pre_draw_guard(void *self, int p1, int p2) { if (self) nova3_post_effects_pre_draw_original(self, p1, p2); }

static uintptr_t install_arm_trampoline_guard(uintptr_t target, const uint32_t expected[2], uintptr_t destination, const char *failure_message) {
    static const uint32_t absolute_jump = 0xE51FF004U; /* ldr pc, [pc, #-4] */
    uintptr_t trampoline = (g_patch_head + 3U) & ~(uintptr_t)3U;
    uintptr_t patch_limit = g_patch_base + g_patch_size;

    if (memcmp((const void *)target, expected, 2U * sizeof(uint32_t)) != 0) { runtime_trace("%s: expected %08X %08X, got %08X %08X at %p", failure_message, expected[0], expected[1], ((uint32_t*)target)[0], ((uint32_t*)target)[1], target); return 0; }
    if (trampoline < g_patch_base || trampoline + 8U * sizeof(uint32_t) > patch_limit) { runtime_trace("No executable arena for an ARM guard trampoline."); return 0; }

    uint32_t trampoline_words[10];
    int t = 0;
    uint32_t literal_pool[2];
    int l = 0;
    for (int i = 0; i < 2; i++) {
        if ((expected[i] & 0x0FFF0000) == 0x059F0000) {
            uint32_t rt = (expected[i] >> 12) & 0xF;
            uint32_t imm = expected[i] & 0xFFF;
            uint32_t add = (expected[i] & 0x00800000) ? 1 : 0;
            int32_t offset = add ? imm : -imm;
            uint32_t source_addr = target + (i * 4) + 8 + offset;
            literal_pool[l] = *(uint32_t *)source_addr;
            uint32_t new_offset = 8 + (l - t) * 4;
            trampoline_words[t++] = 0xE59F0000 | (rt << 12) | new_offset;
            l++;
        } else {
            trampoline_words[t++] = expected[i];
        }
    }
    trampoline_words[t++] = absolute_jump;
    trampoline_words[t++] = target + 8U;
    for (int i = 0; i < l; i++) {
        trampoline_words[t++] = literal_pool[i];
    }
    
    size_t words_size = t * sizeof(uint32_t);
    uintptr_t offset = trampoline - g_patch_base;
    memcpy((void *)((uintptr_t)jitGetRwAddr(&g_jit) + offset), trampoline_words, words_size);
    armDCacheFlush((void *)((uintptr_t)jitGetRwAddr(&g_jit) + offset), words_size);
    armICacheInvalidate((void *)((uintptr_t)jitGetRxAddr(&g_jit) + offset), words_size);
    so_flush_caches(&so_mod);
    g_patch_head = trampoline + words_size;

    so_hook guard = hook_addr(target, destination);
    return trampoline;
}

static void install_post_effects_null_guards(void) {
    static const uint32_t update_expected[] = {
        0xEE061A90U, 0xEEB87AE6U
    };
    static const uint32_t disable_all_expected[] = {
        0xE92D4070U, 0xE2805018U
    };
    static const uint32_t get_no_active_expected[] = {
        0xE3A03000U, 0xE2801018U
    };
    static const uint32_t is_active_expected[] = {
        0xE3510018U, 0xE92D4070U
    };
    static const uint32_t init_hud_expected[] = {
        0xE92D4070U, 0xE5903070U
    };
    static const uint32_t load_expected[] = {
        0xE92D47F0U, 0xE59F80B4U
    };
    static const uint32_t save_expected[] = {
        0xE92D4FF0U, 0xE59F30F8U
    };
    static const uint32_t build_grading_expected[] = {
        0xE92D4010U, 0xE5903064U
    };
    static const uint32_t get_effect_expected[] = {
        0xE59F3084U, 0xE92D4070U
    };
    static const uint32_t action_expected[] = {
        0xE59F30C4U, 0xE92D4070U
    };
    static const uint32_t post_draw_expected[] = {
        0xE92D4FF0U, 0xE1A08001U
    };
    static const uint32_t pre_draw_expected[] = {
        0xE92D40F0U, 0xE59F10CCU
    };

    nova3_post_effects_update_original =
        (nova3_post_effects_action_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_UPDATE_OFFSET,
            update_expected, (uintptr_t)&nova3_post_effects_update_guard,
            "N.O.V.A. 3 PostEffects::Update preimage changed.");
    nova3_post_effects_disable_all_original =
        (nova3_post_effects_update_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_DISABLE_ALL_OFFSET,
            disable_all_expected,
            (uintptr_t)&nova3_post_effects_disable_all_guard,
            "N.O.V.A. 3 PostEffects::DisableAllEffects preimage changed.");
    nova3_post_effects_get_no_active_original =
        (nova3_post_effects_update_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_GET_NO_ACTIVE_OFFSET,
            get_no_active_expected,
            (uintptr_t)&nova3_post_effects_get_no_active_guard,
            "N.O.V.A. 3 PostEffects::GetNoActiveEffects preimage changed.");
    nova3_post_effects_init_hud_original =
        (nova3_post_effects_init_hud_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_INIT_HUD_OFFSET,
            init_hud_expected,
            (uintptr_t)&nova3_post_effects_init_hud_guard,
            "N.O.V.A. 3 PostEffects::InitHudEffects preimage changed.");
    nova3_post_effects_is_active_original =
        (nova3_post_effects_is_active_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_IS_ACTIVE_OFFSET,
            is_active_expected,
            (uintptr_t)&nova3_post_effects_is_active_guard,
            "N.O.V.A. 3 PostEffects::IsEffectActivated preimage changed.");
    nova3_post_effects_load_original =
        (nova3_post_effects_file_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_LOAD_OFFSET,
            load_expected, (uintptr_t)&nova3_post_effects_load_guard,
            "N.O.V.A. 3 PostEffects::Load preimage changed.");
    nova3_post_effects_save_original =
        (nova3_post_effects_file_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_SAVE_OFFSET,
            save_expected, (uintptr_t)&nova3_post_effects_save_guard,
            "N.O.V.A. 3 PostEffects::Save preimage changed.");
    nova3_post_effects_build_grading_original =
        (nova3_post_effects_update_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_BUILD_GRADING_OFFSET,
            build_grading_expected,
            (uintptr_t)&nova3_post_effects_build_grading_guard,
            "N.O.V.A. 3 PostEffects::BuildGradingTexture preimage changed.");
    nova3_post_effects_get_effect_original =
        (nova3_post_effects_get_effect_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_GET_EFFECT_OFFSET,
            get_effect_expected,
            (uintptr_t)&nova3_post_effects_get_effect_guard,
            "N.O.V.A. 3 PostEffects::GetEffect preimage changed.");
    nova3_post_effects_deactivate_original =
        (nova3_post_effects_action_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_DEACTIVATE_OFFSET,
            action_expected,
            (uintptr_t)&nova3_post_effects_deactivate_guard,
            "N.O.V.A. 3 PostEffects::DesactivateEffect preimage changed.");
    nova3_post_effects_activate_original =
        (nova3_post_effects_action_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_ACTIVATE_OFFSET,
            action_expected,
            (uintptr_t)&nova3_post_effects_activate_guard,
            "N.O.V.A. 3 PostEffects::ActivateEffect preimage changed.");
    nova3_post_effects_post_draw_original =
        (nova3_post_effects_action_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_POST_DRAW_OFFSET,
            post_draw_expected, (uintptr_t)&nova3_post_effects_post_draw_guard,
            "N.O.V.A. 3 PostEffects::PostDraw preimage changed.");
    nova3_post_effects_pre_draw_original =
        (nova3_post_effects_update_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_POST_EFFECTS_PRE_DRAW_OFFSET,
            pre_draw_expected, (uintptr_t)&nova3_post_effects_pre_draw_guard,
            "N.O.V.A. 3 PostEffects::PreDraw preimage changed.");

    so_flush_caches(&so_mod);
    runtime_trace(
        "NOVA3 null-safe post-effects surface installed thirteen external entry guards");
}

typedef void (*nova3_navspace_pos_to_grid_fn)(void *, const void *, void *);
typedef int (*nova3_navspace_is_pos_fn)(void *, const void *);
typedef int (*nova3_navspace_get_grid_uint_fn)(void *, uint32_t);
typedef int (*nova3_navspace_get_grid_3i_fn)(void *, int, int, int);
typedef int (*nova3_navspace_get_grid_vec_fn)(void *, const void *);
typedef int (*nova3_navspace_get_closest_cell_fn)(void *, void *, const void *, void *);
typedef void (*nova3_navspace_get_closest_pos_to_cell_fn)(void *, void *, const void *, const void *, void *);
typedef int (*nova3_navspace_get_closest_nav_pos_fn)(void *, void *, const void *, void *);
typedef int (*nova3_navspace_test_ray_fn)(void *, void *, const void *, const void *);
typedef int (*nova3_navspace_test_move_fn)(void *, void *, void *, void *, const void *, int);

static nova3_navspace_pos_to_grid_fn nova3_navspace_pos_to_grid_safe_original;
static nova3_navspace_pos_to_grid_fn nova3_navspace_pos_to_grid_original;
static nova3_navspace_is_pos_fn nova3_navspace_is_pos_not_outside_original;
static nova3_navspace_is_pos_fn nova3_navspace_is_pos_valid_dest_original;
static nova3_navspace_get_grid_uint_fn nova3_navspace_get_grid_uint_original;
static nova3_navspace_get_grid_3i_fn nova3_navspace_get_grid_3i_original;
static nova3_navspace_get_grid_vec_fn nova3_navspace_get_grid_vec_original;
static nova3_navspace_get_closest_cell_fn nova3_navspace_get_closest_cell_original;
static nova3_navspace_get_closest_pos_to_cell_fn nova3_navspace_get_closest_pos_to_cell_original;
static nova3_navspace_get_closest_nav_pos_fn nova3_navspace_get_closest_nav_pos_original;
static nova3_navspace_test_ray_fn nova3_navspace_test_ray_original;
static nova3_navspace_test_move_fn nova3_navspace_test_move_original;

static void nova3_navspace_pos_to_grid_safe_guard(void *self, const void *pos, void *out) {
    if (!self || !pos || !out) {
        if (out) memset(out, 0, 6);
        return;
    }
    nova3_navspace_pos_to_grid_safe_original(self, pos, out);
}

static void nova3_navspace_pos_to_grid_guard(void *self, const void *pos, void *out) {
    if (!self || !pos || !out) {
        if (out) memset(out, 0, 6);
        return;
    }
    nova3_navspace_pos_to_grid_original(self, pos, out);
}

static int nova3_navspace_is_pos_not_outside_guard(void *self, const void *pos) {
    if (!self || !pos) return 0;
    return nova3_navspace_is_pos_not_outside_original(self, pos);
}

static int nova3_navspace_is_pos_valid_dest_guard(void *self, const void *pos) {
    if (!self || !pos) return 0;
    return nova3_navspace_is_pos_valid_dest_original(self, pos);
}

static int nova3_navspace_get_grid_uint_guard(void *self, uint32_t idx) {
    if (!self) return 2;
    return nova3_navspace_get_grid_uint_original(self, idx);
}

static int nova3_navspace_get_grid_3i_guard(void *self, int x, int y, int z) {
    if (!self) return 2;
    return nova3_navspace_get_grid_3i_original(self, x, y, z);
}

static int nova3_navspace_get_grid_vec_guard(void *self, const void *coord) {
    if (!self || !coord) return 2;
    return nova3_navspace_get_grid_vec_original(self, coord);
}

static int nova3_navspace_get_closest_cell_guard(void *self, void *navSpace, const void *pos, void *out) {
    if (!self || !navSpace || !pos || !out) {
        if (out) memset(out, 0, 6);
        return 0;
    }
    return nova3_navspace_get_closest_cell_original(self, navSpace, pos, out);
}

static void nova3_navspace_get_closest_pos_to_cell_guard(void *self, void *navSpace, const void *pos, const void *cell, void *out) {
    if (!self || !navSpace || !pos || !cell || !out) {
        if (out && pos) memcpy(out, pos, 12);
        else if (out) memset(out, 0, 12);
        return;
    }
    nova3_navspace_get_closest_pos_to_cell_original(self, navSpace, pos, cell, out);
}

static int nova3_navspace_get_closest_nav_pos_guard(void *self, void *navSpace, const void *pos, void *out) {
    if (!self || !navSpace || !pos || !out) {
        if (out && pos) memcpy(out, pos, 12);
        else if (out) memset(out, 0, 12);
        return 0;
    }
    return nova3_navspace_get_closest_nav_pos_original(self, navSpace, pos, out);
}

static int nova3_navspace_test_ray_guard(void *self, void *navSpace, const void *start, const void *end) {
    if (!self || !navSpace || !start || !end) return 0;
    return nova3_navspace_test_ray_original(self, navSpace, start, end);
}

static int nova3_navspace_test_move_guard(void *self, void *navSpace, void *pos, void *cell, const void *dest, int b) {
    if (!self || !navSpace || !pos || !cell || !dest) return 0;
    return nova3_navspace_test_move_original(self, navSpace, pos, cell, dest, b);
}

/* CAirNavigationComponent::TestStraightPath / CGroundNavigationComponent::TestStraightMove
 * call TestStrMove and then dereference the nav space/mesh it wrote back
 * ([navspace+4]); if the zone or its nav data is gone that is a null deref
 * (far 0x4). Re-implemented with null checks; semantics are otherwise identical. */
typedef int (*nova3_test_str_move_fn)(void *, void *, void *, const void *);
static int nova3_air_test_straight_path_guard(void *self, void *pos, void **zoneRef, const void *dest) {
    if (!self || !pos || !zoneRef || !dest) return 0;
    uint8_t *zone = (uint8_t *)*zoneRef;
    void *query = *(void **)((uint8_t *)self + 140);
    if (!zone || !query) return 0;
    void *ns = *(void **)(zone + 332);
    if (!ns) return 0;
    nova3_test_str_move_fn fn = (nova3_test_str_move_fn)((uintptr_t)so_mod.load_virtbase + 0x005FDAF4U);
    int r = fn(query, &ns, pos, dest);
    if (ns) *zoneRef = *(void **)((uint8_t *)ns + 4);
    return r;
}

static int nova3_ground_test_straight_move_guard(void *self, void **zoneRef, void *pos, const void *dest) {
    if (!self || !zoneRef || !pos || !dest) return 0;
    uint8_t *zone = (uint8_t *)*zoneRef;
    void *query = *(void **)((uint8_t *)self + 152);
    if (!zone || !query) return 0;
    void *mesh = *(void **)(zone + 328);
    if (!mesh) return 0;
    nova3_test_str_move_fn fn = (nova3_test_str_move_fn)((uintptr_t)so_mod.load_virtbase + 0x0015774CU);
    int r = fn(query, &mesh, pos, dest);
    if (mesh) *zoneRef = *(void **)((uint8_t *)mesh + 4);
    return r;
}

static void install_navspace_null_guards(void) {
    static const uint32_t air_tsp_expected[]  = { 0xE92D4010U, 0xE1A04002U };
    static const uint32_t gnd_tsm_expected[]  = { 0xE92D4010U, 0xE1A04001U };
    static const uint32_t pos_to_grid_safe_expected[] = { 0xE92D40F0U, 0xEDD15A01U };
    static const uint32_t pos_to_grid_expected[]      = { 0xE92D40F0U, 0xEDD15A01U };
    static const uint32_t is_pos_not_outside_expected[] = { 0xE92D4070U, 0xEDD15A01U };
    static const uint32_t is_pos_valid_dest_expected[]  = { 0xE92D4070U, 0xEDD15A01U };
    static const uint32_t get_grid_uint_expected[]    = { 0xE92D4070U, 0xE1D031B8U };
    static const uint32_t get_grid_3i_expected[]      = { 0xE192C001U, 0xE92D0030U };
    static const uint32_t get_grid_vec_expected[]     = { 0xE92D0030U, 0xE1D130F0U };
    static const uint32_t get_closest_cell_expected[] = { 0xE92D4FF0U, 0xED2D8B04U };
    static const uint32_t get_closest_pos_expected[]  = { 0xE1D300F0U, 0xEE040A10U };
    static const uint32_t get_closest_nav_pos_expected[] = { 0xE92D41F0U, 0xE24DD010U };
    static const uint32_t test_ray_expected[]         = { 0xE92D4FF0U, 0xE24DD044U };
    static const uint32_t test_move_expected[]        = { 0xE92D4FF0U, 0xED2D8B08U };

    nova3_navspace_pos_to_grid_safe_original =
        (nova3_navspace_pos_to_grid_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_NAVSPACE_POS_TO_GRID_SAFE_OFFSET,
            pos_to_grid_safe_expected, (uintptr_t)&nova3_navspace_pos_to_grid_safe_guard,
            "CNavSpace::PosToGridCoordSafe preimage changed.");

    nova3_navspace_pos_to_grid_original =
        (nova3_navspace_pos_to_grid_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_NAVSPACE_POS_TO_GRID_OFFSET,
            pos_to_grid_expected, (uintptr_t)&nova3_navspace_pos_to_grid_guard,
            "CNavSpace::PosToGridCoord preimage changed.");

    nova3_navspace_is_pos_not_outside_original =
        (nova3_navspace_is_pos_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_NAVSPACE_IS_POS_NOT_OUTSIDE_OFFSET,
            is_pos_not_outside_expected, (uintptr_t)&nova3_navspace_is_pos_not_outside_guard,
            "CNavSpace::IsPosNotOutside preimage changed.");

    nova3_navspace_is_pos_valid_dest_original =
        (nova3_navspace_is_pos_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_NAVSPACE_IS_POS_VALID_DEST_OFFSET,
            is_pos_valid_dest_expected, (uintptr_t)&nova3_navspace_is_pos_valid_dest_guard,
            "CNavSpace::IsPosValidDest preimage changed.");

    nova3_navspace_get_grid_uint_original =
        (nova3_navspace_get_grid_uint_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_NAVSPACE_GET_GRID_UINT_OFFSET,
            get_grid_uint_expected, (uintptr_t)&nova3_navspace_get_grid_uint_guard,
            "CNavSpace::GetGrid(uint) preimage changed.");

    nova3_navspace_get_grid_3i_original =
        (nova3_navspace_get_grid_3i_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_NAVSPACE_GET_GRID_3I_OFFSET,
            get_grid_3i_expected, (uintptr_t)&nova3_navspace_get_grid_3i_guard,
            "CNavSpace::GetGrid(3i) preimage changed.");

    nova3_navspace_get_grid_vec_original =
        (nova3_navspace_get_grid_vec_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_NAVSPACE_GET_GRID_VEC_OFFSET,
            get_grid_vec_expected, (uintptr_t)&nova3_navspace_get_grid_vec_guard,
            "CNavSpace::GetGrid(vec) preimage changed.");

    nova3_navspace_get_closest_cell_original =
        (nova3_navspace_get_closest_cell_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_NAVSPACE_QUERY_GET_CLOSEST_CELL_OFFSET,
            get_closest_cell_expected, (uintptr_t)&nova3_navspace_get_closest_cell_guard,
            "CNavSpaceQuery::GetClosestNavCell preimage changed.");

    nova3_navspace_get_closest_pos_to_cell_original =
        (nova3_navspace_get_closest_pos_to_cell_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_NAVSPACE_QUERY_GET_CLOSEST_POS_OFFSET,
            get_closest_pos_expected, (uintptr_t)&nova3_navspace_get_closest_pos_to_cell_guard,
            "CNavSpaceQuery::GetClosestPosToCell preimage changed.");

    nova3_navspace_get_closest_nav_pos_original =
        (nova3_navspace_get_closest_nav_pos_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_NAVSPACE_QUERY_GET_CLOSEST_NAV_POS_OFFSET,
            get_closest_nav_pos_expected, (uintptr_t)&nova3_navspace_get_closest_nav_pos_guard,
            "CNavSpaceQuery::GetClosestNavPos preimage changed.");

    nova3_navspace_test_ray_original =
        (nova3_navspace_test_ray_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_NAVSPACE_QUERY_TEST_RAY_OFFSET,
            test_ray_expected, (uintptr_t)&nova3_navspace_test_ray_guard,
            "CNavSpaceQuery::TestRay preimage changed.");

    nova3_navspace_test_move_original =
        (nova3_navspace_test_move_fn)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + NOVA3_NAVSPACE_QUERY_TEST_MOVE_OFFSET,
            test_move_expected, (uintptr_t)&nova3_navspace_test_move_guard,
            "CNavSpaceQuery::TestMove preimage changed.");

    (void)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + 0x00565C6CU,
            air_tsp_expected, (uintptr_t)&nova3_air_test_straight_path_guard,
            "CAirNavigationComponent::TestStraightPath preimage changed.");

    (void)install_arm_trampoline_guard(
            (uintptr_t)so_mod.load_virtbase + 0x00569DD0U,
            gnd_tsm_expected, (uintptr_t)&nova3_ground_test_straight_move_guard,
            "CGroundNavigationComponent::TestStraightMove preimage changed.");

    so_flush_caches(&so_mod);
    runtime_trace("NOVA3 null-safe navspace surface installed fourteen navigation guards");
}

static void install_null_safe_collada_scene_consumers(void) {
    static const uint32_t expected_laser_a_mesh_failure_exit[] = {
        0xEB1F59F1U, /* bl glf::Console::Println */
        0xEAFFFFDAU  /* b addChild path */
    };
    static const uint32_t replacement_laser_a_mesh_failure_exit[] = {
        0xE1A0000BU, /* mov r0, fp: load the intrusive_ptr wrapper */
        0xEAFFFFE0U  /* b 0x00107044: release wrapper, then continue */
    };
    static const uint32_t expected_laser_a_scene_failure_exit[] = {
        0xEB1F59E8U, /* bl glf::Console::Println */
        0xE59D3014U, /* ldr r3, [sp, #20] */
        0xEAFFFFBBU  /* b scene-use path */
    };
    static const uint32_t replacement_laser_a_scene_failure_exit[] = {
        0xE1A0000BU, /* mov r0, fp: load the null intrusive_ptr wrapper */
        0xE59D3014U, /* preserve the guarded wrapper read */
        0xEAFFFFD6U  /* b 0x00107044: release wrapper, then continue */
    };
    static const uint32_t expected_laser_b_mesh_failure_exit[] = {
        0xEB1F57E7U, /* bl glf::Console::Println */
        0xEAFFFFDAU  /* b addChild path */
    };
    static const uint32_t replacement_laser_b_mesh_failure_exit[] = {
        0xE1A0000BU, /* mov r0, fp: load the intrusive_ptr wrapper */
        0xEAFFFFE0U  /* b 0x0010786C: release wrapper, then continue */
    };
    static const uint32_t expected_laser_b_scene_failure_exit[] = {
        0xEB1F57DEU, /* bl glf::Console::Println */
        0xE59D3014U, /* ldr r3, [sp, #20] */
        0xEAFFFFBBU  /* b scene-use path */
    };
    static const uint32_t replacement_laser_b_scene_failure_exit[] = {
        0xE1A0000BU, /* mov r0, fp: load the null intrusive_ptr wrapper */
        0xE59D3014U, /* preserve the guarded wrapper read */
        0xEAFFFFD6U  /* b 0x0010786C: release wrapper, then continue */
    };
    static const uint32_t expected_set_bdae_mesh_tail = 0xEAFFFFE6U;
    static const uint32_t replacement_set_bdae_mesh_tail = 0xEA000007U;
    static const uint32_t expected_set_bdae_cleanup[] = {
        0xE59D0004U, /* ldr r0, [sp, #4] */
        0xEAFFFFD6U  /* b 0x00107938 */
    };
    static const uint32_t replacement_set_bdae_cleanup[] = {
        0xE59D3004U, /* ldr r3, [sp, #4] */
        0xEAFFFFE4U  /* b 0x00107970: release if needed, then return */
    };
    static const uint32_t expected_animator_sampler_empty_scene_exit =
        0x0A000007U; /* beq continue constructing animator sets */
    static const uint32_t replacement_animator_sampler_empty_scene_exit =
        0x0A000181U; /* beq fully initialized constructor epilogue */
    struct word_patch {
        uint32_t offset;
        const uint32_t *expected;
        const uint32_t *replacement;
        size_t bytes;
    } patches[] = {
        {NOVA3_LASER_CTOR_A_MESH_FAILURE_EXIT_OFFSET,
         expected_laser_a_mesh_failure_exit,
         replacement_laser_a_mesh_failure_exit,
         sizeof(expected_laser_a_mesh_failure_exit)},
        {NOVA3_LASER_CTOR_A_SCENE_FAILURE_EXIT_OFFSET,
         expected_laser_a_scene_failure_exit,
         replacement_laser_a_scene_failure_exit,
         sizeof(expected_laser_a_scene_failure_exit)},
        {NOVA3_LASER_CTOR_B_MESH_FAILURE_EXIT_OFFSET,
         expected_laser_b_mesh_failure_exit,
         replacement_laser_b_mesh_failure_exit,
         sizeof(expected_laser_b_mesh_failure_exit)},
        {NOVA3_LASER_CTOR_B_SCENE_FAILURE_EXIT_OFFSET,
         expected_laser_b_scene_failure_exit,
         replacement_laser_b_scene_failure_exit,
         sizeof(expected_laser_b_scene_failure_exit)},
        {NOVA3_LASER_SET_BDAE_MESH_NULL_TAIL_OFFSET,
         &expected_set_bdae_mesh_tail, &replacement_set_bdae_mesh_tail,
         sizeof(uint32_t)},
        {NOVA3_LASER_SET_BDAE_FAILURE_CLEANUP_OFFSET,
         expected_set_bdae_cleanup, replacement_set_bdae_cleanup,
         sizeof(expected_set_bdae_cleanup)},
        {NOVA3_ANIMATOR_SAMPLER_CTOR_A_EMPTY_SCENE_EXIT_OFFSET,
         &expected_animator_sampler_empty_scene_exit,
         &replacement_animator_sampler_empty_scene_exit,
         sizeof(expected_animator_sampler_empty_scene_exit)},
        {NOVA3_ANIMATOR_SAMPLER_CTOR_B_EMPTY_SCENE_EXIT_OFFSET,
         &expected_animator_sampler_empty_scene_exit,
         &replacement_animator_sampler_empty_scene_exit,
         sizeof(expected_animator_sampler_empty_scene_exit)}
    };

    for (size_t index = 0; index < sizeof(patches) / sizeof(patches[0]);
         index++) {
        uintptr_t target = (uintptr_t)so_mod.load_virtbase + patches[index].offset;
        if (memcmp((const void *)target, patches[index].expected,
                   patches[index].bytes) != 0)
            fatal_error("N.O.V.A. 3 Collada scene consumer preimage changed.");
    }
    for (size_t index = 0; index < sizeof(patches) / sizeof(patches[0]);
         index++) {
        uintptr_t target = (uintptr_t)so_mod.load_virtbase + patches[index].offset;
        so_patch_code(
            (void *)target, patches[index].replacement, patches[index].bytes);
        if (memcmp((const void *)target, patches[index].replacement,
                   patches[index].bytes) != 0)
            fatal_error("Could not install null-safe Collada scene consumers.");
    }

    /* Install null-safe skybox trampolines that correctly preserve r6 = sp + 72 */
    static const uint32_t expected_skybox_preimage[4] = {
        0xE5943104U, /* ldr r3, [r4, #260] */
        0xE28D6048U, /* add r6, sp, #72 */
        0xE1A00003U, /* mov r0, r3 */
        0xE5933000U  /* ldr r3, [r3] */
    };
    struct skybox_trampoline_spec {
        uint32_t offset;
        uint32_t cleanup_offset;
        uint32_t resume_offset;
    } skybox_specs[2] = {
        { NOVA3_SKYBOX_CTOR_A_SCENE_USE_OFFSET, 0x00107C3CU, 0x00107BE4U },
        { NOVA3_SKYBOX_CTOR_B_SCENE_USE_OFFSET, 0x00107F24U, 0x00107ECCU }
    };

    for (int s = 0; s < 2; s++) {
        uintptr_t target = (uintptr_t)so_mod.load_virtbase + skybox_specs[s].offset;
        uintptr_t cleanup_addr = (uintptr_t)so_mod.load_virtbase + skybox_specs[s].cleanup_offset;
        uintptr_t resume_addr = (uintptr_t)so_mod.load_virtbase + skybox_specs[s].resume_offset;

        if (memcmp((const void *)target, expected_skybox_preimage, sizeof(expected_skybox_preimage)) != 0) {
            fatal_error("Skybox CTOR preimage changed at %p.", (void *)target);
        }

        uintptr_t trampoline = (g_patch_head + 3U) & ~(uintptr_t)3U;
        uintptr_t patch_limit = g_patch_base + g_patch_size;

        uint32_t code[9] = {
            0xE5940104U, /* ldr r0, [r4, #260] */
            0xE3500000U, /* cmp r0, #0 */
            0x1A000001U, /* bne normal_path (skip cleanup jump) */
            0xE51FF004U, /* ldr pc, [pc, #-4] */
            (uint32_t)cleanup_addr,
            /* normal_path: */
            0xE28D6048U, /* add r6, sp, #72 (restores r6 correctly!) */
            0xE5903000U, /* ldr r3, [r0] (vtable load) */
            0xE51FF004U, /* ldr pc, [pc, #-4] */
            (uint32_t)resume_addr
        };

        size_t words_size = sizeof(code);
        if (trampoline + words_size > patch_limit) {
            fatal_error("No executable arena for skybox trampoline.");
        }

        uintptr_t jit_offset = trampoline - g_patch_base;
        memcpy((void *)((uintptr_t)jitGetRwAddr(&g_jit) + jit_offset), code, words_size);
        armDCacheFlush((void *)((uintptr_t)jitGetRwAddr(&g_jit) + jit_offset), words_size);
        g_patch_head = trampoline + words_size;

        uintptr_t rx_trampoline = (uintptr_t)jitGetRxAddr(&g_jit) + jit_offset;
        hook_arm(target, rx_trampoline);

        uint32_t nop[2] = {0xE1A00000U, 0xE1A00000U};
        so_patch_code((void *)(target + 8), nop, sizeof(nop));
    }

    so_flush_caches(&so_mod);
    runtime_trace(
        "NOVA3 Collada scene consumers installed destructor-safe optional-node route");
    runtime_trace(
        "V80 skybox null guard preserves scene vtable before virtual call and restores r6");
    runtime_trace(
        "NOVA3 animator sampler constructors disabled without Collada scene");
}

static void install_vita_file_stream_manager_limit(void) {
    static const uint32_t expected = 0xE35200DFU;    /* cmp r2, #223 */
    static const uint32_t replacement = 0xE352001FU; /* cmp r2, #31 */
    uintptr_t target = (uintptr_t)so_mod.load_virtbase + 0x008E296CU;

    if (memcmp((const void *)target, &expected, sizeof(expected)) != 0) {
        runtime_trace("N.O.V.A. 3 FileStreamMgr limit preimage changed.");
        return;
    }
    so_patch_code((void *)target, &replacement, sizeof(replacement));
    if (memcmp((const void *)target, &replacement, sizeof(replacement)) != 0) {
        runtime_trace("Could not install the FileStreamMgr limit.");
    }
    so_flush_caches(&so_mod);
    runtime_trace("NOVA3 FileStreamMgr live-stream ceiling lowered from 224 to 32");
}

static void disable_tracking_manager_parsexml(void) {
    uint32_t nop = 0xe1a00000; // ARM nop
    uintptr_t target = (uintptr_t)so_mod.load_virtbase + 0xb8c6e8;
    so_patch_code((void *)target, &nop, 4);
    
    // Also patch 0xb8bc64 just in case
    uint32_t bx_lr = 0xe12fff1e;
    uintptr_t target2 = (uintptr_t)so_mod.load_virtbase + 0xb8bc64;
    so_patch_code((void *)target2, &bx_lr, 4);
    
    so_flush_caches(&so_mod);
    runtime_trace("Disabled TrackingManager::ParseXML call at %p to prevent crash!", (void*)target);

}

static void disable_engine_touch_doubletap_sprint(void) {
    uint32_t arm_nop = 0xe1a00000;

    // 1. NOP the write to player[0x44] in PlayerComponent::ComputeControlVelocity.
    //    Original: 1dda10: e5c40044 (strb r0, [r4, #68])
    //    r7 was incorrectly assumed to be 0 here — in paths where CanRush=1 AND
    //    weapon[0x64]=1 AND speed < threshold, r7=1 and my previous strb r7 patch
    //    wrote 1 to player[0x44], triggering forward sprint while walking backward.
    //    Simply NOP the write: port.c zeroes player[0x44U] every frame before
    //    native_step, and since the write is gone it stays 0 throughout the step.
    uintptr_t target_store = (uintptr_t)so_mod.load_virtbase + 0x1dda10;
    so_patch_code((void *)target_store, &arm_nop, 4);

    // 2. NOP the branch on player[0x44] in PlayerComponent::ComputeControlVelocity (start rush).
    //    Original: 1dda54: 1a000007 (bne 1dda78)
    //    With player[0x44] always 0 this branch never fired anyway; NOP is a no-op safety measure.
    uintptr_t target_branch1 = (uintptr_t)so_mod.load_virtbase + 0x1dda54;
    so_patch_code((void *)target_branch1, &arm_nop, 4);

    // 3. NOP the branch on player[0x44] in the rush-speed path (PlayerComponent::ComputeControlVelocity).
    //    Original: 1ddd44: 1a000002 (bne 1ddd54)
    uintptr_t target_branch2 = (uintptr_t)so_mod.load_virtbase + 0x1ddd44;
    so_patch_code((void *)target_branch2, &arm_nop, 4);

    // 4. Set WalkJoystick::s_DoubleTapTime to -1.0f so the press-based double-tap
    //    detector (RaisePressEvent) can never fire: elapsed >= 0 > -1.0f always.
    float neg_one = -1.0f;
    uintptr_t target_dt_time = (uintptr_t)so_mod.load_virtbase + 0x00d780b0;
    so_patch_code((void *)target_dt_time, &neg_one, 4);

    // 5. NOP the branch in WalkJoystick::RaiseMoveEvent that triggers Event 16 (EvDoubleTap / Rush).
    //    Original: 327120: da000021 (ble 3271ac)
    //    When flicking the left stick upwards twice quickly, lr <= 149 and the engine branched
    //    to 3271ac to fire Event 16 (EvGeneric) via EventManager::raiseSync, which triggers rush.
    //    NOPing this branch ensures WalkJoystick NEVER raises Event 16 under any circumstance!
    uintptr_t target_move_branch = (uintptr_t)so_mod.load_virtbase + 0x00327120U;
    so_patch_code((void *)target_move_branch, &arm_nop, 4);

    // 5b. NOP the branch in MobileAnalogJoystick::RaiseMoveEvent that triggers Event 16 (EvDoubleTap / Rush).
    //     Original: 327578: da000021 (ble 327604)
    //     Certain levels and control schemes instantiate MobileAnalogJoystick rather than WalkJoystick.
    //     NOPing this branch ensures MobileAnalogJoystick NEVER raises Event 16 under any circumstance!
    uintptr_t target_mobile_move_branch = (uintptr_t)so_mod.load_virtbase + 0x00327578U;
    so_patch_code((void *)target_mobile_move_branch, &arm_nop, 4);

    // 5c. Prevent WalkJoystick::RaiseReleaseEvent from ever emitting Event 15 (quick tap-release).
    //     Original: 324d84: da00000b (ble 324db8)
    //     Changing to unconditional branch "b 324db8" (0xea00000b) completely skips Event 15.
    uint32_t b_skip_ev15 = 0xea00000b;
    uintptr_t target_skip_ev15 = (uintptr_t)so_mod.load_virtbase + 0x00324d84U;
    so_patch_code((void *)target_skip_ev15, &b_skip_ev15, 4);

    // 6. As an absolute guarantee, NOP the handlers for Event 16 in CPlayerControl::onEvent:
    //    1bf214: 0a000339 (beq 1bff00) -> NOP (skip rush start trigger on Event 16)
    //    1c0e00: 03a03001 (moveq r3, #1) -> NOP (don't set rush flag)
    //    1c0e04: 05c43034 (strbeq r3, [r4, #52]) -> NOP (don't write player_control[0x34])
    uintptr_t target_ctrl_ev16_a = (uintptr_t)so_mod.load_virtbase + 0x001bf214U;
    so_patch_code((void *)target_ctrl_ev16_a, &arm_nop, 4);

    uintptr_t target_ctrl_ev16_b = (uintptr_t)so_mod.load_virtbase + 0x001c0e00U;
    so_patch_code((void *)target_ctrl_ev16_b, &arm_nop, 4);

    uintptr_t target_ctrl_ev16_c = (uintptr_t)so_mod.load_virtbase + 0x001c0e04U;
    so_patch_code((void *)target_ctrl_ev16_c, &arm_nop, 4);

    // 6b. NOP the Event 16 entry in the CPlayerControl::onEvent jump table (0x001bf718).
    //     Original: 1bf718: ea0002c8 (b 1c0240)
    //     NOPing this causes Event 16 in the switch table to fall through to line 1bf71c (b 1bf22c, clean return).
    uintptr_t target_ctrl_ev16_table = (uintptr_t)so_mod.load_virtbase + 0x001bf718U;
    so_patch_code((void *)target_ctrl_ev16_table, &arm_nop, 4);

    // 6c. NOP the Event 15 write to player_control[0x30] in CPlayerControl::onEvent (0x001c0df4).
    //     Original: 1c0df4: 05c43030 (strbeq r3, [r4, #48])
    uintptr_t target_ctrl_ev15_write = (uintptr_t)so_mod.load_virtbase + 0x001c0df4U;
    so_patch_code((void *)target_ctrl_ev15_write, &arm_nop, 4);

    // 7. In PlayerComponent::ComputeControlVelocity (Path A at 0x1dda64):
    //    Original: 1dda64: e59f3198 (ldr r3, [pc, #408] -> &m_bRushbyDoubleTouch)
    //    If player[0x45] == 0 (gamepad sprint not active), the engine checked m_bRushbyDoubleTouch.
    //    If m_bRushbyDoubleTouch was 1, it fell through to 0x1dda78 and called StartRush!
    //    Patch 0x1dda64 with "b 0x1ddc18" (0xea00006b): unconditionally branch to normal walk
    //    (0x1ddc18, which also calls EndRush), completely bypassing m_bRushbyDoubleTouch!
    uint32_t b_normal_walk = 0xea00006b;
    uintptr_t target_skip_rush_doubletouch = (uintptr_t)so_mod.load_virtbase + 0x001dda64U;
    so_patch_code((void *)target_skip_rush_doubletouch, &b_normal_walk, 4);

    // 8. In PlayerComponent::ComputeControlVelocity (Path B at 0x1de3d0):
    //    Original: 1de3d0: 1afffda8 (bne 1dda78)
    //    This is the alternative control scheme path in ComputeControlVelocity. If player[0x44] == 1,
    //    it branched back to 0x1dda78 to StartRush!
    //    NOPing this branch ensures Path B also exclusively honors player[0x45] (L3 gamepad sprint)
    //    and branches to normal walk (0x1ddc18) when player[0x45] == 0!
    uintptr_t target_ctrl_scheme_b = (uintptr_t)so_mod.load_virtbase + 0x001de3d0U;
    so_patch_code((void *)target_ctrl_scheme_b, &arm_nop, 4);

    so_flush_caches(&so_mod);
    runtime_trace("Disabled engine double-tap and touch sprint detection in PlayerComponent, WalkJoystick & MobileAnalogJoystick");
}

/* ------------------------------------------------- End-Level Auto-Save Hook */
typedef int (*nova3_levels_save_fn)(void *self);
typedef void *(*nova3_app_get_instance_fn)(void);
typedef int (*nova3_app_save_fn)(void *self, const char *path, int mode);

static void nova3_end_level_autosave_hook(void *self) {
    // Call the original UnlockCrtLevel first
    typedef void (*unlock_crt_level_fn)(void *);
    unlock_crt_level_fn orig_unlock = (unlock_crt_level_fn)((uintptr_t)so_mod.load_virtbase + 0x002425D8U);
    orig_unlock(self);

    // 1. Force CLevelsManager::Save (saves unlocked missions to a3.dat immediately)
    nova3_levels_save_fn levels_save = (nova3_levels_save_fn)((uintptr_t)so_mod.load_virtbase + 0x00252048U);
    if (self) {
        levels_save(self);
    }

    // 2. Force Application::Save if application instance is available (saves campaign profile to a1.dat)
    nova3_app_get_instance_fn get_app = (nova3_app_get_instance_fn)((uintptr_t)so_mod.load_virtbase + 0x0059D9D0U);
    void *app = get_app ? get_app() : NULL;
    if (app) {
        nova3_app_save_fn app_save = (nova3_app_save_fn)((uintptr_t)so_mod.load_virtbase + 0x005A1208U);
        app_save(app, NULL, 1);
    }

    runtime_trace("Mission complete: immediately saved progress (a3.dat & a1.dat flushed)");
}

static void install_end_level_autosave_hook(void) {
    // In GS_EndLevel::Create at 0x003AA574:
    // Original:
    //   3aa574: ebfa6017 (bl 0x2425d8 UnlockCrtLevel)
    //   3aa578: e7956006 (ldr r6, [r5, r6])
    uintptr_t target = (uintptr_t)so_mod.load_virtbase + 0x003AA574U;
    static const uint32_t expected_bl[] = { 0xEBFA6017U, 0xE7956006U };
    (void)install_arm_trampoline_guard(target, expected_bl, (uintptr_t)&nova3_end_level_autosave_hook,
                                       "GS_EndLevel::Create UnlockCrtLevel call changed");

    so_flush_caches(&so_mod);
    runtime_trace("End-level instant autosave hook installed successfully");
}

void so_patch(void) {
    if (R_FAILED(jitCreate(&g_jit, g_patch_size))) fatal_error("jitCreate failed!");
    jitTransitionToExecutable(&g_jit);
    g_patch_base = (uintptr_t)jitGetRxAddr(&g_jit);
    g_patch_head = g_patch_base;

    install_vita_file_stream_manager_limit();
    disable_tracking_manager_parsexml();
    install_post_effects_null_guards();
    install_navspace_null_guards();
    install_null_safe_collada_scene_consumers();
    disable_engine_touch_doubletap_sprint();
    install_end_level_autosave_hook();
}
