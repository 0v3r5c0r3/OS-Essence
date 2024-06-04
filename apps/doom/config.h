#ifndef __CONFIG__
#define __CONFIG__

/* Define to the full name of this package. */
#define PACKAGE_NAME "Doom Essence"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Doom Essence 0.1"

/* Change this when you create your awesome forked version */
#define PROGRAM_PREFIX "doomessence"

/* Enables multiplayer support (network games) */
#undef FEATURE_MULTIPLAYER

/* Enables sound output */
#undef FEATURE_SOUND

/* Enabled console debug */
#undef FEATURE_CONSOLE

/* Define to the directory where all game files are located */
#define FILES_DIR "|Settings:/WAD"

/* Define temporary directory path */
#define TMP_DIR "|Settings:/TMP"

/* Define screen resolution */
#define RES_X 640
#define RES_Y 400

/* Define game configs */
#define INITIAL_HEALTH 100
#define INITIAL_BULLETS 50
#define MAX_HEALTH 200
#define MAX_ARMOR 200
#define GREEN_ARMOR_CLASS 1
#define BLUE_ARMOR_CLASS 2
#define MAX_SOULSPHERE 200
#define SOULSPHERE_HEALTH 100
#define MEGASPHERE_HEALTH 200
#define GOD_MODE_HEALTH 100
#define IDFA_ARMOR 200
#define IDFA_ARMOR_CLASS 2
#define IDKFA_ARMOR 200
#define IDKFA_ARMOR_CLASS 2
#define BFG_CELLS_PER_SHOT 40
#define SPECIES_INFIGHTING 0

#endif
