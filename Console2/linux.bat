@echo off
rem linux cmd
doskey ls=dir /w $*
doskey ll=dir /S $*
doskey la=dir /a $*
doskey pwd=cd $*
doskey wc=wc $*
doskey mkdir=md $*
doskey rmdir=rd $*
doskey rmdirf=rmd.bat $*
doskey rm=del /q $*
doskey rma=del /s/q $*
doskey mv=move $*
doskey cat=type $*
doskey more=more *
doskey less=more *
doskey clear=cls
doskey clr=color $*
rem doskey uname=%yqycmd% ver
doskey uname=wininfo.bat $*
doskey vi=gvim $*
doskey vim=gvim $*
doskey edit=gvim $*
doskey man=$* /?
doskey find=dir /s $*
doskey grep=findstr /I /N /A:4 $1 $* *
doskey grepa=findstr /I /N /S /A:4 $1 $* *
doskey cp=cp.bat $*
doskey ps=ps.bat
doskey top=tasklist
doskey kill=kill.bat $*
doskey killall= killall.bat $*
doskey free=mem
doskey whoami=whoami.bat
doskey daemon=bootstrap.bat
doskey myctags=ctags -R --c++-kinds=+p --fields=+iaS --extra=+q --language-force=C++ ./*
rem crt file
doskey touch=copy con $*
doskey ifconfig=ipconfig /all
doskey /=cd \
doskey ..=cd ..
doskey ...=cd ..\..
doskey ....=cd ..\..\..
doskey c=c:
doskey d=d:
doskey e=e:
doskey f=f:
doskey g=cd $*
doskey j=j:
doskey z=z:
doskey a=start shell:RecycleBinFolder
rem showcut defined for windows
doskey open=explorer $*
doskey fs=fullscreen.bat
doskey fsxp=fullscreen_xp.bat
doskey font=font.bat
doskey black=color 0f
doskey blue=color 10
doskey green=color 20
doskey lake=color 30
doskey red=color 40
doskey purple=color 50
doskey yellow=color 60
doskey white=color 70
doskey grey=color 80
doskey lblue=color 90
doskey lgreen=color a0
doskey llake=color b0
doskey lred=color c0
doskey lpurple=color d0
doskey lyellow=color e0
doskey lwhite=color 7e
doskey dos=color 1f
doskey eye=color 60
doskey tradition=color 0f
doskey matrix=color a0
doskey row=mode con lines=$1
doskey col=mode con cols=$1
doskey window=window.bat
doskey colors=colors.bat
doskey desktop=c:;cd %userprofile%\desktop
doskey halt=shutdown -s -f -t 0
doskey restart=shutdown -r -f -t 0
doskey logout=shutdown -l
doskey q=exit
doskey disks=fsutil fsinfo drives
doskey lsd=listdisk.bat
doskey svrinfo=svrinfo.bat
doskey cs=check_sys.bat
doskey ie="C:\Program Files\Internet Explorer\iexplore.exe" $*
doskey cmp="D:\Program Files\WinMerge-2.13.20-exe\WinMergeU.exe" $*
doskey sw=start www.baidu.com/s?wd=$*
doskey sb="C:\Program Files\Sublime Text3\sublime_text.exe" $*

rem set local variable
set sec=%USERPROFILE%\\share\\securecrt

title linux simulator By: Alexander Stocks
SETLOCAL ENABLEEXTENSIONS&SETLOCAL ENABLEDELAYEDEXPANSION
cls
echo set dos command as linux style over now.
