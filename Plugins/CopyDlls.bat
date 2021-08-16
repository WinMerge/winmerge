for /d %%d in (src_VCPP\*) do (
  copy %%d\Release\*.dll dlls\ 
  copy %%d\X64\Release\*.dll dlls\X64\
  copy %%d\ARM64\Release\*.dll dlls\ARM64\
)

pause