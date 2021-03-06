project(AutoSweep)
cmake_minimum_required(VERSION 2.8)


#set(CMAKE_BUILD_TYPE "Release")
remove_definitions(-DNDEBUG)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wsign-compare")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


#--- Game logic base code ---
add_library(sweep-core SHARED
    utils.cc
    log.cc
    Cell.cc
    MineField.cc
    SharedObject.cc
    GameLogic.cc
    AutoSweep.cc)

target_link_libraries(sweep-core
  dl)


#--- Basic nearest neighbour solver ---
add_library(neighbour-sweep SHARED
  solvers/NeighbourSweep.cc
  )

target_link_libraries(neighbour-sweep
  sweep-core)



#--- Constraint Equation based solver --- 
add_library(constraint-sweep SHARED
  solvers/ConstraintSweep.cc
  )

target_link_libraries(constraint-sweep
  sweep-core)
  


#--- command line test tool ---
add_executable(cas
  as.cc
  )

target_link_libraries(cas sweep-core)


#--- command line test tool ---
add_executable(test_Grid
  test/test_Grid.cc
  )

#target_link_libraries(cas sweep-core)


# check if Qt development files are available

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows*")
  # use windows specific qt config file
  set(QT_MKSPECS_DIR ${CMAKE_FIND_ROOT_PATH}/qt/)
endif()

find_package(Qt4)


if(QT4_FOUND)

  include(${QT_USE_FILE})
  include_directories(${QT_INCLUDE_DIR})
  link_directories(${QT_LIBRARY_DIR})

  qt4_add_resources(AS_MOC_QRCS qt/icons.qrc)

  if (APPLE)
  
    set(OSX_ICON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/autosweep.icns) 
    set_source_files_properties( ${OSX_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)  
  endif()

  qt4_wrap_cpp(AS_MOC_SRCS
    qt/MineFieldWidget.h
    qt/MineFieldButton.h
    qt/QtGameLogic.h
    )

  # --- basic qt visualization of the sweeping process ---
  add_executable(qasview MACOSX_BUNDLE
    qt/qasview.cc
    qt/IconFactory.cc 
    qt/MineFieldWidget.cc 
    qt/MineFieldButton.cc
    qt/QtGameLogic.cc
    qt/QtGameView.cc

    ${AS_MOC_QRCS}
    ${AS_MOC_SRCS}
    ${OSX_ICON_FILES}
    )

  set_target_properties(qasview PROPERTIES 
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/etc/autosweep-Info.plist
    MACOSX_BUNDLE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icons/autosweep.icns)

  target_link_libraries(qasview
    sweep-core
    ${QT_LIBRARIES}
    )

  install(TARGETS qasview DESTINATION "AutoSweep")

else(QT4_FOUND)

  message(STATUS "Qt4 not found, Qt apps disabled")

endif(QT4_FOUND)

# disabled (for now) package generation code
#set(CPACK_BUNDLE_NAME "AutoSweep")
#set(CPACK_BUNDLE_PLIST ${CMAKE_SOURCE_DIR}/autosweep-Info.plist)
#set(CPACK_BUNDLE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icons/autosweep.icns)
#set(CPACK_PACKAGE_NAME "AutoSweep")
#include(CPack)