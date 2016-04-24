/*PMD Mob Character Assembler
PMDモブキャラクタアセンブラ
*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

//#define DLLImport
#include "mPMD.h"
#include "PMCA.h"

LIST list;
PARTS parts[PARTS_MAX];
TEXTURES textures[TEXTURES_MAX];
MATERIALS materials[MATERIALS_MAX];
BONE_GROUP bone_group[BONE_GROUP_MAX];
unsigned short parts_max, textures_max, materials_max, bone_group_max;

int main(int argc, char *argv[])
{
	int i, j, k;
	char out[256];
	FILE *fp;
	printf("PMD Character Assembler 設定ファイルコンバーター\n");
	printf("v1.0 -> v2.0\n");
	
	//load_list(&list, "list.txt");
	
	if(argc < 2){
		printf("入力ファイルがありません\nこの実行ファイルに変換したいファイルをドラッグ＆ドロップしてください\n");
	}
	
	for(i=1; i<argc; ++i)  /* コマンドライン引数を出力するループ */
	{
		puts( argv[i] );
		parts_max = 0;
		textures_max = 0;
		materials_max = 0;
		bone_group_max = 0;
		load_config(argv[i], parts, &parts_max, textures, &textures_max, materials, &materials_max, bone_group, &bone_group_max);
		
		printf("パーツ数:%d\n", parts_max);
		printf("設定マテリアル数:%d\n", materials_max);
		printf("設定テクスチャ数:%d\n", textures_max);
		printf("ボーンセット数:%d\n\n", bone_group_max);
		
		sprintf(out, "%s", argv[i]);
		fp = fopen(out, "w");
		if(parts_max != 0){
			fprintf(fp, "PMCA Parts list v2.0\n\n");
			//fprintf(fp, "SETDIR ./parts/\n\n");
			fprintf(fp, "SETDIR ./parts/\n\n");
			for(j=0; j<parts_max; j++){
				if(j!=0)fprintf(fp, "\nNEXT\n\n");
				fprintf(fp, "[name] %s\n", parts[j].name);
				fprintf(fp, "[comment] %s\n", parts[j].comment);
				fprintf(fp, "[type] %s\n", parts[j].type);
				fprintf(fp, "[path] %s\n", parts[j].file);
				fprintf(fp, "[joint] ");
				for(k=0; k<8; k++){
					if(parts[j].join[k][0][0]=='\0')break;
					if(k!=0)fprintf(fp, ",");
					fprintf(fp, "%s", parts[j].join[k][0]);
				}
				fprintf(fp, "\n");
				if(*parts[j].pic != '\0'){
					fprintf(fp, "[pic] %s\n", parts[j].pic);
				}
			}
			printf("パーツリスト '%s' を変換しました\n", argv[i]);
		}else if(textures_max != 0){
			fprintf(fp, "PMCA Materials list v2.0\n\n");
			fprintf(fp, "SETDIR ./parts/\n\n");
			for(j=0; j<textures_max; j++){
				if(j!=0)fprintf(fp, "\nNEXT\n\n");
				fprintf(fp, "[name] %s\n", textures[j].name);
				fprintf(fp, "[comment] %s\n", textures[j].comment);
				for(k=0; k<16; k++){
					if(*textures[j].tex[k] == '\0')break;
					fprintf(fp, "\n[ENTRY] %s\n", textures[j].tex_comment[k]);
					fprintf(fp, "[tex] %s\n", textures[j].tex[k]);
					if(*textures[j].pic[k] != '\0'){
						fprintf(fp, "[pic] %s\n", textures[j].pic[k]);
					}
				}
			}
			printf("テクスチャリスト '%s' を変換しました\n", argv[i]);
		}else if(materials_max != 0){
			fprintf(fp, "PMCA Materials list v2.0\n\n");
			fprintf(fp, "SETDIR ./parts/\n\n");
			for(j=0; j<materials_max; j++){
				if(j!=0)fprintf(fp, "\nNEXT\n\n");
				fprintf(fp, "[name] %s\n", materials[j].name);
				fprintf(fp, "[comment] %s\n", materials[j].comment);
				for(k=0; k<16; k++){
					if(*materials[j].mat[k].comment == '\0')break;
					fprintf(fp, "\n[ENTRY] %s\n", materials[j].mat[k].comment);
					fprintf(fp, "[tex] %s\n", materials[j].mat[k].tex);
					fprintf(fp, "[diff_rgb] %f %f %f\n", materials[j].mat[k].dif[0], materials[j].mat[k].dif[1], materials[j].mat[k].dif[2]);
					fprintf(fp, "[spec_rgb] %f %f %f\n", materials[j].mat[k].spec[0], materials[j].mat[k].spec[1], materials[j].mat[k].spec[2]);
					fprintf(fp, "[mirr_rgb] %f %f %f\n", materials[j].mat[k].mir[0], materials[j].mat[k].mir[1], materials[j].mat[k].mir[2]);
					if(*materials[j].mat[k].toon_name != '\0'){
						fprintf(fp, "[toon] %d %s\n", materials[j].mat[k].toon_num, materials[j].mat[k].toon_name);
					}
					if(*materials[j].mat[k].pic != '\0'){
						fprintf(fp, "[pic] %s\n", materials[j].mat[k].pic);
					}
				}
			}
			printf("マテリアルリスト '%s' を変換しました\n", argv[i]);
		}
		fclose(fp);
	}
	
	
	
	/*
	puts("\n設定読み込み完了");
	printf("パーツ数:%d\n", parts_max);
	printf("設定マテリアル数:%d\n", materials_max);
	printf("設定テクスチャ数:%d\n", textures_max);
	printf("ボーンセット数:%d\n\n", bone_group_max);
	*/
	
	return 0;
	
}

