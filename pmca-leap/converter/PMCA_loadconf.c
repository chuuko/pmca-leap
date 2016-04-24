/*PMD Mob Character Assembler
Version 0.0.5 by mato
PMDモブキャラクタアセンブラ
*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "mPMD.h"
#include "PMCA.h"

int load_parts_list(PARTS parts[], unsigned short *parts_max, FILE *list);
int load_textures_list(TEXTURES textures[], unsigned short *textures_max, FILE *list);
int load_materials_list(MATERIALS materials[], unsigned short *materials_max, FILE *list);
int load_bone_group_list(BONE_GROUP bone_group[], unsigned short *bone_group_max, FILE *list);
int load_rc();

int load_config(char str[], 
	PARTS parts[],
	unsigned short *parts_max,
	TEXTURES textures[],
	unsigned short *textures_max,
	MATERIALS materials[],
	unsigned short *materials_max,
	BONE_GROUP bone_group[],
	unsigned short *bone_group_max
	)
{
	
	FILE *list;
	
	//load_rc();
	
	list = fopen(str, "r");
	if(list != NULL){
		load_parts_list(parts, parts_max, list);
		fseek(list, 0, SEEK_SET);
		
		load_materials_list(materials, materials_max, list);
		fseek(list, 0, SEEK_SET);
		
		load_textures_list(textures, textures_max, list);
		fseek(list, 0, SEEK_SET);
		
		load_bone_group_list(bone_group, bone_group_max, list);
		fseek(list, 0, SEEK_SET);
		
		fclose(list);
	}else{
		puts("ファイルが開けませんでした");
	}
	
	return 0;
}

int load_parts_list(PARTS parts[], unsigned short *parts_max, FILE *list)
{
	int i, j;
	char str[256], *p;
	
	int max;
	
	fgets(str, 256, list);
	p = strchr( str, '\n' );
	if(p != NULL)*p = '\0';
	#ifdef DEBUG
		printf("%s\n", str);
	#endif
	max = *parts_max;
	
	{
		char *p;
		p = str;
		while(*p!='\0'){
			printf("%02x ", *p);
			p++;
		}
		puts("");
	}
	
	{
		char str[256], *p;
		strcpy(str,"PMCA Parts list v1.0");
		p = str;
		while(*p!='\0'){
			printf("%02x ", *p);
			p++;
		}
		puts("");
	}
	
	if(strcmp(str, "PMCA Parts list v1.0") != 0)return max;	//ファイルが指定の文字列で始まっているか
	
	while(fgets(str, 256, list) != NULL){
		p = strchr( str, '\n' );
		if(p != NULL)*p = '\0';
		p = strchr( str, 0x0d );
	if(p != NULL)*p = '\0';
		#ifdef DEBUG
			printf("%s\n", str);
		#endif
		
		if(strcmp(str, "") != 0){
			sscanf(str, "%s %s", parts[max].name, parts[max].comment);
			
			#ifdef DEBUG
				printf("name:%s\ncomment:%s\n", parts[max].name, parts[max].comment);
			#endif
			
			fgets(parts[max].type, 16, list);
			p = strchr(parts[max].type, '\n' );
			if(p != NULL)*p = '\0';
			p = strchr(parts[max].type, 0x0d );
			if(p != NULL)*p = '\0';
			
			#ifdef DEBUG
				printf("type:%s\n", parts[max].type);
			#endif
			
			fgets(parts[max].file, 256, list);
			p = strchr(parts[max].file, '\n' );
			if(p != NULL)*p = '\0';
			p = strchr(parts[max].file, 0x0d );
			if(p != NULL)*p = '\0';
			
			#ifdef DEBUG
				printf("file:%s\n", parts[max].file);
			#endif
			
			fgets(parts[max].pic, 96, list);
			p = strchr(parts[max].pic, '\n' );
			if(p != NULL)*p = '\0';
			p = strchr(parts[max].pic, 0x0d );
			if(p != NULL)*p = '\0';
			
			memset(parts[max].join,'\0', 1024);
			
			for(i=0; i<8; i++){
				if(fgets(str, 256, list) == NULL)break;
				if(strcmp(str, "\n") == 0 || strcmp(str, "") == 0)break;
				p = str;
				for(j=0; j<8; j++){
					p = read_line(p, parts[max].join[i][j]);
					if(p == NULL)break;
				}
			}
			/*
			#ifdef DEBUG
				printf("%d\n", max);
				printf("%s %s\n", parts[max].name, parts[max].comment);
				printf("%s\n", parts[max].type);
				printf("%s\n\n", parts[max].file);
			#endif
			*/
			max++;
			
		}
	}
	
	/*
	パーツリスト書式
	PMCA Parts list v1.0	#ファイル先頭に
	
	#空白行
	
	パーツ名(char[64]) コメント(char[256])
	タイプ(char[16])
	ファイル名
	ピクチャ名
	接合可能タイプ(char[8][8][16])
	
	例
	PMCA Parts list v1.0
	
	セーラー服上半身01(スカート,長袖) 普通の長袖セーラー服、縦リボン
	root
	ub_slr001_l_sk.pmd
	ub_slr001_l_sk.bmp
	head
	hand
	lb
	
	顔f01 今までセーラー服さんと同じ様な顔
	head
	head_mato001_1.pmd
	head_mato001_1.bmp
	hair_f hair_r
	
	EOF
	*/
	
	printf("parts_count = %d\n", max);
	
	*parts_max = max;
	return 0;
}

