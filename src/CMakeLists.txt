add_library(asound_module_pcm_tads SHARED pcm_tads.c tads_algo.c)

target_link_libraries(asound_module_pcm_tads asound)

add_definitions(-DPIC)

install(TARGETS asound_module_pcm_tads LIBRARY DESTINATION alsa-lib)
