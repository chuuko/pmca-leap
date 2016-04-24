/*PMD関係のライブラリ、読み込み書き込み系*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

#include "mPMD.h"

size_t dbg_fread(void* p, size_t s, size_t n, FILE* fp){
	size_t r;
	if(s*n == 0)return 0;
	r = fread(p, s, n, fp);
	if(r < n){
		printf("ファイル読み込みに失敗\nFILE:%p\nn:%d < %d\n", fp, (int)r, (int)n);
		exit(1);
	}
	
	return r;
}

void* dbg_fgets(char* p, size_t s, FILE* fp){
	void* r;
	r = fgets(p, s, fp);
	if(r == NULL){
		printf("ファイル読み込みに失敗\nFILE:%p %p\n", fp, r);
		exit(1);
	}
	
	return r;
}

void* dbg_malloc(size_t s){
	void *p;
	p = malloc(s);
	if(p==NULL){
		puts("メモリの確保に失敗しました");
		exit(-1);
	}
	//printf("malloc:%p\n", p);
	return p;
}


void dbg_free(void* p){
	//printf("free:%p\n", p);
	if(p==NULL)return 0;
	free(p);
	p = NULL;
	//exit(1);
}

int load_list(LIST *list, const char file_name[])
{
	int i;
	char str[256];
	FILE *lst_file;
	
	if(strcmp(file_name, "") == 0){
		printf("ファイル名がありません\n");
		return 1;
	}
	
	lst_file = fopen(file_name,"r");
	if(lst_file == NULL){
		printf("ファイル %s を開けません\n", file_name);
		return 1;
	}
	FGETS(str, 256, lst_file);
	
	i = 0;
	while(FGETS(str, 256, lst_file)!=NULL){
		if(strcmp(str, "skin\n") == 0)break;
		i++;
	}
	list->bone_count = i;
	i = 0;
	while(FGETS(str, 256, lst_file)!=NULL){
		if(strcmp(str, "bone_disp\n") == 0)break;
		i++;
	}
	list->skin_count = i;
	
	i = 0;
	while(FGETS(str, 256, lst_file)!=NULL){
		if(strcmp(str, "end\n") == 0)break;
		i++;
	}
	list->disp_count = i;
	
	fclose(lst_file);
	lst_file = fopen(file_name,"r");
	if(lst_file == NULL){
		printf("ファイル %s を開けません\n", file_name);
		return 1;
	}
	
	
	printf("リストボーン数:%d\n", list->bone_count);
	printf("リスト表情数:%d\n", list->skin_count);
	printf("リスト表示枠数:%d\n", list->disp_count);
	
	list->bone = MALLOC((size_t)list->bone_count * sizeof(char)* NAME_LEN);
	list->bone_eng = MALLOC((size_t)list->bone_count * sizeof(char)*NAME_LEN);
	list->skin = MALLOC((size_t)list->skin_count * sizeof(char)*NAME_LEN);
	list->skin_eng = MALLOC((size_t)list->skin_count * sizeof(char)*NAME_LEN);
	list->disp = MALLOC((size_t)list->disp_count * sizeof(char)*NAME_LEN);
	list->disp_eng = MALLOC((size_t)list->disp_count * sizeof(char)*NAME_LEN);
	if(list->bone == NULL)return -1;
	if(list->bone_eng == NULL)return -1;
	if(list->skin == NULL)return -1;
	if(list->skin_eng == NULL)return -1;
	if(list->disp == NULL)return -1;
	if(list->disp_eng == NULL)return -1;
	
	FGETS(str, 256, lst_file);
	for(i=0; i<list->bone_count; i++){
		FGETS(str, 256, lst_file);
		sscanf(str, "%s %s\n", list->bone[i], list->bone_eng[i]);
	}
	
	FGETS(str, 256, lst_file);
	for(i=0; i<list->skin_count; i++){
		FGETS(str, 256, lst_file);
		sscanf(str, "%s %s\n", list->skin[i], list->skin_eng[i]);
	}
	
	FGETS(str, 256, lst_file);
	for(i=0; i<list->disp_count; i++){
		FGETS(str, 256, lst_file);
		sscanf(str, "%s %s\n", list->disp[i], list->disp_eng[i]);
	}
	fclose(lst_file);
	
	return 0;
}
int delete_list(LIST *list){
	
	list->bone_count = 0;
	list->skin_count = 0;
	list->disp_count = 0;
	
	FREE(list->bone);
	FREE(list->bone_eng);
	FREE(list->skin);
	FREE(list->skin_eng);
	FREE(list->disp);
	FREE(list->disp_eng);
	
	list->bone = NULL;
	list->bone_eng = NULL;
	list->skin = NULL;
	list->skin_eng = NULL;
	list->disp = NULL;
	list->disp_eng = NULL;
	
	return 0;
}

int load_PMD(MODEL *model, const char file_name[])
{
	int i, j, tmp;
	char *char_p, str[PATH_LEN];
	FILE *pmd;
	
	static MODEL cache_model[64];
	static unsigned char count[64];
	static unsigned char cur_count=255;
	
	//int *check;
	
	//check = MALLOC((size_t)1000*sizeof(int));
	
	//memset(check, 0, (size_t)1000*sizeof(int));
	
	//dbg_0check(check,1000);
	
	//キャッシュまわり初期化
	if(cur_count == 255){
		for(i=0; i<64; i++){
			count[i] = 255;
			create_PMD(&cache_model[i]);
		}
		cur_count = 0;
	}
	
	if(strcmp(file_name, "") == 0){
		printf("ファイル名がありません\n");
		return 1;
	}
	
	if(cur_count > 64){
		cur_count=0;
	}else{
		cur_count++;
	}
	
	for(i=0; i<64; i++){
		if(count[i]!=255){
			//printf("%s, %s\n", cache_model[i].header.path, file_name);
			if(strncmp(cache_model[i].header.path, file_name, PATH_LEN) == 0){
				copy_PMD(model, &cache_model[i]);
				count[i] = cur_count;
				break;
			}
		}
	}
	
	tmp = cur_count+1;
	if(tmp >= 64)tmp = 0;
	for(j=0; j<64; j++){
		if(count[i] == tmp){
			count[i] = 255;
			delete_PMD(&cache_model[i]);
		}
	}
	if(i != 64){
		printf("Use Cache\n");
		return 0;
	}
	
	pmd = fopen(file_name,"rb");
	if(pmd == NULL  ){
		printf("Can't open file:%s\n", file_name);
		return 1;
	}
	
	//printf("%s\n", file_name);
	strncpy(model->header.path, file_name, PATH_LEN);
	
	FREAD(model->header.magic, 1, 3,pmd);
	FREAD(&model->header.version, 4, 1, pmd);
	
	if(memcmp(model->header.magic, "Pmd", 3) != 0 || model->header.version != 1.0){
		printf("Format error\n");
		return -1;
	}
	
	FREAD(model->header.name, 1, 20, pmd);
	FREAD(model->header.comment, 1, 256, pmd);
	
	#ifdef DEBUG
		printf("%s \n %f \n %s \n %s \n",
			model->header.magic,
			model->header.version,
			model->header.name,
			model->header.comment);
	#endif
	FREAD(&model->vt_count, 4,  1, pmd);
	#ifdef DEBUG
		printf("頂点数:%d\n", model->vt_count);
	#endif
	model->vt =(VERTEX *)MALLOC((size_t)model->vt_count*sizeof(VERTEX));
	if(model->vt == NULL  ){
		printf("配列を確保できません\n");
		return 1;
	}
	
	for(i=0; i<(model->vt_count); i++){
		//fseek(pmd, 38, SEEK_CUR);
		FREAD(model->vt[i].loc, 4, 3, pmd);
		FREAD(model->vt[i].nor, 4, 3, pmd);
		FREAD(model->vt[i].uv, 4, 2, pmd);
		FREAD(model->vt[i].bone_num, 2, 2, pmd);
		FREAD(&model->vt[i].bone_weight, 1, 1, pmd);
		FREAD(&model->vt[i].edge_flag, 1, 1, pmd);
	}
	
	FREAD(&model->vt_index_count, 4,  1, pmd);
	#ifdef DEBUG
		printf("面頂点数:%d\n", model->vt_index_count);
	#endif
	model->vt_index  = (unsigned short *)MALLOC((size_t)model->vt_index_count*sizeof(unsigned short));
	if(model->vt_index == NULL  ){
		printf("配列を確保できません\n");
		return 1;
	}
	
	for(i=0; i<model->vt_index_count; i++){
		FREAD(&model->vt_index[i], 2, 1, pmd);
		if(model->vt_index[i] >= model->vt_count){
			printf("頂点インデックスが不正です\n");
			return 1;
		}
	}
	
	FREAD(&model->mat_count, 4,  1, pmd);
	#ifdef DEBUG
		printf("材質数:%d\n", model->mat_count);
	#endif
	model->mat =(MATERIAL*)MALLOC((size_t)model->mat_count*sizeof(MATERIAL));
	if(model->mat == NULL  ){
		printf("配列を確保できません\n");
		return 1;
	}
	for(i=0; i<model->mat_count; i++){
		FREAD(model->mat[i].diffuse, 4, 3, pmd);
		FREAD(&model->mat[i].alpha, 4, 1, pmd);
		FREAD(&model->mat[i].spec, 4, 1, pmd);
		FREAD(&model->mat[i].spec_col, 4, 3, pmd);
		FREAD(&model->mat[i].mirror_col, 4, 3, pmd);
		FREAD(&model->mat[i].toon_index, 1, 1, pmd);
		FREAD(&model->mat[i].edge_flag, 1, 1, pmd);
		FREAD(&model->mat[i].vt_index_count, 4, 1, pmd);
		FREAD(&model->mat[i].tex, 1, 20, pmd);
		model->mat[i].tex[21] = '\0';
		
		strcpy(str, file_name);
		char_p = strrchr(str, '/');
		if(char_p!=NULL){
			char_p++;
			*char_p = '\0';
		}else{
			*str = '\0';
		}
		char_p = strchr(model->mat[i].tex, '*');
		if(char_p != NULL){
			*char_p='\0';
			++char_p;
			strcpy(model->mat[i].sph, char_p);
			sprintf(model->mat[i].sph_path,"%s%s\0", str, model->mat[i].sph);
		}else{
			*model->mat[i].sph='\0';
		}
		sprintf(model->mat[i].tex_path,"%s%s\0", str, model->mat[i].tex);
	}
	
	FREAD(&model->bone_count, 2,  1, pmd);
	#ifdef DEBUG
		printf("ボーン数:%d\n", model->bone_count);
	#endif
	model->bone =(BONE*)MALLOC((size_t)model->bone_count*sizeof(BONE));
	if(model->bone == NULL  ){
		printf("配列を確保できません\n");
		return 1;
	}
	
	for(i=0; i<model->bone_count; i++){
		FREAD(model->bone[i].name, 1, 20, pmd);
		FREAD(&model->bone[i].PBone_index, 2, 1, pmd);
		FREAD(&model->bone[i].TBone_index, 2, 1, pmd);
		FREAD(&model->bone[i].type, 1, 1, pmd);
		FREAD(&model->bone[i].IKBone_index, 2, 1, pmd);
		FREAD(model->bone[i].loc, 4, 3, pmd);
		model->bone[i].name_eng[0] = '\0';
		model->bone[i].name[21] = '\0';
	}
	
	FREAD(&model->IK_count, 2,  1, pmd);
	#ifdef DEBUG
		printf("IKデータ数:%d\n", model->IK_count);
	#endif
	model->IK_list =(IK_LIST*)MALLOC(model->IK_count*sizeof(IK_LIST));
	if(model->IK_list == NULL  ){
		printf("配列を確保できません\n");
		return 1;
	}
	for(i=0; i<model->IK_count; i++){
		FREAD(&model->IK_list[i].IKBone_index, 2, 1, pmd);
		FREAD(&model->IK_list[i].IKTBone_index, 2, 1, pmd);
		FREAD(&model->IK_list[i].IK_chain_len, 1, 1, pmd);
		FREAD(&model->IK_list[i].iterations, 2, 1, pmd);
		FREAD(&model->IK_list[i].weight, 4, 1, pmd);
		model->IK_list[i].IKCBone_index = (unsigned short*)MALLOC((size_t)model->IK_list[i].IK_chain_len*sizeof(unsigned short));
		if(model->IK_list[i].IKCBone_index == NULL)return -1;
		if(model->IK_list[i].IK_chain_len > 0){
			FREAD(model->IK_list[i].IKCBone_index, 2, model->IK_list[i].IK_chain_len, pmd);
		}
	}
	
	FREAD(&model->skin_count, 2,  1, pmd);
	#ifdef DEBUG
		printf("表情数:%d\n", model->skin_count);
	#endif
	model->skin =(SKIN*)MALLOC((size_t)model->skin_count*sizeof(SKIN));
	if(model->skin == NULL  ){
		printf("配列を確保できません\n");
		return 1;
	}
	for(i=0; i<model->skin_count; i++){
		FREAD(model->skin[i].name, 1, 20, pmd);
		FREAD(&model->skin[i].skin_vt_count, 4, 1, pmd);
		FREAD(&model->skin[i].type, 1, 1, pmd);
		model->skin[i].data=(SKIN_DATA*)MALLOC((size_t)model->skin[i].skin_vt_count*sizeof(SKIN_DATA));
		if(model->skin[i].data == NULL)return -1;
		for(j=0; j<model->skin[i].skin_vt_count; j++){
			FREAD(&model->skin[i].data[j].index, 4, 1, pmd);
			if(model->skin[i].data[j].index > model->vt_count){
				exit(1);
			}
			FREAD(&model->skin[i].data[j].loc, 4, 3, pmd);
		}
		model->skin[i].name_eng[0] = '\0';
		model->skin[i].name[20] = '\0';
	}
	
	FREAD(&model->skin_disp_count, 1,  1, pmd);
	#ifdef DEBUG
		printf("表情枠:%d\n", model->skin_disp_count);
	#endif
	model->skin_index = (unsigned short*)MALLOC((size_t)model->skin_disp_count*sizeof(unsigned short));
	if(model->skin_index==NULL)return -1;
	FREAD(model->skin_index, 2, model->skin_disp_count, pmd);
	
	FREAD(&model->bone_group_count, 1,  1, pmd);
	#ifdef DEBUG
		printf("ボーン枠:%d\n", model->bone_group_count);
	#endif
	model->bone_group = (BONE_GROUP*)MALLOC(sizeof(BONE_GROUP)*(size_t)model->bone_group_count);
	if(model->bone_group == NULL)return -1;
	for(i=0; i<model->bone_group_count; i++){
		FREAD(&model->bone_group[i].name, 1, 50, pmd);
		model->bone_group[i].name_eng[0] = '\0';
	}
	
	FREAD(&model->bone_disp_count, 4,  1, pmd);
	#ifdef DEBUG
		printf("表示ボーン数:%d\n", model->bone_disp_count);
	#endif
	model->bone_disp = (BONE_DISP*)MALLOC((size_t)model->bone_disp_count*sizeof(BONE_DISP));
	if(model->bone_disp==NULL)return -1;
	for(i=0; i<model->bone_disp_count; i++){
		FREAD(&model->bone_disp[i].index, 2,  1, pmd);
		FREAD(&model->bone_disp[i].bone_group, 1,  1, pmd);
	}
	
	
	FREAD(&model->eng_support, 1,  1, pmd);
	
	if(feof(pmd)!=0){
		#ifdef DEBUG
			printf("拡張部分なし\n");
		#endif
		model->eng_support = 0;
		for(i=0; i<10; i++){
			j=i+1;
			sprintf(model->toon[i], "toon%02d.bmp\0", j);
			sprintf(model->toon_path[i], "toon%02d.bmp\0", j);
		}
		model->rbody_count = 0;
		model->rbody = NULL;
		model->joint_count = 0;
		model->joint = NULL;
		return 0;
	}
	
	#ifdef DEBUG
		printf("英名対応:%d\n", model->eng_support);
	#endif
	
	if(model->eng_support == 1){
		printf("英名対応PMD\n");
		FREAD(model->header.name_eng, 1,  20, pmd);
		model->header.name_eng[21] = '\0';
		
		FREAD(model->header.comment_eng, 1,  256, pmd);
		model->header.comment_eng[255] = '\0';
		
		for(i=0; i<model->bone_count; i++){
			FREAD(model->bone[i].name_eng, 1,  20, pmd);
			model->bone[i].name_eng[20] = '\0';
		}
		
		if(model->skin_count > 0){
			strcpy(model->skin[0].name_eng, "base");
		}
		for(i=1; i<model->skin_count; i++){
			FREAD(model->skin[i].name_eng, 1,  20, pmd);
			model->skin[i].name_eng[20] = '\0';
		}
		for(i=0; i<model->bone_group_count; i++){
			FREAD(model->bone_group[i].name_eng, 1,  50, pmd);
			model->bone_group[i].name_eng[50] = '\0';
		}
	}else{
		printf("英名非対応PMD\n");
		
		*model->header.name_eng = '\0';
		*model->header.comment_eng = '\0';
		
		for(i=0; i<model->bone_count; i++){
			*model->bone[i].name_eng = '\0';
		}
		
		//strncpy(model->skin[0].name_eng, "base", 20);
		//puts(model->skin[0].name);
		for(i=0; i<model->skin_count; i++){
			*model->skin[i].name_eng = '\0';
		}
		
		for(i=0; i<model->bone_group_count; i++){
			*model->bone_group[i].name_eng = '\0';
		}
		
	}
	
	for(i=0; i<10; i++){
		FREAD(model->toon[i], 1, 100, pmd);
		strncpy(str, file_name, PATH_LEN);
		char_p = strrchr(str, '/');
		if(char_p!=NULL){
			char_p++;
			*char_p = '\0';
		}else{
			*str = '\0';
		}
		sprintf(model->toon_path[i],"%s%s\0", str, model->toon[i]);
		#ifdef DEBUG
		printf("%s%s\n", str, model->toon[i]);
		#endif
	}
	
	
	
	FREAD(&model->rbody_count, 4,  1, pmd);
	#ifdef DEBUG
		printf("剛体数:%d\n", model->rbody_count);
	#endif
	model->rbody =(RIGID_BODY *) MALLOC((size_t)model->rbody_count*sizeof(RIGID_BODY));
	
	if(model->rbody == NULL  ){
		printf("配列を確保できません\n");
		return 1;
	}
	for(i=0; i<model->rbody_count; i++){
		FREAD(model->rbody[i].name, 1,  20, pmd);
		FREAD(&model->rbody[i].bone, 2,  1, pmd);
		FREAD(&model->rbody[i].group, 1,  1, pmd);
		FREAD(&model->rbody[i].target, 2,  1, pmd);
		FREAD(&model->rbody[i].shape, 1,  1, pmd);
		FREAD(model->rbody[i].size, 4,  3, pmd);
		FREAD(model->rbody[i].loc, 4,  3, pmd);
		FREAD(model->rbody[i].rot, 4,  3, pmd);
		FREAD(model->rbody[i].property, 4,  5, pmd);
		FREAD(&model->rbody[i].type, 1,  1, pmd);
		model->rbody[i].name[21] = '\0';
	}
	
	FREAD(&model->joint_count, 4,  1, pmd);
	#ifdef DEBUG
		printf("ジョイント数:%d\n", model->joint_count);
	#endif
	model->joint =(JOINT *) MALLOC((size_t)model->joint_count*sizeof(JOINT));
	if(model->joint == NULL  ){
		printf("配列を確保できません\n");
		return 1;
	}
	for(i=0; i<model->joint_count; i++){
		FREAD(model->joint[i].name, 1,  20, pmd);
		FREAD(model->joint[i].rbody, 4,  2, pmd);
		FREAD(model->joint[i].loc, 4,  3, pmd);
		FREAD(model->joint[i].rot, 4,  3, pmd);
		FREAD(model->joint[i].limit, 4,  12, pmd);
		FREAD(model->joint[i].spring, 4,  6, pmd);
		model->joint[i].name[20] = '\0';
	}
	
	fclose(pmd);
	
	//dbg_0check(check,1000);
	
	//FREE(check);
	
	//モデルをキャッシュに保存
	for(i=0; i<64; i++){
		if(count[i]==255){
			copy_PMD(&cache_model[i],model);
			count[i] = cur_count;
			break;
		}
	}
	
	return 0;
}


int write_PMD(MODEL *model, const char file_name[])
{
	int i, j, ret=0;
	char str[PATH_LEN];
	
	FILE *pmd;
	
	if(strcmp(file_name, "") == 0){
		printf("ファイル名がありません\n");
		return 1;
	}
	
	pmd = fopen(file_name,"wb");
	if(pmd == NULL  ){
		printf("ファイル %s を開けません\n", file_name);
		return 1;
	}
	
	/*ゼロ埋め*/
	/*{
		int total_bytes;
		char b = '\0';
		total_bytes = 283+4+38*model->vt_count
						+4+2*model->vt_index_count
						+4*70*model->mat_count;
		for(i=0; i<total_bytes; i++){
			fwrite(&b,1,1,pmd);
		}
		fseek(pmd, 0, SEEK_SET);
	}*/
	
	//ヘッダー書き換え
	strcpy(model->header.magic, "Pmd");
	model->header.version = 1.0;
	
	fwrite(model->header.magic, 3, 1,pmd);
	fwrite(&model->header.version, 4, 1, pmd);
	fwrite(model->header.name, 20, 1, pmd);
	fwrite(model->header.comment, 256, 1, pmd);
	
	#ifdef DEBUG
		printf("ヘッダ\n");
	#endif
	
	fwrite(&model->vt_count, 4,  1, pmd);
	
	for(i=0; i<(model->vt_count); i++){
		//fseek(pmd, 38, SEEK_CUR);
		fwrite(model->vt[i].loc, 4, 3, pmd);
		fwrite(model->vt[i].nor, 4, 3, pmd);
		fwrite(model->vt[i].uv, 4, 2, pmd);
		fwrite(model->vt[i].bone_num, 2, 2, pmd);
		fwrite(&model->vt[i].bone_weight, 1, 1, pmd);
		fwrite(&model->vt[i].edge_flag, 1, 1, pmd);
	}
	
	#ifdef DEBUG
		printf("頂点\n");
	#endif
	
	fwrite(&model->vt_index_count, 4,  1, pmd);
	
	for(i=0; i<model->vt_index_count; i++){
		if(model->vt_index[i] >= model->vt_count){
			printf("頂点インデックスが不正です :%d\n", model->vt_index[i]);
			return 1;
		}
		fwrite(&model->vt_index[i], 2, 1, pmd);
	}
	#ifdef DEBUG
		printf("面頂点\n");
	#endif
	
	fwrite(&model->mat_count, 4,  1, pmd);
	
	for(i=0; i<model->mat_count; i++){
		//70bytes
		fwrite(model->mat[i].diffuse, 4, 3, pmd);
		fwrite(&model->mat[i].alpha, 4, 1, pmd);
		fwrite(&model->mat[i].spec, 4, 1, pmd);
		fwrite(model->mat[i].spec_col, 4, 3, pmd);
		fwrite(model->mat[i].mirror_col, 4, 3, pmd);
		fwrite(&model->mat[i].toon_index, 1, 1, pmd);
		fwrite(&model->mat[i].edge_flag, 1, 1, pmd);
		fwrite(&model->mat[i].vt_index_count, 4, 1, pmd);
		
		if(*model->mat[i].sph != '\0'){
			sprintf(str, "%s*%s\0", model->mat[i].tex, model->mat[i].sph);
			if(strlen(str) > 20){
				ret = 2;
			}
			fwrite(str, 1, 20, pmd);
		}else{
			fwrite(model->mat[i].tex, 1, 20, pmd);
		}
	}
	#ifdef DEBUG
		printf("材質\n");
	#endif
	
	
	fwrite(&model->bone_count, 2,  1, pmd);
	
	for(i=0; i<model->bone_count; i++){
		fwrite(model->bone[i].name, 1, 20, pmd);
		fwrite(&model->bone[i].PBone_index, 2, 1, pmd);
		fwrite(&model->bone[i].TBone_index, 2, 1, pmd);
		fwrite(&model->bone[i].type, 1, 1, pmd);
		fwrite(&model->bone[i].IKBone_index, 2, 1, pmd);
		fwrite(model->bone[i].loc, 4, 3, pmd);
	}
	#ifdef DEBUG
		printf("ボーン\n");
	#endif
	
	fwrite(&model->IK_count, 2,  1, pmd);
	
	for(i=0; i<model->IK_count; i++){
		fwrite(&model->IK_list[i].IKBone_index, 2, 1, pmd);
		fwrite(&model->IK_list[i].IKTBone_index, 2, 1, pmd);
		fwrite(&model->IK_list[i].IK_chain_len, 1, 1, pmd);
		fwrite(&model->IK_list[i].iterations, 2, 1, pmd);
		fwrite(&model->IK_list[i].weight, 4, 1, pmd);
		fwrite(model->IK_list[i].IKCBone_index, 2, model->IK_list[i].IK_chain_len, pmd);
	}
	#ifdef DEBUG
		printf("IKリスト\n");
	#endif
	
	fwrite(&model->skin_count, 2,  1, pmd);
	
	for(i=0; i<model->skin_count; i++){
		fwrite(model->skin[i].name, 1, 20, pmd);
		fwrite(&model->skin[i].skin_vt_count, 4, 1, pmd);
		fwrite(&model->skin[i].type, 1, 1, pmd);
		for(j=0; j<model->skin[i].skin_vt_count; j++){
			fwrite(&model->skin[i].data[j].index, 4, 1, pmd);
			fwrite(model->skin[i].data[j].loc, 4, 3, pmd);
		}
	}
	#ifdef DEBUG
		printf("表情\n");
	#endif
	
	
	fwrite(&model->skin_disp_count, 1,  1, pmd);
	fwrite(model->skin_index, 2, model->skin_disp_count, pmd);
	#ifdef DEBUG
		printf("表情表示\n");
	#endif
	
	fwrite(&model->bone_group_count, 1,  1, pmd);
	for(i=0; i<model->bone_group_count; i++){
		fwrite(model->bone_group[i].name, 1, 50, pmd);
	}
	
	fwrite(&model->bone_disp_count, 4,  1, pmd);
	for(i=0; i<model->bone_disp_count; i++){
		fwrite(&model->bone_disp[i].index, 2,  1, pmd);
		fwrite(&model->bone_disp[i].bone_group, 1,  1, pmd);
	}
	#ifdef DEBUG
		printf("ボーン表示\n");
	#endif
	
	//extension
	fwrite(&model->eng_support, 1,  1, pmd);
	
	if(model->eng_support==1){
		fwrite(model->header.name_eng, 1,  20, pmd);
		fwrite(model->header.comment_eng, 1,  256, pmd);
		for(i=0; i<model->bone_count; i++){
			fwrite(model->bone[i].name_eng, 1,  20, pmd);
		}
		for(i=1; i<model->skin_count; i++){
			fwrite(model->skin[i].name_eng, 1,  20, pmd);
		}
		for(i=0; i<model->bone_group_count; i++){
			fwrite(model->bone_group[i].name_eng, 1,  50, pmd);
		}
	}
	#ifdef DEBUG
		printf("英名\n");
	#endif
	
	for(i=0; i<10; i++){
		fwrite(model->toon[i], 1, 100, pmd);
	}
	
	fwrite(&model->rbody_count, 4,  1, pmd);
	
	for(i=0; i<model->rbody_count; i++){
		fwrite(model->rbody[i].name, 1,  20, pmd);
		fwrite(&model->rbody[i].bone, 2,  1, pmd);
		fwrite(&model->rbody[i].group, 1,  1, pmd);
		fwrite(&model->rbody[i].target, 2,  1, pmd);
		fwrite(&model->rbody[i].shape, 1,  1, pmd);
		fwrite(model->rbody[i].size, 4,  3, pmd);
		fwrite(model->rbody[i].loc, 4,  3, pmd);
		fwrite(model->rbody[i].rot, 4,  3, pmd);
		fwrite(model->rbody[i].property, 4,  5, pmd);
		fwrite(&model->rbody[i].type, 1,  1, pmd);
	}
	#ifdef DEBUG
		printf("剛体\n");
	#endif
	
	fwrite(&model->joint_count, 4,  1, pmd);
	
	for(i=0; i<model->joint_count; i++){
		fwrite(model->joint[i].name, 1,  20, pmd);
		fwrite(model->joint[i].rbody, 4,  2, pmd);
		fwrite(model->joint[i].loc, 4,  3, pmd);
		fwrite(model->joint[i].rot, 4,  3, pmd);
		fwrite(model->joint[i].limit, 4,  12, pmd);
		fwrite(model->joint[i].spring, 4,  6, pmd);
	}
	#ifdef DEBUG
		printf("ジョイント\n");
	#endif
	
	fclose(pmd);
	printf("%sへ出力しました。\n",file_name);
	
	return ret;
}

