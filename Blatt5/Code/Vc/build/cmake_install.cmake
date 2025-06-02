# Install script for directory: /home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/users8/wioo/s9292109/Vc")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/build/libVc.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/build/CMakeFiles/Vc.dir/install-cxx-module-bmi-Release.cmake" OPTIONAL)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/Vc" TYPE DIRECTORY FILES "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/" FILES_MATCHING REGEX "/*.(h|tcc|def)$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/Vc" TYPE FILE FILES
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/Allocator"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/IO"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/Memory"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/SimdArray"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/Utils"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/Vc"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/algorithm"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/array"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/iterators"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/limits"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/simdize"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/span"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/type_traits"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/Vc/vector"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Vc/VcTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Vc/VcTargets.cmake"
         "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/build/CMakeFiles/Export/581a4d2dece2886dcb64e7a909b5650d/VcTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Vc/VcTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Vc/VcTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Vc" TYPE FILE FILES "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/build/CMakeFiles/Export/581a4d2dece2886dcb64e7a909b5650d/VcTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Vc" TYPE FILE FILES "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/build/CMakeFiles/Export/581a4d2dece2886dcb64e7a909b5650d/VcTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Vc" TYPE FILE FILES
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/cmake/UserWarning.cmake"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/cmake/VcMacros.cmake"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/cmake/AddCompilerFlag.cmake"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/cmake/CheckCCompilerFlag.cmake"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/cmake/CheckCXXCompilerFlag.cmake"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/cmake/OptimizeForArchitecture.cmake"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/cmake/FindVc.cmake"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/build/cmake/VcConfig.cmake"
    "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/build/cmake/VcConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
  file(WRITE "/home/users8/wioo/s9292109/Desktop/HLRA_Codes/blatt_4/HLRAPrak/Blatt5/Code/Vc/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