int load_textures_list(TEXTURES textures[], unsigned short *textures_max, FILE *list)
{
	int i, tmp;
	char str[256], *p;
	
	int max;
	
	fgets(str, 256, list);
	p = strchr( str, '\n' );
	if(p != NULL)*p = '\0';
	p = strchr( str, 0x0d );
	if(p != NULL)*p = '\0';
	#ifdef DEBUG
		printf("%s\n", str);
	#endif
	if(strcmp(str, "PMCA Textures list v1.0") != 0)return 1;	//ファイルが指定の文字列で始まっているか
	
	max = *textures_max;
	
	while(fgets(str, 256, list) != NULL){
		p = strchr( str, '\n' );
		if(p != NULL)*p = '\0';
		p = strchr( str, 0x0d );
		if(p != NULL)*p = '\0';
		#ifdef DEBUG
			printf("%s\n", str);
		#endif
		
		if(strcmp(str, "texture") == 0){
			int index;
			fgets(str, 256, list);
			p = strchr(str, '\n' );
			if(p != NULL)*p = '\0';
			p = strchr(str, 0x0d );
			if(p != NULL)*p = '\0';
			sscanf(str, "%s %s", textures[max].name, textures[max].comment);
			#ifdef DEBUG
				printf("name:%s\ncomment:%s\n", textures[max].name, textures[max].comment);
			#endif
			
			
			//多重定義のチェック
			tmp = 0;
			for(i=0; i<max; i++){
				if(strcmp(textures[max].name, textures[i].name) == 0){
					tmp = 1;
					break;
				}
			}
			if(tmp == 0){
				index = max;
				max++;
				memset(textures[index].tex,'\0', 336);
				memset(textures[index].tex_comment,'\0', 1024);
			}else{
				index = i;
			}
			
			for(i=0; i<16; i++){
				if(textures[index].tex[i][0] == 0)break;
			}
			for(; i<16; i++){
				if(fgets(str, 256, list) == NULL)break;
				if(strcmp(str, "\n") == 0 || strcmp(str, "") == 0)break;
				p = strchr(str, '\n' );
				if(p != NULL)*p = '\0';
				p = strchr(str, 0x0d );
				if(p != NULL)*p = '\0';
				p = read_line(str, textures[index].tex[i]);
				p = read_line(p, textures[index].tex_comment[i]);
				read_line(p, textures[index].pic[i]);
				//sscanf(str, "%s %s %s", textures[index].tex[i], textures[index].tex_comment[i], textures[index].pic[i]);
			}
			/*
			#ifdef DEBUG
				printf("%d\n", max);
				printf("%s %s\n", parts[index].name, parts[index].comment);
				printf("%s\n", parts[index].type);
				printf("%s\n\n", parts[index].file);
			#endif
			*/
		}
	}
	
	/*
	テクスチャリスト書式
	PMCA Textures list v1.0	#ファイル先頭に
	
	#空白行
	
	Texture
	テクスチャ置換名(char[21]) コメント(char[256])
	テクスチャ名(char[16][21]) テクスチャコメント(char[16][32]) ピクチャ名(char[16][96])
	
	例
	PMCA Textures list v1.0
	
	texture
	cloth_slr1_l 長袖服
	cl_slr1_l_bl.png 紺 cl_slr1_l_bl.bmp
	cl_slr1_l_bk.png 黒 cl_slr1_l_bk.bmp
	
	texture
	cloth_bzr1_l ブレザー
	cl_bzr1_l_bk_b.png 青ネクタイ cl_bzr1_l_bk_b.bmp
	cl_bzr1_l_bk_r.png 赤ネクタイ cl_bzr1_l_bk_r.bmp
	
	EOF
	*/
	printf("parts_count = %d", max);
	*textures_max = max;
	return 0;
}