int print_PMD(MODEL *model, const char file_name[])
{
	int i, j, k;
	
	FILE *txt;
	
	if(strcmp(file_name, "") == 0){
		printf("ファイル名がありません\n");
		return 1;
	}
	
	txt = fopen(file_name,"w");
	if(txt == NULL  ){
		fprintf(txt, "出力テキストファイルを開けません\n");
		return 1;
	}
	
	fprintf(txt, "%s \n %f \n %s \n %s \n",
		model->header.magic,
		model->header.version,
		model->header.name,
		model->header.comment);
	
	for(i=0; i<model->vt_count; i++){
		fprintf(txt, "No:%d\n", i);
		fprintf(txt, "Loc:");
		for(j=0; j<3; j++){
			fprintf(txt, "%f ", model->vt[i].loc[j]);
		}
		fprintf(txt, "\nNor:");
		for(j=0; j<3; j++){
			fprintf(txt, "%f ", model->vt[i].nor[j]);
		}
		fprintf(txt, "\nUV:");
		for(j=0; j<2; j++){
			fprintf(txt, "%f ", model->vt[i].uv[j]);
		}
		fprintf(txt, "\nBONE:");
		for(j=0; j<2; j++){
			fprintf(txt, "%d ", model->vt[i].bone_num[j]);
		}
		fprintf(txt, "\nbone_weight:%d\n", model->vt[i].bone_weight);
		fprintf(txt, "edge_flag:%d\n\n", model->vt[i].edge_flag);
		
	}
	
	fprintf(txt, "面頂点数:%d\n", model->vt_index_count);
	
	for(i=0; i<model->vt_index_count; i++){
		fprintf(txt, "%d\n", model->vt_index[i]);
	}
	fprintf(txt, "\n");
	
	fprintf(txt, "材質数:%d\n", model->mat_count);
	
	
	for(i=0; i<model->mat_count; i++){
		fprintf(txt, "No:%d\n", i);
		fprintf(txt, "diffuse:");
		for(j=0; j<3; j++){
			fprintf(txt, "%f ", model->mat[i].diffuse[j]);
		}
		fprintf(txt, "\n%f", model->mat[i].alpha);
		fprintf(txt, "\n%f", model->mat[i].spec);
		fprintf(txt, "\nspec_col:");
		for(j=0; j<3; j++){
			fprintf(txt, "%f ", model->mat[i].spec_col[j]);
		}
		fprintf(txt, "\nmirror_col:");
		for(j=0; j<3; j++){
			fprintf(txt, "%f ", model->mat[i].mirror_col[j]);
		}
		fprintf(txt, "\ntoon_index:%d\n", model->mat[i].toon_index);
		fprintf(txt, "edge_flag:%d\n", model->mat[i].edge_flag);
		fprintf(txt, "vt_index_count:%d\n", model->mat[i].vt_index_count);
		fprintf(txt, "texture:%s\n\n", model->mat[i].tex);
	}
	fprintf(txt, "ボーン数:%d\n", model->bone_count);
	
	for(i=0; i<model->bone_count; i++){
		fprintf(txt, "ボーン名:%s\n", model->bone[i].name);
		fprintf(txt, "親ボーン:%d\n", model->bone[i].PBone_index);
		fprintf(txt, "テイルボーン:%d\n", model->bone[i].TBone_index);
		fprintf(txt, "タイプ:%d\n", model->bone[i].type);
		fprintf(txt, "IKボーン:%d\n", model->bone[i].IKBone_index);
		fprintf(txt, "位置:");
		for(j=0; j<3; j++){
			fprintf(txt, "%f ", model->bone[i].loc[j]);
		}
		fprintf(txt, "\n\n");
	}
	
	fprintf(txt, "IKデータ数:%d\n", model->IK_count);
	
	for(i=0; i<model->IK_count; i++){
		fprintf(txt, "IKボーン:%d\n", model->IK_list[i].IKBone_index);
		fprintf(txt, "IKテイルボーン:%d\n", model->IK_list[i].IKTBone_index);
		fprintf(txt, "IKチェーン長:%d\n", model->IK_list[i].IK_chain_len);
		fprintf(txt, "iteration:%d\n", model->IK_list[i].iterations);
		fprintf(txt, "ウエイト:%f\n", model->IK_list[i].weight);
		for(j=0; j<model->IK_list[i].IK_chain_len; j++){
			fprintf(txt, "IK子 %d:%d\n", j, model->IK_list[i].IKCBone_index[j]);
		}
		fprintf(txt, "\n");
	}
	
	fprintf(txt, "表情数:%d\n", model->skin_count);
	
	for(i=0; i<model->skin_count; i++){
		fprintf(txt, "表情名:%s\n", model->skin[i].name);
		fprintf(txt, "表情頂点数:%d\n", model->skin[i].skin_vt_count);
		fprintf(txt, "表情タイプ:%d\n", model->skin[i].type);
		for(j=0; j<model->skin[i].skin_vt_count; j++){
			fprintf(txt, "%d ", model->skin[i].data[j].index);
			for(k=0; k<3; k++){
				fprintf(txt, "%f ", model->skin[i].data[j].loc[k]);
			}
			fprintf(txt, "\n");
		}
		fprintf(txt, "\n");
	}
	
	fprintf(txt, "\n表情枠:%d\n", model->skin_disp_count);
	for(i=0; i<model->skin_disp_count; i++){
		fprintf(txt, "%d\n", model->skin_index[i]);
	}
	
	fprintf(txt, "\nボーン枠:%d\n", model->bone_group_count);
	
	for(i=0; i<model->bone_group_count; i++){
		fprintf(txt, "%d %s", i,model->bone_group[i].name);
	}
	
	fprintf(txt, "\n表示ボーン数:%d\n", model->bone_disp_count);
	
	for(i=0; i<model->bone_disp_count; i++){
		fprintf(txt, "ボーン番号:%d\n", model->bone_disp[i].index);
		fprintf(txt, "表示番号:%d\n", model->bone_disp[i].bone_group);
	}
	
	fprintf(txt, "英名対応:%d\n", model->eng_support);
	if(model->eng_support == 1){
		fprintf(txt, "%s\n", model->header.name_eng);
		fprintf(txt, "%s\n", model->header.comment_eng);
		for(i=0; i<model->bone_count; i++){
			fprintf(txt, "%s\n", model->bone[i].name_eng);
		}
		for(i=0; i<model->skin_count; i++){
			fprintf(txt, "%s\n", model->skin[i].name_eng);
		}
		for(i=0; i<model->bone_group_count; i++){
			fprintf(txt, "%s\n", model->bone_group[i].name_eng);
		}
	}
	
	for(i=0; i<10; i++){
		fprintf(txt, "%s\n", model->toon[i]);
	}
	
	
	fprintf(txt,"剛体数:%d\n", model->rbody_count);
	
	for(i=0; i<model->rbody_count; i++){
		fprintf(txt, "%s\n", model->rbody[i].name);
		fprintf(txt, "ボーン:%d\n", model->rbody[i].bone);
		fprintf(txt, "グループ:%d\n", model->rbody[i].group);
		fprintf(txt, "ターゲット:%d\n",model->rbody[i].target);
		fprintf(txt, "形状:%d\n",model->rbody[i].shape);
		fprintf(txt, "size:");
		for(j=0; j<3; j++){
			fprintf(txt, "%f ",model->rbody[i].size[j]);
		}
		fprintf(txt, "\nloc:");
		for(j=0; j<3; j++){
			fprintf(txt, "%f ",model->rbody[i].loc[j]);
		}
		fprintf(txt, "\nrot:");
		for(j=0; j<3; j++){
			fprintf(txt, "%f ",model->rbody[i].rot[j]);
		}
		fprintf(txt, "\nproperty:");
		for(j=0; j<5; j++){
			fprintf(txt, "%f ",model->rbody[i].property[j]);
		}
		fprintf(txt, "\n");
		fprintf(txt, "タイプ:%d\n\n",model->rbody[i].type);
	}
	
	fprintf(txt,"ジョイント数:%d\n", model->joint_count);
	
	for(i=0; i<model->joint_count; i++){
		fprintf(txt, "%s\n", model->joint[i].name);
		fprintf(txt, "剛体:");
		for(j=0; j<2; j++){
			fprintf(txt, "%d ",model->joint[i].rbody[j]);
		}
		fprintf(txt, "\nloc:");
		for(j=0; j<3; j++){
			fprintf(txt, "%f ",model->joint[i].loc[j]);
		}
		fprintf(txt, "\nrot:");
		for(j=0; j<3; j++){
			fprintf(txt, "%f ",model->joint[i].rot[j]);
		}
		fprintf(txt, "\nlimit:");
		for(j=0; j<12; j++){
			fprintf(txt, "%f ",model->joint[i].limit[j]);
		}
		fprintf(txt, "\nspring:");
		for(j=0; j<6; j++){
			fprintf(txt, "%f ",model->joint[i].spring[j]);
		}
		fprintf(txt, "\n");
	}
	
	fclose(txt);
	
	
	return 0;
}

