/*PMD関係のライブラリ、PMD編集など */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

//#define DLLExport edit
#include "mPMD.h"


int translate(MODEL *model, LIST *list, short mode)
{
	int i,j;
	char str[NAME_LEN], *p;
	/*
	モード1 英名追加
	モード2 日本語名を英語名に(ボーン、スキンのみ)
	モード3 英語名を日本語名に(ボーン、スキンのみ)
	*/
	
	
	
	if(mode == 1){
		
		if(model->eng_support != 1){
			model->eng_support = 1;
			//sprintf(model->skin[0].name, "base");
			/*puts("test point");
			FREE(model->skin);
			puts("end test");
			exit(1);*/
			strncpy(model->header.name_eng, model->header.name, NAME_LEN);
			strncpy(model->header.comment_eng, model->header.comment, 256);
		}
		
		for(i=0; i<model->bone_count; i++){
			for(j=0; j<list->bone_count; j++){
				if(strcmp(model->bone[i].name, list->bone[j]) == 0){
					strncpy(model->bone[i].name_eng, list->bone_eng[j], NAME_LEN);
					j = -1;
					break;
				}
			}
			if(j != -1){
				if(model->bone[i].name[0] == '\0'){
					strncpy(model->bone[i].name_eng, model->bone[i].name, NAME_LEN);
				}
			
			}
		}
		#ifdef DEBUG
			printf("bone\n");
		#endif
		
		for(i=1; i<model->skin_count; i++){
			for(j=1; j<list->skin_count; j++){
				if(strcmp(model->skin[i].name, list->skin[j]) == 0){
					strncpy(model->skin[i].name_eng, list->skin_eng[j], NAME_LEN);
					j = -1;
					break;
				}
			}
			if(j != -1){
				strncpy(model->skin[i].name_eng, model->skin[i].name, NAME_LEN);
			}
		}
		
		#ifdef DEBUG
			printf("skin\n");
		#endif
		for(i=0; i<model->bone_group_count; i++){
			
			strncpy(str, model->bone_group[i].name, NAME_LEN);
			p = strchr( str, '\n' );
			if(p != NULL)*p = '\0';
			j = 0;
			for(j=0; j<list->disp_count; j++){
				if(strcmp(str, list->disp[j]) == 0){
					strncpy(model->bone_group[i].name_eng, list->disp_eng[j], NAME_LEN);
					j = -1;
					break;
				}
			}
			#ifdef DEBUG
				printf("%d ",i);
			#endif
			if(j != -1){
				strncpy(model->bone_group[i].name_eng, str, NAME_LEN);
			}
		}
		
		#ifdef DEBUG
			printf("\nbone表示枠\n");
		#endif
		
	}else if(mode == 2){
		for(i=0; i<model->bone_count; i++){
			for(j=0; j<list->bone_count; j++){
				if(strcmp(model->bone[i].name, list->bone[j]) == 0){
					strncpy(model->bone[i].name, list->bone_eng[j], NAME_LEN);
					j = -1;
					break;
				}
			}
			if(j != -1 && model->eng_support == 1){
				strncpy(model->bone[i].name, model->bone[i].name_eng, NAME_LEN);
			}
		}
		for(i=0; i<model->skin_count; i++){
			for(j=0; j<list->skin_count; j++){
				if(strcmp(model->skin[i].name, list->skin[j]) == 0){
					strncpy(model->skin[i].name, list->skin_eng[j], NAME_LEN);
					j = -1;
					break;
				}
			}
			if(j != -1 && model->eng_support == 1){
				strncpy(model->skin[i].name, model->skin[i].name_eng, NAME_LEN);
			}
		}
	}else if(mode == 3){
		for(i=0; i<model->bone_count; i++){
			for(j=0; j<list->bone_count; j++){
				if(strcmp(model->bone[i].name, list->bone_eng[j]) == 0){
					strncpy(model->bone[i].name, list->bone[j], NAME_LEN);
					j = -1;
					break;
				}
			}
		}
		for(i=0; i<model->skin_count; i++){
			for(j=0; j<list->skin_count; j++){
				if(strcmp(model->skin[i].name, list->skin_eng[j]) == 0){
					strncpy(model->skin[i].name, list->skin[j], NAME_LEN);
					j = -1;
					break;
				}
			}
		}
	}
	
	
	
	return 0;
}

