cl /c game.c && link /NODEFAULTLIB /ENTRY:WinMain /SUBSYSTEM:WINDOWS game.obj ucrt.lib user32.lib kernel32.lib LIBCMT.lib gdi32.lib
game