int create_PMD(MODEL *model)
{
	int i, j;
	strcpy(model->header.name, "");
	strcpy(model->header.comment, "");
	
	model->vt_count = 0;
	model->vt =NULL;
	
	model->vt_index_count = 0;
	model->vt_index = NULL;
	
	model->mat_count = 0;
	model->mat = NULL;
	
	model->bone_count = 0;
	model->bone = NULL;
	
	model->IK_count = 0;
	model->IK_list = NULL;
	
	model->skin_count = 0;
	model->skin = NULL;
	
	model->skin_index = NULL;
	model->bone_group_count = 0;
	model->bone_group = NULL;
	model->bone_disp_count = 0;
	model->bone_disp = NULL;
	
	model->eng_support = 0;
	
	for(i=0; i<10; i++){
		j=i+1;
		//*model->toon[i] = '\0';
		//*model->toon_path[i] = '\0';
		sprintf(model->toon[i], "toon%02d.bmp\0", j);
		sprintf(model->toon_path[i], "toon%02d.bmp\0", j);
	}
	
	model->rbody_count = 0;
	model->rbody = NULL;
	model->joint_count = 0;
	model->joint = NULL;
	
	return 0;
}

int delete_PMD(MODEL *model)
{
	int i, j;
	
	model->header.name[0]='\0';
	model->header.comment[0]='\0';
	
	FREE(model->vt);
	model->vt_count = 0;
	model->vt =NULL;
	
	model->vt_index_count = 0;
	FREE(model->vt_index);
	
	model->vt_index = NULL;
	
	FREE(model->mat);
	model->mat_count = 0;
	model->mat = NULL;
	
	FREE(model->bone);
	model->bone_count = 0;
	model->bone = NULL;
	
	#ifdef DEBUGC
		printf("ボーン\n");
	#endif
	for(i=0; i<model->IK_count; i++){
		FREE(model->IK_list[i].IKCBone_index);
		model->IK_list[i].IKCBone_index = NULL;
	}
	model->IK_count = 0;
	
	
	FREE(model->IK_list);
	model->IK_list = NULL;
	
	
	#ifdef DEBUGC
		printf("IKリスト\n");
	#endif
	for(i=0; i<model->skin_count; i++){
		#ifdef DEBUG
			printf("%d \n", i);
		#endif
		FREE(model->skin[i].data);
		model->skin[i].data = NULL;
	}
	
	printf("%p %p\n", model->skin_index, model->skin);
	
	FREE(model->skin);
	model->skin_count = 0;
	model->skin = NULL;
	
	FREE(model->skin_index);
	model->skin_disp_count = 0;
	model->skin_index = NULL;
	
	FREE(model->bone_group);
	model->bone_group_count = 0;
	model->bone_group = NULL;
	
	FREE(model->bone_disp);
	model->bone_disp_count = 0;
	model->bone_disp = NULL;
	
	model->eng_support = 0;
	
	for(i=0; i<10; i++){
		j=i+1;
		/**model->toon[i] = '\0';
		*model->toon_path[i] = '\0';
		*/
		sprintf(model->toon[i], "toon%02d.bmp\0", j);
		sprintf(model->toon_path[i], "toon%02d.bmp\0", j);
	}
	
	FREE(model->rbody);
	model->rbody_count = 0;
	model->rbody = NULL;
	
	FREE(model->joint);
	model->joint_count = 0;
	model->joint = NULL;
	
	return 0;
}