int sort_bone(MODEL *model, LIST *list)
{
		int i, j;
	int tmp;
	int *index;
	BONE *bone;
	
	
	
	index = malloc(model->bone_count * sizeof(int));
	bone = malloc(model->bone_count * sizeof(BONE));
	
	#ifdef MEM_DBG
		printf("malloc %p %p\n", index, bone);
	#endif
	
	for(i=0; i<model->bone_count; i++){
		index[i] = -1;	//リストに無いボーンには-1
		for(j=0; j<list->bone_count; j++){
			if(strcmp(list->bone[j], model->bone[i].name) == 0){
				index[i] = j;	//indexにリスト中の番号を代入
				break;
			}
		}
		#ifdef DEBUG
			printf("index[%d]=%d\n", i, index[i]);
		#endif
	}
	
	tmp = 0;
	for(i=0; i<list->bone_count; i++){
		for(j=0; j<model->bone_count; j++){
			if(index[j] == i){	//indexにiが存在したら
				//printf("index[%d]に%dが存在します\n", j, i);
				index[j] = index[j] - tmp;
				j = -1;
				break;
			}
		}
		if(j != -1){
			tmp++;
		}
	}
	tmp = -1;
	for(i=0; i<model->bone_count; i++){
		if(tmp < index[i]){
			tmp = index[i];	//indexの最大値を見つける
		}
	}
	tmp++;
	for(i=0; i<model->bone_count; i++){
		if(strcmp(model->bone[i].name, "-0") == 0){
			index[i] = model->bone_count-1;
		}else if(index[i] == -1){
			index[i] = tmp;
			tmp++;
		}
	}
	
	//親子関係修正
	
	{
		int tmp_PBone_index;
		for(i=0;  i<model->bone_count; i++){
			if(model->bone[i].PBone_index != 65535 &&
			   index[model->bone[i].PBone_index] > index[i] &&
			   strcmp(model->bone[i].name, "-0")!=0){
				
				tmp = index[model->bone[i].PBone_index];
				tmp_PBone_index = index[i];
				for(j=0; j<model->bone_count; j++){
					if(index[j] >= tmp_PBone_index && index[j] < tmp){
						index[j]++;	//一つ後ろにずらす
					}
				}
				index[model->bone[i].PBone_index] = tmp_PBone_index;	//ボーンの一つ前に親ボーンを移動
			}
		}
	}
	
	for(i=0; i<model->bone_count; i++){	//ボーン並び変え
		#ifdef DEBUG
			printf("index[%d]=%d\n", i, index[i]);
		#endif
		strcpy(bone[index[i]].name, model->bone[i].name);
		strcpy(bone[index[i]].name_eng, model->bone[i].name_eng);
		if(model->bone[i].PBone_index == 65535){
			bone[index[i]].PBone_index = 65535;
		}else{
			bone[index[i]].PBone_index = index[model->bone[i].PBone_index];
		}
		if(model->bone[i].TBone_index == 0){
			bone[index[i]].TBone_index = 0;
		}else{
			bone[index[i]].TBone_index = index[model->bone[i].TBone_index];
		}
		
		bone[index[i]].type = model->bone[i].type;
		
		if(model->bone[i].IKBone_index == 0){
			bone[index[i]].IKBone_index = 0;
		}else{
			bone[index[i]].IKBone_index = index[model->bone[i].IKBone_index];
		}
		for(j=0; j<3; j++){
			bone[index[i]].loc[j] = model->bone[i].loc[j];
		}
	}
	
	update_bone_index(model, index);
	
	#ifdef MEM_DBG
		printf("FREE %p %p\n", index, model->bone);
	#endif
	FREE(model->bone);
	FREE(index);
	
	model->bone = bone;
	
	if(strcmp(model->bone[model->bone_count-1].name, "-0") == 0){
		model->bone_count--;
	}
	return 0;
}

int update_bone_index(MODEL *model,int index[])
{
	int i, j;
	unsigned short (*tmp_vt)[2];
	IK_LIST *tmp_ik;
	unsigned short *tmp_disp;
	char (*tmp_eng)[20];
	unsigned short *tmp_rb;
	
	//頂点のボーン番号を書き換え
	tmp_vt = malloc(sizeof(unsigned short)*2*model->vt_count);
	#ifdef MEM_DBG
		printf("malloc %p\n", tmp_vt);
	#endif
	
	
	for(i=0; i<model->vt_count; i++){
		for(j=0; j<2; j++){
			tmp_vt[i][j] = model->vt[i].bone_num[j];
		}
	}
	for(i=0; i<model->vt_count; i++){
		for(j=0; j<2; j++){
			model->vt[i].bone_num[j] = index[tmp_vt[i][j]];
		}
	}
	#ifdef MEM_DBG
		printf("FREE %p\n", tmp_vt);
	#endif
	FREE(tmp_vt);
	
	//IKリストのボーン番号を書き換え
	tmp_ik = (IK_LIST*)malloc(sizeof(IK_LIST)*model->IK_count);
	#ifdef MEM_DBG
		printf("malloc %p\n", tmp_ik);
	#endif
	memcpy(tmp_ik, model->IK_list, sizeof(IK_LIST)*model->IK_count);
	for(i=0; i<model->IK_count; i++){
		tmp_ik[i].IKCBone_index = (unsigned short*)malloc(sizeof(unsigned short)*tmp_ik[i].IK_chain_len);
		#ifdef MEM_DBG
			printf("malloc %p\n", tmp_ik[i].IKCBone_index);
		#endif
		memcpy(tmp_ik[i].IKCBone_index, model->IK_list[i].IKCBone_index, sizeof(unsigned short)*tmp_ik[i].IK_chain_len);
	}
	for(i=0; i<model->IK_count; i++){
		#ifdef DEBUG
			printf("%d \n", i);
		#endif
		model->IK_list[i].IKBone_index = index[tmp_ik[i].IKBone_index];
		model->IK_list[i].IKTBone_index = index[tmp_ik[i].IKTBone_index];
		for(j=0; j<tmp_ik[i].IK_chain_len; j++){
			model->IK_list[i].IKCBone_index[j] = index[tmp_ik[i].IKCBone_index[j]];
		}
		#ifdef MEM_DBG
			printf("FREE %p\n", tmp_ik[i].IKCBone_index);
		#endif
		FREE(tmp_ik[i].IKCBone_index);
	}
	//printf("%d\n", tmp_ik[i].IK_chain_len);getch();
	#ifdef MEM_DBG
		printf("FREE %p\n", tmp_ik);
	#endif
	FREE(tmp_ik);
	
	
	//表示ボーン番号を書き換え
	tmp_disp = (unsigned short*)malloc(sizeof(unsigned short)*model->bone_disp_count);
	#ifdef MEM_DBG
		printf("malloc %p\n", tmp_disp);
	#endif
	for(i=0; i<model->bone_disp_count; i++){
		tmp_disp[i] = model->bone_disp[i].index;
	}
	
	for(i=0; i<model->bone_disp_count; i++){
		model->bone_disp[i].index = index[tmp_disp[i]];
	}
	#ifdef MEM_DBG
		printf("FREE %p\n", tmp_disp);
	#endif
	FREE(tmp_disp);
	
	
	#ifdef DEBUG
		puts("剛体書き換え");
	#endif
	
	//剛体ボーン番号を書き換え
	tmp_rb = (unsigned short*)malloc(sizeof(unsigned short)*model->rbody_count);
	#ifdef MEM_DBG
		printf("malloc %p\n", tmp_rb);
	#endif
	
	for(i=0; i<model->rbody_count; i++){
		#ifdef DEBUG
			printf("%d %d\n", i, model->rbody[i].bone);
		#endif
		tmp_rb[i] = model->rbody[i].bone;
	}
	for(i=0; i<model->rbody_count; i++){
		#ifdef DEBUG
			printf("%d\n", i);
		#endif
		if(tmp_rb[i] == USHORT_MAX){
			model->rbody[i].bone = USHORT_MAX;
		}else{
			model->rbody[i].bone = index[tmp_rb[i]];
		}
	}
	#ifdef MEM_DBG
		printf("FREE %p\n", tmp_rb);
	#endif
	FREE(tmp_rb);
	#ifdef DEBUG
		puts("ボーンインデックス更新完了");
	#endif
	
	return 0;
}

