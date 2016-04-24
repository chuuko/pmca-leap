/*PMD Mob Character Assembler
PMDモブキャラクタアセンブラ
ヘッダファイル
*/


#define MODEL_MAX 3	//0:モデル 1:組み立て用一時モデル 2:表示用モデル
#define PARTS_MAX 256
#define TEXTURES_MAX 64
#define MATERIALS_MAX 32
#define BONE_GROUP_MAX 64
#define NODE_MAX 32
#define CHILD_MAX 8
#define COL_LST_MAX 32
#define SEL_LST_MAX 64
#define THUMBNAIL_SIZE 184
#define VERSION_NO "v0.0.5r12"

#define UNSIGSHORT_MAX 65535

//#define DEBUG


#define BTN_MAX 16
#define SLD_MAX 4

#define MENU_BTN_MAX 3

typedef struct{	/*1616byte*/
	char name[64];
	char comment[256];
	char type[32];
	char file[256];
	char join[CHILD_MAX][8][16];
	char pic[96];
	void *ext;
} PARTS;

typedef struct{	/*1125byte*/
	char name[21];
	char comment[256];
	char tex[16][21];
	char tex_comment[16][64];
	char pic[16][96];
	void *ext;
} TEXTURES;

typedef struct{
	char tex[21];
	char comment[64];
	float dif[3];
	float spec[3];
	float mir[3];
	int toon_num;
	char toon_name[100];
	char pic[96];
} MAT_CONF;

typedef struct{
	char name[21];
	char comment[256];
	MAT_CONF mat[16];
	void *ext;
} MATERIALS;

typedef struct{	/*384byte*/
	char name[64];
	char bone_name[16][20];
} BONE_GROUP;


#ifndef PMCA_PLUGIN

//loadconf
int load_config(char str[],
	PARTS parts[],
	unsigned short *parts_max,
	TEXTURES textures[],
	unsigned short *textures_max,
	MATERIALS materials[],
	unsigned short *materials_max,
	BONE_GROUP bone_group[],
	unsigned short *bone_group_max
	);
char* read_line(char *in, char *out);

#endif
