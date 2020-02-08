#生成 exe 就 "EXE"，生成 lib 就 "LIB"
set(mode "LIB")

Ubpa_GetDirName(DIRNAME)
set(FOLDER_NAME ${DIRNAME})

Ubpa_QtBegin()

Ubpa_AddSubDirs()

Ubpa_QtEnd()