int sort_skin(MODEL *model, LIST *list)
{
	int i, j;
	int tmp;
	int *index;
	SKIN *skin;
	
	index = (int*)MALLOC(model->skin_count * sizeof(int));
	skin = (SKIN*)MALLOC(model->skin_count * sizeof(SKIN));
	
	#ifdef MEM_DBG
		printf("malloc %p %p\n", index, skin);
	#endif
	
	for(i=0; i<model->skin_count; i++){
		index[i] = -1;	//リストに無い表情には-1
		for(j=0; j<list->skin_count; j++){
			if(strcmp(list->skin[j], model->skin[i].name) == 0){
				index[i] = j;	//indexにリスト中の番号を代入
				break;
			}
		}
		#ifdef DEBUG
			printf("index[%d]=%d\n", i, index[i]);
		#endif
	}
	
	tmp = 0;
	for(i=0; i<list->skin_count; i++){
		for(j=0; j<model->skin_count; j++){
			if(index[j] == i){	//indexにiが存在したら
				//printf("index[%d]に%dが存在します\n", j, i);
				index[j] = index[j] - tmp;
				j = -1;
				break;
			}
		}
		if(j != -1){
			tmp++;
		}
	}
	tmp = -1;
	for(i=0; i<model->skin_count; i++){
		if(tmp < index[i]){
			tmp = index[i];	//indexの最大値を見つける
		}
	}
	tmp++;
	for(i=0; i<model->skin_count; i++){
		if(index[i] == -1){
			index[i] = tmp;
			tmp++;
		}
	}
	
	for(i=0; i<model->skin_count; i++){	//表情並び変え
		#ifdef DEBUG
			printf("index[%d]=%d\n", i, index[i]);
		#endif
		skin[index[i]] = model->skin[i];
	}
	
	for(i=0; i<model->skin_disp_count; i++){	//表情並び変え
		model->skin_index[i] = i+1;
	}
	
	#ifdef MEM_DBG
		printf("FREE %p %p\n", index, model->skin);
	#endif
	FREE(model->skin);
	FREE(index);
	
	model->skin = skin;
	
	return 0;
}

int sort_disp(MODEL *model, LIST *list)
{
	int i, j;
	int tmp;
	int *index;
	BONE_GROUP *bone_group, tmpg;
	char *p;
	
	
	BONE_DISP *bone_disp;
	
	index = (int*)MALLOC(model->bone_group_count * sizeof(int));
	bone_group = (BONE_GROUP*)MALLOC(model->bone_group_count * sizeof(BONE_GROUP));
	bone_disp = (BONE_DISP*)MALLOC(model->bone_disp_count * sizeof(BONE_DISP));
	#ifdef MEM_DBG
		printf("malloc %p %p %p\n", index, bone_group, bone_disp);
	#endif
	
	for(i=0; i<model->bone_group_count; i++){
		index[i] = -1;	//リストに無い枠には-1
		tmpg = model->bone_group[i];
		p = strchr( tmpg.name, '\n' );
		if(p != NULL)*p = '\0';
		for(j=0; j<list->disp_count; j++){
			#ifdef DEBUG
				printf("%d %s : %s\n", j, list->disp[j], tmpg.name);
			#endif
			if(strcmp(list->disp[j], tmpg.name) == 0){
				index[i] = j;	//indexにリスト中の番号を代入
				break;
			}
		}
		#ifdef DEBUG
			printf("index[%d]=%d\n", i, index[i]);
		#endif
	}
	
	tmp = 0;
	for(i=0; i<list->disp_count; i++){
		for(j=0; j<model->bone_group_count; j++){
			if(index[j] == i){	//indexにiが存在したら
				//printf("index[%d]に%dが存在します\n", j, i);
				index[j] = index[j] - tmp;
				j = -1;
				break;
			}
		}
		if(j != -1){
			tmp++;
		}
	}
	tmp = -1;
	for(i=0; i<model->bone_group_count; i++){
		if(tmp < index[i]){
			tmp = index[i];	//indexの最大値を見つける
		}
	}
	tmp++;
	for(i=0; i<model->bone_group_count; i++){
		if(index[i] == -1){
			index[i] = tmp;
			tmp++;
		}
	}
	
	for(i=0; i<model->bone_group_count; i++){	//表示枠並び変え
		#ifdef DEBUG
			printf("index[%d]=%d\n", i, index[i]);
		#endif
		bone_group[index[i]] = model->bone_group[i];
	}
	
	for(i=0; i<model->bone_group_count; i++){	//表示枠コピー
		model->bone_group[i] = bone_group[i];
	}
	
	for(i=0; i<model->bone_disp_count; i++){	//表示ボーン並び変え
		model->bone_disp[i].bone_group = index[model->bone_disp[i].bone_group-1] + 1;
	}
	
	tmp = 0;
	for(i=1; i<=model->bone_group_count; i++){
		for(j=0; j<model->bone_disp_count; j++){
			if(model->bone_disp[j].bone_group == i){
			 	bone_disp[tmp] = model->bone_disp[j];
			 	tmp++;
			 }
		}
	}
	#ifdef DEBUG
		printf("%d\n", tmp);
	#endif
	
	#ifdef MEM_DBG
		printf("FREE %p %p %p\n", index, bone_group, model->bone_disp);
	#endif
	FREE(model->bone_disp);
	FREE(bone_group);
	FREE(index);
	
	model->bone_disp = bone_disp;
	
	return 0;
}