int load_materials_list(MATERIALS materials[], unsigned short *materials_max, FILE *list)
{
	int i, tmp;
	char *ret;
	char str[256], *p;
	
	int max;
	
	fgets(str, 256, list);
	p = strchr( str, '\n' );
	if(p != NULL)*p = '\0';
	p = strchr(str, 0x0d );
	if(p != NULL)*p = '\0';
	#ifdef DEBUG
		printf("%s\n", str);
	#endif
	if(strcmp(str, "PMCA Materials list v1.0") != 0
	&& strcmp(str, "PMCA Materials list v1.1") != 0)return 1;	//ファイルが指定の文字列で始まっているか
	
	max = *materials_max;
	
	ret = fgets(str, 256, list);
	p = strchr( str, '\n' );
	if(p != NULL)*p = '\0';
	p = strchr(str, 0x0d );
	if(p != NULL)*p = '\0';
	#ifdef DEBUG
		printf("%s\n", str);
	#endif
	
	while(ret != NULL){
		if(strcmp(str, "material") == 0){
			int index;
			if(fgets(str, 256, list) == NULL)break;
			p = strchr(str, '\n' );
			if(p != NULL)*p = '\0';
			p = strchr( str, 0x0d );
			if(p != NULL)*p = '\0';
			printf(">%s", str);
			sscanf(str, "%s %s", materials[max].name, materials[max].comment);
			#ifdef DEBUG
				printf("\n%d name:%s\ncomment:%s\n",  max,materials[max].name, materials[max].comment);
			#endif
			//多重定義のチェック
			tmp = 0;
			for(i=0; i<max; i++){
				if(strcmp(materials[max].name, materials[i].name) == 0){
					tmp = 1;
					break;
				}
			}
			if(tmp == 0){
				index = max;
				max++;
			}else{
				index = i;
			}
			
			for(i=0; i<16; i++){
				if(materials[index].mat[i].tex[0] == 0)break;
			}
			for(; i<16; i++){
				tmp = 0;
				materials[index].mat[i].tex[0] = '\0';
				do{
					if(fgets(str, 256, list) == NULL){
						ret = NULL;
						break;
					}
				}while(strcmp(str, "\n") == 0);
				if(ret == NULL)break;
				p = strchr(str, '\n' );
				if(p != NULL)*p = '\0';
				p = strchr( str, 0x0d );
				if(p != NULL)*p = '\0';
				if(strcmp(str, "material") == 0)break;
				p = read_line(str, materials[index].mat[i].tex);
				puts(p);
				p = read_line(p, materials[index].mat[i].comment);
				
				materials[index].mat[i].toon_num = -1;
				
				while(p != NULL){
					char tmp_str[128];
					p = read_line(p, tmp_str);
					if(strcmp(tmp_str, "toon") == 0){
						p = read_line(p, tmp_str);
						sscanf(tmp_str, ("%d"), &materials[index].mat[i].toon_num);
						p = read_line(p, materials[index].mat[i].toon_name);
					}else{
						strcpy(materials[index].mat[i].pic, tmp_str);
					}
				}
				//sscanf(str, "%s %s", materials[index].mat[i].tex, materials[index].mat[i].comment, materials[index].mat[i].pic);
				
				if(fgets(str, 256, list) == NULL)break;
				p = strchr(str, '\n' );
				if(p != NULL)*p = '\0';
				p = strchr( str, 0x0d );
				if(p != NULL)*p = '\0';
				if(strcmp(str, "") == 0 || strcmp(str, "material") == 0)break;
				sscanf(str, "%f %f %f", &materials[index].mat[i].dif[0], &materials[index].mat[i].dif[1], &materials[index].mat[i].dif[2]);
				
				if(fgets(str, 256, list) == NULL)break;
				p = strchr(str, '\n' );
				if(p != NULL)*p = '\0';
				p = strchr( str, 0x0d );
				if(p != NULL)*p = '\0';
				if(strcmp(str, "") == 0 || strcmp(str, "material") == 0)break;
				sscanf(str, "%f %f %f", &materials[index].mat[i].spec[0], &materials[index].mat[i].spec[1], &materials[index].mat[i].spec[2]);
				
				if(fgets(str, 256, list) == NULL)break;
				p = strchr(str, '\n' );
				if(p != NULL)*p = '\0';
				p = strchr( str, 0x0d );
				if(p != NULL)*p = '\0';
				if(strcmp(str, "") == 0 || strcmp(str, "material") == 0)break;
				sscanf(str, "%f %f %f", &materials[index].mat[i].mir[0], &materials[index].mat[i].mir[1], &materials[index].mat[i].mir[2]);
				
				#ifdef DEBUG
					printf("   %d %s %s\n", i, materials[index].mat[i].tex, materials[index].mat[i].comment);
				#endif
			}
			
		}else{
			ret = fgets(str, 256, list);
			p = strchr( str, '\n' );
			if(p != NULL)*p = '\0';
			p = strchr( str, 0x0d );
			if(p != NULL)*p = '\0';
			#ifdef DEBUG
				printf("%s\n", str);
			#endif
		}
	}
	
	/*
	マテリアルリスト書式
	PMCA Materials list v1.0	#ファイル先頭に
	
	#空白行
	
	material
	テクスチャ置換名(char[21]) コメント(char[256])
	struct[16]
		テクスチャ名(char[21]) コメント(char[64])) ピクチャ名(char[96])
		dif(float[3])
		spec(float[3])
		mir(float[3])
	
	例
	PMCA Materials list v1.0
	
	material
	hair 髪
	hair_01.png 青 hair_01.bmp
	0.5 0.5 0.1
	0.0 0.0 0.0
	0.2 0.2 0.2
	hair_02.png 茶髪 hair_01.bmp
	0.8 0.8 0.8
	0.0 0.0 0.0
	0.2 0.2 0.2
	
	EOF
	*/
	printf("parts_count = %d", max);
	*materials_max = max;
	return 0;
}

