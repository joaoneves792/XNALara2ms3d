#include "MS3DFile.h"
#include "MS3DFileI.h"
#include "Textures.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <set>


#define MAKEDWORD(a, b)      ((unsigned int)(((word)(a)) | ((word)((word)(b))) << 16))

bool CMS3DFile::LoadXNAFromFile(const char* lpszFileName)
{
	FILE *fp = fopen(lpszFileName, "rb");
	if (!fp)
		return false;

	size_t i;
	
	// joints
	word nNumJoints;
	fread(&nNumJoints, 1, sizeof(int), fp);
	std::vector<XNA_joint_t> joints(nNumJoints);
	_i->arrJoints.resize(nNumJoints);
	for (i = 0; i < nNumJoints; i++)
	{
		joints[i].flags = 0;
		
		joints[i].id = i;
		int stringSize = 0;
		fread(&stringSize, 1, sizeof(char), fp);
		int j;
		for(j=0; (j<stringSize)&&(j<31); j++){
			fread(&joints[i].name[j], 1, sizeof(char), fp); 
		}
		joints[i].name[j] = '\0';
		if(stringSize>32){ /*If the name is longer than 32 bytes then skip the excess*/
			fseek(fp, stringSize-32, SEEK_CUR);
		}
		
		//This needs to be done in another pass
		//fread(&_i->arrJoints[i].parentName, 32, sizeof(char), fp);
		fread(&joints[i].parentId, 1, sizeof(short), fp);

		//XNALara has no rotation so we set it to 0.0
		joints[i].rotation[0] = 0.0;
		joints[i].rotation[1] = 0.0;
		joints[i].rotation[2] = 0.0;
		
		fread(&joints[i].position, 3, sizeof(float), fp);

		//No animations are suported so we just set these values to safe defaults
		joints[i].numKeyFramesRot = 0;
		joints[i].numKeyFramesTrans = 0;

		//_i->arrJoints[i].keyFramesRot = new ms3d_keyframe_rot_t[_i->arrJoints[i].numKeyFramesRot];
		//_i->arrJoints[i].keyFramesTrans = new ms3d_keyframe_pos_t[_i->arrJoints[i].numKeyFramesTrans];

		//fread(_i->arrJoints[i].keyFramesRot, _i->arrJoints[i].numKeyFramesRot, sizeof(ms3d_keyframe_rot_t), fp);
		//fread(_i->arrJoints[i].keyFramesTrans, _i->arrJoints[i].numKeyFramesTrans, sizeof(ms3d_keyframe_pos_t), fp);
	}

	//Now we set the parenthood of the joints properly
	for(int i=0; i<nNumJoints; i++){
		unsigned int parentId = joints[i].parentId;
		if(parentId == 0xFFFF){ //The root bone on XNALara has the parent set to 0xFFFF so we need to put it pointing to itself
			parentId = i;
		}	
		strncpy(joints[i].parentName, joints[parentId].name, 32);
	}

	//And now we can place the information on normal ms3d joints
	for(int i=0; i<nNumJoints; i++){
		_i->arrJoints[i].flags = joints[i].flags;
		strncpy(_i->arrJoints[i].name, joints[i].name, 32);				
    		strncpy(_i->arrJoints[i].parentName, joints[i].parentName, 32);
		_i->arrJoints[i].rotation[0] = joints[i].rotation[0];
		_i->arrJoints[i].rotation[1] = joints[i].rotation[1];
		_i->arrJoints[i].rotation[2] = joints[i].rotation[1];
		
		_i->arrJoints[i].position[0] = joints[i].position[0];
		_i->arrJoints[i].position[1] = joints[i].position[1];
		_i->arrJoints[i].position[2] = joints[i].position[1];
    
		_i->arrJoints[i].numKeyFramesRot = joints[i].numKeyFramesRot;
		_i->arrJoints[i].numKeyFramesTrans = joints[i].numKeyFramesTrans;
	}
	
	joints.clear();
	std::vector<XNA_joint_t>().swap(joints);

	// groups
	word nNumGroups;
	fread(&nNumGroups, 1, sizeof(int), fp);
	_i->arrGroups.resize(nNumGroups);
	_i->arrMaterials.resize(nNumGroups);
	for (i = 0; i < nNumGroups; i++)
	{

		std::cout << i << "\n";
		_i->arrGroups[i].flags = 0;
		
		int stringSize = 0;
		fread(&stringSize, 1, sizeof(char), fp);
		int j;
		for(j=0; (j<stringSize)&&(j<31); j++){
			fread(&_i->arrGroups[i].name[j], 1, sizeof(char), fp); 
		}
		_i->arrGroups[i].name[j] = '\0';
		if(stringSize>32){ /*If the name is longer than 32 bytes then skip the excess*/
			fseek(fp, stringSize-32, SEEK_CUR);
		}
		
		int uvLayerCount;
		fread(&uvLayerCount, 1, sizeof(int), fp);
		int textureCount;
		fread(&textureCount, 1, sizeof(int), fp);
		
		for(j=0; j<textureCount; j++){
			if(j==0){
				
				strncpy(_i->arrMaterials[i].name, _i->arrGroups[i].name, 32);
				
				int stringSize = 0;
				fread(&stringSize, 1, sizeof(char), fp);
				int k;
				for(k=0; (k<stringSize)&&(k<127); k++){
					fread(&_i->arrMaterials[i].texture[k], 1, sizeof(char), fp); 
				}
				_i->arrMaterials[i].texture[j] = '\0';
				if(stringSize>127){ /*If the name is longer than 32 bytes then skip the excess*/
					fseek(fp, stringSize-127, SEEK_CUR);
				}
				int uvLayerIndex;
				fread(&uvLayerIndex, 1, sizeof(int), fp);

				//XNA doesnt specify these so we use some safe defaults
				_i->arrMaterials[i].ambient[0] = 51;
				_i->arrMaterials[i].ambient[1] = 51;
				_i->arrMaterials[i].ambient[2] = 51;
				_i->arrMaterials[i].ambient[3] = 0;
				
				_i->arrMaterials[i].diffuse[0] = 204;
				_i->arrMaterials[i].diffuse[1] = 204;
				_i->arrMaterials[i].diffuse[2] = 204;
				_i->arrMaterials[i].diffuse[3] = 0;
				
				_i->arrMaterials[i].specular[0] = 0;
				_i->arrMaterials[i].specular[1] = 0;
				_i->arrMaterials[i].specular[2] = 0;
				_i->arrMaterials[i].specular[3] = 0;
				
				_i->arrMaterials[i].emissive[0] = 0;
				_i->arrMaterials[i].emissive[1] = 0;
				_i->arrMaterials[i].emissive[2] = 0;
				_i->arrMaterials[i].emissive[3] = 0;
    				
				_i->arrMaterials[i].shininess = 0.0;

				_i->arrMaterials[i].transparency = 0.0;

				_i->arrMaterials[i].mode = 0;

				_i->arrMaterials[i].alphamap[0] = '\0';

				
			}else{ //ms3d groups can only have one texture so skip everything after the first
				int stringSize = 0;
				fread(&stringSize, 1, sizeof(char), fp);
				fseek(fp, stringSize, SEEK_CUR);
				fseek(fp, sizeof(int), SEEK_CUR); //Skip UV layer index

			}
		}

		int vertexCount = 0;
		fread(&vertexCount, 1, sizeof(int), fp);
		int vertStartIndex = _i->arrVertices.size();
		_i->arrVertices.resize(_i->arrVertices.size() + vertexCount);
		std::vector<XNA_vertex_t> vertices(vertexCount);
		for(int vertexId=0; vertexId<vertexCount; vertexId++){
			vertices[vertexId].flags = 0;
			
			fread(&vertices[vertexId].vertex[0], 1, sizeof(float), fp);
			fread(&vertices[vertexId].vertex[1], 1, sizeof(float), fp);
			fread(&vertices[vertexId].vertex[2], 1, sizeof(float), fp);
			
			fread(&vertices[vertexId].normal[0], 1, sizeof(float), fp);
			fread(&vertices[vertexId].normal[1], 1, sizeof(float), fp);
			fread(&vertices[vertexId].normal[2], 1, sizeof(float), fp);

			fseek(fp, 4, SEEK_CUR); //Skip vertex colors

			fread(&vertices[vertexId].uv[0], 1, sizeof(float), fp);
			fread(&vertices[vertexId].uv[1], 1, sizeof(float), fp);

			fseek(fp, (uvLayerCount > 0)?((uvLayerCount-1)*2*sizeof(float)):0, SEEK_CUR); //Skip UVs for other textures

			fseek(fp, uvLayerCount*4*sizeof(float), SEEK_CUR); //Skip tangents

			fread(&vertices[vertexId].boneIndicesGlobal, 4, sizeof(short), fp);
			
			fread(&vertices[vertexId].weights, 4, sizeof(float), fp);
			
			//Since ms3d doesnt support weights we have to pick the joint with the biggest weight and assign this vertex to it
			float maxWeight = -1;
			int assignedJointId = 0;
			for(int j=0; j<4; j++){
				if(vertices[vertexId].weights[j] > maxWeight){
					assignedJointId = vertices[vertexId].boneIndicesGlobal[j];
				}
			}
			vertices[vertexId].boneId = assignedJointId;
			vertices[vertexId].referenceCount = 0;
		}
		
		int nNumTriangles = 0;
		fread(&nNumTriangles, 1, sizeof(int), fp);
		_i->arrGroups[i].numtriangles = nNumTriangles;
		
		int triStartIndex = _i->arrTriangles.size();
		_i->arrTriangles.resize(_i->arrTriangles.size() + nNumTriangles);
		
		_i->arrGroups[i].triangleIndices = new word[_i->arrGroups[i].numtriangles];
		for(int k=0; k<nNumTriangles; k++){
			_i->arrGroups[i].triangleIndices[k] = triStartIndex + k;
		}

		for(int triId = triStartIndex; triId < (int)_i->arrTriangles.size(); triId++){
    			
			_i->arrTriangles[triId].flags = 0;
		
			fread(&_i->arrTriangles[triId].vertexIndices[0], 1, sizeof(int), fp);
			vertices[_i->arrTriangles[triId].vertexIndices[0]].referenceCount++;
			_i->arrTriangles[triId].vertexIndices[0] += vertStartIndex;
			
			fread(&_i->arrTriangles[triId].vertexIndices[1], 1, sizeof(int), fp);
			vertices[_i->arrTriangles[triId].vertexIndices[1]].referenceCount++;
			_i->arrTriangles[triId].vertexIndices[1] += vertStartIndex;
			
			fread(&_i->arrTriangles[triId].vertexIndices[2], 1, sizeof(int), fp);
			vertices[_i->arrTriangles[triId].vertexIndices[2]].referenceCount++;
			_i->arrTriangles[triId].vertexIndices[2] += vertStartIndex;
			
			//Get the normals from the temporary XNA vertice structure
			for(int v=0; v<3; v++){
				for(int coord=0; coord<3; coord++){
					_i->arrTriangles[triId].vertexNormals[v][coord] = vertices[_i->arrTriangles[triId].vertexIndices[v]-vertStartIndex].normal[coord];
				}
			}
	
			//Get the UV coordinates from the temporary XNA vertice structure
			for(int v=0; v<3; v++){
					_i->arrTriangles[triId].s[v] = vertices[_i->arrTriangles[triId].vertexIndices[v]-vertStartIndex].uv[0];
					_i->arrTriangles[triId].t[v] = vertices[_i->arrTriangles[triId].vertexIndices[v]-vertStartIndex].uv[1];
			}
			
			_i->arrTriangles[triId].smoothingGroup = 0;
			_i->arrTriangles[triId].groupIndex = i;
		}


		//Now we can translate the XNA vertex structure into the ms3d vertex struct
		for(int j=vertStartIndex; j< (int)_i->arrVertices.size(); j++){
			_i->arrVertices[j].flags = vertices[j-vertStartIndex].flags;
			
			_i->arrVertices[j].vertex[0] = vertices[j-vertStartIndex].vertex[0];
			_i->arrVertices[j].vertex[1] = vertices[j-vertStartIndex].vertex[1];
			_i->arrVertices[j].vertex[2] = vertices[j-vertStartIndex].vertex[2];
    			
			_i->arrVertices[j].boneId = vertices[j-vertStartIndex].boneId;

			_i->arrVertices[j].referenceCount = vertices[j-vertStartIndex].referenceCount;
		}
		vertices.clear();
		_i->arrGroups[i].materialIndex = i;
	}

	// edges
	std::set<unsigned int> setEdgePair;
	for (i = 0; i < _i->arrTriangles.size(); i++)
	{
		word a, b;
		a = _i->arrTriangles[i].vertexIndices[0];
		b = _i->arrTriangles[i].vertexIndices[1];
		if (a > b)
			std::swap(a, b);
		if (setEdgePair.find(MAKEDWORD(a, b)) == setEdgePair.end())
			setEdgePair.insert(MAKEDWORD(a, b));

		a = _i->arrTriangles[i].vertexIndices[1];
		b = _i->arrTriangles[i].vertexIndices[2];
		if (a > b)
			std::swap(a, b);
		if (setEdgePair.find(MAKEDWORD(a, b)) == setEdgePair.end())
			setEdgePair.insert(MAKEDWORD(a, b));

		a = _i->arrTriangles[i].vertexIndices[2];
		b = _i->arrTriangles[i].vertexIndices[0];
		if (a > b)
			std::swap(a, b);
		if (setEdgePair.find(MAKEDWORD(a, b)) == setEdgePair.end())
			setEdgePair.insert(MAKEDWORD(a, b));
	}

	for(std::set<unsigned int>::iterator it = setEdgePair.begin(); it != setEdgePair.end(); ++it)
	{
		unsigned int EdgePair = *it;
		ms3d_edge_t Edge;
		Edge.edgeIndices[0] = (word) EdgePair;
		Edge.edgeIndices[1] = (word) ((EdgePair >> 16) & 0xFFFF);
		_i->arrEdges.push_back(Edge);
	}


	_i->fAnimationFPS = 30.0;
	_i->fCurrentTime = 0;
 	_i->iTotalFrames = 0;

	fclose(fp);

	return true;
}