int rename_tail(MODEL *model){
	int i, j, tmp;
	int flag = 0;
	
	//全てのbone tailに"-0"の名前をつける
	for(i=0; i<model->bone_count; i++){
		if(model->bone[i].type == 6 || model->bone[i].type == 7){
			flag = 0;
			for(j=0; j<model->bone_count; j++){
				if (model->bone[j].TBone_index == i){
					flag = 1;
					break;
				}
			}
			if(flag == 1){
				strncpy(model->bone[i].name, "-0", 4);
				strncpy(model->bone[i].name_eng, "-0", 4);
			}
		}
	}
	
	//子ボーンがtailならば+親ボーン名という名前にする
	for(i=0; i<model->bone_count; i++){
		tmp = model->bone[i].TBone_index;
		if(tmp < model->bone_count){
			if(model->bone[tmp].type == 6 || model->bone[tmp].type == 7){
				sprintf(model->bone[tmp].name, "+%s", model->bone[i].name);
				sprintf(model->bone[tmp].name_eng, "+%s", model->bone[i].name_eng);
				//printf("%s\n", model->bone[tmp].name);
			}
		}else{
			printf("範囲外のボーンインデックスを見つけました\n");
		}
	}
	
	return 0;
}

int scale_bone(MODEL *model, int index, double sx, double sy, double sz)
{
	int i, j, k, l;
	
	double vec[3];
	double vec_size;
	double nor_vec[3];
	double mtr[3][3];
	double mtrz[3][3];
	double mtrx[3][3];
	double loc[3] = {0.0, 0.0, 0.0};
	double rot[3];	//ZXY
	double theta;
	
	double tmp[3];
	
	double (*tmp_vt)[3];
	unsigned int len_vt;
	unsigned int *index_vt;
	
	double (*tmp_bone)[3];
	unsigned int len_bone;
	unsigned int *index_bone;
	double (*diff_bone)[3];
	
	//ベクトルがY軸に沿う向きになるようにする
	if(bone_vec(model, index, loc, vec) < 0)return -1;
	
	//ベクトルのノーマライズ
	vec_size = 0.0;
	for(i=0; i<3; i++){
		vec_size = vec_size + vec[i]*vec[i];
	}
	vec_size = sqrt(vec_size);
	for(i=0; i<3; i++){
		nor_vec[i] = vec[i]/vec_size;
	}
	
	//ベクトルのZXY角を求める
	rot[0] = angle_from_vec(vec[0], vec[1]);
	rot[1] = angle_from_vec(vec[2], sqrt(vec[0]*vec[0] + vec[1]*vec[1]));
	rot[2] = 0;
	
	//printf("%f %f %f\n", rot[0], rot[1], rot[2]);
	
	//行列初期化
	memset(mtr , 0, 9*sizeof(double));
	memset(mtrz, 0, 9*sizeof(double));
	memset(mtrx, 0, 9*sizeof(double));
	
	//回転行列を求める
	//Z軸
	theta = rot[0];
	mtrz[0][0] = cos(theta);
	mtrz[1][0] = sin(theta);
	mtrz[0][1] = -sin(theta);
	mtrz[1][1] = cos(theta);
	mtrz[2][2] = 1;
	
	//X軸
	theta = rot[1];
	mtrx[0][0] = 1;
	mtrx[1][1] = cos(theta);
	mtrx[1][2] = sin(theta);
	mtrx[2][1] = -sin(theta);
	mtrx[2][2] = cos(theta);
	
	//合成
	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			for(k=0; k<3; k++){
				mtr[i][j]=mtr[i][j] + mtrx[i][k] * mtrz[k][j];
			}
		}
	}
	/*
	printf("\n");
	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			printf("%f ", mtr[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	*/
	//座標変換
		//変換する頂点をtmp_vtに格納
	len_vt = 0;
	for(i=0; i<(model->vt_count); i++){
		if(model->vt[i].bone_num[0] == index || model->vt[i].bone_num[1] == index){
			len_vt++;
		}
	}
	tmp_vt = (double*)MALLOC(sizeof(double) * len_vt * 3);
	index_vt = (unsigned int*)MALLOC(sizeof(unsigned int)*len_vt);
	j = 0;
	for(i=0; i<model->vt_count; i++){
		if(model->vt[i].bone_num[0] == index || model->vt[i].bone_num[1] == index){
			index_vt[j] = i;
			for(k=0; k<3; k++){
				tmp_vt[j][k] = model->vt[i].loc[k];
			}
			j++;
		}
	}
		//変換するボーンの子をtmp_boneに格納
	len_bone = 0;
	for(i=0; i<(model->bone_count); i++){
		if(model->bone[i].PBone_index == index){
			len_bone++;
		}
	}
	tmp_bone = (double*)MALLOC(sizeof(double) * len_bone * 3);
	diff_bone = (double*)MALLOC(sizeof(double) * len_bone * 3);
	index_bone = (unsigned int*)MALLOC(sizeof(unsigned int)*len_bone);
	j = 0;
	for(i=0; i<model->bone_count; i++){
		if(model->bone[i].PBone_index == index){
			index_bone[j] = i;
			for(k=0; k<3; k++){
				tmp_bone[j][k] = model->bone[i].loc[k];
				diff_bone[j][k] = tmp_bone[j][k];
			}
			j++;
			
		}
	}
		//変換
	coordtrans(tmp_vt, len_vt, loc, mtr);
	coordtrans(tmp_bone, len_bone, loc, mtr);
	
	
	//変形
	for(i=0; i<len_vt; i++){
		tmp_vt[i][0] = sx * tmp_vt[i][0];
		tmp_vt[i][1] = sy * tmp_vt[i][1];
		tmp_vt[i][2] = sz * tmp_vt[i][2];
	}
	for(i=0; i<len_bone; i++){
		tmp_bone[i][0] = sx * tmp_bone[i][0];
		tmp_bone[i][1] = sy * tmp_bone[i][1];
		tmp_bone[i][2] = sz * tmp_bone[i][2];
	}
	//逆変換
	coordtrans_inv(tmp_vt, len_vt, loc, mtr);
	coordtrans_inv(tmp_bone, len_bone, loc, mtr);
	
	//変換結果を元のデータに書き込む
		//頂点
	for(i=0; i<len_vt; i++){
		k = index_vt[i];
		tmp[0] = 0.0;
		if(model->vt[k].bone_num[0] == index){
			tmp[0] += (double)model->vt[k].bone_weight/100;
		}
		if(model->vt[k].bone_num[1] == index){
			tmp[0] += 1.0-(double)model->vt[k].bone_weight/100;
		}
		//printf("%f %f\n", tmp[0], tmp[1]);
		
		tmp[1] = 1-tmp[0];
		for(j=0; j<3; j++){
			model->vt[k].loc[j] = model->vt[k].loc[j]*tmp[1] + tmp_vt[i][j]*tmp[0];
		}
	}
	
		//ボーン
	for(i=0; i<len_bone; i++){
		for(j=0; j<3; j++){
			diff_bone[i][j] = tmp_bone[i][j] - diff_bone[i][j];
		}
	}
	
	for(i=0; i<model->bone_count; i++){
		l = i;
		for(j=0; j<model->bone_count; j++){
			if(model->bone[l].PBone_index == 65535){
				break;
			}else if(model->bone[l].PBone_index == index){
				break;
			}
			l = model->bone[l].PBone_index;
		}
		if(model->bone[l].PBone_index != 65535){
			for(j=0; j<len_bone; j++){
				if(index_bone[j] == l){
					move_bone(model, i, diff_bone[j]);
					//printf("%d %s %f %f %f\n", j, model->bone[i].name, diff_bone[j][0], diff_bone[j][1], diff_bone[j][2]);
					break;
				}
			}
		}
	}
	
	
	return 0;
}