int copy_PMD(MODEL *out, MODEL *model)
{
	int i;
	size_t size;
	int tmp[3];
	
	out->header =model->header;
	
	out->vt_count = model->vt_count;
	out->vt = (VERTEX*)MALLOC((size_t)model->vt_count * sizeof(VERTEX));
	if(out->vt==NULL)return -1;
	for(i=0; i<model->vt_count; i++){
		out->vt[i] = model->vt[i];
	}
	#ifdef DEBUG
		printf("頂点\n");
	#endif
	//面頂点
	out->vt_index_count = model->vt_index_count;
	out->vt_index = (unsigned short*)MALLOC((size_t)model->vt_index_count * sizeof(unsigned short));
	if(out->vt_index == NULL)return -1;
	for(i=0; i<model->vt_index_count; i++){
		out->vt_index[i] = model->vt_index[i];
	}
	
	#ifdef DEBUG
		printf("面頂点\n");
	#endif
	//材質
	out->mat_count = model->mat_count;
	out->mat = (MATERIAL*)MALLOC((size_t)model->mat_count * sizeof(MATERIAL));
	if(out->mat==NULL)return -1;
	for(i=0; i<model->mat_count; i++){
		out->mat[i] = model->mat[i];
	}
	
	#ifdef DEBUG
		printf("材質\n");
	#endif
	//ボーン
	out->bone_count = model->bone_count;
	out->bone = (BONE*)MALLOC((size_t)model->bone_count * sizeof(BONE));
	if(out->bone==NULL)return -1;
	for(i=0; i<model->bone_count; i++){
		out->bone[i] = model->bone[i];
	}
	#ifdef DEBUG
		printf("ボーン\n");
	#endif
	//IKリスト
	out->IK_count = model->IK_count;
	out->IK_list = (IK_LIST*)MALLOC((size_t)model->IK_count * sizeof(IK_LIST));
	if(out->IK_list==NULL)return -1;
	for(i=0; i<model->IK_count; i++){
		out->IK_list[i] = model->IK_list[i];
		size = (size_t)model->IK_list[i].IK_chain_len * sizeof(unsigned short);
		out->IK_list[i].IKCBone_index = (unsigned short*)MALLOC(size);
		if(out->IK_list[i].IKCBone_index==NULL)return -1;
		memcpy(out->IK_list[i].IKCBone_index, model->IK_list[i].IKCBone_index, size);
	}
	#ifdef DEBUG
		printf("IKリスト\n");
	#endif
	
	//表情
	out->skin_count = model->skin_count;
	out->skin = (SKIN*)MALLOC((size_t)model->skin_count * sizeof(SKIN));
	if(out->skin==NULL)return -1;
	for(i=0; i<model->skin_count; i++){
		out->skin[i] = model->skin[i];
		size = (size_t)out->skin[i].skin_vt_count * sizeof(SKIN_DATA);
		out->skin[i].data = (SKIN_DATA*)MALLOC(size);
		if(out->skin[i].data==NULL)return -1;
		memcpy(out->skin[i].data, model->skin[i].data, size);
	}
	#ifdef DEBUG
		printf("表情\n");
	#endif
	
	//表情表示
	out->skin_disp_count = model->skin_disp_count;
	out->skin_index = (unsigned short*)MALLOC((size_t)model->skin_disp_count * sizeof(unsigned short));
	if(out->skin_index==NULL)return -1;
	for(i=0; i<model->skin_disp_count; i++){
		out->skin_index[i] = model->skin_index[i];
	}
	
	#ifdef DEBUG
		printf("表情表示\n");
	#endif
	//ボーン表示グループ
	out->bone_group_count = model->bone_group_count;
	out->bone_group = (BONE_GROUP*)MALLOC((size_t)model->bone_group_count * sizeof(BONE_GROUP));
	if(out->bone_group==NULL)return -1;
	for(i=0; i<model->bone_group_count; i++){
		out->bone_group[i] = model->bone_group[i];
	}
	#ifdef DEBUG
		printf("ボーン表示グループ\n");
	#endif
	
	//表示ボーン
	size = (size_t)model->bone_disp_count * sizeof(BONE_DISP);
	out->bone_disp_count = model->bone_disp_count;
	out->bone_disp = (BONE_DISP*)MALLOC(size);
	if(out->bone_disp==NULL)return -1;
	memcpy(out->bone_disp, model->bone_disp, size);
	/*
	for(i=0; i<model->bone_disp_count; i++){
		out->bone_disp[i] = model->bone_disp[i];
	}
	*/
	#ifdef DEBUG
		printf("表示ボーン\n");
	#endif
	
	memcpy(out->toon, model->toon, sizeof(char)*10*100);
	memcpy(out->toon_path, model->toon_path, sizeof(char)*10*NAME_LEN);
	
	//英名
	out->eng_support = model->eng_support;
	#ifdef DEBUG
		printf("英名\n");
	#endif
	
	//剛体
	out->rbody_count = model->rbody_count;
	out->rbody = (RIGID_BODY*)MALLOC((size_t)model->rbody_count * sizeof(RIGID_BODY));
	if(out->rbody==NULL)return -1;
	for(i=0; i<model->rbody_count; i++){
		out->rbody[i] = model->rbody[i];
	}
	#ifdef DEBUG
		printf("剛体\n");
	#endif
	//ジョイント
	out->joint_count = model->joint_count;
	out->joint = (JOINT*)MALLOC((size_t)model->joint_count * sizeof(JOINT));
	if(out->joint==NULL)return -1;
	for(i=0; i<model->joint_count; i++){
		out->joint[i] = model->joint[i];
	}
	#ifdef DEBUG
		printf("ジョイント\n");
	#endif
	
	return 0;
}

