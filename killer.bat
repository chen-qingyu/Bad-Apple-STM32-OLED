%官方给的%
del *.bak /s
del *.ddk /s
del *.edk /s
del *.lst /s
del *.lnp /s
del *.mpf /s
del *.mpj /s
del *.obj /s
del *.omf /s
del *.opt /s
del *.plg /s
del *.rpt /s
del *.tmp /s
del *.__i /s
del *.crf /s
del *.o /s
del *.d /s
del *.axf /s
del *.tra /s
del *.dep /s           
del *.iex /s
del *.htm /s
del *.sct /s
del *.map /s
del JLinkLog.txt /s

%自己加的%
del *.ini /s
del *.dbgconf /s
del *.uvguix.* /s
del *.uvgui.* /s
del *.m51 /s

%删除空文件夹%
for /f "tokens=*" %%i in ('dir/s/b/ad^|sort /r') do rd "%%i"

exit
