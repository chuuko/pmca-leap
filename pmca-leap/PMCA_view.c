#include "PMCA.h"

#define WM_TITLE "PMCA 3D View"

FLAGS myflags;
VIEW_STATE vs;

static int createwindow();
static int setup_opengl( int width, int height );
static void draw_screen( void );
static void handle_key_down( SDL_keysym* keysym );
static void process_events( void );

/* 現在のビデオ設定についての情報 */
const SDL_VideoInfo* info = NULL;
/* ウィンドウの色のピクセル深度 */
int bpp = 0;
/* SDL_SetVideoMode に渡すフラグ */
int flags = 0;


int viewer_thread()
{
	SDL_Event event;
	
	myflags.model_lock=0;
	
	createwindow();
	SDL_Quit( );
	return 0;
}

static int createwindow()
{
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		fprintf( stderr, "ビデオの初期化に失敗しました: %s\n", SDL_GetError( ) );
		SDL_Quit( );
		return 1;
	}
	
	info = SDL_GetVideoInfo( );

	if( !info ) {
		fprintf( stderr, "ビデオの問い合わせに失敗しました: %s\n", SDL_GetError( ) );
		SDL_Quit( );
		return 1;
	}
	
	vs.width = 640;
	vs.height = 480;
	bpp = info->vfmt->BitsPerPixel;
	
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	
	flags = SDL_OPENGL|SDL_RESIZABLE;
	
	if( SDL_SetVideoMode( vs.width, vs.height, bpp, flags ) == 0 ) {
		fprintf( stderr, "ビデオモードのセットに失敗しました: %s\n", SDL_GetError( ) );
		SDL_Quit( );
		return 1;
	}
	
	setup_opengl( vs.width, vs.height );
	SDL_WM_SetCaption(WM_TITLE, NULL);
	
	/*MODEL* model;
	model = model_mgr(1,0,NULL);
	load_texture(model, "");
	*/
	while( myflags.quit != 1 ) {
		process_events( );
		draw_screen( );
	}
	
	return 0;
}

static int setup_opengl( int width, int height )
{

	/* シェーディングモデルは Gouraud (なめらか) */
	//glShadeModel( GL_SMOOTH );

	/* 裏面を取り除く */
	glEnable(GL_CULL_FACE);
	glFrontFace( GL_CCW );
	glCullFace(GL_FRONT);
	
	/* 消去時の色をセット */
	glClearColor( 0, 0, 0, 0 );
	
	/* ビューポートを設定 */
	glViewport( 0, 0, width, height );

	/*
	 * 射影行列を変更し、ビューボリュームにセット。
	 */
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	
	/*アルファブレンドを有効に*/
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	/*Zバッファを有効に*/
	glEnable(GL_DEPTH_TEST);
	
	glTranslatef(0.0, -10.0, -20.0);
	//glFrustum( -1.0, 1.0, -ratio, ratio, -20, 20 );
	
	{
		double tmp[4] = {1.0, 0.0, 0.0, 0.0};
		memcpy(vs.tq, tmp, 4*sizeof(double));
		memcpy(vs.cq, tmp, 4*sizeof(double));
		qrot(vs.rt, vs.tq);
		vs.scale = 15.0;
	}
	
	return 0;
}

static void draw_screen( void )
{
	double asp = (double) vs.width / (double) vs.height;
	
	while(myflags.model_lock!=0){
		SDL_Delay(30);
	}
	myflags.model_lock = 1;
	/*座標軸表示*/
	vs.show_axis = 0x01 | 0x02 | 0x04;
	
	/* 色・デプスバッファを消去 */
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	/*ビュー設定*/
	glLoadIdentity( );
	glOrtho(-vs.scale*asp, vs.scale*asp, -vs.scale, vs.scale, -20, 20);
	
	/* z 軸の方向に下げる */
	glTranslatef( 0.0, -10.0, 0.0 );
	/* 回転移動 */
	glMultMatrixd(vs.rt);
	glTranslatef( vs.move[0], vs.move[1], vs.move[2] );
	
	
	/*座標軸描画*/
	glBegin(GL_LINES);
		if(0 != (vs.show_axis & 0x02)){	//y_axis
			glColor4f(0.0, 1.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 40.0, 0.0);
		}
		if(0 != (vs.show_axis & 0x01)){	//x_axis
			glColor4f(1.0, 0.0, 0.0, 1.0);
			glVertex3f( 40.0, 0.0, 0.0);
			glVertex3f(-40.0, 0.0, 0.0);
		}
		if(0 != (vs.show_axis & 0x04)){	//z_axis
			glColor4f(0.0, 0.0, 1.0, 1.0);
			glVertex3f(0.0, 0.0,  40.0);
			glVertex3f(0.0, 0.0, -40.0);
		}
	glEnd();
	
	render_model(0);
	glFinish();
	myflags.model_lock = 0;
	SDL_GL_SwapBuffers( );
	SDL_Delay(30);
}

