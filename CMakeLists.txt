project(LogSync)

#file(GLOB_RECURSE sources src/*/*.cpp src/*/*.h)
#
#add_executable(main )

add_executable( main
        src/controlled_logical_clock/clc.hpp
        src/controlled_logical_clock/clc.cpp

        src/iterative_linear_least_squares/ills.hpp
        src/iterative_linear_least_squares/ills.cpp

        src/logical_clock_with_direct_dependency/lcwdd.hpp
        src/logical_clock_with_direct_dependency/lcwdd.cpp

        src/math/misc.hpp
        src/math/misc.cpp

        src/main.cpp
        src/ristenpart/stat_synchronize.hpp src/ristenpart/stat_synchronize.cpp)