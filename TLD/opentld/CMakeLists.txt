
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(OpenTLD)



SET(HEADERS    main/Config.h main/Gui.h main/Main.h main/Settings.h main/Trajectory.h MainOTLD.h )
SET(SOURCES    main/Config.cpp main/Gui.cpp main/Settings.cpp main/Trajectory.cpp MainOTLD.cpp)

ADD_EXECUTABLE (MainOTLD ${HEADERS} ${SOURCES} MainOTLD.cpp)

#Include opencv
FIND_PACKAGE( OpenCV REQUIRED )
IF(OpenCV_FOUND)
  INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
  TARGET_LINK_LIBRARIES( MainOTLD ${OpenCV_LIBS} )
ENDIF()

#-------------------------------------------------------------------------------
# main
INCLUDE_DIRECTORIES(main
    ../libopentld/imacq
    ../libopentld/mftracker
    ../libopentld/tld
    ../3rdparty/cvblobs)

if(NOT USE_SYSTEM_LIBS)
    include_directories(../3rdparty/libconfig)
endif(NOT USE_SYSTEM_LIBS)
ADD_DEFINITIONS(-fPIC) 
add_library(main
    main/Config.cpp
    main/Gui.cpp
    main/Main.cpp
    main/Settings.cpp
	main/Trajectory.cpp
    main/Config.h
    main/Gui.h
    main/Main.h
    main/Settings.h
	main/Trajectory.h)
ADD_DEFINITIONS(-fPIC)
add_library(libopentld
    ../libopentld/imacq/ImAcq.cpp
    ../libopentld/mftracker/BB.cpp
    ../libopentld/mftracker/BBPredict.cpp
    ../libopentld/mftracker/FBTrack.cpp
   ../libopentld/mftracker/Lk.cpp
   ../libopentld/mftracker/Median.cpp
    ../libopentld/tld/Clustering.cpp
    ../libopentld/tld/DetectionResult.cpp
    ../libopentld/tld/DetectorCascade.cpp
    ../libopentld/tld/EnsembleClassifier.cpp
    ../libopentld/tld/ForegroundDetector.cpp
    ../libopentld/tld/MedianFlowTracker.cpp
    ../libopentld/tld/NNClassifier.cpp
    ../libopentld/tld/TLD.cpp
    ../libopentld/tld/TLDUtil.cpp
    ../libopentld/tld/VarianceFilter.cpp
    ../libopentld/imacq/ImAcq.h
    ../libopentld/mftracker/BB.h
    ../libopentld/mftracker/BBPredict.h
    ../libopentld/mftracker/FBTrack.h
    ../libopentld/mftracker/Lk.h
    ../libopentld/mftracker/Median.h
    ../libopentld/tld/Clustering.h
    ../libopentld/tld/DetectionResult.h
    ../libopentld/tld/DetectorCascade.h
    ../libopentld/tld/EnsembleClassifier.h
    ../libopentld/tld/ForegroundDetector.h
    ../libopentld/tld/IntegralImage.h
    ../libopentld/tld/MedianFlowTracker.h
    ../libopentld/tld/NNClassifier.h
    ../libopentld/tld/NormalizedPatch.h
    ../libopentld/tld/TLD.h
    ../libopentld/tld/TLDUtil.h
    ../libopentld/tld/VarianceFilter.h)
ADD_DEFINITIONS(-fPIC)
add_library(cvblobs
    ../3rdparty/cvblobs/blob.cpp
    ../3rdparty/cvblobs/BlobContour.cpp
    ../3rdparty/cvblobs/BlobOperators.cpp
    ../3rdparty/cvblobs/BlobProperties.cpp
    ../3rdparty/cvblobs/BlobResult.cpp
    ../3rdparty/cvblobs/ComponentLabeling.cpp
    ../3rdparty/cvblobs/blob.h
    ../3rdparty/cvblobs/BlobContour.h
    ../3rdparty/cvblobs/BlobLibraryConfiguration.h
    ../3rdparty/cvblobs/BlobOperators.h
    ../3rdparty/cvblobs/BlobProperties.h
    ../3rdparty/cvblobs/BlobResult.h
    ../3rdparty/cvblobs/ComponentLabeling.h)




target_link_libraries(main libopentld cvblobs config++ ${OpenCV_LIBS})


TARGET_LINK_LIBRARIES(MainOTLD  main libopentld cvblobs config++ )

FIND_PACKAGE(OpenMP)
IF(OPENMP_FOUND)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_C_FLAGS}")
ENDIF()


#-------------------------------------------

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs REQUIRED)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(MainOTLD.i PROPERTIES CPLUSPLUS ON)
SWIG_ADD_MODULE(MainOTLD python MainOTLD.i ${SOURCES} )# PedestrianDetector_wrap.cxx)
SWIG_LINK_LIBRARIES(MainOTLD main libopentld cvblobs config++ ${OpenCV_LIBS} ${PYTHON_LIBRARIES}  )