int add_PMD(MODEL *model, MODEL *add)
{
	
	int i, j, k;
	int size;
	int tmp[3];
	unsigned int bone_count;
	BONE *bone;
	
	unsigned int vt_count;
	VERTEX *vt;
	unsigned int vt_index_count;
	unsigned short *vt_index;
	unsigned int mat_count;
	MATERIAL *mat;
	unsigned short IK_count;
	IK_LIST *IK_list;
	unsigned short skin_count;
	SKIN *skin;
	unsigned char skin_disp_count;
	unsigned short *skin_index;
	unsigned char bone_group_count;
	BONE_GROUP *bone_group;
	unsigned int bone_disp_count;
	BONE_DISP *bone_disp;
	//ENGLISH eg;
	unsigned int rbody_count;
	RIGID_BODY *rbody;
	unsigned int joint_count;
	JOINT *joint;
	
	//頂点
	vt_count = model->vt_count + add->vt_count;
	
	vt = (VERTEX*)MALLOC((size_t)vt_count * sizeof(VERTEX));
	if(vt==NULL)return -1;
	
	for(i=0; i<model->vt_count; i++){
		vt[i] = model->vt[i];
	}
	j = 0;
	
	for(i=model->vt_count; i<vt_count; i++){
		vt[i] = add->vt[j];
		for(k=0; k<2; k++){
			vt[i].bone_num[k] += model->bone_count;
		}
		j++;
	}
	
	#ifdef DEBUG
		printf("頂点\n");
	#endif
	
	//面頂点
	vt_index_count = model->vt_index_count + add->vt_index_count;
	vt_index = (unsigned short*)MALLOC((size_t)vt_index_count * sizeof(unsigned short));
	if(vt_index==NULL)return -1;
	for(i=0; i<model->vt_index_count; i++){
		vt_index[i] = model->vt_index[i];
	}
	j = 0;
	for(i=model->vt_index_count; i<vt_index_count; i++){
		vt_index[i] = add->vt_index[j] + model->vt_count;
		j++;
	}
	
	#ifdef DEBUG
		printf("面頂点\n");
	#endif
	
	//材質
	mat_count = model->mat_count + add->mat_count;
	mat = (MATERIAL*)MALLOC((size_t)mat_count * sizeof(MATERIAL));
	if(mat==NULL)return -1;
	
	for(i=0; i<model->mat_count; i++){
		mat[i] = model->mat[i];
	}
	j = 0;
	
	for(i=model->mat_count; i<mat_count; i++){
		mat[i] = add->mat[j];
		j++;
	}
	
	#ifdef DEBUG
		printf("材質\n");
	#endif
	
	//ボーン
	bone_count = model->bone_count + add->bone_count;
	bone = (BONE*)MALLOC((size_t)bone_count * sizeof(BONE));
	if(bone==NULL)return -1;
	for(i=0; i<model->bone_count; i++){
		bone[i] = model->bone[i];
	}
	j=0;
	for(i=model->bone_count; i<bone_count; i++){
		bone[i] = add->bone[j];
		if(bone[i].PBone_index != USHORT_MAX)
		bone[i].PBone_index = bone[i].PBone_index + model->bone_count;
		if(bone[i].TBone_index != 0)
		bone[i].TBone_index = bone[i].TBone_index + model->bone_count;
		if(bone[i].IKBone_index != 0)
		bone[i].IKBone_index = bone[i].IKBone_index + model->bone_count;
		j++;
	}
	#ifdef DEBUG
		printf("ボーン\n");
	#endif
	//IKリスト
	IK_count = model->IK_count + add->IK_count;
	IK_list = (IK_LIST*)MALLOC((size_t)IK_count * sizeof(IK_LIST));
	if(IK_list==NULL)return -1;
	for(i=0; i<model->IK_count; i++){
		IK_list[i] = model->IK_list[i];
		size = IK_list[i].IK_chain_len * sizeof(unsigned short);
		IK_list[i].IKCBone_index = (unsigned short*)MALLOC(size);
		if(IK_list[i].IKCBone_index==NULL)return -1;
		memcpy(IK_list[i].IKCBone_index, model->IK_list[i].IKCBone_index, size);
	}
	j = 0;
	for(i=model->IK_count; i<IK_count; i++){
		IK_list[i] = add->IK_list[j];
		IK_list[i].IKBone_index = IK_list[i].IKBone_index + model->bone_count;
		IK_list[i].IKTBone_index = IK_list[i].IKTBone_index + model->bone_count;
		size = IK_list[i].IK_chain_len * sizeof(unsigned short);
		IK_list[i].IKCBone_index = (unsigned short*)MALLOC(size);
		if(IK_list[i].IKCBone_index==NULL)return -1;
		memcpy(IK_list[i].IKCBone_index, add->IK_list[j].IKCBone_index, size);
		for(k=0; k<IK_list[i].IK_chain_len; k++){
			IK_list[i].IKCBone_index[k] = IK_list[i].IKCBone_index[k] + model->bone_count;
		}
		j++;
	}
	
	#ifdef DEBUG
		printf("IKリスト\n");
	#endif
	
	//表情
	skin_count = model->skin_count + add->skin_count;
	skin = (SKIN*)MALLOC((size_t)skin_count * sizeof(SKIN));
	if(skin==NULL)return -1;
	
	if(add->skin_count == 0){
		for(i=0; i<skin_count; i++){
			skin[i] = model->skin[i];
			size = (size_t)skin[i].skin_vt_count * sizeof(SKIN_DATA);
			skin[i].data = (SKIN_DATA*)MALLOC(size);
			if(skin[i].data == NULL)return -1;
			memcpy(skin[i].data, model->skin[i].data, size);
		}
		
	}else if(model->skin_count == 0){
		for(i=0; i<skin_count; i++){
			skin[i] = add->skin[i];
			size = (size_t)skin[i].skin_vt_count * sizeof(SKIN_DATA);
			skin[i].data = (SKIN_DATA*)MALLOC(size);
			if(skin[i].data == NULL)return -1;
			memcpy(skin[i].data, add->skin[i].data, size);
		}
		for(i=0; i<skin[0].skin_vt_count; i++){
			skin[0].data[i].index = skin[0].data[i].index + model->vt_count;
		}
		
	}else if(model->skin_count != 0 && add->skin_count != 0){
		skin_count--;
		skin[0] = model->skin[0];
		skin[0].skin_vt_count = model->skin[0].skin_vt_count + add->skin[0].skin_vt_count;
		size = (size_t)skin[0].skin_vt_count * sizeof(SKIN_DATA);
		skin[0].data = (SKIN_DATA*)MALLOC(size);
		if(skin[0].data == NULL)return -1;
		tmp[0] = model->skin[0].skin_vt_count;
		memcpy(skin[0].data, model->skin[0].data, tmp[0] * sizeof(SKIN_DATA));
		memcpy(&skin[0].data[tmp[0]], add->skin[0].data, add->skin[0].skin_vt_count * sizeof(SKIN_DATA));
		//baseの合成
		
		for(i=0; i < model->skin[0].skin_vt_count; i++){
			skin[0].data[i].index = model->skin[0].data[i].index;
		}
		//printf("%d %d %d\n", skin[0].skin_vt_count, model->skin[0].skin_vt_count, add->skin[0].skin_vt_count);
		j = 0;
		for(i = model->skin[0].skin_vt_count; i < skin[0].skin_vt_count; i++){
			//printf("%d \n", i);
			skin[0].data[i].index = add->skin[0].data[j].index + model->vt_count;
			j++;
		}
		//表情追加
		for(i=1; i<model->skin_count; i++){
			skin[i] = model->skin[i];
			size = (size_t)skin[i].skin_vt_count * sizeof(SKIN_DATA);
			skin[i].data = (SKIN_DATA*)MALLOC(size);
			if(skin[i].data == NULL)return -1;
			memcpy(skin[i].data, model->skin[i].data, size);
			//printf("%d %d \n", i, size);
		}
		j = 1;
		for(i=model->skin_count; i<skin_count; i++){
			//printf("%d\n", j);
			skin[i] = add->skin[j];
			size = (size_t)skin[i].skin_vt_count * sizeof(SKIN_DATA);
			skin[i].data = (SKIN_DATA*)MALLOC(size);
			if(skin[i].data == NULL)return -1;
			memcpy(skin[i].data, add->skin[j].data, size);
			for(k=0; k < skin[i].skin_vt_count; k++){
				skin[i].data[k].index = skin[i].data[k].index + model->skin[0].skin_vt_count;
			}
			j++;
		}
	}
	#ifdef DEBUG
		printf("表情\n");
	#endif
	
	//表情表示
	skin_disp_count = model->skin_disp_count + add->skin_disp_count;
	skin_index = (unsigned short*)MALLOC((size_t)skin_disp_count * sizeof(unsigned short));
	if(skin_index == NULL)return -1;
	//for(i=0; i<model->skin_disp_count; i++){
	//	skin_index[i] = model->skin_index[i];
	//}
	memcpy(skin_index, model->skin_index, model->skin_disp_count * sizeof(unsigned short));
	j = 0;
	for(i=model->skin_disp_count; i<skin_disp_count; i++){
		skin_index[i] = add->skin_index[j] + model->skin_disp_count;
		j++;
	}
	
	#ifdef DEBUG
		printf("表情表示\n");
	#endif
	//ボーン表示
	bone_group_count = model->bone_group_count + add->bone_group_count;
	bone_group = (BONE_GROUP*)MALLOC((size_t)bone_group_count * sizeof(BONE_GROUP));
	if(bone_group == NULL)return -1;
	for(i=0; i<model->bone_group_count; i++){
		bone_group[i] = model->bone_group[i];
	}
	j = 0;
	for(i=model->bone_group_count; i<bone_group_count; i++){
		bone_group[i] = add->bone_group[j];
		j++;
	}
	
	bone_disp_count = model->bone_disp_count + add->bone_disp_count;
	bone_disp = (BONE_DISP*)MALLOC((size_t)bone_disp_count * sizeof(BONE_DISP));
	for(i=0; i<model->bone_disp_count; i++){
		bone_disp[i] = model->bone_disp[i];
	}
	j = 0;
	for(i=model->bone_disp_count; i<bone_disp_count; i++){
		bone_disp[i].index = add->bone_disp[j].index + model->bone_count;
		bone_disp[i].bone_group = add->bone_disp[j].bone_group + model->bone_group_count;
		j++;
		//printf("%d %d %d %d\n", add->bone_disp[j].index, add->bone_disp[j].bone_group, bone_disp[i].index, bone_disp[i].bone_group);
	}
	
	#ifdef DEBUG
		printf("ボーン表示\n");
	#endif
	
	//英名
	model->eng_support = add->eng_support;
	
	#ifdef DEBUG
		printf("英名\n");
	#endif
	
	//剛体
	rbody_count = model->rbody_count + add->rbody_count;
	rbody = (RIGID_BODY*)MALLOC((size_t)rbody_count * sizeof(RIGID_BODY));
	
	for(i=0; i<model->rbody_count; i++){
		rbody[i] = model->rbody[i];
	}
	j=0;
	for(i=model->rbody_count; i<rbody_count; i++){
		rbody[i] = add->rbody[j];
		rbody[i].bone = rbody[i].bone + model->bone_count;
		j++;
	}
	#ifdef DEBUG
		printf("剛体\n");
	#endif
	//ジョイント
	joint_count = model->joint_count + add->joint_count;
	joint = (JOINT*)MALLOC((size_t)joint_count * sizeof(JOINT));
	
	for(i=0; i<model->joint_count; i++){
		joint[i] = model->joint[i];
	}
	j=0;
	for(i=model->joint_count; i<joint_count; i++){
		joint[i] = add->joint[j];
		for(k=0; k<2; k++){
			joint[i].rbody[k] = joint[i].rbody[k] + model->rbody_count;
		}
		j++;
	}
	
	#ifdef DEBUG
		printf("ジョイント\n");
	#endif
	
	#ifdef MEM_DBG
		printf("free %p\n", model->vt);
	#endif
	FREE(model->vt);
	model->vt = vt;
	model->vt_count = vt_count;
	
	#ifdef MEM_DBG
		printf("free %p\n", model->vt_index);
	#endif
	FREE(model->vt_index);
	model->vt_index = vt_index;
	model->vt_index_count = vt_index_count;
	
	#ifdef MEM_DBG
		printf("free %p\n", model->mat);
	#endif
	FREE(model->mat);
	model->mat = mat;
	model->mat_count = mat_count;
	
	#ifdef MEM_DBG
		printf("free %p\n", model->bone);
	#endif
	FREE(model->bone);
	model->bone = bone;
	model->bone_count = bone_count;
	
	for(i=0; i<model->IK_count; i++){
		#ifdef MEM_DBG
			printf("free %p\n", model->IK_list[i].IKCBone_index);
		#endif
		FREE(model->IK_list[i].IKCBone_index);
		model->IK_list[i].IKCBone_index = NULL;
	}
	#ifdef MEM_DBG
		printf("free %p\n", model->IK_list);
	#endif
	FREE(model->IK_list);
	model->IK_list = IK_list;
	model->IK_count = IK_count;
	
	
	for(i=0; i<model->skin_count; i++){
		#ifdef MEM_DBG
			printf("free %p\n", model->skin[i].data);
		#endif
		FREE(model->skin[i].data);
		model->skin[i].data = NULL;
	}
	#ifdef MEM_DBG
		printf("free %p\n", model->skin);
	#endif
	FREE(model->skin);
	model->skin = skin;
	model->skin_count = skin_count;
	
	#ifdef MEM_DBG
		printf("free %p\n", model->skin_index);
	#endif
	FREE(model->skin_index);
	model->skin_index = skin_index;
	model->skin_disp_count = skin_disp_count;
	
	#ifdef MEM_DBG
		printf("free %p\n", model->bone_group);
	#endif
	FREE(model->bone_group);
	model->bone_group = bone_group;
	model->bone_group_count = bone_group_count;
	
	#ifdef MEM_DBG
		printf("free %p\n", model->bone_disp);
	#endif
	FREE(model->bone_disp);
	model->bone_disp = bone_disp;
	model->bone_disp_count = bone_disp_count;
	
	#ifdef MEM_DBG
		printf("free %p\n", model->rbody);
	#endif
	FREE(model->rbody);
	model->rbody = rbody;
	model->rbody_count = rbody_count;
	
	#ifdef MEM_DBG
		printf("free %p\n", model->joint);
	#endif
	FREE(model->joint);
	model->joint = joint;
	model->joint_count = joint_count;
	
	return 0;
}