static void handle_key_down( SDL_keysym* keysym )
{
	switch( keysym->sym ) {
		case SDLK_ESCAPE:
			myflags.quit = 1;
			break;
		default:
			break;
	}
}

static void process_events( void )
{
	/* SDL イベントの置き場 */
	SDL_Event event;
	
	/* すべてのイベントをキューからつかみ取る */
	while( SDL_PollEvent( &event )) {
	
		switch( event.type ) {
		case SDL_KEYDOWN:
			/* キー押下を処理 */
			handle_key_down( &event.key.keysym );
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch(event.button.button){
			case SDL_BUTTON_LEFT:
				myflags.button1 = 1;
				break;
			case SDL_BUTTON_RIGHT:
				myflags.button2 = 1;
				break;
			case SDL_BUTTON_MIDDLE:
				myflags.button3 = 1;
				break;
			}
			vs.sx = event.button.x;
			vs.sy = event.button.y;
			break;
		case SDL_MOUSEBUTTONUP:
			switch(event.button.button){
			case SDL_BUTTON_LEFT:
				myflags.button1 = 0;
				break;
			case SDL_BUTTON_RIGHT:
				myflags.button2 = 0;
				break;
			case SDL_BUTTON_MIDDLE:
				myflags.button3 = 0;
				break;
			}
			memcpy(vs.cq, vs.tq, 4*sizeof(double));
			break;
		case SDL_MOUSEMOTION:
			if(myflags.button1 == 1){
				double dx,dy;
				double a;
				dx = (event.motion.xrel)/10.0;
				dy = (event.motion.yrel)/10.0;
				a = sqrt(dx * dx + dy * dy);
				if( a != 0.0 ){
					int i, j;
					double tmp[3];
					tmp[0] = dx;
					tmp[1] = dy;
					tmp[2] = 0.0;
					//変換行列から移動ベクトルを回転
					for(i=0; i<3; i++){
						for(j=0; j<3; j++){
							if(i==0){
								vs.move[i] += tmp[j]*vs.rt[j*4+i];
							}else{
								vs.move[i] -= tmp[j]*vs.rt[j*4+i];
							}
						}
					}
				}
			}
			if(myflags.button2 == 1){
				double dx,dy;
				double a;
				
				dx = (event.motion.x - vs.sx)/(double)vs.width;
				dy = (event.motion.y - vs.sy)/(double)vs.height;
				a = sqrt(dx * dx + dy * dy);
				if( a != 0.0 ){
					// マウスのドラッグに伴う回転のクォータニオン dq を求める
					double ar = a * SCALE * 0.5;
					double as = sin(ar) / a;
					double dq[4] = { cos(ar), dy * as, dx * as, 0.0 };

					// 回転の初期値 cq に dq を掛けて回転を合成
					qmul(vs.tq, dq, vs.cq);

					// クォータニオンから回転の変換行列を求める
					qrot(vs.rt, vs.tq);
				}
			}
			if(myflags.button3 == 1){
				vs.scale -= event.motion.yrel*0.1;
				if(vs.scale < 0){
					vs.scale = 0.001;
				}
			}
			break;
		case SDL_VIDEORESIZE:
			vs.width = event.resize.w;
			vs.height = event.resize.h;
			if( SDL_SetVideoMode( vs.width, vs.height, bpp, flags ) == 0 ) {
				fprintf( stderr, "ビデオモードのセットに失敗しました: %s\n", SDL_GetError( ) );
				SDL_Quit( );
				return 1;
			}
			setup_opengl( vs.width, vs.height );
			break;
		case SDL_QUIT:
			/* 終了要求 (Ctrl-c など) を処理 */
			myflags.quit = 1;
			
			return -1;
		}
	}
}