int bone_vec(MODEL *model, int index, double loc[], double vec[]){
	int i;
	int tail;
	
	for(i=0; i<3; i++){
		loc[i] = model->bone[index].loc[i];
	}
	
	tail = model->bone[index].TBone_index;
	if(tail == 0){
		for(i=0; i<model->bone_count; i++){
			if(model->bone[index].PBone_index == index)
			tail = i;
			break;
		}
	}
	if(tail == 0)return -1;
	
	for(i=0; i<3; i++){
		vec[i] = model->bone[tail].loc[i] - model->bone[index].loc[i];
		//printf("%f ", vec[i]);
	}
	
	return 0;
}

double angle_from_vec(double u, double v){
	double angle;
	double pi = M_PI;
	//ベクトルがv軸方向を向く回転を求める
	
	angle = asin(u/sqrt(u*u+v*v));
	//printf("angle %f\n", angle);
	if(v < 0){
		angle = pi - angle;
	}
	
	return angle;
}

int coordtrans(double array[][3], unsigned int len, double loc[], double mtr[3][3]){
	/*配列は大きさ[len][3]の2次元配列で、点の座標が格納されている
	*/
	int i, j, k;
	double tmp[3];
	
	//座標変換
	for(i=0; i<len; i++){
		if(&loc != 0){
			for(j=0; j<3; j++){
				tmp[j] = array[i][j] - loc[j];
			}
		}
		for(j=0; j<3; j++){
			array[i][j] = 0;
			for(k=0; k<3; k++){
				array[i][j] = array[i][j] + mtr[j][k]*tmp[k];
				
			}
		}
	}
	return 0;
}

int coordtrans_inv(double array[][3], unsigned int len, double loc[], double mtr[3][3]){
	/*配列は大きさ[len][3]の2次元配列で、点の座標が格納されている
	*/
	int i, j, k;
	double tmp[3];
	double inverse_mtr[3][3];
	
	//転置行列
	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			inverse_mtr[i][j]=mtr[j][i];
		}
	}
	
	//座標変換
	for(i=0; i<len; i++){
		for(j=0; j<3; j++){
			tmp[j] = 0;
			for(k=0; k<3; k++){
				tmp[j] = tmp[j] + inverse_mtr[j][k]*array[i][k];
				
			}
		}
		
		if(&loc != 0){
			for(j=0; j<3; j++){
				array[i][j] = tmp[j] + loc[j];
			}
		}
	}
	
	return 0;
}

int move_bone(MODEL *model, unsigned int index, double diff[])
{
	int i, j, k;
	double tmp;
	
	if(index > model->bone_count)return -1;
	
	for(i=0; i<3; i++){
		model->bone[index].loc[i] = model->bone[index].loc[i] + diff[i];
	}
	for(i=0; i<model->vt_count; i++){
		k=0;
		tmp = 0.0;
		if(model->vt[i].bone_num[0] == index){
			tmp += (double)model->vt[i].bone_weight/100;
			k=1;
		}
		if(model->vt[i].bone_num[1] == index){
			tmp += 1.0-(double)model->vt[i].bone_weight/100;
			k=1;
		}
		
		if(k == 1){
			for(j=0; j<3; j++){
				model->vt[i].loc[j] = model->vt[i].loc[j] + diff[j]*tmp;
			}
		}
	}
	
	return 0;
}

int index_bone(MODEL *model, const char bone[])
{
	int i;
	int index = -1;
	
	for(i=0; i<model->bone_count; i++){
		if(strcmp(model->bone[i].name, bone)==0){
			index = i;
			break;
		}
	}
	
	return index;
}