int listup_bone(MODEL *model, const char file_name[]){
	int i;
	char str[64], *p;
	
	FILE *txt;
	
	if(strcmp(file_name, "") == 0){
		printf("ファイル名がありません\n");
		return 1;
	}
	txt = fopen(file_name,"w");
	if(txt == NULL  ){
		fprintf(txt, "出力テキストファイルを開けません\n");
		return 1;
	}
	
	if(model->eng_support == 0){
		printf("リスト出力ができるのは英名対応モデルのみです\n");
		return 1;
	}
	
	fprintf(txt, "%s \n %f \n %s \n %s \n",
		model->header.magic,
		model->header.version,
		model->header.name,
		model->header.comment);
	
	fprintf(txt, "ボーン数:%d\n", model->bone_count);
	
	for(i=0; i<model->bone_count; i++){
		fprintf(txt, "%s %s\n", model->bone[i].name, model->bone[i].name_eng);
	}
	
	fprintf(txt, "表情数:%d\n", model->skin_count);
	for(i=0; i<model->skin_count; i++){
		fprintf(txt, "%s %s\n", model->skin[i].name, model->skin[i].name_eng);
	}
	fprintf(txt, "ボーン枠数:%d\n", model->bone_group_count);
	for(i=0; i<model->bone_group_count; i++){
		strcpy(str, model->bone_group[i].name);
		p = strchr( str, '\n' );
		if(p != NULL)*p = '\0';
		fprintf(txt, "%s %s\n", str, model->bone_group[i].name_eng);
	}
	
	fclose(txt);
	
	return 0;
}


int get_file_name(char file_name[])
{
	int i;
	char input[256];
	printf("ファイル名:");
	gets(input);
	if(input[0] == '\"'){
		for(i=1; i<256; i++){
			file_name[i-1] = input[i];
			if(input[i] == '\"'){
				file_name[i-1] = '\0';
				input[i] = '\0';
				break;
			}else if(input[i] == '\0'){
				break;
			}
		}
	}else{
		strcpy(file_name, input);
	}
	
	return 0;
}