/*描画用のモデルを管理する関数*/
void* model_mgr(int flag, int num, void* p){
	static MODEL model[16];
	static DSP_MODEL dsp_model[16];
	static int init = 1;
	
	int i;
	
	/*flag
	-1:初期化
	0:書き込み
	1:読み出し
	2:描画モデル読み出し
	3:テクスチャはそのままで再セット
	num
	0:表示
	*/
	if(flag == -1){
		for(i=0; i<16; i++){
			create_PMD(&model[i]);
			make_dsp_model(&model[i], &dsp_model[i]);
		}
		init = 1;
	}else if(flag == 0){
		init = -1;
		delete_PMD(&model[num]);
		copy_PMD(&model[num],(MODEL*)p);
		make_dsp_model(&model[num], &dsp_model[num]);
		init = 1;
	}else if(flag == 1){
		//if(myflags.model_lock != 0)return NULL;
		if(init == 1){
			load_tex(&model[num], &dsp_model[num]);
			init = 0;
		}else if(init == -1 ){
			return NULL;
		}
		return &model[num];
	
	}else if(flag == 2){
		return &dsp_model[num];
	
	}else if(flag == 3){
		init = -1;
		delete_PMD(&model[num]);
		copy_PMD(&model[num],(MODEL*)p);
		make_dsp_model(&model[num], &dsp_model[num]);
		init = 0;
	}
	return 0;
}

