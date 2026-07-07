add_executable(test_rng test_rng.c)
target_link_libraries(test_rng PRIVATE frogger_core)
add_test(NAME test_rng COMMAND test_rng)

add_executable(test_frogger_env test_frogger_env.c)
target_link_libraries(test_frogger_env PRIVATE frogger_core)
add_test(NAME test_frogger_env COMMAND test_frogger_env)

add_executable(test_map test_map.c)
target_link_libraries(test_map PRIVATE frogger_core)
add_test(NAME test_map COMMAND test_map)

add_executable(test_observation test_observation.c)
target_link_libraries(test_observation PRIVATE frogger_core)
add_test(NAME test_observation COMMAND test_observation)

add_executable(test_reward test_reward.c)
target_link_libraries(test_reward PRIVATE frogger_core)
add_test(NAME test_reward COMMAND test_reward)

add_executable(test_agents test_agents.c)
target_link_libraries(test_agents PRIVATE frogger_core)
add_test(NAME test_agents COMMAND test_agents)

add_executable(test_determinism test_determinism.c)
target_link_libraries(test_determinism PRIVATE frogger_core)
add_test(NAME test_determinism COMMAND test_determinism)

add_executable(test_replay test_replay.c)
target_link_libraries(test_replay PRIVATE frogger_core)
add_test(NAME test_replay COMMAND test_replay)

add_executable(test_neuro_agent test_neuro_agent.c)
target_link_libraries(test_neuro_agent PRIVATE frogger_core)
add_test(NAME test_neuro_agent COMMAND test_neuro_agent
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_genetic test_genetic.c)
target_link_libraries(test_genetic PRIVATE frogger_core)
add_test(NAME test_genetic COMMAND test_genetic
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_config test_config.c)
target_link_libraries(test_config PRIVATE frogger_core)
add_test(NAME test_config COMMAND test_config
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_metrics test_metrics.c)
target_link_libraries(test_metrics PRIVATE frogger_core)
add_test(NAME test_metrics COMMAND test_metrics
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
