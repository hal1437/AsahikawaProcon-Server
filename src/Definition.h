#ifndef DEFINITION_H
#define DEFINITION_H

//Windows
#ifdef OS_WIN32
#define MUSIC_DIRECTORY qApp->applicationDirPath()
#endif

//MacOSX
#ifdef OS_MACOSX
#define MUSIC_DIRECTORY QString(":/Music")
#endif


#endif // DEFINITION_H