/*モデルデータを描画*/
int render_model(int num){
	int i, j;
	int index, c;
	MODEL *model;
	DSP_MODEL *dsp_model;
	
	static float *loc;
	static float *nor;
	static float *uv;
	static DSP_MAT *mats;
	
	model = model_mgr(1, num, NULL);
	if(model==NULL)return -1;
	dsp_model = model_mgr(2, num, NULL);
	
	loc = dsp_model->loc;
	nor = dsp_model->nor;
	uv  = dsp_model->uv;
	mats= dsp_model->mats;
	
	if(loc == NULL)return -1;
	if(nor == NULL)return -1;
	if(uv  == NULL)return -1;
	if(mats== NULL)return -1;
	
	c=0;
	for(i=0; i<model->mat_count; i++){
		if(mats[i].texbits != NULL){
			glEnable(GL_TEXTURE_2D);
			//glBindTexture(GL_TEXTURE_2D , dsp_model->texid[i]);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			
			glTexImage2D(
				GL_TEXTURE_2D , 0 , GL_RGBA, mats[i].texsize[0], mats[i].texsize[1],
				0 , GL_RGBA , GL_UNSIGNED_BYTE , mats[i].texbits
			);
		}
		glBegin(GL_TRIANGLES);
		glColor4fv(mats[i].col);
			
		for(j=0; j<model->mat[i].vt_index_count; j++){
			index = model->vt_index[c++];
			glTexCoord2fv(uv+2*index);
			glVertex3fv(loc+3*index);
			/*
			glTexCoord2fv(model->vt[index].uv);
			glVertex3fv(model->vt[index].loc);
			*/
			//c++;
		}
		
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	/*
	GLenum num;
	num = glGetError;
	*/
	//glDeleteTextures(model->mat_count, dsp_model->texid);
	return 0;
}

//テクスチャ読み込み
int load_tex(MODEL *model, DSP_MODEL *dsp_model)
{
	
	DSP_MAT *mats;
	
	char tex_name[128];
	int i, j, tmp;
	SDL_Surface *image = NULL;
	GLubyte *texbits = NULL;
	/*
	printf("LockstateA %d\n", myflags.model_lock);
	while(myflags.model_lock != 0){
		SDL_Delay(30);
		//printf("A");
	}
	myflags.model_lock=1;
	*/
	
	//printf("tex読み込み\n");
	
	mats = dsp_model->mats;
	
	if(dsp_model->mats_c != model->mat_count){
		return -1;
	}
	
	glDeleteTextures(model->mat_count, dsp_model->texid);
	
	for(i=0; i<dsp_model->mats_c; i++){
		for(j=0; j<3; j++){
			printf("%f %f %f\n", model->mat[i].diffuse[j], model->mat[i].mirror_col[j], model->mat[i].spec_col[j]);
			mats[i].col[j] = (model->mat[i].diffuse[j]*2+model->mat[i].mirror_col[j])/2.5 + model->mat[i].spec_col[j]/4;
		}
		printf("col %f %f %f %f\n", mats[i].col[0], mats[i].col[1], mats[i].col[2], mats[i].col[3]);
		mats[i].col[3] = model->mat[i].alpha;
		mats[i].texname[0] = '\0';
		memset(mats[i].texsize, 0, 2*sizeof(int));
		if(mats[i].texbits != NULL){
			FREE(mats[i].texbits);
			mats[i].texbits = NULL;
		}
		
		image = IMG_Load(model->mat[i].tex_path);
		//image = NULL;
		
		if(image == NULL){
			memset(mats[i].texsize, 0, 2*sizeof(int));
			texbits = NULL;
			printf("画像が読み込めません %s\n", model->mat[i].tex_path);
		}else{
			mats[i].texsize[0] = image->w;
			mats[i].texsize[1] = image->h;
			texbits = MALLOC(mats[i].texsize[0] * mats[i].texsize[1] * sizeof(GLubyte) * 4);
			//printf("\nsize(byte):%d\n", mats[i].texsize[0] * mats[i].texsize[1] * sizeof(GLubyte) * 4);
			
			if(texbits == NULL)printf("メモリ確保失敗 %d\n", i);
		}
	
		if(texbits != NULL){
			char *p;
			SDL_PixelFormat *fm;
			p = image->pixels;
			fm= image->format;
			int li, lj;
			int lk=0;
			SDL_LockSurface(image);
			Uint8 r,g,b,a;
			
			for (li = 0 ; li < mats[i].texsize[1] ; li++) {
				for (lj = 0 ; lj < mats[i].texsize[0] ; lj++) {
					
					r = p[fm->Rshift/8]<<fm->Rloss;
					g = p[fm->Gshift/8]<<fm->Gloss;
					b = p[fm->Bshift/8]<<fm->Bloss;
					a = p[fm->Ashift/8]<<fm->Aloss;
					//printf("%x%x%x%x ", r, g, b, a);
				
					texbits[lk++] = (GLubyte)r;
					texbits[lk++] = (GLubyte)g;
					texbits[lk++] = (GLubyte)b;
					texbits[lk++] = (GLubyte)a;
					//lk += 4;
					p  += fm->BytesPerPixel;
				}
			}
		
			if(fm->Amask == 0){
				lk = 0;
				for (li = 0 ; li < (mats[i].texsize[0]*mats[i].texsize[1]) ; li++) {
					texbits[lk+3] = (GLubyte)255;
					lk += 4;
				}
			}
			SDL_UnlockSurface(image);
			//#ifdef DEBUG
				printf("texture %d\n", i);
				printf("%s\n", model->mat[i].tex);
				printf("%d x %d  %d\n", image->w, image->h, image->pitch);
				printf("BitsPerPixel:%d\n",image->format->BitsPerPixel);
				printf("BytesPerPixel:%d\n",image->format->BytesPerPixel);
				printf("Mask:%x %x %x %x\n",image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
				printf("Shift:%d %d %d %d\n",image->format->Rshift, image->format->Gshift, image->format->Bshift, image->format->Ashift);
				printf("Loss:%d %d %d %d\n\n",image->format->Rloss, image->format->Gloss, image->format->Bloss, image->format->Aloss);
			//#endif
			{
				double log_w = log(mats[i].texsize[0])/log(2);
				double log_h = log(mats[i].texsize[1])/log(2);
				if(ceil(log_w) != floor(log_w) || ceil(log_h) != floor(log_h)){
					GLubyte *tmp_bits;
					int w, h;
					w = 2;
					h = 2;
					for(j=0; j<floor(log_w); j++){
						w = w*2;
					}
					for(j=0; j<floor(log_h); j++){
						h = h*2;
					}
					tmp_bits = MALLOC(h * w * sizeof(GLubyte) * 6);
					if(tmp_bits == NULL)puts("メモリ確保失敗");
				
					tmp = gluScaleImage(GL_RGBA, mats[i].texsize[0], mats[i].texsize[1], GL_UNSIGNED_BYTE, texbits, w, h, GL_UNSIGNED_BYTE, tmp_bits);
					mats[i].texsize[0] = w;
					mats[i].texsize[1] = h;
					#ifdef DEBUG
						printf("log %f x %f\n", log_w, log_h);
						printf("リサイズ %d x %d  %x %s\n", w, h, tmp,gluErrorString(tmp));
					#endif
					FREE(texbits);
					texbits = tmp_bits;
				}
			}
			
		}
		if(image != NULL)SDL_FreeSurface(image);
		mats[i].texbits = texbits;
	}
	glGenTextures(model->mat_count , dsp_model->texid);
	
	for(i=0; i<model->mat_count; i++){
		if(mats[i].texbits != NULL){
			glBindTexture(GL_TEXTURE_2D , dsp_model->texid[i]);
		}
	}
	
	#ifdef DEBUG
		printf("テクスチャ読み込み完了\n");
	#endif
	
	//myflags.model_lock=0;
	
	return 0;
		
}

int make_dsp_model(MODEL *model, DSP_MODEL *dsp_model)
{
	int i,j;
	float *loc;
	float *nor;
	float *uv;
	DSP_MAT *mats;
	GLuint *texid;
	//unsigned int index;
	
	/*
	while(myflags.model_lock != 0){
		SDL_Delay(30);
		printf("B");
	}
	myflags.model_lock=1;
	*/
	FREE(dsp_model->loc);
	FREE(dsp_model->nor);
	FREE(dsp_model->uv);
	for(i=0; i<dsp_model->mats_c; i++){
		FREE(dsp_model->mats[i].texbits);
		dsp_model->mats[i].texbits = NULL;
		memset(dsp_model->mats[i].texsize, 0, 2*sizeof(int));
		
	}
	FREE(dsp_model->mats);
	FREE(dsp_model->texid);
	dsp_model->loc = NULL;
	dsp_model->nor = NULL;
	dsp_model->uv  = NULL;
	dsp_model->mats = NULL;
	dsp_model->texid= NULL;
	
	
	loc = MALLOC(model->vt_count * 3 * sizeof(float));
	nor = MALLOC(model->vt_count * 3 * sizeof(float));
	uv  = MALLOC(model->vt_count * 2 * sizeof(float));
	mats= MALLOC(model->mat_count * sizeof(DSP_MAT));
	memset(mats, 0, model->mat_count * sizeof(DSP_MAT));
	texid = MALLOC(model->mat_count*sizeof(GLuint));
	if(loc == NULL ||
		nor == NULL ||
		uv  == NULL ||
		mats== NULL){
		//myflags.model_lock=0;
		return -1;
	}
	dsp_model->loc = loc;
	dsp_model->nor = nor;
	dsp_model->uv  = uv;
	dsp_model->mats = mats;
	dsp_model->texid= texid;
	dsp_model->mats_c=model->mat_count;
	
	for(i=0; i<model->vt_count; i++){
		memcpy(loc, model->vt[i].loc, 2*sizeof(float));
		loc += 2;
		*loc = -model->vt[i].loc[2];
		memcpy(nor, model->vt[i].nor, 3*sizeof(float));
		memcpy(uv , model->vt[i].uv, 2*sizeof(float));
		loc ++;
		nor += 3;
		uv  += 2;
	}
	
	for(i=0; i<dsp_model->mats_c; i++){
		dsp_model->mats[i].texbits = NULL;
		memset(dsp_model->mats[i].texsize, 0, 2*sizeof(int));
	}
	
	//myflags.model_lock=0;
	return 0;
	
}

/*
** クォータニオンの積 r <- p x q
*/
void qmul(double r[], const double p[], const double q[])
{
  r[0] = p[0] * q[0] - p[1] * q[1] - p[2] * q[2] - p[3] * q[3];
  r[1] = p[0] * q[1] + p[1] * q[0] + p[2] * q[3] - p[3] * q[2];
  r[2] = p[0] * q[2] - p[1] * q[3] + p[2] * q[0] + p[3] * q[1];
  r[3] = p[0] * q[3] + p[1] * q[2] - p[2] * q[1] + p[3] * q[0];
}
  
/*
** 回転の変換行列 r <- クォータニオン q
*/
void qrot(double r[], double q[])
{
  double x2 = q[1] * q[1] * 2.0;
  double y2 = q[2] * q[2] * 2.0;
  double z2 = q[3] * q[3] * 2.0;
  double xy = q[1] * q[2] * 2.0;
  double yz = q[2] * q[3] * 2.0;
  double zx = q[3] * q[1] * 2.0;
  double xw = q[1] * q[0] * 2.0;
  double yw = q[2] * q[0] * 2.0;
  double zw = q[3] * q[0] * 2.0;
  
  r[ 0] = 1.0 - y2 - z2;
  r[ 1] = xy + zw;
  r[ 2] = zx - yw;
  r[ 4] = xy - zw;
  r[ 5] = 1.0 - z2 - x2;
  r[ 6] = yz + xw;
  r[ 8] = zx + yw;
  r[ 9] = yz - xw;
  r[10] = 1.0 - x2 - y2;
  r[ 3] = r[ 7] = r[11] = r[12] = r[13] = r[14] = 0.0;
  r[15] = 1.0;
}