int move_model(MODEL *model, double diff[])
{
	int i, j;
	
	for(i=0; i<model->bone_count; i++){
		for(j=0; j<3; j++){
			model->bone[i].loc[j] = model->bone[i].loc[j] + diff[j];
		}
	}
	for(i=0; i<model->vt_count; i++){
		for(j=0; j<3; j++){
			model->vt[i].loc[j] = model->vt[i].loc[j] + diff[j];
		}
		
	}
	
	
	return 0;
}

int resize_model(MODEL *model, double size)
{
	int i, j, k;
	
	for(i=0; i<model->bone_count; i++){
		for(j=0; j<3; j++){
			model->bone[i].loc[j] = model->bone[i].loc[j] * size;
		}
	}
	for(i=0; i<model->vt_count; i++){
		for(j=0; j<3; j++){
			model->vt[i].loc[j] = model->vt[i].loc[j] * size;
		}
		
	}
	
	for(i=1; i<model->skin_count; i++){
		for(j=0; j<model->skin[i].skin_vt_count; j++){
			for(k=0; k<3; k++){
				model->skin[i].data[j].loc[k] = model->skin[i].data[j].loc[k] * size;
			}
		}
	}
	
	return 0;
}

int marge_bone(MODEL *model)
{
	int i, j, tmp;
	int *index;
	char *marge;
	BONE *bone;
	
	index = (int*)MALLOC(model->bone_count * sizeof(int));
	marge = (char*)MALLOC(model->bone_count * sizeof(char));
	memset(marge, 0, model->bone_count * sizeof(char));
	bone =  (BONE*)MALLOC(model->bone_count * sizeof(BONE));
	
	#ifdef MEM_DBG
		printf("malloc %p %p %p\n", index, marge, bone);
	#endif
	
	tmp = 0;
	for(i=0; i<model->bone_count; i++){
		if(marge[i] == 0){
			index[i] = i - tmp;
			for(j=i+1; j<model->bone_count; j++){
				if(strcmp(model->bone[i].name, model->bone[j].name)==0){
					if(model->bone[i].type == 7){
						model->bone[i].TBone_index = model->bone[j].TBone_index;
						model->bone[i].type = model->bone[j].type;
						model->bone[i].IKBone_index = model->bone[j].IKBone_index;
						memcpy(model->bone[i].loc, model->bone[j].loc, sizeof(float)*3);
					}
					index[j] = i - tmp;
					marge[j] = 1;
				}
			}
		}else{
			tmp++;
		}
		#ifdef DEBUG
			printf("%d:%d %d\n", i, index[i], marge[i]);
		#endif
	}
	
	for(i=0; i<model->bone_count; i++){
		if(index[i] >= model->bone_count){
			return -1;
		}else if(marge[i] == 0){
			bone[index[i]] = model->bone[i];
			if(model->bone[i].PBone_index >= model->bone_count){
				bone[index[i]].PBone_index = 65535;
			}else{
				#ifdef DEBUG
				printf("%d :%d %d \n", i, model->bone[i].PBone_index, bone[index[i]].PBone_index);
				#endif
				bone[index[i]].PBone_index = index[model->bone[i].PBone_index];
			}
			if(model->bone[i].TBone_index == 0 || model->bone[i].TBone_index >= model->bone_count){
				bone[index[i]].TBone_index = 0;
			}else{
				bone[index[i]].TBone_index = index[model->bone[i].TBone_index];
			}
			bone[index[i]].type = model->bone[i].type;
			if(model->bone[i].IKBone_index == 0 || model->bone[i].IKBone_index >= model->bone_count){
				bone[index[i]].IKBone_index = 0;
			}else{
				bone[index[i]].IKBone_index = index[model->bone[i].IKBone_index];
			}
		}
	}
	#ifdef DEBUG
		printf("ボーンインデックスをアップデート\n", index, marge, model->bone);
	#endif
	update_bone_index(model, index);
	model->bone_count = model->bone_count - tmp;
	
	#ifdef MEM_DBG
		printf("FREE %p %p %p\n", index, marge, model->bone);
	#endif
	
	FREE(index);
	FREE(marge);
	FREE(model->bone);
	
	model->bone = bone;
	
	return 0;
}

