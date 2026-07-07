#include "core/config.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const char* json_content =
        "{\n"
        "    \"width\": 15,\n"
        "    \"height\": 16,\n"
        "    \"mode\": \"survival\",\n"
        "    \"max_steps\": 800,\n"
        "    \"seed\": 999,\n"
        "    \"river_start_row\": 2,\n"
        "    \"river_end_row\": 7,\n"
        "    \"road_start_row\": 8,\n"
        "    \"road_end_row\": 14,\n"
        "    \"goal_row\": 0,\n"
        "    \"start_row\": 15,\n"
        "    \"log_speed_min\": 0.3,\n"
        "    \"log_speed_max\": 1.5,\n"
        "    \"car_speed_min\": 0.8,\n"
        "    \"car_speed_max\": 3.0\n"
        "}\n";

    const char* path = "test_config.json";
    FILE* f = fopen(path, "w");
    assert(f != NULL);
    fputs(json_content, f);
    fclose(f);

    FroggerConfig cfg;
    assert(config_load_json(&cfg, path) == 1);

    assert(cfg.grid_width == 15);
    assert(cfg.grid_height == 16);
    assert(cfg.mode == MODE_SURVIVAL);
    assert(cfg.max_steps == 800);
    assert(cfg.seed == 999);
    assert(cfg.river_start_row == 2);
    assert(cfg.river_end_row == 7);
    assert(cfg.road_start_row == 8);
    assert(cfg.road_end_row == 14);
    assert(cfg.goal_row == 0);
    assert(cfg.start_row == 15);
    assert(cfg.log_speed_min > 0.29f && cfg.log_speed_min < 0.31f);
    assert(cfg.log_speed_max > 1.49f && cfg.log_speed_max < 1.51f);
    assert(cfg.car_speed_min > 0.79f && cfg.car_speed_min < 0.81f);
    assert(cfg.car_speed_max > 2.99f && cfg.car_speed_max < 3.01f);

    assert(config_load_json(&cfg, "nonexistent_file.json") == 0);

    printf("test_config: ALL PASSED\n");
    return 0;
}
