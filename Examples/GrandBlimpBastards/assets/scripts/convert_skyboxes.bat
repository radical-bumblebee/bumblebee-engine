@echo off

set raw_path=..\textures\raw\
set out_path=..\textures\

for /f %%f in ('dir /b %raw_path%') do (call :process_texture %%~nf)
goto :eof

:process_texture
	if not exist %out_path%%1.dds (
		convert %raw_path%%1.png %out_path%%1.dds
	)