int marge_mat(MODEL *model)
{
	int i, j, k;
	int tmp, size, sum;
	int *tmp_count;
	int vt_index_count;
	int *index;
	char *marge;
	char *p;
	
	unsigned short *vt_index;
	MATERIAL *tmp_mat;
	
	index = (int*)MALLOC(model->mat_count * sizeof(int));
	marge = (char*)MALLOC(model->mat_count * sizeof(char));
	memset(marge, 0, model->mat_count * sizeof(char));
	vt_index = (unsigned short*)MALLOC(model->vt_index_count * sizeof(unsigned short));
	if(vt_index == NULL)return -1;	
	tmp_count = (int*)MALLOC(model->mat_count * sizeof(int));
	if(tmp_count == NULL)return -1;	
	memset(tmp_count, 0, model->mat_count * sizeof(int));
	tmp_mat = (MATERIAL*)MALLOC(model->mat_count * sizeof(MATERIAL));
	memcpy(tmp_mat, model->mat, model->mat_count * sizeof(MATERIAL));
	#ifdef MEM_DBG
		printf("malloc %p %p %p %p\n", index, marge, vt_index, tmp_count);
	#endif
	
	//テクスチャ名の同じ材質を探す
	tmp = 0;
	sum = 0;
	/*
	for(i=0; i<model->mat_count; i++){
		if(marge[i] == 0){
			
			if(model->mat[i].alpha >= 0.999){
				sum++;
			}
			if(model->mat[i].tex[0] != '\0'){
				for(j=i+1; j<model->mat_count; j++){
					if(strcmp(model->mat[i].tex, model->mat[j].tex) == 0){
						p = strrchr(model->mat[i].tex, '.');
						if(p != NULL){
							if( strcmp(p, ".sph") ==0 ||
								strcmp(p, ".spa") ==0 ||
								strcmp(p, ".tga") ==0 ||
								strcmp(p, ".bmp") ==0 ||
								strcmp(p, ".png") ==0 ||
								strcmp(p, ".jpg") ==0){
									marge[j] = -1;
							}else{
								p = NULL;
							}
						}
						if(p == NULL){
							marge[j] = 1;
						}
					}
				}
			}
		}else{
			tmp++;
		}
	}
	*/
	//printf("%d %d %d\n", model->mat_count, tmp, sum);
	memset(marge, 0, model->mat_count * sizeof(char));
	
	tmp = 0;
	for(i=0; i<model->mat_count; i++){
		if(marge[i] == 0){
			/*
			if(model->mat[i].alpha >= 0.999){
				index[i] = i - tmp;
			}else{
			*/
			index[i] = sum;
			sum++;
			
			if(model->mat[i].tex[0] != '\0'){
				for(j=i+1; j<model->mat_count; j++){
					if(strcmp(model->mat[i].tex, model->mat[j].tex) == 0){
						if( 0.0001 < abs(model->mat[i].tex - model->mat[j].tex)){
							p = strrchr(model->mat[i].tex, '.');
							if(p != NULL){
								if( strcmp(p, ".sph") ==0 ||
									strcmp(p, ".spa") ==0){
										marge[j] = 0;
								}else{
									p = NULL;
								}
							}
							if(p == NULL){
								index[j] = index[i];
								marge[j] = 1;
							}
						}
					}
				}
			}
		}else{
			tmp++;
		}
		#ifdef DEBUG
			printf("%d:%d %d\n", i, index[i], marge[i]);
		#endif
	}
	
	//面頂点リスト並び替え
	k=0;
	
	for(i=0; i<model->mat_count; i++){
		vt_index_count = 0;
		sum = 0;
		for(j=0; j<model->mat_count; j++){
			if(index[j] == i){
				size = model->mat[j].vt_index_count * sizeof(unsigned short);
				
				memcpy(&vt_index[k], &model->vt_index[sum], size);
				#ifdef DEBUG
					printf("%d <- %d  %d\n", i, j, k);
				#endif
				k = k + model->mat[j].vt_index_count;
				vt_index_count = vt_index_count + model->mat[j].vt_index_count;
			}
			sum = sum + model->mat[j].vt_index_count;
		}
		tmp_count[i] = vt_index_count;
		#ifdef DEBUG
			printf("%d %d %d\n", i, vt_index_count, model->mat[i].vt_index_count);
		#endif
	}
	
	//材質並び替え
	for(i=0; i<model->mat_count; i++){
		if(marge[i] == 0){
			if(index[i] != i)model->mat[index[i]] = tmp_mat[i];
			model->mat[index[i]].vt_index_count = tmp_count[index[i]];
		}
	}
	
	model->mat_count = model->mat_count - tmp;
	
	#ifdef MEM_DBG
		printf("FREE %p %p %p %p\n", index, marge, model->vt, tmp_count);
	#endif
	
	FREE(model->vt_index);
	FREE(index);
	FREE(marge);
	FREE(tmp_count);
	FREE(tmp_mat);
	model->vt_index = vt_index;
	
	return 0;
}

int marge_IK(MODEL *model)
{
	int i, j, tmp;
	int *index;
	char *marge;
	
	//重複IKを削除
	
	index = (int*)MALLOC(model->IK_count * sizeof(int));
	marge = (char*)MALLOC(model->IK_count * sizeof(char));
	memset(marge, 0, model->IK_count * sizeof(char));
	#ifdef MEM_DBG
		printf("malloc %p %p\n", index, marge);
	#endif
	
	
	tmp = 0;
	for(i=0; i<model->IK_count; i++){
		if(marge[i] == 0){
			index[i] = i - tmp;
			for(j=i+1; j<model->IK_count; j++){
				if(model->IK_list[i].IKBone_index == model->IK_list[j].IKBone_index){
					index[j] = i - tmp;
					marge[j] = 1;
				}
			}
		}else{
			tmp++;
		}
		#ifdef DEBUG
			printf("%d:%d %d\n", i, index[i], marge[i]);
		#endif
	}
	
	
	for(i=0; i<model->IK_count; i++){
		if(marge[i] == 0 && index[i] != i){
			model->IK_list[index[i]] = model->IK_list[i];
		}
	}
	model->IK_count = model->IK_count - tmp;
	
	#ifdef MEM_DBG
		printf("FREE %p %p\n", index, marge);
	#endif
	
	FREE(index);
	FREE(marge);
	
	
	
	return 0;
}

