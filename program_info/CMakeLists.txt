if(UNIX)
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_search_module(SCDOC scdoc)
        if(SCDOC_FOUND)
            pkg_get_variable(SCDOC_SCDOC scdoc scdoc)
        endif()
    endif()
endif()

set(Launcher_CommonName "EnderForgeMC")

set(Launcher_Copyright "EnderForgeMC Contributors\\n© 2012-2021 EnderForgeMC Contributors")
set(Launcher_Copyright "${Launcher_Copyright}" PARENT_SCOPE)
set(Launcher_Domain "enderforge.pages.dev" PARENT_SCOPE)
set(Launcher_Name "${Launcher_CommonName}" PARENT_SCOPE)
set(Launcher_DisplayName "${Launcher_CommonName}" PARENT_SCOPE)
set(Launcher_UserAgent "${Launcher_CommonName}/${Launcher_VERSION_NAME}" PARENT_SCOPE)
set(Launcher_ConfigFile "enderforgemc.cfg" PARENT_SCOPE)
set(Launcher_Git "https://github.com/AbhinavVS/EnderForgeMC" PARENT_SCOPE)
set(Launcher_DesktopFileName "org.enderforgemc.EnderForgeMC.desktop" PARENT_SCOPE)

set(Launcher_Desktop "program_info/org.enderforgemc.EnderForgeMC.desktop" PARENT_SCOPE)
set(Launcher_MetaInfo "program_info/org.enderforgemc.EnderForgeMC.metainfo.xml" PARENT_SCOPE)
set(Launcher_SVG "program_info/org.enderforgemc.EnderForgeMC.svg" PARENT_SCOPE)
set(Launcher_Branding_ICNS "program_info/enderforgemc.icns" PARENT_SCOPE)
set(Launcher_Branding_ICO "program_info/enderforgemc.ico")
set(Launcher_Branding_ICO "${Launcher_Branding_ICO}" PARENT_SCOPE)
set(Launcher_Branding_WindowsRC "program_info/enderforgemc.rc" PARENT_SCOPE)
set(Launcher_Branding_LogoQRC "program_info/enderforgemc.qrc" PARENT_SCOPE)

set(Launcher_Portable_File "program_info/portable.txt" PARENT_SCOPE)

configure_file(org.enderforgemc.EnderForgeMC.desktop.in org.enderforgemc.EnderForgeMC.desktop)
configure_file(org.enderforgemc.EnderForgeMC.metainfo.xml.in org.enderforgemc.EnderForgeMC.metainfo.xml)
configure_file(enderforgemc.rc.in enderforgemc.rc @ONLY)
configure_file(enderforgemc.manifest.in enderforgemc.manifest @ONLY)
configure_file(enderforgemc.ico enderforgemc.ico COPYONLY)
configure_file(win_install.nsi.in win_install.nsi @ONLY)

if(SCDOC_FOUND)
    set(in_scd "${CMAKE_CURRENT_SOURCE_DIR}/enderforgemc.6.scd")
    set(out_man "${CMAKE_CURRENT_BINARY_DIR}/enderforgemc.6")
    add_custom_command(
        DEPENDS "${in_scd}"
        OUTPUT "${out_man}"
        COMMAND ${SCDOC_SCDOC} < "${in_scd}" > "${out_man}"
    )
    add_custom_target(man ALL DEPENDS ${out_man})
    set(Launcher_ManPage "program_info/enderforgemc.6" PARENT_SCOPE)
endif()
