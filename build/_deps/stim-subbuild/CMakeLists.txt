# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.16.3)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(stim-populate NONE)

include(ExternalProject)
ExternalProject_Add(stim-populate
                     "UPDATE_DISCONNECTED" "False" "GIT_REPOSITORY" "https://github.com/quantumlib/Stim.git" "GIT_TAG" "da4594c5ede00a063ec2b84bd830f846b5d097dd"
                    SOURCE_DIR          "/home/users/davide.salonico/sparse_blossom_prj/build/_deps/stim-src"
                    BINARY_DIR          "/home/users/davide.salonico/sparse_blossom_prj/build/_deps/stim-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)
