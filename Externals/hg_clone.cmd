set urls_revs=^
  https://bitbucket.org/sdottaka/freeimage!05e4e65 ^
  https://bitbucket.org/sdottaka/winimerge!1.0.7.0 ^
  https://bitbucket.org/sdottaka/sevenzip!1510f74 ^
  https://bitbucket.org/sdottaka/frhed!d00b64b ^
  https://bitbucket.org/sdottaka/patch!b0a7238

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
