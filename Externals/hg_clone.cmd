set urls_revs=^
  https://bitbucket.org/winmerge/freeimage!aa9fd20 ^
  https://bitbucket.org/winmerge/winimerge!1.0.9.0 ^
  https://bitbucket.org/winmerge/sevenzip!e3aff09 ^
  https://bitbucket.org/winmerge/frhed!7f942c2 ^
  https://bitbucket.org/winmerge/patch!b0a7238

pushd "%~dp0"
for %%i in (%urls_revs%) do (
  for /F "tokens=1,2 delims=!" %%j in ("%%i") do (
    if not exist %%~nj\.hg hg clone %%j
    pushd %%~nj
    hg pull -u -r %%k
    popd
  )
)
popd
