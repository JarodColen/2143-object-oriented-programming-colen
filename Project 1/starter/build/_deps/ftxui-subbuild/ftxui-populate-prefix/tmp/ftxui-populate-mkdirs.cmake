# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/jarod/Documents/College/2026/CMPS 2143/JarodColen - Github/2143-object-oriented-programming-colen/Project 1/starter/build/_deps/ftxui-src")
  file(MAKE_DIRECTORY "C:/Users/jarod/Documents/College/2026/CMPS 2143/JarodColen - Github/2143-object-oriented-programming-colen/Project 1/starter/build/_deps/ftxui-src")
endif()
file(MAKE_DIRECTORY
  "C:/Users/jarod/Documents/College/2026/CMPS 2143/JarodColen - Github/2143-object-oriented-programming-colen/Project 1/starter/build/_deps/ftxui-build"
  "C:/Users/jarod/Documents/College/2026/CMPS 2143/JarodColen - Github/2143-object-oriented-programming-colen/Project 1/starter/build/_deps/ftxui-subbuild/ftxui-populate-prefix"
  "C:/Users/jarod/Documents/College/2026/CMPS 2143/JarodColen - Github/2143-object-oriented-programming-colen/Project 1/starter/build/_deps/ftxui-subbuild/ftxui-populate-prefix/tmp"
  "C:/Users/jarod/Documents/College/2026/CMPS 2143/JarodColen - Github/2143-object-oriented-programming-colen/Project 1/starter/build/_deps/ftxui-subbuild/ftxui-populate-prefix/src/ftxui-populate-stamp"
  "C:/Users/jarod/Documents/College/2026/CMPS 2143/JarodColen - Github/2143-object-oriented-programming-colen/Project 1/starter/build/_deps/ftxui-subbuild/ftxui-populate-prefix/src"
  "C:/Users/jarod/Documents/College/2026/CMPS 2143/JarodColen - Github/2143-object-oriented-programming-colen/Project 1/starter/build/_deps/ftxui-subbuild/ftxui-populate-prefix/src/ftxui-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/jarod/Documents/College/2026/CMPS 2143/JarodColen - Github/2143-object-oriented-programming-colen/Project 1/starter/build/_deps/ftxui-subbuild/ftxui-populate-prefix/src/ftxui-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/jarod/Documents/College/2026/CMPS 2143/JarodColen - Github/2143-object-oriented-programming-colen/Project 1/starter/build/_deps/ftxui-subbuild/ftxui-populate-prefix/src/ftxui-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