int load_bone_group_list(BONE_GROUP bone_group[], unsigned short *bone_group_max, FILE *list)
{
	int i;
	char str[256], *p;
	
	int max;
	
	fgets(str, 256, list);
	p = strchr( str, '\n' );
	if(p != NULL)*p = '\0';
	p = strchr( str, 0x0d );
	if(p != NULL)*p = '\0';
	#ifdef DEBUG
		printf("%s\n", str);
	#endif
	if(strcmp(str, "PMCA Bone_Group list v1.0") != 0)return 1;	//ファイルが指定の文字列で始まっているか
	
	max = *bone_group_max;
	
	while(fgets(str, 256, list) != NULL){
		p = strchr( str, '\n' );
		if(p != NULL)*p = '\0';
		p = strchr( str, 0x0d );
		if(p != NULL)*p = '\0';
		
		#ifdef DEBUG
			printf("%s\n", str);
		#endif
		if(strcmp(str, "") != 0){
			p = read_line(str, bone_group[max].name);
			for(i=0; i<16; i++){
				p = read_line(p, bone_group[max].bone_name[i]);
				if(p == NULL)break;
			}
			max++;
		}
	}
	*bone_group_max = max;
	
	/*
	pmca_conf.txtの書式
	basic_parts_list.txt
	neta_parts_list.txt
	
	bone_group list
	upper_body 上半身
	arm 右腕 右ひじ 左腕 左ひじ
	lower_body 下半身 右足 右ひざ 左足 左ひざ
	skirt 右スカート前 右スカート後 左スカート前 左スカート後
	
	*/
	
	return 0;
}
/*
int load_rc()
{
	extern myUI g_ui;
	extern myUI2 g_ui2;
	
	int i;
	char *pc, *pc2;
	unsigned int  *pi;
	FILE *rc;
	char str[1024], *p;
	char buf[1024];
	//char buf2[1024];
	
	for(i=0; i<BTN_MAX; i++){
		g_ui.btn_txt[i][0] = '\0';
	}
	
	strcpy(g_ui.btn_txt[0], _T("OK"));
	strcpy(g_ui.btn_txt[1], _T("Prev"));
	strcpy(g_ui.btn_txt[2], _T("Return"));
	strcpy(g_ui.btn_txt[3], _T("Parts Select"));
	strcpy(g_ui.btn_txt[4], _T("Select"));
	strcpy(g_ui.btn_txt[5], _T("Cancel"));
	strcpy(g_ui.btn_txt[6], _T("Details"));
	strcpy(g_ui.btn_txt[7], _T("Parts Select"));
	strcpy(g_ui.btn_txt[8], _T("Viewer"));
	strcpy(g_ui.btn_txt[9], _T("Colors"));
	strcpy(g_ui.btn_txt[10], _T("Shapes"));
	strcpy(g_ui.btn_txt[11], _T("SAVE"));
	strcpy(g_ui.btn_txt[12], _T("Quit"));
	strcpy(g_ui.btn_txt[13], _T("Load"));
	strcpy(g_ui.btn_txt[14], _T("Apply"));
	strcpy(g_ui.btn_txt[15], _T(" Plugins "));
	
	g_ui.pfont_name[0] = '\0';
	g_ui.font_name[0] = '\0';
	memset(&g_ui.logfont , 0, sizeof(g_ui.logfont));
	memset(&g_ui.logpfont, 0, sizeof(g_ui.logpfont));
	g_ui.logfont.lfCharSet  = DEFAULT_CHARSET;
	g_ui.logpfont.lfCharSet = DEFAULT_CHARSET;
	g_ui.logfont.lfHeight  = 16;
	g_ui.logpfont.lfHeight = 16;
	
	g_ui.col_bg				= 0x00000000;
	g_ui.col_linebg			= 0x00303030;
	g_ui.col_boxbg			= 0x001a1a1a;
	g_ui.col_btn			= 0x00ffffff;
	g_ui.col_text			= 0x0000ff00;
	g_ui.col_attention		= 0x000000ff;
	g_ui.col_btn_attention	= 0x000000ff;
	
	memset(g_ui2.gl_col_bg, 0.0, 3*sizeof(float));
	g_ui2.show_axis = 0;
	g_ui2.show_grid = 0;
	
	strcpy(g_ui.welcome, "PMD Character Assembler\n"
		VERSION_NO " by mato\n");
	
	rc = fopen("pmcarc.txt", "r");
	if(rc == NULL){
		puts("pmcarcが見つかりません、デフォルトの設定を使用します");
		return -1;
	}
	
	while(fgets(str, 1024, rc) != NULL){
		
		p = strchr( str, '#');	//コメントアウト
		if(p != NULL)*p = '\0';
		p = strchr( str, '\n' );
		if(p != NULL)*p = '\0';	//改行削除
		if(str[0] == '\0')continue;
		
		if(strcmp(str, "comment") == 0){	//コメントは飛ばす
			while(fgets(str, 1024, rc) != NULL){
				if(strcmp(str, "/comment\n") == 0)break;
			}
		}
		p = read_line(str, buf);
		#ifdef DEBUG
			puts(buf);
		#endif
		if(strcmp(buf, "btn_txt") == 0){
			p = read_line(p, buf);
			if(p != NULL){
				sscanf(buf, "%d", &i);
				strcpy(g_ui.btn_txt[i], p);
			}
		}else if(strcmp(str, "title") == 0){
			char *ver;
			p = buf;
			*p = '\0';
			while(fgets(str, 1024, rc) != NULL){
				if(strcmp(str, "/title\n") == 0)break;
				p = strchr(p, '\0');
				ver = strstr(str, "VERSION_NO");
				if(ver != NULL){
					*ver = '\0';
					strcpy(p, str);
					p = strchr(p, '\0');
					strcpy(p, VERSION_NO);
					ver += strlen("VERSION_NO");
					p = strchr(p, '\0');
					strcpy(p, ver);
				}else{
					strcpy(p, str);
				}
			}
			p = g_ui.welcome;
			if(strstr(buf, "mato") == NULL || strstr(buf, VERSION_NO) == NULL){
				p = strchr(g_ui.welcome, '\0');
			}
			strcpy(p, buf);
		}else if(strcmp(buf, "col_bg") == 0){
			sscanf(p, "%x", &g_ui.col_bg);
		}else if(strcmp(buf, "col_linebg") == 0){
			sscanf(p, "%x", &g_ui.col_linebg);
		}else if(strcmp(buf, "col_boxbg") == 0){
			sscanf(p, "%x", &g_ui.col_boxbg);
		}else if(strcmp(buf, "col_btn") == 0){
			sscanf(p, "%x", &g_ui.col_btn);
		}else if(strcmp(buf, "col_text") == 0){
			sscanf(p, "%x", &g_ui.col_text);
		}else if(strcmp(buf, "col_attention") == 0){
			sscanf(p, "%x", &g_ui.col_attention);
		}else if(strcmp(buf, "col_btn_attention") == 0){
			sscanf(p, "%x", &g_ui.col_btn_attention);
		}else if(strcmp(buf, "gl_col_bg") == 0){
			sscanf(p, "%f %f %f", &g_ui2.gl_col_bg[0], &g_ui2.gl_col_bg[1], &g_ui2.gl_col_bg[2]);
		}else if(strcmp(buf, "gl_show") == 0){
			read_line(p, buf);
			if(strcmp(buf, "axis")  == 0)pc = &g_ui2.show_axis;
			else if(strcmp(buf, "grid")  == 0)pc = &g_ui2.show_grid;
			else continue;
			if(strchr(p, 'x') != NULL)*pc = *pc | 0x01;	//全て加算して書いてある部分全てを1にする
			if(strchr(p, 'y') != NULL)*pc = *pc | 0x02;
			if(strchr(p, 'z') != NULL)*pc = *pc | 0x04;
			if(strchr(p, 'b') != NULL)*pc = *pc | 0x08;
			p = strchr(p, '-');
			if(p != NULL){	//-で除外する部分以降を反転して0にする
				if(strchr(p, 'x') != NULL)*pc = *pc ^ 0x01;
				if(strchr(p, 'y') != NULL)*pc = *pc ^ 0x02;
				if(strchr(p, 'z') != NULL)*pc = *pc ^ 0x04;
				if(strchr(p, 'b') != NULL)*pc = *pc ^ 0x08;
			}
			
		}else if(strcmp(buf, "font") == 0){
			p = read_line(p, buf);
			if(strcmp(buf, "prop")  == 0){
				pc  = g_ui.pfont_name;
				pc2 = g_ui.logpfont.lfFaceName;
			}else if(strcmp(buf, "mono")  == 0){
				pc  = g_ui.font_name;
				pc2 = g_ui.logfont.lfFaceName;
			}else{
				continue;
			}
			strcpy(pc, p);
			strcpy(pc2, p);
			puts(p);
		}
	}
	
	fclose(rc);
	return 0;
}
*/

char* read_line(char *in, char *out)
{
	char *p, *pt;
	
	if(out != NULL)out[0] = '\0';
	if(in == NULL)return NULL;
	if(in[0] == '\n' || in[0] == '\0'){
		return NULL;
	}else if(in[0] == '\n'){
		in++;
	}
	
	while(*in == ' ' || *in == '\t' ){
		in++;
	}
	if(*in == '\n' || *in == '\0')return NULL;
	
	p = strchr(in, ' ');
	pt = strchr(in, '\t');
	if(p == NULL || (pt != NULL && pt < p))p = pt;
	if(p != NULL){
		*p = '\0';
	}else{
		p = strchr(in, '\n');
		if(p != NULL){
			*p = '\0';
			p = strchr( in, 0x0d );
			if(p != NULL)*p = '\0';
			p = NULL;
		}
	}
	if(out != NULL)strcpy(out, in);
	if(p!= NULL){
		*p = ' ';
		while(*p == ' ' || *p == '\t' ){
			p++;
		}
		if(*p == '\n' || *p == '\0')return NULL;
	}
	return p;
}

