// mesh.cpp: implementation of the mesh class.
//
//////////////////////////////////////////////////////////////////////

#include "mesh.h"
#include <iostream>

const char* obj_database = "";	// 定義 mesh 的預設目錄

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Mesh::Mesh(const char* objFile)
{
	mTotal_ = 0;		// mList_[0] reserved for default meterial
	vTotal_ = tTotal_ = nTotal_ = fTotal_ = 0;
	
	init(objFile);
}

Mesh::Mesh()
{
	mTotal_ = 0;			
	vTotal_ = tTotal_ = nTotal_ = fTotal_ = 0;
}

Mesh::~Mesh()
{
}

void Mesh::loadMesh(string objFile)
{
	FILE	*scene;
	char	token[100], buf[100], v[5][100];	// v[5] 表示一個 polygon 最多可以有 5個 vertex
	float	vec[3];

	size_t	n_vertex, n_texture, n_normal;
	size_t	cur_mtl = 0;				// state variable: 目前所使用的 Material

	scene = fopen(objFile.c_str(),"r");
	objFile_ = objFile;

	if (!scene) 
	{
		cout<< string("Can not open object File \"") << objFile << "\" !" << endl;
		return;
	}

	cout<<endl<<objFile<<endl;
		
	while(!feof(scene))
	{
		token[0] = NULL;
		fscanf(scene,"%s", token);		// 讀 token

		if (!strcmp(token,"g"))
		{
			fscanf(scene,"%s",buf);
		}

		else if (!strcmp(token,"mtllib"))
		{
			char mat_file[256];
			matFile_ = mat_file;
  			fscanf(scene,"%s", mat_file);
			loadMtl(string(obj_database) + string(mat_file));
		}

		else if (!strcmp(token,"usemtl"))
		{
			fscanf(scene,"%s",buf);
			cur_mtl = matMap_[objFile_+string("_")+string(buf)];
		}

		else if (!strcmp(token,"v"))
		{
			fscanf(scene,"%f %f %f",&vec[0],&vec[1],&vec[2]);
			vList_.push_back(Vec3(vec));
		}

		else if (!strcmp(token,"vn"))
		{
			fscanf(scene,"%f %f %f",&vec[0],&vec[1],&vec[2]);
			nList_.push_back(Vec3(vec));
		}
		else if (!strcmp(token,"vt"))
		{
			fscanf(scene,"%f %f",&vec[0],&vec[1]);
			tList_.push_back(Vec3(vec));
		}

		else if (!strcmp(token,"f"))
		{
			size_t i;
			for (i=0;i<3;i++)		// face 預設為 3，假設一個 polygon 都只有 3 個 vertex
			{
				fscanf(scene,"%s",v[i]);
				//printf("[%s]",v[i]);
			}
			//printf("\n");
		  
			Vertex	tmp_vertex[3];		// for faceList_ structure

			for (i=0;i<3;i++)		// for each vertex of this face
			{
				char str[20], ch;
				size_t base,offset;
				base = offset = 0;

				// calculate vertex-list index
				while( (ch=v[i][base+offset]) != '/' && (ch=v[i][base+offset]) != '\0')
				{
					str[offset] = ch;
					offset++;
				}
				str[offset] = '\0';
				n_vertex = atoi(str);
				base += (ch == '\0')? offset : offset+1;
				offset = 0;

				// calculate texture-list index
				while( (ch=v[i][base+offset]) != '/' && (ch=v[i][base+offset]) != '\0')
				{
					str[offset] = ch;
					offset++;
				}
				str[offset] = '\0';
				n_texture = atoi(str);	// case: xxx//zzz，texture 設為 0 (tList_ 從 1 開始)
				base += (ch == '\0')? offset : offset+1;
				offset = 0;

				// calculate normal-list index
				while( (ch=v[i][base+offset]) != '\0')
				{
					str[offset] = ch;
					offset++;
				}
				str[offset] = '\0';
				n_normal = atoi(str);	// case: xxx/yyy，normal 設為 0 (nList_ 從 1 開始)

				tmp_vertex[i].v = n_vertex;
				tmp_vertex[i].t = n_texture;
				tmp_vertex[i].n = n_normal;
			}

			faceList_.push_back(Face(tmp_vertex[0],tmp_vertex[1],tmp_vertex[2], cur_mtl));
		}

		else if (!strcmp(token,"#"))	  // 註解
			fgets(buf,100,scene);

//		printf("[%s]\n",token);
	}

	if (scene) fclose(scene);

	vTotal_ = vList_.size();
	nTotal_ = nList_.size();
	tTotal_ = tList_.size();
	fTotal_ = faceList_.size();
	printf("vetex: %d, normal: %d, texture: %d, triangles: %d\n",vTotal_, nTotal_, tTotal_, fTotal_);
}

