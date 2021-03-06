REM -----------------------------------------------------------------------------
REM driver batch file for MAF building
REM -----------------------------------------------------------------------------

IF "%1" == "MAFMED_DEBUG" GOTO MAFMED_DEBUG
IF "%1" == "MAFMED_DEBUG_2010" GOTO MAFMED_DEBUG_2010
IF "%1" == "VAPP_DEBUG" GOTO VAPP_DEBUG
IF "%1" == "VAPP_RELEASE" GOTO VAPP_RELEASE
IF "%1" == "MAFMED_DEBUG_2005" GOTO MAFMED_DEBUG_2005
IF "%1" == "VAPP_DEBUG_2005" GOTO VAPP_DEBUG_2005
IF "%1" == "VAPP_RELEASE_2005" GOTO VAPP_RELEASE_2005
IF "%1" == "MAFMED_DEBUG_2008" GOTO MAFMED_DEBUG_2008
IF "%1" == "VAPP_DEBUG_2008" GOTO VAPP_DEBUG_2008
IF "%1" == "VAPP_RELEASE_2008" GOTO VAPP_RELEASE_2008
IF "%1" == "VAPP_RELEASE_2010" GOTO VAPP_RELEASE_2010
IF NOT "%1" == "MAF_DEBUG" GOTO UNKNOWN_CONDITION

:MAFMED_DEBUG
CALL "%PROGRAMFILES%/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:MAFMED_DEBUG_2010
CALL "%PROGRAMFILES%/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcxproj /build debug /out build_log.txt
GOTO END

:VAPP_DEBUG
CALL "%PROGRAMFILES%/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:VAPP_RELEASE
CALL "%PROGRAMFILES%/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build release /out build_log.txt
GOTO END

:MAFMED_DEBUG_2005
CALL "%PROGRAMFILES%/Microsoft Visual Studio 8/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:VAPP_DEBUG_2005
CALL "%PROGRAMFILES%/Microsoft Visual Studio 8/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:VAPP_RELEASE_2005
CALL "%PROGRAMFILES%/Microsoft Visual Studio 8/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build release /out build_log.txt
GOTO END

:MAFMED_DEBUG_2008
CALL "%PROGRAMFILES%/Microsoft Visual Studio 9.0/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:VAPP_DEBUG_2008
CALL "%PROGRAMFILES%/Microsoft Visual Studio 9.0/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:VAPP_RELEASE_2008
CALL "%PROGRAMFILES%/Microsoft Visual Studio 9.0/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build release /out build_log.txt
GOTO END

:VAPP_RELEASE_2010
CALL "%PROGRAMFILES%/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat"
cd Medical_Parabuild
devenv MED.sln /project ALL_BUILD.vcxproj /build release /out build_log.txt
cd ..
GOTO END

:UNKNOWN_CONDITION

:END
