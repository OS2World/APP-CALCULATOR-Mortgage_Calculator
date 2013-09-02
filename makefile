# makefile
# Created by IBM WorkFrame/2 MakeMake at 19:29:41 on 5 Sept 1997
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Link::Linker

.SUFFIXES: .C .obj 

.all: \
    .\Mortgage.exe

.C.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ti /G5 /C %s

{G:\Programming\Programs\C\Mortgage}.C.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ti /G5 /C %s

.\Mortgage.exe: \
    .\Mortgage.obj
    @echo " Link::Linker "
    icc.exe @<<
     /B" /de"
     /FeMortgage.exe 
     .\Mortgage.obj
<<

.\Mortgage.obj: \
    G:\Programming\Programs\C\Mortgage\Mortgage.C \
    {G:\Programming\Programs\C\Mortgage;$(INCLUDE);}Mortgage.H