void Mesh::loadMtl(string tex_file)
{
	char	token[100], buf[100];
	float	r,g,b;

	mtlFilePtr_ = fopen(tex_file.c_str(),"r");

	if (!mtlFilePtr_) 
	{
		cout << "Can't open Material file \"" << tex_file << "\"!" << endl;
		return;
	}

	cout<<tex_file<<endl;

	size_t cur_mat;

	while(!feof(mtlFilePtr_))
	{
		token[0] = NULL;
		fscanf(mtlFilePtr_,"%s", token);		// 讀 token

		if (!strcmp(token,"newmtl"))
		{
			fscanf(mtlFilePtr_,"%s",buf);
			Material newMtl;
			mList_.push_back(newMtl);
			cur_mat = mTotal_++;					// 從 mList_[1] 開始，mList_[0] 空下來給 default Material 用
			matMap_[objFile_+string("_")+string(buf)] = cur_mat; 	// matMap_["Material_name"] = Material_id;
		}

		else if (!strcmp(token,"Ka"))
		{
			fscanf(mtlFilePtr_,"%f %f %f",&r,&g,&b);
			mList_[cur_mat].Ka[0] = r;
			mList_[cur_mat].Ka[1] = g;
			mList_[cur_mat].Ka[2] = b;
			mList_[cur_mat].Ka[3] = 1;
		}

		else if (!strcmp(token,"Kd"))
		{
			fscanf(mtlFilePtr_,"%f %f %f",&r,&g,&b);
			mList_[cur_mat].Kd[0] = r;
			mList_[cur_mat].Kd[1] = g;
			mList_[cur_mat].Kd[2] = b;
			mList_[cur_mat].Kd[3] = 1;
		}

		else if (!strcmp(token,"Ks"))
		{
			fscanf(mtlFilePtr_,"%f %f %f",&r,&g,&b);
			mList_[cur_mat].Ks[0] = r;
			mList_[cur_mat].Ks[1] = g;
			mList_[cur_mat].Ks[2] = b;
			mList_[cur_mat].Ks[3] = 1;
		}

		else if (!strcmp(token,"Ns"))
		{
			fscanf(mtlFilePtr_,"%f",&r);
			mList_[cur_mat].Ns = r;
		}

		else if (!strcmp(token,"Tr"))
		{
			fscanf(mtlFilePtr_,"%f",&r);
			mList_[cur_mat].Tr = r;
		}

		else if (!strcmp(token,"d"))
		{
			fscanf(mtlFilePtr_,"%f",&r);
			mList_[cur_mat].Tr = r;
		}

		if (!strcmp(token,"map_Kd"))
		{
			fscanf(mtlFilePtr_,"%s",buf);
			mList_[cur_mat].map_Kd = buf;
		}

		if (!strcmp(token,"map_Ks"))
		{
			fscanf(mtlFilePtr_,"%s",buf);
			mList_[cur_mat].map_Ks = buf;
		}

		if (!strcmp(token,"map_Ka"))
		{
			fscanf(mtlFilePtr_,"%s",buf);
			mList_[cur_mat].map_Ka = buf;
		}

		else if (!strcmp(token,"#"))	  // 註解
			fgets(buf,100,mtlFilePtr_);

//		printf("[%s]\n",token);
	}

	printf("total Material:%d\n",matMap_.size());

	if (mtlFilePtr_) fclose(mtlFilePtr_);
}

void Mesh::init(const char* objFile)
{
	float default_value[3] = {1,1,1};

	vList_.push_back(Vec3(default_value));	// 因為 *.obj 的 index 是從 1 開始
	nList_.push_back(Vec3(default_value));	// 所以要先 push 一個 default value 到 vList_[0],nList_[0],tList_[0]
	tList_.push_back(Vec3(default_value));

	Material defaultMtl;
	mList_.push_back(defaultMtl);
	// 定義 default meterial: mList_[0]
	mList_[0].Ka[0] = 0.0f; mList_[0].Ka[1] = 0.0f; mList_[0].Ka[2] = 0.0f; mList_[0].Ka[3] = 1.0f; 
	mList_[0].Kd[0] = 1.0f; mList_[0].Kd[1] = 1.0f; mList_[0].Kd[2] = 1.0f; mList_[0].Kd[3] = 1.0f; 
	mList_[0].Ks[0] = 0.8f; mList_[0].Ks[1] = 0.8f; mList_[0].Ks[2] = 0.8f; mList_[0].Ks[3] = 1.0f;
	mList_[0].Ns = 32.0f;
	mTotal_++;

	loadMesh(string(objFile));		// 讀入 .obj 檔 (可處理 Material)
}

