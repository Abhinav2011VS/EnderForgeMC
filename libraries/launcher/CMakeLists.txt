cmake_minimum_required(VERSION 3.9.4)
project(launcher Java)
find_package(Java 1.8 REQUIRED COMPONENTS Development)

include(UseJava)
set(CMAKE_JAVA_JAR_ENTRY_POINT org.enderforgemc.EntryPoint)
set(CMAKE_JAVA_COMPILE_FLAGS -target 8 -source 8 -Xlint:deprecation -Xlint:unchecked)

set(SRC
    org/enderforgemc/EntryPoint.java
    org/enderforgemc/Launcher.java
    org/enderforgemc/LauncherFactory.java
    org/enderforgemc/impl/OneSixLauncher.java
    org/enderforgemc/applet/LegacyFrame.java
    org/enderforgemc/exception/ParameterNotFoundException.java
    org/enderforgemc/exception/ParseException.java
    org/enderforgemc/utils/Parameters.java
    org/enderforgemc/utils/Utils.java
    net/minecraft/Launcher.java
)
add_jar(NewLaunch ${SRC})
install_jar(NewLaunch "${JARS_DEST_DIR}")