int marge_bone_disp(MODEL *model)
{
	int i, j, k, tmp;
	int *index;
	char *marge;
	BONE_DISP *bone_disp;
	
	//同名枠をマージ
	#ifdef DEBUG
		printf("%d\n", model->bone_group_count);
	#endif
	
	index = (int*)MALLOC(model->bone_group_count * sizeof(int));
	marge = (char*)MALLOC(model->bone_group_count * sizeof(char));
	memset(marge, 0, model->bone_group_count * sizeof(char));
	bone_disp = (BONE_DISP*)MALLOC(model->bone_disp_count * sizeof(BONE_DISP));
	#ifdef MEM_DBG
		printf("malloc %p %p\n", index, marge);
	#endif
	
	tmp = 0;
	for(i=0; i<model->bone_group_count; i++){
		if(marge[i] == 0){
			index[i] = i - tmp;
			for(j=i+1; j<model->bone_group_count; j++){
				if(strcmp(model->bone_group[i].name, model->bone_group[j].name) == 0){
					index[j] = i - tmp;
					marge[j] = 1;
				}
			}
		}else{
			tmp++;
		}
		#ifdef DEBUG
			printf("%d:%d %d\n", i, index[i], marge[i]);
		#endif
	}
	
	
	for(i=0; i<model->bone_group_count; i++){
		if(marge[i] == 0 && index[i] != i){
			model->bone_group[index[i]] = model->bone_group[i];
		}
	}
	
	#ifdef DEBUG
		for(j=0; j<model->bone_disp_count; j++){
			printf("%d: %d %d\n", j, model->bone_disp[j].index, model->bone_disp[j].bone_group);
		}
	#endif
	
	k=0;
	for(i=0; i<model->bone_group_count; i++){
		for(j=0; j<model->bone_disp_count; j++){
			if(index[model->bone_disp[j].bone_group-1] == i){
				bone_disp[k] = model->bone_disp[j];
				
				bone_disp[k].bone_group = index[bone_disp[k].bone_group-1]+1;
				
				#ifdef DEBUG
					printf("%d %d %d\n", k, bone_disp[k].index, bone_disp[k].bone_group);
				#endif
				
				k++;
			}
		}
		
	}
	
	
	model->bone_group_count = model->bone_group_count - tmp;
	model->bone_disp_count = k;
	
	#ifdef MEM_DBG
		printf("FREE %p %p\n", index, marge);
	#endif
	
	FREE(model->bone_disp);
	FREE(index);
	FREE(marge);
	
	model->bone_disp = bone_disp;
	
	//重複登録ボーンを削除
	
	index = (int*)MALLOC(model->bone_disp_count * sizeof(int));
	marge = (char*)MALLOC(model->bone_disp_count * sizeof(char));
	memset(marge, 0, model->bone_disp_count * sizeof(char));
	#ifdef MEM_DBG
		printf("malloc %p %p\n", index, marge);
	#endif
	
	tmp = 0;
	for(i=0; i<model->bone_disp_count; i++){
		if(marge[i] == 0){
			index[i] = i - tmp;
			for(j=i+1; j<model->bone_disp_count; j++){
				if(model->bone_disp[i].index == model->bone_disp[j].index){
					index[j] = i - tmp;
					marge[j] = 1;
				}
			}
		}else{
			tmp++;
		}
		#ifdef DEBUG
			printf("%d:%d %d - %d\n", i, index[i], marge[i], model->bone_disp[i].index);
		#endif
	}
	
	for(i=0; i<model->bone_disp_count; i++){
		if(marge[i] == 0 && index[i] != i){
			model->bone_disp[index[i]].index = model->bone_disp[i].index;
			model->bone_disp[index[i]].bone_group = model->bone_disp[i].bone_group;
			
		}
		#ifdef DEBUG
			printf("%d:%d %d\n", i, model->bone_disp[i].index, marge[i]);
		#endif
	}
	
	model->bone_disp_count = model->bone_disp_count - tmp;
	
	#ifdef MEM_DBG
		printf("FREE %p %p\n", index, marge);
	#endif
	
	FREE(index);
	FREE(marge);
	
	return 0;
}

int marge_rb(MODEL *model)
{
	int i, j, tmp;
	int *index;
	char *marge;
	
	//同名の剛体を削除
	
	index = (int*)MALLOC(model->rbody_count * sizeof(int));
	marge = (char*)MALLOC(model->rbody_count * sizeof(char));
	memset(marge, 0, model->rbody_count * sizeof(char));
	#ifdef MEM_DBG
		printf("malloc %p %p\n", index, marge);
	#endif
	
	
	tmp = 0;
	for(i=0; i<model->rbody_count; i++){
		if(marge[i] == 0){
			index[i] = i - tmp;
			for(j=i+1; j<model->rbody_count; j++){
				if(strcmp(model->rbody[i].name, model->rbody[j].name) == 0){
					index[j] = i - tmp;
					marge[j] = 1;
				}
			}
		}else{
			tmp++;
		}
		#ifdef DEBUG
			printf("%d:%d %d\n", i, index[i], marge[i]);
		#endif
	}
	
	//ジョイント書き換え
	for(i=0; i<model->joint_count; i++){
		for(j=0; j<2; j++){
			model->joint[i].rbody[j] = index[model->joint[i].rbody[j]];
			
		}
	}
	
	//重複削除
	
	for(i=0; i<model->rbody_count; i++){
		if(marge[i] == 0 && index[i] != i){
			model->rbody[index[i]] = model->rbody[i];
		}
	}
	model->rbody_count = model->rbody_count - tmp;
	
	#ifdef MEM_DBG
		printf("FREE %p %p\n", index, marge);
	#endif
	
	FREE(index);
	FREE(marge);
	
	
	
	return 0;
}

int update_skin(MODEL *model)
{
	int i, j, k;
	//表情baseの頂点位置を更新する
	if(model->skin_count == 0)return 0;
	for(i=0; i<model->skin[0].skin_vt_count; i++){
		for(j=0; j<3; j++){
			k = model->skin[0].data[i].index;
			model->skin[0].data[i].loc[j] = model->vt[k].loc[j];
		}
	}
	return 0;
}

int adjust_joint(MODEL *model)
{
	int i, j;
	//同じ名前のボーンにジョイントの位置を合わせる
	
	for(i=0; i<model->joint_count; i++){
		for(j=0; j<model->bone_count; j++){
			if(strcmp(model->joint[i].name, model->bone[j].name) == 0){
				memcpy(model->joint[i].loc, model->bone[j].loc, sizeof(float)*3);
			}
		}
	}
	
	return 0;
}

int show_detail(MODEL *model)
{
	printf("%s \n %f \n %s \n %s \n",
		model->header.magic,
		model->header.version,
		model->header.name,
		model->header.comment);
	printf("頂点数:%d\n", model->vt_count);
	printf("面頂点数:%d\n", model->vt_index_count);
	printf("材質数:%d\n", model->mat_count);
	printf("ボーン数:%d\n", model->bone_count);
	printf("IKデータ数:%d\n", model->IK_count);
	printf("表情数:%d\n", model->skin_count);
	printf("表情枠:%d\n", model->skin_disp_count);
	printf("ボーン枠:%d\n", model->bone_group_count);
	printf("表示ボーン数:%d\n", model->bone_disp_count);
	printf("英名対応:%d\n", model->eng_support);
	printf("剛体数:%d\n", model->rbody_count);
	printf("ジョイント数:%d\n\n", model->joint_count);
	return 0;
}
