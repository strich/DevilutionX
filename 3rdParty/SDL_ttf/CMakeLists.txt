if(NOT DEVILUTIONX_SYSTEM_SDL_TTF)
  include(functions/FetchContent_ExcludeFromAll_backport)

  set(SDL2TTF_SAMPLES OFF CACHE BOOL "Disable SDL_ttf samples" FORCE)
  set(SDL2TTF_VENDORED ON CACHE BOOL "Use vendored dependencies" FORCE)

  if(DEVILUTIONX_STATIC_SDL_TTF)
    set(BUILD_SHARED_LIBS OFF)
  else()
    set(BUILD_SHARED_LIBS ON)
  endif()

  include(FetchContent)
  FetchContent_Declare_ExcludeFromAll(SDL2_ttf
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
      GIT_TAG release-2.24.0
  )
  FetchContent_MakeAvailable_ExcludeFromAll(SDL2_ttf)

  if(NOT TARGET SDL2_ttf::SDL2_ttf)
      if(TARGET SDL2_ttf)
          add_library(SDL2_ttf::SDL2_ttf ALIAS SDL2_ttf)
      endif()
  endif()

  if(NOT TARGET SDL2::SDL2_ttf)
      if(TARGET SDL2_ttf)
          add_library(SDL2::SDL2_ttf ALIAS SDL2_ttf)
      endif()
  endif()
endif()
