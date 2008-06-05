project (yavipin)

# jme- get all the file with the .cxx file extensions
FILE(GLOB_RECURSE ALL_CPP *.cpp)


FOREACH(filecpp ${ALL_CPP})
	IF(${filecpp} MATCHES utest.cpp)
		SET(ALL_UTEST_CPP ${ALL_UTEST_CPP} ${filecpp})
	ELSE(${filecpp} MATCHES utest.cpp)
		IF(${filecpp} MATCHES wikidbg)
			SET(ALL_WIKIDBG_CPP ${ALL_WIKIDBG_CPP} ${filecpp})
		ELSE(${filecpp} MATCHES wikidbg)
			SET(ALL_NORMAL_CPP ${ALL_NORMAL_CPP} ${filecpp})
		ENDIF(${filecpp} MATCHES wikidbg)
	ENDIF(${filecpp} MATCHES utest.cpp)
ENDFOREACH(filecpp)

FOREACH(filecpp ${ALL_CPP})
ENDFOREACH(filecpp)


MESSAGE(STATUS "all_normal_cpp=${ALL_NORMAL_CPP}")
MESSAGE(STATUS "all_utest_cpp=${ALL_UTEST_CPP}")
MESSAGE(STATUS "all_wikidbg_cpp=${ALL_WIKIDBG_CPP}")

#ADD_LIBRARY(neoip_httpd ${NEOIP_HTTPD_SRCS})