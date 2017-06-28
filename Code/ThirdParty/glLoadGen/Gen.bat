lua-5.1.exe glLoadGen_2_0_5/LoadGen.lua -style=pointer_c -spec=gl -version=4.5 -profile=compatibility -extfile=extensions Load
md .\GL
move .\gl_Load.* .\GL\
timeout 5