# Core simulation library
add_library(frogger_core STATIC
    core/rng.c
    core/config.c
    core/math_util.c
    core/ring_buffer.c
    core/replay.c
    core/metrics.c
    env/frogger_env.c
    env/frogger_rules.c
    env/frogger_map.c
    env/frogger_danger.c
    env/frogger_observation.c
    env/frogger_reward.c
    agents/agent.c
    sim/runner.c
    sim/benchmark.c
    sim/evaluator.c
)

target_include_directories(frogger_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# CLI: headless runner
add_executable(frogger_headless cli/main_headless.c)
target_link_libraries(frogger_headless PRIVATE frogger_core)

# CLI: benchmark
add_executable(frogger_benchmark cli/main_benchmark.c)
target_link_libraries(frogger_benchmark PRIVATE frogger_core)

# Visualizer (raylib)
if(AI_FROGGER_BUILD_VIZ)
    find_package(raylib QUIET)
    if(NOT raylib_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.0
        )
        set(FETCHCONTENT_QUIET OFF)
        set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
        FetchContent_MakeAvailable(raylib)
    endif()

    add_executable(frogger_viz
        viz/main_viz.c
        viz/renderer.c
        viz/dashboard.c
        viz/charts.c
        viz/ui_controls.c
        viz/viz_session.c
    )
    target_link_libraries(frogger_viz PRIVATE frogger_core raylib)
endif()
