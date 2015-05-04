@echo off

set project_path=%1
set raw_path=%project_path%assets\textures\raw\
set out_path=%project_path%assets\textures\
set script_path=%project_path%assets\scripts\
shift

for /f %%f in ('dir /b %raw_path%') do (call :process_texture %%~nf)
goto :eof

:process_texture
	if not exist %out_path%%1.dds (
		%script_path%pngquant --output %raw_path%%1-temp.png %raw_path%%1.png
		%script_path%optipng -o 3 -out %raw_path%%1-temp.png %raw_path%%1-temp.png
		%script_path%pvrtextool -f BC3 -o %out_path%%1.dds -i %raw_path%%1-temp.png
		del %raw_path%%1-temp.png
	)